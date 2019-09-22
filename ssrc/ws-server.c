#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
/* #include <netinet/in.h> */
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
/* #include <sys/types.h> */
#include <netdb.h>
#include <stdio.h>

#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#define PORT 4202

#define FIN(head) !!(head[0] & 0x80)
#define RSV1(head) !!(head[0] & 0x40)
#define RSV2(head) !!(head[0] & 0x20)
#define RSV3(head) !!(head[0] & 0x10)
#define OPCODE(head) ((unsigned char) (head[0] & 0x0f))
#define MASKED(head) !!(head[1] & 0x80)
#define PAYLOAD_LEN(head) ((unsigned char) (head[1] & 0x7f))

#define HTMLSIZ (BUFSIZ * 4)

#if 0
static inline void
profile_tick(char *s)
{
	struct timeval tick;
	if (!gettimeofday(&tick, NULL))
		fprintf(stderr, "PROFILE %s %llu:%lu\n", s, tick.tv_sec, tick.tv_usec);
}
#else
#define profile_tick(...) do {} while (0)
#endif

#define debug(...) fprintf(stderr, __VA_ARGS__)

enum ws_flags {
	WSF_CLOSING = 1,
	WSF_RECONNECT = 2,
	WSF_AUTH = 4,
	WSF_OLD = 8,
};

struct attr { int fg, bg, x; };

struct ws {
	SSL *cSSL;
	char username[32];
	char password[32];
	int tfd;
	int flags;
	int esc_state;
	unsigned addr; 
	struct attr c_attr, csi;
};

struct frame {
	char head[2];
	char mk[4];
	uint64_t pl;
	char data[BUFSIZ];
};

static struct ws wss[FD_SETSIZE];
static int cfds[FD_SETSIZE];
static int sfd = -1; 
static unsigned nt = 0;

int __b64_ntop(unsigned char const *src, size_t srclength,
	       char *target, size_t targsize);

static inline void
ws_handshake(SSL *cSSL) {
	static char const common_resp[]
		= "HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: upgrade\r\n"
		"Sec-Websocket-Protocol: text\r\n"
		"Sec-Websocket-Accept: ",
		kkey[] = "Sec-WebSocket-Key",
		debugv[] = "dGhlIHNhbXBsZSBub25jZQ==";
	unsigned char hash[SHA_DIGEST_LENGTH];
	unsigned char result[29];
	char buf[BUFSIZ];
	char *s;

	while (SSL_read(cSSL, buf, sizeof(buf)) > 0) {
		for (s = buf; s && *s; s = strchr(s, '\n'))
			if (!strncasecmp(++s, kkey, sizeof(kkey) - 1)) {
				SHA_CTX c;
				char *s2;
				s += sizeof(kkey) + 1;
				SHA1_Init(&c);
				s2 = strchr(s, '\r');
				SHA1_Update(&c, s, s2 - s);
				SHA1_Update(&c, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 36);
				SHA1_Final(hash, &c);
				__b64_ntop(hash, sizeof(hash), result, sizeof(result));
				SSL_write(cSSL, common_resp, sizeof(common_resp) - 1);
				SSL_write(cSSL, result, sizeof(result) - 1);
				SSL_write(cSSL, "\r\n\r\n", 4);
				return;
			}
	}
}

static int
telnet_connect()
{
	struct sockaddr_in tservaddr; 
	int tfd = socket(AF_INET, SOCK_STREAM, 0);

	if (tfd == -1)
		return -1;

	bzero(&tservaddr, sizeof(tservaddr));
	tservaddr.sin_family = AF_INET;
	tservaddr.sin_addr.s_addr = 0x0100007f;
	tservaddr.sin_port = htons(4201);

	if (connect(tfd, (struct sockaddr *) &tservaddr, sizeof(tservaddr)))
		return -1;

	return tfd;
}

static inline void
forward_ip(struct ws *ws)
{
	unsigned old = !!(ws->flags & WSF_OLD);
	char w = 'w';

	write(ws->tfd, &w, sizeof(w));
	write(ws->tfd, &ws->addr, sizeof(ws->addr));
	write(ws->tfd, &old, sizeof(old));
}

static inline void
auth(struct ws *ws)
{
	char buf[126];
	size_t len = snprintf(buf, sizeof(buf), "c %s %s\r\n",
			      ws->username, ws->password);
	write(ws->tfd, buf, len);
	ws->flags |= WSF_AUTH;
}

static inline void
telnet_close(struct ws *ws, fd_set *fdset)
{
	/* ws->flags &= ~WSF_AUTH; */
	debug("TELNET_CLOSE %d\n", ws->tfd);
	cfds[ws->tfd] = 0;
	FD_CLR(ws->tfd, fdset);
	close(ws->tfd);
	ws->tfd = -1;
	nt--;
}

static inline void
ws_close(int cfd, fd_set *fdset)
{
	struct ws *ws = &wss[cfd];
	debug("WS_CLOSE %d\n", cfd);
	if (ws->tfd != -1)
		telnet_close(ws, fdset);
	FD_CLR(cfd, fdset);
	SSL_shutdown(ws->cSSL);
	SSL_free(ws->cSSL);
	close(cfd);
	memset(ws, 0, sizeof(struct ws));
}

static inline void
ws_close_policy(int cfd) {
	struct ws * ws = &wss[cfd];
	unsigned char head[2] = { 0x88, 0x02 };
	unsigned code = 1008;

	SSL_write(ws->cSSL, head, sizeof(head));
	SSL_write(ws->cSSL, &code, sizeof(code));
	ws->flags |= WSF_CLOSING;
}

static void
ws_read(int cfd, fd_set *fdset)
{
	struct frame frame;
	struct ws *ws = &wss[cfd];
	SSL *cSSL = ws->cSSL;
	uint64_t pl;
	int i, n;

	profile_tick("WS_READ");

	n = SSL_read(cSSL, frame.head, sizeof(frame.head));
	if (n != sizeof(frame.head))
		goto error;

	pl = PAYLOAD_LEN(frame.head);

	if (pl == 126) {
		uint16_t rpl;
		n = SSL_read(cSSL, &rpl, sizeof(rpl));
		if (n != sizeof(rpl))
			goto error;
		pl = rpl;
	} else if (pl == 127) {
		uint64_t rpl;
		n = SSL_read(cSSL, &rpl, sizeof(rpl));
		if (n != sizeof(rpl))
			goto error;
		pl = rpl;
	}

	frame.pl = pl;

	n = SSL_read(cSSL, frame.mk, sizeof(frame.mk));
	if (n != sizeof(frame.mk))
		goto error;

	if (OPCODE(frame.head) == 8) {
		ws_close(cfd, fdset);
		return;
	}

	n = SSL_read(cSSL, frame.data, pl);
	if (n != pl) {
		debug("BAD PL %d / %llu\n", n, pl);
		// should disconnect
		goto error;
	}

	for (i = 0; i < pl; i++)
		frame.data[i] ^= frame.mk[i % 4];

	frame.data[i] = '\0';
	if ((ws->flags & WSF_AUTH)) {
		if (ws->tfd != -1) {
			frame.data[i++] = '\n';
			write(ws->tfd, frame.data, i);
		}
	} else {
		char *p = strchr(frame.data, ';');
		if (!p) {
			debug("BAD AUTH STRING\n");
			return;
		}

		*p = '\0';
		strncpy(ws->username, frame.data, sizeof(ws->username));
		frame.data[frame.pl] = '\0';
		strncpy(ws->password, p + 1, sizeof(ws->password));
		auth(ws);
	}

	return;
error:
	ws_close_policy(cfd);
}

static inline int
telnet_open(int cfd, fd_set *fdset) {
	struct ws *ws = &wss[cfd];
	debug("TELNET OPEN %d\n", cfd);
	ws->tfd = telnet_connect();
	if (ws->tfd == -1)
		return -1;
	cfds[ws->tfd] = cfd;
	FD_SET(ws->tfd, fdset);
	forward_ip(ws);
	ws->flags |= WSF_OLD;
	nt++;
	debug("%d!\n", ws->tfd);
	if ((ws->flags & WSF_AUTH))
		auth(ws);
	return 0;
}

static inline int
ws_new(SSL_CTX *sslctx, fd_set *afdset)
{
	profile_tick("WS_NEW");
	/* static const char noservice[] = "HTTP/1.1 503 Service Unavailable\r\nConnection: close\r\n\r\n"; */
	struct sockaddr_in cli; 
	SSL *cSSL;
	char *ip = NULL;
	int len = sizeof(struct sockaddr),
	    cfd = accept(sfd, (struct sockaddr *) &cli, &len);

	if (cfd <= 0)
		return 1;

	ip = inet_ntoa(cli.sin_addr);
	debug("WS_NEW %d %s\n", cfd, ip);

	cSSL = SSL_new(sslctx);
	SSL_set_fd(cSSL, cfd);
	
	if (SSL_accept(cSSL) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_shutdown(cSSL);
		SSL_free(cSSL);
		close(cfd);
		return 1;
	}

	ws_handshake(cSSL);
	FD_SET(cfd, afdset);

	struct ws *ws = &wss[cfd];

	ws->cSSL = cSSL;
	debug("start cSSL %p\n", cSSL);
	ws->csi.fg = ws->c_attr.fg = 7;
	ws->csi.bg = ws->c_attr.bg = 0;
	ws->csi.x = ws->c_attr.x = 0;
	ws->addr = cli.sin_addr.s_addr;
	telnet_open(cfd, afdset);

	return 0;
}

static void params_push(struct ws *ws, int x) {
        int     fg = ws->c_attr.fg,
                bg = ws->c_attr.bg;
 
        switch (x) {
                case 0: fg = 7; bg = 0; break;
                case 1: fg += 8; break;
                default: if (x >= 40)
                                bg = x - 40;
			 else if (x >= 30)
                                fg = (fg >= 8 ? 8 : 0) + x - 30;
        }

	ws->csi.fg = fg;
	ws->csi.bg = bg;
	ws->csi.x = x;
}

static size_t
telnet_parse(struct ws *ws, char *html, char *data, size_t n, fd_set *fdset)
{
	size_t html_len;
	char *p, *end_tag = "";
	register int esc_state = ws->esc_state;

	for (html_len = 0, p = data; html_len < HTMLSIZ && *p && p < data + n; p++) {
		register char ch = *p;

		switch (ch) {
                        case '\x18':
                        case '\x1a':
                                esc_state = 0;
                                continue;
                        case '\x1b':
                                esc_state = 1;
                                continue;
                        case '\x9b':
                                esc_state = 2;
                                continue;
                        case '\x07': 
				esc_state = 7;
				continue;
                        case '\x00':
                        case '\x7f':
                        case '\v':
                        case '\r':
                        case '\f':
				continue;
		}

                switch (esc_state) {
                        case 0:
                                switch (ch) {
                                        case '&':
						html_len += snprintf(&html[html_len], HTMLSIZ - html_len, "&amp;");
						break;
                                        /* case '<': */
						/* html_len += snprintf(&html[html_len], HTMLSIZ - html_len, "&lt;"); */
						/* break; */
                                        /* case '>': */
						/* html_len += snprintf(&html[html_len], HTMLSIZ - html_len, "&gt;"); */
						/* break; */
					default:
						html[html_len] = ch;
						html_len++;
                                }
                                break;
                        case 1:
                                switch (ch) {
                                        case '[':
                                                esc_state = 2;
                                                break;

                                        case '=':
                                        case '>':
                                        case 'H':
                                                esc_state = 0; /* IGNORED */
                                                break;
                                }
                                break;
                        case 2: // just saw CSI
                                switch (ch) {
                                        case 'K':
                                        case 'H':
                                        case 'J':
                                                esc_state = 0;
                                                continue;
                                        case '?':
                                                esc_state = 5;
                                                continue;
                                }
                                params_push(ws, 0);
                                esc_state = 3;
                        case 3: // saw CSI and parameters
                                switch (ch) {
				case 'm':
					if (ws->c_attr.bg != ws->csi.bg
					    || ws->c_attr.fg != ws->csi.fg) {
						char * span_class_end = "";
						int a = ws->csi.fg != 7, b = ws->csi.bg != 0;
						html_len += snprintf(&html[html_len],
								     HTMLSIZ - html_len,
								     "%s", end_tag);
						if (a || b) {
							html_len += snprintf(&html[html_len],
									     HTMLSIZ - html_len,
									     "<span class=\"");
							span_class_end = "\">";
							end_tag = "</span>";
						}

						if (a)
							html_len += snprintf(&html[html_len],
									     HTMLSIZ - html_len,
									     "fg%d ", ws->csi.fg);
						if (b)
							html_len += snprintf(&html[html_len],
									     HTMLSIZ - html_len,
									     "bg%d ", ws->csi.bg);

						html_len += snprintf(&html[html_len],
								     HTMLSIZ - html_len,
								     "%s", span_class_end);

						ws->c_attr.fg = ws->csi.fg;
						ws->c_attr.bg = ws->csi.bg;
						ws->c_attr.x = 0;
						ws->csi.x = 0;
					}

					esc_state = 0;
					break;
				case '[':
					esc_state = 4;
					break;
				case ';':
					params_push(ws, 0);
					break;
				default:
					if (ch >= '0' && ch <= '9')
						params_push(ws, ws->csi.x * 10 + (ch - '0'));
					else
						esc_state = 0;
                                }
                                break;
                        case 5: params_push(ws, ch);
                                esc_state = 6;
                                break;
			case 7:
				switch (ch) {
				case '0':
					ws->flags &= ~WSF_AUTH;
					break;
				case '1':
					telnet_close(ws, fdset);
				}
                        case 4:
                        case 6:
                                esc_state = 0;
                                break;
                }
        }
	ws->esc_state = esc_state;
	html[html_len] = '\0';
	return html_len;
}

static void
telnet_read(int tfd, fd_set *fdset)
{
	profile_tick("TELNET_READ");
	char data[BUFSIZ];
	char html[HTMLSIZ];
	unsigned char head[2] = { 0x81, 0x00 };
	int cfd = cfds[tfd];
	struct ws *ws = &wss[cfd];
	SSL *cSSL = ws->cSSL;
	ssize_t n;

	debug("telnet_read %d cSSL %p\n", tfd, cSSL);
	n = read(tfd, data, sizeof(data));

	if (n == 0) {
		telnet_close(ws, fdset);
		return;
	} else if (n < 0) {
		perror("TELNET_READ");
		return;
	}

	data[n] = '\0';
	n = telnet_parse(ws, html, data, n, fdset);

	if (n < 126) {
		head[1] |= n;
		SSL_write(cSSL, head, sizeof(head));
	} else if (n < (1 << 16)) {
		uint16_t nn = htons(n);
		head[1] |= 126;
		SSL_write(cSSL, head, sizeof(head));
		SSL_write(cSSL, &nn, sizeof(nn));
	} else {
		uint64_t nn = htonl(n);
		head[1] |= 127;
		SSL_write(cSSL, head, sizeof(head));
		SSL_write(cSSL, &nn, sizeof(nn));
	}

	SSL_write(cSSL, html, n);
}

int main() 
{ 
	struct sockaddr_in servaddr; 
	SSL_CTX *sslctx;
	fd_set rfdset, afdset;

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1
	    || bind(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0
	    || listen(sfd, 5) != 0) {
		perror("...");
		return 1;
	}

	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	sslctx = SSL_CTX_new(TLS_server_method());
	SSL_CTX_set_ecdh_auto(sslctx, 1);
	SSL_CTX_use_certificate_file(sslctx, "/etc/ssl/0.0.0.0:443.crt" , SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(sslctx, "/etc/ssl/private/0.0.0.0:443.key", SSL_FILETYPE_PEM);
	ERR_print_errors_fp(stderr);
	FD_ZERO(&afdset);
	FD_SET(sfd, &afdset);

	for (; ; ) {
		struct timeval interval = { 3, 0 }, *timeptr = NULL;
		int i;
		rfdset = afdset;

		if (nt == 0) {
			// so that we know telnet status
			int tfd = telnet_connect();
			close(tfd);

			for (i = 0; i < FD_SETSIZE; i++)

				if (wss[i].tfd == -1)
					telnet_open(i, &afdset);

			timeptr = &interval;
		}

		if (select(FD_SETSIZE, &rfdset, NULL, NULL, timeptr) <= 0)
			continue;

		for (i = 0; i < FD_SETSIZE; i++) {
			if (!FD_ISSET(i, &rfdset))
				continue;

			if (i == sfd)
				ws_new(sslctx, &afdset);
			else if (wss[i].tfd == 0)
				telnet_read(i, &afdset);
			else if ((wss[i].flags & WSF_CLOSING))
				ws_close(i, &afdset);
			else
				ws_read(i, &afdset);
		}
	}

	close(sfd);
	return 0;
}

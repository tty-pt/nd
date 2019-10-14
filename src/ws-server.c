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
#include <fcntl.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "debug.h"

#define PORT 4202

#define FIN(head) !!(head[0] & 0x80)
#define RSV1(head) !!(head[0] & 0x40)
#define RSV2(head) !!(head[0] & 0x20)
#define RSV3(head) !!(head[0] & 0x10)
#define OPCODE(head) ((unsigned char) (head[0] & 0x0f))
#define MASKED(head) !!(head[1] & 0x80)
#define PAYLOAD_LEN(head) ((unsigned char) (head[1] & 0x7f))

#define HTMLSIZ (BUFSIZ * 16)

#define GET_FLAG(ws, x)		( (ws)->flags & x )
#define SET_FLAGS(ws, x)	{ (ws)->flags |= x ; }
#define UNSET_FLAGS(mcp, x)	{ (ws)->flags &= ~(x) ; }

#define WRITEF(...) html_len += snprintf(&html[html_len], HTMLSIZ - html_len, __VA_ARGS__)

enum ws_flags {
	CLOSING = 1,
	AUTH = 2,
	OLD = 4,
	MCP_CONTINUES = 8,
	MCP_INBAND = 16,
	MCP_VALUE = 32,
	MCP_OPEN = 64,
	MCP_CLOSED = 128,
	MCP_ONCE = 256,
};

enum mcp_state {
        // 1 means "\n" was read from telnet
        // 2 means "\n#"
        // 3 means "\n#$"
        MCP_CONFIRMED = 4, // "\n#$#"
	MCP_ECHO_KEY,
        MCP_SKIP_HASH, 
        MCP_ECHO_ARG_KEY,
        MCP_SEEK_ARG_VALUE,
        MCP_ECHO_ARG_VALUE,
        MCP_SEEK_ARG_KEY,
        MCP_SEEK_ML_VALUE, 
        MCP_ECHO_ML_VALUE,
        MCP_SKIP_LINE, 
};

struct attr { int fg, bg, x; };

struct mcp_arg {
        char key[32];
        char data[1024];
};

struct ws {
	SSL *cSSL;
	char username[32];
	char password[32];
	char *mcpk, *mcpv, *end_tag;
	unsigned long hash;
	int tfd;
	int flags;
	int esc_state, mcp, csi_changed;
	unsigned addr; 
	struct attr c_attr, csi;
        struct mcp_arg mcp_args[3];
        int mcp_args_l;
};

struct frame {
	char head[2];
	char mk[4];
	uint64_t pl;
	char data[BUFSIZ];
};

int __b64_ntop(unsigned char const *src, size_t srclength,
	       char *target, size_t targsize);

static struct ws wss[FD_SETSIZE];
static int cfds[FD_SETSIZE];
static int sfd = -1; 
static unsigned nt = 0;

static inline void
ws_handshake(struct ws *ws) {
	SSL *cSSL = ws->cSSL;
	static char const common_resp[]
		= "HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: upgrade\r\n"
		"Sec-Websocket-Protocol: text\r\n"
		"Sec-Websocket-Accept: ",
		kkey[] = "Sec-WebSocket-Key";
	unsigned char hash[SHA_DIGEST_LENGTH];
	unsigned char result[29];
	char buf[BUFSIZ];
	char *s;

	while (SSL_read(cSSL, buf, sizeof(buf)) > 0)
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
				ws->hash = * (unsigned long *) result;
				SSL_write(cSSL, "\r\n\r\n", 4);
				return;
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

	fcntl(tfd, F_SETFL, O_NONBLOCK);
	return tfd;
}

static inline void
mcp_auth(struct ws *ws)
{
	char buf[BUFSIZ];
	size_t len = snprintf(buf, sizeof(buf),
			     "#$#com-qnixsoft-web-auth %lu username: \"%s\" password: \"%s\"\r\n"
			      , ws->hash, ws->username, ws->password);
	write(ws->tfd, buf, len);
	SET_FLAGS(ws, AUTH);
}

static inline void
mcp_negotiate(struct ws *ws) {
	char buf[BUFSIZ];
	size_t n = snprintf
		(buf, sizeof(buf),
		 "#$#mcp authentication-key: %lu version: \"1.0\" to: \"2.1\"\r\n"
		 "#$#mcp-negotiate-can %lu package: \"mcp-negotiate\" min-version: \"1.0\" max-version: \"2.0\"\r\n"
		 "#$#mcp-negotiate-can %lu package: \"com-qnixsoft-web\" min-version: \"1.0\" max-version: \"1.0\"\r\n"
		 "#$#mcp-negotiate-can %lu package: \"org-fuzzball-notify\" min-version: \"1.0\" max-version: \"1.0\"\r\n"
		 "#$#mcp-negotiate-can %lu package: \"org-fuzzball-help\" min-version: \"1.0\" max-version: \"1.0\"\r\n"
		 "#$#mcp-negotiate-end %lu\r\n"
		 "#$#com-qnixsoft-web-identify %lu ip: \"%u\" old: \"%u\"\r\n"
		 , ws->hash, ws->hash, ws->hash, ws->hash, ws->hash, ws->hash, ws->hash,
		 ws->addr, !!GET_FLAG(ws, OLD));
	write(ws->tfd, buf, n);
	if (GET_FLAG(ws, AUTH))
		mcp_auth(ws);
}

static inline void
telnet_close(struct ws *ws, fd_set *fdset)
{
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
	SET_FLAGS(ws, CLOSING);
}

static inline void
ws_read(int cfd, fd_set *fdset)
{
	struct frame frame;
	struct ws *ws = &wss[cfd];
	SSL *cSSL = ws->cSSL;
	uint64_t pl;
	int i, n;

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
	if (n != pl)
		goto error;

	for (i = 0; i < pl; i++)
		frame.data[i] ^= frame.mk[i % 4];

	frame.data[i] = '\0';
	if (GET_FLAG(ws, AUTH)) {
		if (ws->tfd == -1)
			return;
		frame.data[i++] = '\n';
		write(ws->tfd, frame.data, i);
	} else {
		char *p = strchr(frame.data, ' ');
		if (!p)
			goto error;

		*p = '\0';
		strncpy(ws->username, frame.data, sizeof(ws->username));
		frame.data[frame.pl] = '\0';
		strncpy(ws->password, p + 1, sizeof(ws->password));
		SET_FLAGS(ws, AUTH);
		mcp_auth(ws);
	}

	return;
error:	ws_close_policy(cfd);
}

static inline int
telnet_open(int cfd, fd_set *fdset) {
	struct ws *ws = &wss[cfd];
	ws->tfd = telnet_connect();
	if (ws->tfd == -1)
		return -1;
	cfds[ws->tfd] = cfd;
	FD_SET(ws->tfd, fdset);
	nt++;
	mcp_negotiate(ws);
	SET_FLAGS(ws, OLD);
	return 0;
}

static inline int
ws_new(SSL_CTX *sslctx, fd_set *afdset)
{
	/* static const char noservice[] = "HTTP/1.1 503 Service Unavailable\r\nConnection: close\r\n\r\n"; */
	struct sockaddr_in cli; 
	SSL *cSSL;
	char *ip = NULL;
	int len = sizeof(struct sockaddr),
	    cfd = accept(sfd, (struct sockaddr *) &cli, &len);

	if (cfd <= 0)
		return 1;

	ip = inet_ntoa(cli.sin_addr);
	debug("WS_NEW %d %s", cfd, ip);

	cSSL = SSL_new(sslctx);
	SSL_set_fd(cSSL, cfd);

	if (SSL_accept(cSSL) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_shutdown(cSSL);
		SSL_free(cSSL);
		close(cfd);
		return 1;
	}

	struct ws *ws = &wss[cfd];
	ws->cSSL = cSSL;
	ws->csi.fg = ws->c_attr.fg = 7;
	ws->csi.bg = ws->c_attr.bg = 0;
	ws->csi.x = ws->c_attr.x = 0;
	ws->addr = cli.sin_addr.s_addr;
	ws->mcp = 1;
	ws->end_tag = "";
	UNSET_FLAGS(ws, MCP_CONTINUES);

	ws_handshake(ws);
	FD_SET(cfd, afdset);

	telnet_open(cfd, afdset);

	return 0;
}

static inline void
params_push(struct ws *ws, int x)
{
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

static inline int
ws_write(SSL *cSSL, char *data, size_t n)
{
	unsigned char head[2] = { 0x81, 0x00 };

	if (n < 126) {
		head[1] |= n;
		if (SSL_write(cSSL, head, sizeof(head)) < sizeof(head))
			return -1;
	} else if (n < (1 << 16)) {
		uint16_t nn = htons(n);
		head[1] |= 126;
		if (SSL_write(cSSL, head, sizeof(head)) < sizeof(head)
		    || SSL_write(cSSL, &nn, sizeof(nn)) < sizeof(nn))
			return -1;
	} else {
		uint64_t nn = htonl(n);
		head[1] |= 127;
		if (SSL_write(cSSL, head, sizeof(head)) < sizeof(head)
		    || SSL_write(cSSL, &nn, sizeof(nn)) < sizeof(nn))
			return -1;
	}

	return SSL_write(cSSL, data, n) < n;
}

static char data[BUFSIZ];
static ssize_t data_len;
static char html[HTMLSIZ];
static size_t html_len;

static inline void
end_tag_reset(struct ws *ws)
{
	WRITEF("%s", ws->end_tag);
	ws->end_tag = "";
}

static inline void
json_close(struct ws *ws)
{
	if (GET_FLAG(ws, MCP_OPEN)) {
                if (ws->mcp != MCP_ECHO_ML_VALUE && !GET_FLAG(ws, MCP_CONTINUES)
                    && !(GET_FLAG(ws, MCP_INBAND) && GET_FLAG(ws, MCP_VALUE))) {
                        html_len -= 6;
                }
		end_tag_reset(ws);
                WRITEF("\" }");
		SET_FLAGS(ws, MCP_CLOSED);
		UNSET_FLAGS(ws, MCP_OPEN | MCP_INBAND | MCP_VALUE);
	}
}

static inline void
json_open(struct ws *ws, const char *str)
{
	if (GET_FLAG(ws, MCP_ONCE)) {
		json_close(ws);
		html[html_len++] = ',';
	}
        WRITEF("{ \"key\": \"%s", str);
	UNSET_FLAGS(ws, MCP_CLOSED | MCP_VALUE);
	SET_FLAGS(ws, MCP_OPEN | MCP_ONCE);
}

static inline void
json_inband_open(struct ws *ws)
{
	json_open(ws, "inband\", \"value\": \"");
	SET_FLAGS(ws, MCP_VALUE | MCP_INBAND);
}

static inline void
mcp_handler(struct ws *ws) {
	char *buf = html;
	static const char end[] = "mcp-negotiate-end";
	/* static const char view[] = "com-qnixsoft-web-view"; */
	/* static const char art[] = "com-qnixsoft-web-art"; */
	/* static const char aerr[] = "com-qnixsoft-web-auth-error"; */
	if (!strncmp(buf, end, sizeof(end) - 1)) {
		if (GET_FLAG(ws, AUTH))
			mcp_auth(ws);
	/* } else if (!strncmp(buf, view, sizeof(view) - 1) */
	/* 	   || !strncmp(buf, art, sizeof(art) - 1) */
	/* 	   || !strncmp(buf, aerr, sizeof(aerr) - 1)) { */
		/* ws_write(ws->cSSL, buf, html_len - 1); */
	/* } else { */
	/* 	debug("oops unfiltered\n"); */
	}
	json_close(ws);
	/* debug("mcp handling '%s' '%s'", ws->mcpk, html); */
	ws->mcpk = NULL;
}

static inline void
csi_change(struct ws *ws)
{
	int a = ws->csi.fg != 7, b = ws->csi.bg != 0;
        WRITEF("%s", ws->end_tag);

	if (a || b) {
                WRITEF("<span class=\\\"");
		if (a)
			WRITEF("fg%d ", ws->csi.fg);
		if (b)
			WRITEF("bg%d", ws->csi.bg);

		WRITEF("\\\">");
		ws->end_tag = "</span>";
	} else
		ws->end_tag = "";
}

static inline void
esc_state_0(struct ws *ws, char *p) {
	switch (*p) {
	case '#':
		switch (ws->mcp) {
		case 3:
		case 1:
			ws->mcp++;
			return;
		}
		break;
	case '$':
		if (ws->mcp == 2) {
			ws->mcp++;
			return;
		}
		break;
	case '"':
                if (ws->mcp == MCP_SEEK_ARG_VALUE) {
                        ws->mcp = MCP_ECHO_ARG_VALUE;
                        return;
                }
                if (ws->mcp == MCP_ECHO_ARG_VALUE) {
			end_tag_reset(ws);
                        WRITEF("\", \"");
                        ws->mcp = MCP_SEEK_ARG_KEY;
                        return;
                }
	case '\\':
	case '\t':
		// chars that must be json escaped
		if (ws->mcp != MCP_ECHO_ML_VALUE)
			return;
	case '/':
		html[html_len++] = '\\';
		html[html_len++] = *p;
		return;
	case '*':
		switch (ws->mcp) {
		case MCP_CONFIRMED:
			ws->mcp = MCP_SEEK_ML_VALUE;
			return;
		case MCP_ECHO_ARG_KEY:
			SET_FLAGS(ws, MCP_CONTINUES | MCP_VALUE);
			ws->mcp = MCP_SKIP_LINE;
                        WRITEF("\": \"");
			return;
		}
		break;
	case '\n':
		if (GET_FLAG(ws, MCP_VALUE)) {
			html[html_len++] = '\\';
			html[html_len++] = 'n';
		} else if (!GET_FLAG(ws, MCP_INBAND)) {
			mcp_handler(ws);
		}
		ws->mcp = 1;
		return;
	case ':':
		switch (ws->mcp) {
                case MCP_ECHO_ARG_KEY:
                        ws->mcp = MCP_SEEK_ARG_VALUE;
                        WRITEF("\": \"");
                        return;
		case MCP_CONFIRMED:
			mcp_handler(ws);
			ws->mcp = MCP_SKIP_LINE;
			UNSET_FLAGS(ws, MCP_CONTINUES);
			return;
		case MCP_SEEK_ML_VALUE:
			ws->mcp = MCP_ECHO_ML_VALUE;
			return;
		}
		break;
	case ' ':
                switch (ws->mcp) {
                case MCP_SEEK_ARG_KEY:
                case MCP_SKIP_HASH:
                        ws->mcp = MCP_ECHO_ARG_KEY;
                        return;
                case MCP_ECHO_KEY:
                        if (ws->mcpk && !GET_FLAG(ws, MCP_CONTINUES)) {
                                SET_FLAGS(ws, MCP_VALUE);
                                ws->mcp = MCP_SKIP_HASH;
                                WRITEF("\", \"");
                                return;
                        }
                }
	}

	switch (ws->mcp) {
	case MCP_SKIP_HASH:
	case MCP_SKIP_LINE:
	case MCP_SEEK_ML_VALUE:
	case MCP_SEEK_ARG_VALUE:
	case MCP_SEEK_ARG_KEY:
                return;

	case MCP_ECHO_KEY:
        case MCP_ECHO_ARG_KEY:
        case MCP_ECHO_ARG_VALUE:
	case MCP_ECHO_ML_VALUE:
                break;

	case MCP_CONFIRMED: // new mcp command
		json_open(ws, "");
		ws->mcpk = &html[html_len];
                ws->mcp = MCP_ECHO_KEY;
		break;

	default: // mcp turned out impossible
		if (!GET_FLAG(ws, MCP_INBAND))
			json_inband_open(ws);
		ws->mcp--;
		strncpy(&html[html_len], "#$#", ws->mcp);
		html_len += ws->mcp;
                ws->mcp = MCP_ECHO_ML_VALUE;

	}

	if (ws->csi_changed) {
		csi_change(ws);
		ws->csi_changed = 0;
	}

	html[html_len++] = *p;
}

static inline void
esc_state_any(struct ws *ws, char *p, char **end_tag) {
	register char ch = *p;

	switch (ch) {
	case '\x18':
	case '\x1a':
		ws->esc_state = 0;
		return;
	case '\x1b':
		ws->esc_state = 1;
		return;
	case '\x9b':
		ws->esc_state = 2;
		return;
	case '\x07': 
	case '\x00':
	case '\x7f':
	case '\v':
	case '\r':
	case '\f':
		return;
	}

	switch (ws->esc_state) {
	case 0:
		esc_state_0(ws, p);
		break;
	case 1:
		switch (ch) {
		case '[':
			ws->esc_state = 2;
			break;
		case '=':
		case '>':
		case 'H':
			ws->esc_state = 0; /* IGNORED */
		}
		break;
	case 2: // just saw CSI
		switch (ch) {
		case 'K':
		case 'H':
		case 'J':
			ws->esc_state = 0;
			return;
		case '?':
			ws->esc_state = 5;
			return;
		}
		params_push(ws, 0);
		ws->esc_state = 3;
	case 3: // saw CSI and parameters
		switch (ch) {
		case 'm':
			if (ws->c_attr.bg != ws->csi.bg
			    || ws->c_attr.fg != ws->csi.fg)
			{
				ws->c_attr.fg = ws->csi.fg;
				ws->c_attr.bg = ws->csi.bg;
				ws->c_attr.x = 0;
				ws->csi.x = 0;
				ws->csi_changed = 1;
			}
			ws->esc_state = 0;
			break;
		case '[':
			ws->esc_state = 4;
			break;
		case ';':
			params_push(ws, 0);
			break;
		default:
			if (ch >= '0' && ch <= '9')
				params_push(ws, ws->csi.x * 10 + (ch - '0'));
			else
				ws->esc_state = 0;
		}
		break;

	case 5: params_push(ws, ch);
		ws->esc_state = 6;
		break;
	case 4:
	case 6: ws->esc_state = 0;
		break;

	default: CBUG(1);
	}
}

static void
telnet_parse(int tfd, fd_set *fdset)
{
	char *p, *end_tag = "";
	int cfd = cfds[tfd], retry = 0;
	struct ws *ws = &wss[cfd];
	html_len = 0;
	UNSET_FLAGS(ws, MCP_CLOSED | MCP_OPEN
                    | MCP_VALUE | MCP_INBAND | MCP_ONCE);

	html[html_len++] = '[';

again:	data_len = read(tfd, data, sizeof(data) - 1);

	if (data_len == 0) {
		telnet_close(ws, fdset);
		return;
	} else if (data_len < 0) {
		CBUG(errno != EAGAIN);
		if (GET_FLAG(ws, MCP_CONTINUES))
			goto again;
		retry ++;
		if (retry < 3) {
			struct timespec to = { .tv_sec = 0, .tv_nsec = 3000 };
			nanosleep(&to, NULL);
			goto again;
		}
		if (ws->mcpk)
			mcp_handler(ws);
		json_close(ws);
		html[html_len++] = ']';
#if 0
		html[html_len] = '\0';
		debug("OUT mcp %d '%s'", ws->mcp, html);
#endif
		ws_write(ws->cSSL, html, html_len);
		return;
	}

	for (p = data; html_len < HTMLSIZ && *p && p < data + data_len; p++)
		esc_state_any(ws, p, &end_tag);

	goto again;
}

int main() 
{ 
	struct sockaddr_in servaddr; 
	SSL_CTX *sslctx;
	fd_set rfdset, afdset;
        int tr = 1;

	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1
            || setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int))
	    || bind(sfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0
	    || listen(sfd, 5) != 0) {
		perror("...");
		return 1;
	}

        signal(SIGPIPE, SIG_IGN);
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

                        if (tfd > 0) {
                                for (i = 0; i < FD_SETSIZE; i++)

                                        if (wss[i].tfd == -1)
                                                telnet_open(i, &afdset);
                        } else
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
				telnet_parse(i, &afdset);
			else if (GET_FLAG(&wss[i], CLOSING))
				ws_close(i, &afdset);
			else
				ws_read(i, &afdset);
		}
	}

	close(sfd);
	return 0;
}

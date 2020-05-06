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
#include <ctype.h> 
#include "debug.h"

#define SECURE
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

#ifdef SECURE
#define WS_READ(cfd, to, len) \
	SSL_read(wss[cfd].cSSL, to, len)
#define WS_WRITE(cfd, from, len) \
	SSL_write(wss[cfd].cSSL, from, len)
#else
#define WS_READ(cfd, to, len) read(cfd, to, len)
#define WS_WRITE(cfd, from, len) write(cfd, from, len)
#endif

enum ws_flags {
	CLOSING = 1,
	AUTH = 2,
	OLD = 4,
};

struct attr { int fg, bg, x; };

struct ws {
	SSL *cSSL;
	char username[32];
	char password[32];
	char *mcpk, *mcpv;
	unsigned long hash;
	int cfd, tfd;
	int flags;
	unsigned addr; 
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
#ifdef SECURE
SSL_CTX *sslctx;
#endif

static inline void
ws_handshake(int cfd) {
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

	while (WS_READ(cfd, buf, sizeof(buf)) > 0)
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
				WS_WRITE(cfd, common_resp, sizeof(common_resp) - 1);
				WS_WRITE(cfd, result, sizeof(result) - 1);
				wss[cfd].hash = * (unsigned long *) result;
				WS_WRITE(cfd, "\r\n\r\n", 4);
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
#ifdef SECURE
	SSL_shutdown(ws->cSSL);
	SSL_free(ws->cSSL);
#endif
	close(cfd);
	memset(ws, 0, sizeof(struct ws));
}

static inline void
ws_close_policy(int cfd) {
	unsigned char head[2] = { 0x88, 0x02 };
	unsigned code = 1008;

	WS_WRITE(cfd, head, sizeof(head));
	WS_WRITE(cfd, &code, sizeof(code));
	SET_FLAGS(&wss[cfd], CLOSING);
}

static inline void
ws_read(int cfd, fd_set *fdset)
{
	struct frame frame;
	struct ws *ws = &wss[cfd];
	uint64_t pl;
	int i, n;

	n = WS_READ(cfd, frame.head, sizeof(frame.head));
	if (n != sizeof(frame.head))
		goto error;

	pl = PAYLOAD_LEN(frame.head);

	if (pl == 126) {
		uint16_t rpl;
		n = WS_READ(cfd, &rpl, sizeof(rpl));
		if (n != sizeof(rpl))
			goto error;
		pl = rpl;
	} else if (pl == 127) {
		uint64_t rpl;
		n = WS_READ(cfd, &rpl, sizeof(rpl));
		if (n != sizeof(rpl))
			goto error;
		pl = rpl;
	}

	frame.pl = pl;

	n = WS_READ(cfd, frame.mk, sizeof(frame.mk));
	if (n != sizeof(frame.mk))
		goto error;

	if (OPCODE(frame.head) == 8) {
		ws_close(cfd, fdset);
		return;
	}

	n = WS_READ(cfd, frame.data, pl);
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
ws_new(fd_set *afdset)
{
	/* static const char noservice[] = "HTTP/1.1 503 Service Unavailable\r\nConnection: close\r\n\r\n"; */
	struct sockaddr_in cli; 
	struct ws *ws;
	char *ip = NULL;
	int len = sizeof(struct sockaddr),
	    cfd = accept(sfd, (struct sockaddr *) &cli, &len);

	if (cfd <= 0)
		return 1;

	ip = inet_ntoa(cli.sin_addr);
	debug("WS_NEW %d %s", cfd, ip);

	ws = &wss[cfd];
	ws->cfd = cfd;

#ifdef SECURE
	ws->cSSL = SSL_new(sslctx);
	SSL_set_fd(ws->cSSL, cfd);

	if (SSL_accept(ws->cSSL) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_shutdown(ws->cSSL);
		SSL_free(ws->cSSL);
		close(cfd);
		return 1;
	}
#endif

	/* ws->csi.fg = ws->c_attr.fg = 7; */
	/* ws->csi.bg = ws->c_attr.bg = 0; */
	/* ws->csi.x = ws->c_attr.x = 0; */
	ws->addr = cli.sin_addr.s_addr;

	ws_handshake(cfd);
	FD_SET(cfd, afdset);

	telnet_open(cfd, afdset);

	return 0;
}

static inline int
ws_write(int cfd, char *data, size_t n)
{
	unsigned char head[2] = { 0x81, 0x00 };

	if (n < 126) {
		head[1] |= n;
		if (WS_WRITE(cfd, head, sizeof(head)) < sizeof(head))
			return -1;
	} else if (n < (1 << 16)) {
		uint16_t nn = htons(n);
		head[1] |= 126;
		if (WS_WRITE(cfd, head, sizeof(head)) < sizeof(head)
		    || WS_WRITE(cfd, &nn, sizeof(nn)) < sizeof(nn))
			return -1;
	} else {
		uint64_t nn = htonl(n);
		head[1] |= 127;
		if (WS_WRITE(cfd, head, sizeof(head)) < sizeof(head)
		    || WS_WRITE(cfd, &nn, sizeof(nn)) < sizeof(nn))
			return -1;
	}

	return WS_WRITE(cfd, data, n) < n;
}

static char data[BUFSIZ];
static ssize_t data_len;
static char html[HTMLSIZ];
static size_t html_len;

static void
telnet_parse(int tfd, fd_set *fdset)
{
	char *p;
	int cfd = cfds[tfd];
	struct ws *ws = &wss[cfd];
	html_len = 0;
again:	data_len = read(tfd, data, sizeof(data) - 1);

	if (data_len == 0) {
		telnet_close(ws, fdset);
		return;
	} else if (data_len < 0) {
		ws_write(cfd, html, html_len);
		return;
	}

	for (p = data; html_len < HTMLSIZ && *p && p < data + data_len; p++)
                WRITEF("%c", *p);

	goto again;
}

int main() 
{ 
	struct sockaddr_in servaddr; 
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
#ifdef SECURE
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	sslctx = SSL_CTX_new(TLS_server_method());
	SSL_CTX_set_ecdh_auto(sslctx, 1);
	SSL_CTX_use_certificate_file(sslctx, "/etc/ssl/0.0.0.0:443.crt" , SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(sslctx, "/etc/ssl/private/0.0.0.0:443.key", SSL_FILETYPE_PEM);
	ERR_print_errors_fp(stderr);
#endif
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
				ws_new(&afdset);
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

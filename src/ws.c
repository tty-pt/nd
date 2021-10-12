#include "externs.h"
#include "ws.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
/* #include <arpa/inet.h> */
#include <sys/select.h>
#include <openssl/sha.h>

/* #include <openssl/bio.h> */
/* #include <openssl/ssl.h> */
/* #include <openssl/err.h> */
/* #include <sys/socket.h> */

#ifdef CONFIG_SECURE
#define WS_READ(cfd, to, len) \
	SSL_read(wss[cfd].cSSL, to, len)
#define WS_WRITE(cfd, from, len) \
	SSL_write(wss[cfd].cSSL, from, len)
#else
#define WS_READ(cfd, to, len) read(cfd, to, len)
#define WS_WRITE(cfd, from, len) write(cfd, from, len)
#endif

#define OPCODE(head) ((unsigned char) (head[0] & 0x0f))
#define PAYLOAD_LEN(head) ((unsigned char) (head[1] & 0x7f))

/* int __b64_ntop(unsigned char const *src, size_t srclength, */
/* 	       char *target, size_t targsize); */

struct ws_frame {
	char head[2];
	char mk[4];
	uint64_t pl;
	char data[BUFSIZ];
};

struct ws {
	/* SSL *cSSL; */
	/* char username[32]; */
	/* char password[32]; */
	/* char *mcpk, *mcpv; */
	unsigned long hash;
	/* int cfd, tfd; */
	int flags;
	unsigned addr;
};

static struct ws wss[FD_SETSIZE];

// https://github.com/yasuoka/base64/blob/master/b64_ntop.c

int
b64_ntop(u_char *src, size_t srclength, char *target, size_t target_size)
{
  int		 i, j, expect_siz;
  uint32_t	 bit24;
  const char	 b64str[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  expect_siz = ((srclength + 2) / 3) * 4 + 1;

  if (target == NULL)
    return (expect_siz);
  if (target_size < expect_siz)
    return (-1);

  for (i = 0, j = 0; i < srclength; i += 3) {
    bit24 = src[i] << 16;
    if (i + 1 < srclength)
      bit24 |= src[i + 1] << 8;
    if (i + 2 < srclength)
      bit24 |= src[i + 2];

    target[j++] = b64str[(bit24 & 0xfc0000) >> 18];
    target[j++] = b64str[(bit24 & 0x03f000) >> 12];
    if (i + 1 < srclength)
      target[j++] = b64str[(bit24 & 0x000fc0) >> 6];
    else
      target[j++] = '=';
    if (i + 2 < srclength)
      target[j++] = b64str[(bit24 & 0x00003f)];
    else
      target[j++] = '=';
  }
  target[j] = '\0';

  return j;
}

int
ws_handshake(int cfd, char *buf) {
	static char const common_resp[]
		= "HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: upgrade\r\n"
		"Sec-Websocket-Protocol: text\r\n"
		"Sec-Websocket-Accept: ",
		kkey[] = "Sec-WebSocket-Key";
	unsigned char hash[SHA_DIGEST_LENGTH];
	char result[29];
	/* char buf[BUFSIZ]; */
	char *s;

	warn("ws_handshake %s", buf);
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
                        b64_ntop(hash, sizeof(hash), result, sizeof(result));
                        WS_WRITE(cfd, common_resp, sizeof(common_resp) - 1);
                        WS_WRITE(cfd, result, sizeof(result) - 1);
                        wss[cfd].hash = * (unsigned long *) result;
                        WS_WRITE(cfd, "\r\n\r\n", 4);
                        return 0;
                }

        return 1;
}

int
ws_write(int cfd, void *data, size_t n)
{
	unsigned char head[2] = { 0x81, 0x00 };

        /* warn("ws_write %lu %s\n", n, data); */

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

static inline void
ws_close_policy(int cfd) {
	unsigned char head[2] = { 0x88, 0x02 };
	unsigned code = 1008;

	WS_WRITE(cfd, head, sizeof(head));
	WS_WRITE(cfd, &code, sizeof(code));
	/* SET_FLAGS(&wss[cfd], CLOSING); */
}

int
ws_read(int cfd, char *data, size_t len)
{
	struct ws_frame frame;
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

	if (OPCODE(frame.head) == 8)
		return 0;

	n = WS_READ(cfd, frame.data, pl);
	if (n != pl)
		goto error;

	for (i = 0; i < pl; i++)
		frame.data[i] ^= frame.mk[i % 4];

	frame.data[i] = '\0';
        memcpy(data, frame.data, i + 1);
	return pl;

error:	ws_close_policy(cfd);
        return -1;
}

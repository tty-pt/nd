#ifndef DESCR_H
#define DESCR_H

#include "mdb.h"
#include "mcp.h"

#define CONFIG_SECURE

#ifdef CONFIG_SECURE
#include <openssl/bio.h>
#include <openssl/ssl.h>

#define READ(cfd, to, len) \
	SSL_read(descr_map[cfd].cSSL, to, len)
#define WRITE(cfd, from, len) \
	SSL_write(descr_map[cfd].cSSL, from, len)
#else
#define READ(cfd, to, len) read(cfd, to, len)
#define WRITE(cfd, from, len) write(cfd, from, len)
#endif

typedef struct descr_st {
#ifdef CONFIG_SECURE
	SSL *cSSL;
#endif
	int fd, flags;
	dbref player;
	McpFrame mcpframe;
} descr_t;

extern descr_t descr_map[];

#endif

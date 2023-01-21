#ifndef IO_H
#define IO_H

#include "config.h"
#include "object.h"
#include "mcp.h"

#define DESCR(fd) (&descr_map[fd])

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
	OBJ *player;
	McpFrame mcpframe;
} descr_t;

extern descr_t descr_map[];

int descr_inband(int fd, const char *s);
void descr_close(descr_t *d);
int notify(ENT *eplayer, const char *msg);
void notifyf(ENT *eplayer, char *format, ...);
void anotifyf(OBJ *room, char *format, ...);
void onotifyf(OBJ *player, char *format, ...);
void notify_wts(OBJ *who, char const *a, char const *b, char *format, ...);
void notify_wts_to(OBJ *who, OBJ *tar, char const *a, char const *b, char *format, ...);
void notify_attack(OBJ *player, OBJ *target, struct wts wts, short val, char const *color, short mval);
void wall(const char *msg);

#endif

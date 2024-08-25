#ifndef IO_H
#define IO_H

#include <ndc.h>
#include "config.h"
#include "object.h"
#include "mcp.h"

typedef struct pfd {
	int fd;
	dbref player;
	McpFrame mcpframe;
} descr_t;

#define FD_PLAYER(fd) descr_map[fd].player
#define FD_MCP(fd) descr_map[fd].mcpframe

extern struct pfd descr_map[];

struct hash_cursor fds_iter(dbref player);
int fds_next(struct hash_cursor *c);
int fds_has(dbref player);
void nd_close(dbref player);
void nd_write(dbref player_ref, char *str, size_t len);
void nd_dwritef(dbref player_ref, const char *fmt, va_list args);
void nd_writef(dbref player_ref, const char *fmt, ...);
void nd_rwrite(dbref room_ref, dbref exception_ref, char *str, size_t len);
void nd_owritef(dbref player_ref, char *format, ...);
void notify_wts(dbref player_ref, char const *a, char const *b, char *format, ...);
void notify_wts_to(dbref player_ref, dbref target_ref, char const *a, char const *b, char *format, ...);
void notify_attack(dbref player_ref, dbref target_ref, char *wts, short val, char const *color, short mval);

#endif

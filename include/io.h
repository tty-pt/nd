#ifndef IO_H
#define IO_H

#include <ndc.h>
#include "config.h"
#include "object.h"
#include "mcp.h"

typedef struct pfd {
	int fd;
	OBJ *player;
	McpFrame mcpframe;
} descr_t;

#define FD_PLAYER(fd) descr_map[fd].player
#define FD_MCP(fd) descr_map[fd].mcpframe

extern struct pfd descr_map[];

void nd_rwritef(OBJ *room, OBJ *exception, char *format, ...);
void notify_wts(OBJ *who, char const *a, char const *b, char *format, ...);
void notify_wts_to(OBJ *who, OBJ *tar, char const *a, char const *b, char *format, ...);
void notify_attack(OBJ *player, OBJ *target, struct wts wts, short val, char const *color, short mval);

#endif

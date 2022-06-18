
/* $Header$ */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "copyright.h"

#include "mdb.h"
#include "mcp.h"

/* these symbols must be defined by the interface */
extern void wall_wizards(const char *msg);
extern int boot_off(OBJ *player);
extern void boot_player_off(OBJ *player);

extern McpFrame *descr_mcpframe(int c);

/* the following symbols are provided by game.c */

OBJ *create_player(const char *name);
OBJ *connect_player(const char *name);

extern int init_game();

#endif /* _INTERFACE_H */


/* $Header$ */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "copyright.h"

#include "mdb.h"
#include "mcp.h"
#include "command.h"

/* these symbols must be defined by the interface */
extern void wall_wizards(const char *msg);
extern int boot_off(dbref player);
extern void boot_player_off(dbref player);

extern McpFrame *descr_mcpframe(int c);

/* the following symbols are provided by game.c */

extern dbref create_player(const char *name);
extern dbref connect_player(const char *name);

extern int init_game();
extern void panic(const char *);

#endif /* _INTERFACE_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef interfaceh_version
#define interfaceh_version
const char *interface_h_version = "$RCSfile$ $Revision: 1.12 $";
#endif
#else
extern const char *interface_h_version;
#endif

/* extern void art_print(dbref player, dbref what); */
void art(int descr, const char *art_str);

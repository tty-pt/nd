
/* $Header$ */

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "copyright.h"

#include "db.h"
#include "command.h"

/* these symbols must be defined by the interface */
extern int notify(dbref player, const char *msg);
extern void wall(const char *msg);
extern void wall_wizards(const char *msg);
extern int shutdown_flag;		/* if non-zero, interface should shut down */
extern int restart_flag;		/* if non-zero, should restart after shut down */
extern void emergency_shutdown(void);
extern int boot_off(dbref player);
extern void boot_player_off(dbref player);
extern int* get_player_descrs(dbref player, int*count);
extern int least_idle_player_descr(dbref who);
extern int most_idle_player_descr(dbref who);
extern int dbref_first_descr(dbref c);
extern dbref partial_pmatch(const char *name);

/* the following symbols are provided by game.c */

extern void command_process(command_t *cmd);

extern dbref create_player(const char *name, const char *password);
extern dbref connect_player(const char *name, const char *password);

extern int init_game();
extern void dump_database(void);
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

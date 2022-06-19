/* $Header$ */
#ifndef _EXTERNS_H
#define _EXTERNS_H
#include "copyright.h"
#include <sys/types.h>
#include "object.h"

#include "interface.h"

/* interface.c */
enum opts {
	OPT_DETACH = 1,
};

extern short optflags;

/* from timequeue.c */
void do_pose(command_t *);
void do_score(command_t *);
void do_view(command_t *);
void do_meme(command_t *);

/* From create.c */
extern void do_create(command_t *);
extern void do_clone(command_t *);

/* From help.c */
extern void spit_file(OBJ *player, const char *filename);
extern void do_help(command_t *);
extern void do_man(command_t *);
extern void do_motd(command_t *);
extern void do_info(command_t *);

/* From look.c */
extern void do_look_at(command_t *cmd);
extern void do_examine(command_t *cmd);
extern void do_inventory(command_t *cmd);
extern void do_owned(command_t *);
extern void do_contents(command_t *);

/* from item.c */
extern void do_select(command_t *);
extern void do_equip(command_t *);
extern void do_unequip(command_t *);
extern void do_consume(command_t *);
extern void do_fill(command_t *);

/* from shop.c */
extern void do_shop(command_t *);
extern void do_buy(command_t *);
extern void do_sell(command_t *);

/* from kill.c */
extern void do_kill(command_t *);
extern void do_advitam(command_t *cmd);
extern void do_heal(command_t *);
extern void do_status(command_t *);
extern void do_train(command_t *);
extern void do_sit(command_t *);
extern void do_stand(command_t *);
extern int kill_v(OBJ *player, const char *cmdstr);

/* From move.c */
extern void do_get(command_t *);
extern void do_drop(command_t *);
extern void do_recycle(command_t *);

/* From rob.c */
extern void do_give(command_t *);

/* From set.c */
extern void do_name(command_t *);
extern void do_chown(command_t *);

/* From speech.c */
extern void do_wall(command_t *cmd);
extern void do_say(command_t *cmd);

/* From wiz.c */
extern void do_teleport(command_t *);
extern void do_toad(command_t *);
extern void do_boot(command_t *);
extern void do_usage(command_t *);

#endif /* _EXTERNS_H */

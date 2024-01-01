/* $Header$ */
#ifndef _EXTERNS_H
#define _EXTERNS_H
#include "copyright.h"
#include <sys/types.h>
#include "object.h"

/* from entity.c */
void do_reroll(int fd, int argc, char *argv[]);

/* from timequeue.c */
void do_pose(int fd, int argc, char *argv[]);
void do_score(int fd, int argc, char *argv[]);
void do_view(int fd, int argc, char *argv[]);

/* From create.c */
extern void do_create(int fd, int argc, char *argv[]);
extern void do_clone(int fd, int argc, char *argv[]);

/* From help.c */
extern void spit_file(OBJ *player, const char *filename);
extern void do_man(int fd, int argc, char *argv[]);
extern void do_motd(int fd, int argc, char *argv[]);

/* From look.c */
extern void do_look_at(int fd, int argc, char *argv[]);
extern void do_examine(int fd, int argc, char *argv[]);
extern void do_inventory(int fd, int argc, char *argv[]);
extern void do_owned(int fd, int argc, char *argv[]);
extern void do_contents(int fd, int argc, char *argv[]);

/* from item.c */
extern void do_select(int fd, int argc, char *argv[]);
extern void do_equip(int fd, int argc, char *argv[]);
extern void do_unequip(int fd, int argc, char *argv[]);
extern void do_consume(int fd, int argc, char *argv[]);
extern void do_fill(int fd, int argc, char *argv[]);

/* from shop.c */
extern void do_shop(int fd, int argc, char *argv[]);
extern void do_buy(int fd, int argc, char *argv[]);
extern void do_sell(int fd, int argc, char *argv[]);

/* from kill.c */
extern void do_kill(int fd, int argc, char *argv[]);
extern void do_advitam(int fd, int argc, char *argv[]);
extern void do_heal(int fd, int argc, char *argv[]);
extern void do_status(int fd, int argc, char *argv[]);
extern void do_train(int fd, int argc, char *argv[]);
extern void do_sit(int fd, int argc, char *argv[]);
extern void do_stand(int fd, int argc, char *argv[]);
extern int kill_v(OBJ *player, const char *cmdstr);

/* From move.c */
extern void do_get(int fd, int argc, char *argv[]);
extern void do_drop(int fd, int argc, char *argv[]);
extern void do_recycle(int fd, int argc, char *argv[]);

/* From rob.c */
extern void do_give(int fd, int argc, char *argv[]);

/* From set.c */
extern void do_name(int fd, int argc, char *argv[]);
extern void do_chown(int fd, int argc, char *argv[]);

/* From speech.c */
extern void do_wall(int fd, int argc, char *argv[]);
extern void do_say(int fd, int argc, char *argv[]);

/* From wiz.c */
extern void do_teleport(int fd, int argc, char *argv[]);
extern void do_toad(int fd, int argc, char *argv[]);
extern void do_boot(int fd, int argc, char *argv[]);
extern void do_usage(int fd, int argc, char *argv[]);

/* extern void do_gpt(int fd, int argc, char *argv[]); */

#endif /* _EXTERNS_H */

/* $Header$ */
#ifndef _EXTERNS_H
#define _EXTERNS_H
#include "copyright.h"

/* Definition of pid_t */
#include <sys/types.h>

/* Definition of 'dbref' */
#include "mdb.h"
/* Definition of 'McpFrame' */
#include "mcp.h"
#include "props.h"
/* Definition of match_data */
#include "match.h"
/* Auto-generated list of extern functions */
#include "interface.h"

#define warn(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)

#include "debug.h"

/* interface.c */
enum opts {
	OPT_NOSANITY = 1,
	OPT_SANITY_INTERACTIVE = 2,
	OPT_SANITY_AUTOFIX = 4,
	OPT_DETACH = 8,
	OPT_WIZONLY = 16,
};
extern short optflags;

/* from timequeue.c */
void do_propset(command_t *);
void do_sweep(command_t *);
void do_pose(command_t *);
void do_score(command_t *);
void do_view(command_t *);
void do_meme(command_t *);

/* from db.c */
extern int number(const char *s);
extern int ifloat(const char *s);
extern void putproperties(FILE * f, int obj);
extern void getproperties(FILE * f, int obj, const char *pdir);
extern void db_free_object(dbref i);
extern void db_clear_object(dbref i);

/* From create.c */
extern void do_create(command_t *);
extern void do_clone(command_t *);
extern void copy_one_prop(dbref player, dbref source, dbref destination, char *propname, int ignore);

/* From hashtab.c */
extern unsigned int hash(const char *s, unsigned int hash_size);
extern hash_data *find_hash(const char *s, hash_tab * table, unsigned size);
extern hash_entry *add_hash(const char *name, hash_data data, hash_tab * table, unsigned size);
extern int free_hash(const char *name, hash_tab * table, unsigned size);
extern void kill_hash(hash_tab * table, unsigned size, int freeptrs);

/* From help.c */
extern void spit_file(dbref player, const char *filename);
extern void do_help(command_t *);
extern void do_news(command_t *);
extern void do_man(command_t *);
extern void do_motd(command_t *);
extern void do_info(command_t *);

/* From look.c */
extern void look_room(dbref player, dbref room);
extern long size_object(dbref i, int load);


/* extern void look_room_simple(dbref player, dbref room); */
extern void look_around(dbref player);
extern void do_look_at(command_t *cmd);
extern void do_examine(command_t *cmd);
extern void do_inventory(command_t *cmd);
extern void do_find(command_t *);
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
extern int p_vendor(dbref obj);

/* from kill.c */
extern void do_kill(command_t *);
extern void do_advitam(command_t *cmd);
extern void do_heal(command_t *);
extern void do_status(command_t *);
extern void do_train(command_t *);
extern void do_sit(command_t *);
extern void do_stand(command_t *);
extern int kill_v(dbref player, const char *cmdstr);
int do_stand_silent(dbref player);

/* From move.c */
extern void moveto(dbref what, dbref where);
extern void enter_room(dbref player, dbref loc);
extern int parent_loop_check(dbref source, dbref dest);
extern void go_move(dbref player, const char *dir);
extern void do_get(command_t *);
extern void do_drop(command_t *);
extern void do_recycle(command_t *);
extern void recycle(dbref player, dbref thing);

/* From player.c */
extern dbref lookup_player(const char *name);
extern void do_password(command_t *);
extern void add_player(dbref who);
extern void delete_player(dbref who);
extern void clear_players(void);
extern void do_talk(command_t *);
extern void do_answer(command_t *);
extern int dialog_exists(dbref what);
extern void dialog_stop(dbref player);

/* From predicates.c */
extern int can_doit(dbref player, dbref thing, const char *default_fail_msg);
extern int controls(dbref who, dbref what);
extern int payfor(dbref who, int cost);
extern int ok_ascii_any(const char *name);
extern int ok_name(const char *name);

/* From rob.c */
extern void do_give(command_t *);
extern void do_rob(command_t *);

/* From set.c */
extern void do_name(command_t *);
extern void do_unlink(command_t *);
extern void do_unlink_quiet(command_t *cmd, const char *name);
extern void do_chown(command_t *);

/* From speech.c */
extern void do_wall(command_t *cmd);
extern void do_say(command_t *cmd);
extern void notify_wts(dbref who, char const *a, char const *b, char *format, ...);
extern void notify_wts_to(dbref who, dbref tar, char const *a, char const *b, char *format, ...);

/* From stringutil.c */
extern int string_prefix(const char *string, const char *prefix);
extern char *intostr(int i);
extern int prepend_string(char** before, char* start, const char* what);
extern int is_prop_prefix(const char* Property, const char* Prefix);

/* From wiz.c */
extern void do_teleport(command_t *);
extern void do_force(command_t *);
extern void do_toad(command_t *);
extern void do_boot(command_t *);
extern void do_pcreate(command_t *);
extern void do_usage(command_t *);
extern void do_bless(command_t *);
extern void do_unbless(command_t *);

/* From unparse.c */
struct icon {
        int actions;
        char *icon;
};

extern struct icon icon(dbref what);
extern const char *unparse_object(dbref player, dbref object);

/* From smatch.c */
extern int equalstr(char *s, char *t);

/* from interface.c */
int notify(dbref player, const char *msg);
void notifyf(dbref player, char *format, ...);
void onotifyf(dbref player, char *format, ...);
void anotifyf(dbref room, char *format, ...);

/* from property.c */
char * displayprop(dbref player, dbref obj, const char *name, char *buf, size_t bufsiz);
long size_properties(dbref player, int load);
void untouchprops_incremental(int limit);

/* from props.c */
void clear_propnode(PropPtr p);
void copy_proplist(dbref obj, PropPtr * newer, PropPtr old);
long size_proplist(PropPtr avl);

/* from sanity.c */
void sanechange(dbref player, const char *command);
void sanfix(dbref player);
void sanity(dbref player);

#endif /* _EXTERNS_H */

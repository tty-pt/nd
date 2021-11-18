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
#include "externs-auto.h"
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

/* Prototypes for externs not defined elsewhere */

extern char match_args[];
extern char match_cmdname[];

/* from timequeue.c */
extern void propqueue(command_t *cmd, dbref where, dbref trigger, dbref what,
					  dbref xclude, const char *propname, const char *toparg,

					  int mlev, int mt);
extern void envpropqueue(command_t *cmd, dbref where, dbref trigger, dbref what,
						 dbref xclude, const char *propname, const char *toparg,

						 int mlev, int mt);
void do_newpassword(command_t *);
void do_oecho(command_t *);
void do_propset(command_t *);
void do_sweep(command_t *);
void do_leave(command_t *);
void do_whisper(command_t *);
void do_pecho(command_t *);
void do_pose(command_t *);
void do_score(command_t *);
void do_view(command_t *);
void do_meme(command_t *);
void init_match_remote(command_t *cmd, dbref what, const char *name, int type, struct match_data *md);
void send_contents(command_t *cmd, dbref dest);
void do_doing(command_t *cmd);

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
extern void do_attach(command_t *cmd);
extern int unset_source(dbref player, dbref loc, dbref action);
extern int link_exit(command_t *cmd, dbref exit, char *dest_name, dbref * dest_list);
extern int link_exit_dry(command_t *cmd, dbref exit, char *dest_name, dbref * dest_list);
extern void do_action(command_t *cmd);

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
extern void look_room(command_t *cmd, dbref room, int verbose);
extern long size_object(dbref i, int load);


/* extern void look_room_simple(dbref player, dbref room); */
extern void do_look_around(command_t *cmd);
extern void do_look_at(command_t *cmd);
extern void do_examine(command_t *cmd);
extern void do_inventory(command_t *cmd);
extern void do_find(command_t *);
extern void do_owned(command_t *);
extern void do_trace(command_t *);
extern void do_entrances(command_t *);
extern void do_contents(command_t *);

/* from item.c */
extern void do_select(command_t *);
extern void do_equip(command_t *);
extern void do_unequip(command_t *);
extern void do_drink(command_t *);
extern void do_eat(command_t *);
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
extern int kill_v(command_t *cmd, const char *cmdstr);

int do_stand_silent(dbref player);
extern void do_sit(command_t *);
extern void do_stand(command_t *);

/* From move.c */
extern void moveto(dbref what, dbref where);
extern void enter_room(command_t *cmd, dbref loc, dbref exit);
extern void send_home(command_t *cmd, int homepuppet);
extern int parent_loop_check(dbref source, dbref dest);
extern int can_move(command_t *cmd, const char *direction, int lev);
extern void go_move(command_t *, const char *dir, int lev);
extern void do_move(command_t *);
extern void do_get(command_t *);
extern void do_drop(command_t *);
extern void do_recycle(command_t *);
extern void recycle(command_t *, dbref thing);

/* From player.c */
extern dbref lookup_player(const char *name);
extern void do_password(command_t *);
extern void add_player(dbref who);
extern void delete_player(dbref who);
extern void clear_players(void);
extern void set_password_raw(dbref player, const char*password);
extern void set_password(dbref player, const char*password);
extern int check_password(dbref player, const char*password);
extern void do_talk(command_t *);
extern void do_answer(command_t *);
extern int dialog_exists(dbref what);

/* From predicates.c */
extern int OkObj(dbref obj);
extern int can_link_to(dbref who, object_flag_type what_type, dbref where);
extern int can_link(dbref who, dbref what);
extern int could_doit(command_t *cmd, dbref thing);
extern int can_doit(command_t *cmd, dbref thing, const char *default_fail_msg);
extern int can_see(dbref player, dbref thing, int can_see_location);
extern int controls(dbref who, dbref what);
extern int controls_link(dbref who, dbref what);
extern int restricted(dbref player, dbref thing, object_flag_type flag);
extern int payfor(dbref who, int cost);
extern int ok_ascii_any(const char *name);
extern int ok_name(const char *name);

/* From rob.c */
extern void do_give(command_t *);
extern void do_rob(command_t *);

/* From set.c */
extern void do_name(command_t *);
extern void do_describe(command_t *);
extern void do_idescribe(command_t *);
extern void do_fail(command_t *);
extern void do_success(command_t *);
extern void do_drop_message(command_t *);
extern void do_osuccess(command_t *);
extern void do_ofail(command_t *);
extern void do_odrop(command_t *);
extern int setlockstr(command_t *, dbref thing, const char *keyname);
extern void do_lock(command_t *);
extern void do_unlock(command_t *);
extern void do_unlink(command_t *);
extern void do_unlink_quiet(command_t *cmd, const char *name);
extern void do_chown(command_t *);
extern void do_set(command_t *);
extern void do_chlock(command_t *);
extern void do_conlock(command_t *);
extern void set_flags_from_tunestr(dbref obj, const char* flags);

/* From speech.c */
#define ONOTIFYF(who, ...) notify_except_fmt(DBFETCH(getloc(who))->contents, who, __VA_ARGS__)
extern void do_wall(command_t *cmd);
extern void do_gripe(command_t *cmd);
extern void do_say(command_t *cmd);
extern void do_page(command_t *cmd);
extern void notify_except(dbref first, dbref exception, const char *msg, dbref who);
extern void notify_except_fmt(dbref first, dbref exception, char *format, ...);
extern void notify_wts(dbref who, char const *a, char const *b, char *format, ...);
extern void notify_wts_to(dbref who, dbref tar, char const *a, char const *b, char *format, ...);
extern void parse_oprop(command_t *cmd, dbref dest, dbref exit, const char *propname, const char *prefix, const char *whatcalled);
extern void parse_omessage(command_t *cmd, dbref dest, dbref exit, const char *msg, const char *prefix, const char *whatcalled, int mpiflags) ;

/* From stringutil.c */
extern int string_prefix(const char *string, const char *prefix);
extern const char *string_match(const char *src, const char *sub);
extern char *intostr(int i);
extern int prepend_string(char** before, char* start, const char* what);
extern void prefix_message(char* Dest, const char* Src, const char* Prefix, int BufferLength, int SuppressIfPresent);
extern int is_prop_prefix(const char* Property, const char* Prefix);
extern int has_suffix(const char* text, const char* suffix);
extern int has_suffix_char(const char* text, char suffix);

/* From utils.c */
extern int member(dbref thing, dbref list);
extern dbref remove_first(dbref first, dbref what);

/* From wiz.c */
extern void do_teleport(command_t *);
extern void do_force(command_t *);
extern void do_stats(command_t *);
extern void do_toad(command_t *);
extern void do_boot(command_t *);
extern void do_pcreate(command_t *);
extern void do_usage(command_t *);
extern void do_bless(command_t *);
extern void do_unbless(command_t *);

/* From boolexp.c */
extern int eval_boolexp(command_t *cmd, struct boolexp *b, dbref thing);
extern struct boolexp *parse_boolexp(command_t *cmd, const char *string, int dbloadp);
extern struct boolexp *copy_bool(struct boolexp *old);
extern struct boolexp *getboolexp(FILE * f);
extern struct boolexp *negate_boolexp(struct boolexp *b);
extern void free_boolexp(struct boolexp *b);

/* From unparse.c */
struct icon {
        int actions;
        char *icon;
};

extern struct icon icon(dbref what);
extern const char *unparse_object(dbref player, dbref object);
extern const char *unparse_boolexp(dbref player, struct boolexp *b, int fullname);

/* From smatch.c */
extern int equalstr(char *s, char *t);

/* from interface.c */
void do_flock(command_t *);
int mcpframe_to_user(McpFrame * ptr);
int mcpframe_to_descr(McpFrame * ptr);

/* from tune.c */
extern void tune_load_parmsfile(dbref player);

#endif /* _EXTERNS_H */

#ifdef DEFINE_HEADER_VERSIONS
#ifndef externsh_version
#define externsh_version
const char *externs_h_version = "$RCSfile$ $Revision: 1.48 $";
#endif
#else
extern const char *externs_h_version;
#endif


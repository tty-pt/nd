#ifndef _EXTERNS_AUTO_H
#define _EXTERNS_AUTO_H

void add_property(dbref player, const char *pname, const char *strval, int value);
int can_move(command_t *cmd, const char *direction, int lev);
void clear_propnode(PropPtr p);
void copy_proplist(dbref obj, PropPtr * newer, PropPtr old);
char * displayprop(dbref player, dbref obj, const char *name, char *buf, size_t bufsiz);
int exit_loop_check(dbref source, dbref dest);
const char * exit_prefix(register const char *string, register const char *prefix);
dbref getparent(dbref obj);
void listenqueue(command_t *cmd, dbref where, dbref trigger, dbref what, dbref xclude, const char *propname, const char *toparg, int mlev, int mt, int mpi_p);
int notify(dbref player, const char *msg);
void notifyf(dbref player, char *format, ...);
void notify_except(dbref first, dbref exception, const char *msg, dbref who);
int ok_password(const char *password);
int ok_player_name(const char *name);
void san_main(void);
void sanechange(dbref player, const char *command);
void sanfix(dbref player);
void sanity(dbref player);
void set_signals(void);
void set_source(dbref player, dbref action, dbref source);
long size_boolexp(struct boolexp *b);
long size_properties(dbref player, int load);
long size_proplist(PropPtr avl);
void spit_file(dbref player, const char *filename);
int string_compare(const char *s1, const char *s2);
int string_prefix(const char *string, const char *prefix);
char * strip_ansi(char *buf, const char *input);
char * strip_bad_ansi(char *buf, const char *input);
int test_lock(command_t *cmd, dbref thing, const char *lockprop);
int test_lock_false_default(command_t *cmd, dbref thing, const char *lockprop);
void tune_freeparms(void);
const char * unparse_flags(dbref thing);
void untouchprops_incremental(int limit);

#endif /* _EXTERNS_AUTO_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef externs_autoh_version
#define externs_autoh_version
const char *externs_auto_h_version = "$RCSfile$ $Revision: 1.15 $";
#endif
#else
extern const char *externs_auto_h_version;
#endif


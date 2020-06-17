/* $Header$ */

#include "copyright.h"
#include "config.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

#include <sys/wait.h>

#include "db.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "fbstrings.h"
#include "debug.h"
#include "geography.h"
#include "kill.h"
#include "view.h"
#include "web.h"
#include "search.h"
#include "mob.h"

/* declarations */
static int epoch = 0;
time_t last_monolithic_time = 0;
static int forked_dump_process_flag = 0;
FILE *input_file;

extern void do_showextver(dbref player);
void fork_and_dump(void);
void dump_database(void);

void
do_dump(dbref player, const char *newfile)
{
	char buf[BUFFER_LEN];

	if (Wizard(player)) {
		if (global_dumper_pid != 0) {
			notify(player, "Sorry, there is already a dump currently in progress.");
			return;
		}
		if (*newfile
#ifdef GOD_PRIV
			&& God(player)
#endif							/* GOD_PRIV */
				) {
			snprintf(buf, sizeof(buf), "Dumping to file %s...", STD_DB);
		} else {
			snprintf(buf, sizeof(buf), "Dumping...");
		}
		notify(player, buf);
		add_property((dbref) 0, "_sys/lastdumptime", NULL, (int) (long) time((time_t *) NULL));
		fork_and_dump();
	} else {
		notify(player, "Sorry, you are in a no dumping zone.");
	}
}

void
do_shutdown(dbref player)
{
	if (Wizard(player)) {
		warn("SHUTDOWN: by %s", unparse_object(player, player));
		shutdown_flag = 1;
	} else {
		notify(player, "Your delusions of grandeur have been duly noted.");
		warn("ILLEGAL SHUTDOWN: tried by %s", unparse_object(player, player));
	}
}

static void
dump_database_internal(void)
{
	char tmpfile[2048];
	FILE *f;

	snprintf(tmpfile, sizeof(tmpfile), "%s.#%d#", STD_DB, epoch - 1);
	(void) unlink(tmpfile);		/* nuke our predecessor */

	snprintf(tmpfile, sizeof(tmpfile), "%s.#%d#", STD_DB, epoch);

	if ((f = fopen(tmpfile, "wb")) != NULL) {
		db_write(f);
		fclose(f);

		if (rename(tmpfile, STD_DB) < 0)
			perror(tmpfile);

	} else {
		perror(tmpfile);
	}

	/* Write out the macros */

	snprintf(tmpfile, sizeof(tmpfile), "%s.#%d#", MACRO_FILE, epoch - 1);
	(void) unlink(tmpfile);

	snprintf(tmpfile, sizeof(tmpfile), "%s.#%d#", MACRO_FILE, epoch);

	if ((f = fopen(tmpfile, "wb")) != NULL) {
		macrodump(macrotop, f);
		fclose(f);
		if (rename(tmpfile, MACRO_FILE) < 0)
			perror(tmpfile);
	} else {
		perror(tmpfile);
	}
	sync();
}

void
panic(const char *message)
{
	char panicfile[2048];
	FILE *f;

	warn("PANIC: %s", message);
	fprintf(stderr, "PANIC: %s\n", message);

	/* shut down interface */
	if (!forked_dump_process_flag) {
		emergency_shutdown();
	}

	/* dump panic file */
	snprintf(panicfile, sizeof(panicfile), "%s.PANIC", STD_DB);
	if ((f = fopen(panicfile, "wb")) == NULL) {
		perror("CANNOT OPEN PANIC FILE, YOU LOSE");
		sync();

#ifdef NOCOREDUMP
		exit(135);
#else							/* !NOCOREDUMP */
# ifdef SIGIOT
		signal(SIGIOT, SIG_DFL);
# endif
		abort();
#endif							/* NOCOREDUMP */
	} else {
		warn("DUMPING: %s", panicfile);
		fprintf(stderr, "DUMPING: %s\n", panicfile);
		db_write(f);
		fclose(f);
		warn("DUMPING: %s (done)", panicfile);
		fprintf(stderr, "DUMPING: %s (done)\n", panicfile);
	}

	/* Write out the macros */
	snprintf(panicfile, sizeof(panicfile), "%s.PANIC", MACRO_FILE);
	if ((f = fopen(panicfile, "wb")) != NULL) {
		macrodump(macrotop, f);
		fclose(f);
	} else {
		perror("CANNOT OPEN MACRO PANIC FILE, YOU LOSE");
		sync();
#ifdef NOCOREDUMP
		exit(135);
#else							/* !NOCOREDUMP */
#ifdef SIGIOT
		signal(SIGIOT, SIG_DFL);
#endif
		abort();
#endif							/* NOCOREDUMP */
	}

	sync();

#ifdef NOCOREDUMP
	exit(136);
#else							/* !NOCOREDUMP */
#ifdef SIGIOT
	signal(SIGIOT, SIG_DFL);
#endif
	abort();
#endif							/* NOCOREDUMP */
}

void
dump_database(void)
{
	epoch++;

	warn("DUMPING: %s.#%d#", STD_DB, epoch);
	dump_database_internal();
	warn("DUMPING: %s.#%d# (done)", STD_DB, epoch);
}

/*
 * Named "fork_and_dump()" mostly for historical reasons...
 */
void
fork_and_dump(void)
{
	epoch++;

	last_monolithic_time = time(NULL);
	warn("CHECKPOINTING: %s.#%d#", STD_DB, epoch);

	wall(DUMPING_MESG);

	if ((global_dumper_pid=fork())==0) {
	/* We are the child. */
		forked_dump_process_flag = 1;
# ifdef NICEVAL
	/* Requested by snout of SPR, reduce the priority of the
	 * dumper child. */
		nice(NICEVAL);
# endif /* NICEVAL */
		set_dumper_signals();
		dump_database_internal();
		_exit(0);
	}
	if (global_dumper_pid < 0) {
	    global_dumper_pid = 0;
	    wall_wizards("## Could not fork for database dumping.  Possibly out of memory.");
	    wall_wizards("## Please restart the server when next convenient.");
	}
}

void
dump_warning(void)
{
	wall(DUMPWARN_MESG);
}

int
init_game()
{
	if ((input_file = fopen(STD_DB, "rb")) == NULL)
		return -1;

	db_free();
	SRANDOM(getpid());			/* init random number generator */

	/* ok, read the db in */
	warn("LOADING: %s", STD_DB);
	if (db_read(input_file) < 0)
		return -1;
	warn("LOADING: %s (done)", STD_DB);

	/* initialize the _sys/startuptime property */
	add_property((dbref) 0, "_sys/startuptime", NULL, (int) time((time_t *) NULL));
	add_property((dbref) 0, "_sys/maxpennies", NULL, MAX_PENNIES);
	add_property((dbref) 0, "_sys/dumpinterval", NULL, DUMP_INTERVAL);
	add_property((dbref) 0, "_sys/max_connects", NULL, 0);

	return 0;
}

#if 0
void
do_restrict(dbref player, const char *arg)
{
	if (!Wizard(player)) {
		notify(player, "Permission Denied.");
		return;
	}

	if (!strcmp(arg, "on")) {
		optflags |= OPT_WIZONLY;
		notify(player, "Login access is now restricted to wizards only.");
	} else if (!strcmp(arg, "off")) {
		optflags &= ~OPT_WIZONLY;
		notify(player, "Login access is now unrestricted.");
	} else {
		notify_fmt(player, "Restricted connection mode is currently %s.",
		(optflags & OPT_WIZONLY) ? "on" : "off"
		);
	}
}
#endif

/* use this only in process_command */
#define Matched(string) if (!string_prefix(string, command)) \
	goto bad; else matched = 1;

int force_level = 0;
dbref force_prog = NOTHING; /* Set when a program is the source of FORCE */

static inline int
do_v(int descr, dbref player, char const *cmd)
{
	int ofs = 1;
	char const *s = cmd;

	for (; *s && ofs > 0; s += ofs) {
		switch (*s) {
		case COMMAND_TOKEN:
			return s - cmd;
		case SAY_TOKEN:
			do_say(player, s + 1);
			return s + strlen(s) - cmd;
		case POSE_TOKEN:
			do_pose(player, s + 1);
			return s + strlen(s) - cmd;
		}

		ofs = geo_v(descr, player, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(descr, player, s);
	}

	return s - cmd;
}

typedef void core_command_cb_t(command_t *);

typedef struct {
	char *name;
	size_t nargs;
	core_command_cb_t *cb;
} core_command_t;

void cmd_help(command_t *cmd) { do_help(cmd->player, cmd->argv[1], cmd->argv[2]); }

core_command_t cmds[] = {
	{
		.name = "action",
		.nargs = 2,
		.cb = &cmd_action,
		/* do_action(descr, player, arg1, arg2); */
#if 0
	}, {
		.name = "advitam",
		.nargs = 1,
		.cb = &cmd_advitam,
		/* do_advitam(descr, player, arg1); */
	}, {
		.name = "attach",
		.nargs = 2,
		.cb = &cmd_attach,
		/* do_attach(descr, player, arg1, arg2); */
	}, {
		.name = "bless",
		.nargs = 2,
		.cb = &cmd_bless,
		/* do_bless(descr, player, arg1, arg2); */
	}, {
		.name = "boot",
		.nargs = 1,
		.cb = &cmd_boot,
		/* do_boot(player, arg1); */
	}, {
		.name = "chlock",
		.nargs = 2,
		.cb = &cmd_chlock,
		/* do_chlock(descr, player, arg1, arg2); */
	}, {
		.name = "chown",
		.nargs = 2,
		.cp = &cmd_chown,
		/* do_chown(descr, player, arg1, arg2); */
	}, {
		.name = "chlock",
		.nargs = 2,
		.cp = &cmd_chlock,
		/* do_chlock(descr, player, arg1, arg2); */
	}, {
		.name = "clone",
		.nargs = 2,
		.cp = &cmd_clone,
		/* do_clone(descr, player, arg1); */
	}, {
		.name = "conlock",
		.nargs = 2,
		.cp = &cmd_conlock,
		/* do_conlock(descr, player, arg1, arg2); */
	}, {
		.name = "contents",
		.nargs = 2,
		.cp = &cmd_contents,
		/* do_contents(descr, player, arg1, arg2); */
	}, {
		.name = "create",
		.nargs = 2,
		.cp = &cmd_create,
		/* do_create(player, arg1, arg2); */
	}, {
		.name = "credits",
		.nargs = 2,
		.cp = &cmd_credits,
		/* do_credits(player); */
	}, {
		.name = "serverdebug",
		.nargs = 2,
		.cb = &cmd_serverdebug,
		/* do_serverdebug(descr, player, arg1, arg2); */
	}, {
		.name = "describe",
		.nargs = 2,
		.cb = &cmd_describe,
		/* do_describe(descr, player, arg1, arg2); */
	}, {
		.name = "dig",
		.nargs = 2,
		.cb = &cmd_dig,
		/* do_dig(descr, player, arg1, arg2); */
	}, {
		.name = "doing",
		.nargs = 2,
		.cb = &cmd_doing,
		/* do_doing(descr, player, arg1, arg2); */
	}, {
		.name = "drop_message",
		.nargs = 2,
		.cb = &cmd_drop_message,
		/* do_drop_message(descr, player, arg1, arg2); */
	}, {
		.name = "dump",
		.nargs = 2,
		.cb = &cmd_dump,
		/* do_dump(player, full_command); */
	}, {
		.name = "entrances",
		.nargs = 2,
		.cb = &cmd_entrances,
		/* do_entrances(descr, player, arg1, arg2); */

		/* sane_dump_object(player, arg1); /1* examine *1/ */
	}, {
		.name = "fail",
		.nargs = 2,
		.cb = &cmd_fail,
		/* do_fail(descr, player, arg1, arg2); */
	}, {
		.name = "find",
		.nargs = 2,
		.cb = &cmd_find,
		/* do_find(player, arg1, arg2); */
	}, {
		.name = "flock",
		.nargs = 2,
		.cb = &cmd_flock,
		/* do_flock(descr, player, arg1, arg2); */
	}, {
		.name = "force",
		.nargs = 2,
		.cb = &cmd_force,
		/* do_force(descr, player, arg1, arg2); */
	}, {
		.name = "flock",
		.nargs = 2,
		.cb = &cmd_flock,
		/* do_flock(descr, player, arg1, arg2); */
	}, {
		.name = "heal",
		.nargs = 2,
		.cb = &cmd_heal,
		/* do_heal(descr, player, arg1); */
	}, {
		.name = "idescribe",
		.nargs = 2,
		.cb = &cmd_idescribe,
		/* do_idescribe(descr, player, arg1, arg2); */
	}, {
		.name = "link",
		.nargs = 2,
		.cb = &cmd_link,
		/* do_link(descr, player, arg1, arg2); */
	}, {
		.name = "lock",
		.nargs = 2,
		.cb = &cmd_lock,
		/* do_lock(descr, player, arg1, arg2); */
	}, {
		.name = "name",
		.nargs = 2,
		.cb = &cmd_name,
		/* do_name(descr, player, arg1, arg2); */
	}, {
		/* if (strcmp(command, "@newpassword")) */
		/* 	goto bad; */
		.name = "newpassword",
		.nargs = 2,
		.cb = &cmd_newpassword,
		/* do_newpassword(player, arg1, arg2); */
	}, {
		.name = "odrop",
		.nargs = 2,
		.cb = &cmd_odrop,
		/* do_odrop(descr, player, arg1, arg2); */
	}, {
		.name = "oecho",
		.nargs = 2,
		.cb = &cmd_oecho,
		/* do_oecho(descr, player, arg1, arg2); */
	}, {
		.name = "ofail",
		.nargs = 2,
		.cb = &cmd_ofail,
		/* do_ofail(descr, player, arg1, arg2); */
	}, {
		.name = "open",
		.nargs = 2,
		.cb = &cmd_open,
		/* do_open(descr, player, arg1, arg2); */
	}, {
		.name = "osuccess",
		.nargs = 2,
		.cb = &cmd_osuccess,
		/* do_osuccess(descr, player, arg1, arg2); */
	}, {
		.name = "owned",
		.nargs = 2,
		.cb = &cmd_owned,
		/* do_owned(player, arg1, arg2); */
	}, {
		.name = "password",
		.nargs = 2,
		.cb = &cmd_password,
		/* do_password(player, arg1, arg2); */
	}, {
		.name = "pcreate",
		.nargs = 2,
		.cb = &cmd_pcreate,
		/* do_pcreate(player, arg1, arg2); */
	}, {
		.name = "pecho",
		.nargs = 2,
		.cb = &cmd_pecho,
		/* do_pecho(descr, player, arg1, arg2); */
	}, {
		.name = "propset",
		.nargs = 2,
		.cb = &cmd_propset,
		/* do_propset(descr, player, arg1, arg2); */
	}, {
		.name = "recycle",
		.nargs = 2,
		.cb = &cmd_recycle,
		/* do_recycle(descr, player, arg1); */
	}, {
		.name = "relink",
		.nargs = 2,
		.cb = &cmd_relink,
		/* do_relink(descr, player, arg1, arg2); */
	}, {
		/* Matched("@restrict"); */
		.name = "restrict",
		.nargs = 2,
		.cb = &cmd_restrict,
		/* do_restrict(player, arg1); */
	}, {
		/* sanity(player); */
		.name = "command",
		.nargs = 2,
		.cb = &cmd_command,
		/* sanechange(player, full_command); /1* sanchange *1/ */
	}, {
		/* sanfix(player); */
		.name = "set",
		.nargs = 2,
		.cb = &cmd_set,
		/* do_set(descr, player, arg1, arg2); */
	}, {
		.name = "showextver",
		.nargs = 2,
		.cb = &cmd_showextver,
		/* do_showextver(player); */
	}, {
		.name = "shutdown",
		.nargs = 2,
		.cb = &cmd_shutdown,
		/* do_shutdown(player); */
	}, {
		.name = "stats",
		.nargs = 2,
		.cb = &cmd_stats,
		/* do_stats(player, arg1); */
	}, {
		.name = "success",
		.nargs = 2,
		.cb = &cmd_success,
		/* do_success(descr, player, arg1, arg2); */
	}, {
		.name = "sweep",
		.nargs = 2,
		.cb = &cmd_sweep,
		/* do_sweep(descr, player, arg1); */
	}, {
		.name = "teleport",
		.nargs = 2,
		.cb = &cmd_teleport,
		/* do_teleport(descr, player, arg1, arg2); */
	}, {
		.name = "toad",
		.nargs = 2,
		.cb = &cmd_toad,
		/* do_toad(descr, player, arg1, arg2); */
	}, {
		.name = "trace",
		.nargs = 2,
		.cb = &cmd_trace,
		/* do_trace(descr, player, arg1, atoi(arg2)); */
	}, {
		.name = "unbless",
		.nargs = 2,
		.cb = &cmd_unbless,
		/* do_unbless(descr, player, arg1, arg2); */
	}, {
		.name = "unlink",
		.nargs = 2,
		.cb = &cmd_unlink,
		/* do_unlink(descr, player, arg1); */
	}, {
		.name = "unlock",
		.nargs = 2,
		.cb = &cmd_unlock,
		/* do_unlock(descr, player, arg1); */
	}, {
		.name = "usage",
		.nargs = 2,
		.cb = &cmd_usage,
		/* do_usage(player); */
	}, {
		.name = "version",
		.nargs = 2,
		.cb = &cmd_version,
		/* do_version(player); */
	}, {
		.name = "wall",
		.nargs = 2,
		.cb = &cmd_wall,
		/* do_wall(player, full_command); /1* rename *1/ */
	}, {
		.name = "buy",
		.nargs = 2,
		.cb = &cmd_buy,
		/* do_buy(descr, player, arg1, arg2); */
	}, {
		.name = "leave",
		.nargs = 2,
		.cb = &cmd_leave,
		/* do_leave(descr, player); /1* disembark *1/ */
	}, {
		.name = "drink",
		.nargs = 2,
		.cb = &cmd_drink,
		/* do_drink(descr, player, arg1); */
	}, {
		.name = "drop",
		.nargs = 2,
		.cb = &cmd_drop,
		/* do_drop(descr, player, arg1, arg2); */
	}, {
		.name = "eat",
		.nargs = 2,
		.cb = &cmd_eat,
		/* do_eat(descr, player, arg1); */
	}, {
		.name = "examine",
		.nargs = 2,
		.cb = &cmd_examine,
		/* do_examine(descr, player, arg1, arg2); */
	}, {
		.name = "equip",
		.nargs = 2,
		.cb = &cmd_equip,
		/* do_equip(descr, player, arg1); */
	}, {
		.name = "fill",
		.nargs = 2,
		.cb = &cmd_fill,
		/* do_fill(descr, player, arg1, arg2); */
	}, {
		.name = "get",
		.nargs = 2,
		.cb = &cmd_get,
		/* do_get(descr, player, arg1, arg2); */
	}, {
		.name = "give",
		.nargs = 2,
		.cb = &cmd_give,
		/* do_give(descr, player, arg1, atoi(arg2)); */
	}, {
		.name = "move",
		.nargs = 2,
		.cb = &cmd_move,
		/* do_move(descr, player, arg1, 0); */
	}, {
		.name = "gripe",
		.nargs = 2,
		.cb = &cmd_gripe,
		/* do_gripe(player, full_command); */
	}, {
		.name = "help",
		.nargs = 2,
		.cb = &cmd_help,
		/* do_help(player, arg1, arg2); */
	}, {
		.name = "inventory",
		.nargs = 2,
		.cb = &cmd_inventory,
		/* do_inventory(player); */
	}, {
		.name = "info",
		.nargs = 2,
		.cb = &cmd_info,
		/* do_info(player, arg1, arg2); */
	}, {
		.name = "kill",
		.nargs = 2,
		.cb = &cmd_kill,
		/* do_kill(descr, player, arg1); */
	}, {
		.name = "look_at",
		.nargs = 2,
		.cb = &cmd_look_at,
		/* do_look_at(descr, player, arg1, arg2); */
	}, {
		.name = "leave",
		.nargs = 2,
		.cb = &cmd_leave,
		/* do_leave(descr, player); */
	}, {
		.name = "move",
		.nargs = 2,
		.cb = &cmd_move,
		/* do_move(descr, player, arg1, 0); */
	}, {
		.name = "motd",
		.nargs = 2,
		.cb = &cmd_motd,
		/* /1* do_motd(player, full_command); *1/ */
	}, {
		.name = "view",
		.nargs = 2,
		.cb = &cmd_view,
		/* do_view(descr, player); /1* map *1/ */
	}, {
		.name = "meme",
		.nargs = 2,
		.cb = &cmd_meme,
		/* /1* do_meme(descr, player, arg1); *1/ */
	}, {
		.name = "man",
		.nargs = 2,
		.cb = &cmd_man,
		/* do_man(player, (!*arg1 && !*arg2 && arg1 != arg2) ? "=" : arg1, arg2); */
	}, {
		.name = "news",
		.nargs = 2,
		.cb = &cmd_news,
		/* do_news(player, arg1, arg2); */
	}, {
		.name = "page",
		.nargs = 2,
		.cb = &cmd_page,
		/* do_page(player, arg1, arg2); */
	}, {
		.name = "pose",
		.nargs = 2,
		.cb = &cmd_pose,
		/* do_pose(player, full_command); */
	}, {
		.name = "drop",
		.nargs = 2,
		.cb = &cmd_drop,
		/* do_drop(descr, player, arg1, arg2); /1* put *1/ */
	}, {
		.name = "look_at",
		.nargs = 2,
		.cb = &cmd_look_at,
		/* do_look_at(descr, player, arg1, arg2); /1* read (put alias) *1/ */
	}, {
		.name = "rob",
		.nargs = 2,
		.cb = &cmd_rob,
		/* do_rob(descr, player, arg1); */
	}, {
		.name = "say",
		.nargs = 2,
		.cb = &cmd_say,
		/* do_say(player, full_command); */
	}, {
		.name = "score",
		.nargs = 2,
		.cb = &cmd_score,
		/* do_score(player); */
	}, {
		.name = "sell",
		.nargs = 2,
		.cb = &cmd_sell,
		/* do_sell(descr, player, arg1, arg2); */
	}, {
		.name = "select",
		.nargs = 2,
		.cb = &cmd_select,
		/* do_select(player, arg1); */
	}, {
		.name = "shop",
		.nargs = 2,
		.cb = &cmd_shop,
		/* do_shop(player); */
	}, {
		.name = "sit",
		.nargs = 2,
		.cb = &cmd_sit,
		/* do_sit(descr, player, arg1); */
	}, {
		.name = "stand",
		.nargs = 2,
		.cb = &cmd_stand,
		/* do_stand(player); */
	}, {
		.name = "status",
		.nargs = 2,
		.cb = &cmd_status,
		/* do_status(player); */
	}, {
		.name = "get",
		.nargs = 2,
		.cb = &cmd_get,
		/* do_get(descr, player, arg1, arg2); /1* take *1/ */
	}, {
		.name = "drop",
		.nargs = 2,
		.cb = &cmd_drop,
		/* do_drop(descr, player, arg1, arg2); /1* throw *1/ */
	}, {
		.name = "train",
		.nargs = 2,
		.cb = &cmd_train,
		/* do_train(player, arg1, arg2); */
	}, {
		.name = "unequip",
		.nargs = 2,
		.cb = &cmd_unequip,
		/* do_unequip(descr, player, arg1); */
	}, {
		.name = "whisper",
		.nargs = 2,
		.cb = &cmd_whisper,
		/* do_whisper(descr, player, arg1, arg2); */
#endif
	},
};

core_command_t *
command_match(command_t *cmd) {
	if (cmd->argc < 1 || strcmp(cmd->argv[0], "help"))
		return NULL;
	return &cmds[0];
}

void
command_process(command_t *cmd)
{
	if (cmd->argc < 1) {
		return;
	}

	char *command = cmd->argv[0];
	size_t command_n = 0;
	dbref player = cmd->player;
	int descr = cmd->fd;

	/* struct timeval starttime; */
	/* struct timeval endtime; */
	pos_t pos;
	map_where(pos, getloc(cmd->player));

	command_debug(cmd, "command_process");
	warn("command_process %s", command);
        // set current descriptor (needed for death)
        CBUG(GETLID(player) < 0);
        mobi_t *liv = MOBI(player);
        liv->descr = descr;

	/* robustify player */
	if (player < 0 || player >= db_top ||
		(Typeof(player) != TYPE_PLAYER && Typeof(player) != TYPE_THING)) {
		warn("process_command: bad player %d", player);
		return;
	}

	/* profile how long command takes. */
	/* gettimeofday(&starttime, NULL); */

	/* if player is a wizard, and uses overide token to start line... */
	/* ... then do NOT run actions, but run the command they specify. */
	if (!(TrueWizard(OWNER(player)) && (*command == OVERIDE_TOKEN))
	    && can_move(descr, player, command, 0))
	{
		do_move(descr, player, command, 0);	/* command is exact match for exit */
		*match_args = 0;
		*match_cmdname = 0;
		goto out;
	}

	for (; command_n < cmd->argc; command_n ++)
	{
		command = cmd->argv[command_n];
		core_command_t *cmd_i = command_match(cmd);

		if (cmd_i) {
			cmd_i->cb(cmd);
			// TODO get next command
			break;
		}

		int matched;
		command += do_v(descr, player, command);
		if (*command == COMMAND_TOKEN) {
			command++;
			matched = 1;
			CBUG(1);
		} else if (*command == '\0') {
			continue;
		} else
			break;
			/* goto bad; */
	}
#if 0
			break;
		default:
		  bad:
#if M3_HUH
			{
				char hbuf[BUFFER_LEN];
				snprintf(hbuf,BUFFER_LEN,"HUH? %s", command);
				if(can_move(descr, player, hbuf, 3)) {
					do_move(descr, player, hbuf, 3);
					*match_args = 0;
					*match_cmdname = 0;
					break;
				}
			}	
#endif
			notify(player, HUH_MESSAGE);
#if LOG_FAILED_COMMANDS
			if (!controls(player, DBFETCH(player)->location)) {
				warn("HUH from %s(%d) in %s(%d)[%s]: %s %s",
						   NAME(player), player, NAME(DBFETCH(player)->location),
						   DBFETCH(player)->location,
						   NAME(OWNER(DBFETCH(player)->location)), command, full_command);
			}
#endif
			matched = 1;
			break;
		}

	}

end_of_command:
	/* calculate time command took. */
	gettimeofday(&endtime, NULL);
	if (starttime.tv_usec > endtime.tv_usec) {
		endtime.tv_usec += 1000000;
		endtime.tv_sec -= 1;
	}
	endtime.tv_usec -= starttime.tv_usec;
	endtime.tv_sec -= starttime.tv_sec;
#endif

out:
	{
		pos_t pos2;
		map_where(pos2, getloc(player));
		if (MORTON_READ(pos2) != MORTON_READ(pos))
			do_view(descr, player);
	}
}

#undef Matched
static const char *game_c_version = "$RCSfile$ $Revision: 1.50 $";
const char *get_game_c_version(void) { return game_c_version; }

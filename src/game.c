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
do_shutdown(command_t *cmd)
{
	dbref player = cmd->player;
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
		/* case SAY_TOKEN: */
		/* 	do_say(player, s + 1); */
		/* 	return s + strlen(s) - cmd; */
		/* case POSE_TOKEN: */
		/* 	do_pose(player, s + 1); */
		/* 	return s + strlen(s) - cmd; */
		}

		ofs = geo_v(descr, player, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(descr, player, s);
	}

	return s - cmd;
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
		/* if (MORTON_READ(pos2) != MORTON_READ(pos)) TODO */
		/* 	do_view(descr, player); */
	}
}

#undef Matched
static const char *game_c_version = "$RCSfile$ $Revision: 1.50 $";
const char *get_game_c_version(void) { return game_c_version; }

/* $Header$ */

/* Copyright 1992-2001 by Fuzzball Software */
/* Consider this code protected under the GNU public license, with explicit
 * permission to distribute when linked against openSSL. */

#define DEFINE_HEADER_VERSIONS
#include "fb.h"
#undef DEFINE_HEADER_VERSIONS
#include "copyright.h"
#include "config.h"
#include "match.h"
#include "mpi.h"
#include "web.h"

#include <sys/types.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

# define NEED_SOCKLEN_T
//"do not include netinet6/in6.h directly, include netinet/in.h.  see RFC2553"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <openssl/ssl.h>

#include "db.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "externs.h"
#include "interp.h"
#include "kill.h"
#include "search.h"
#include "view.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#undef NDEBUG
#include "debug.h"
#include "noise.h"

#define DESCR_ITER(di_d) \
	for (di_d = &descr_map[sockfd + 1]; \
	     di_d < &descr_map[FD_SETSIZE]; \
	     di_d++) \
		if (!FD_ISSET(di_d->fd, &readfds) \
		    || !(di_d->flags & DF_CONNECTED)) \
			continue; \
		else

enum descr_flags {
	DF_CONNECTED = 1,
	DF_BOOTED = 2,
};

struct ws {
	int flags;
	unsigned ip;
	unsigned old;
};

#define QUEUE_MAX (BUFSIZ << 3)

typedef struct {
	char buf[QUEUE_MAX], *p;
	ssize_t len;
} queue_t;

typedef struct descr_st {
	int fd, flags;
	int con_number;
	dbref player;
	int output_size;
	struct {
		queue_t input, output;
	} inband;
	struct ws ws;
	long last_time;
	long connected_at;
	long last_pinged_at;
	/* int quota; */
} descr_t;

core_command_t *cmds_hashed[64] = {
	[ 0 ... 63 ] = NULL,
};

core_command_t cmds[] = {
	{
		.name = "action",
		.cb = &do_action,
	}, {
		.name = "advitam",
		.cb = &do_advitam,
	}, {
		.name = "attach",
		.cb = &do_attach,
	}, {
		.name = "bless",
		.cb = &do_bless,
	}, {
		.name = "boot",
		.cb = &do_boot,
	}, {
		.name = "chlock",
		.cb = &do_chlock,
	}, {
		.name = "chown",
		.cb = &do_chown,
	}, {
		.name = "chlock",
		.cb = &do_chlock,
	}, {
		.name = "clone",
		.cb = &do_clone,
	}, {
		.name = "conlock",
		.cb = &do_conlock,
	}, {
		.name = "contents",
		.cb = &do_contents,
	}, {
		.name = "create",
		.cb = &do_create,
	}, {
		.name = "credits",
		.cb = &do_credits,
	}, {
		.name = "describe",
		.cb = &do_describe,
	}, {
		.name = "dig",
		.cb = &do_dig,
	/* }, { */
	/* 	.name = "doing", */
	/* 	.cb = &do_doing, */
	}, {
		.name = "drop_message",
		.cb = &do_drop_message,
	/* }, { */
	/* 	.name = "dump", */
	/* 	.cb = &do_dump, */
	}, {
		.name = "entrances",
		.cb = &do_entrances,

		/* sane_dump_object(player, arg1); /1* examine *1/ */
	}, {
		.name = "fail",
		.cb = &do_fail,
	}, {
		.name = "find",
		.cb = &do_find,
	}, {
		.name = "flock",
		.cb = &do_flock,
	/* }, { */
	/* 	.name = "force", */
	/* 	.cb = &do_force, */
	}, {
		.name = "flock",
		.cb = &do_flock,
	}, {
		.name = "heal",
		.cb = &do_heal,
	}, {
		.name = "idescribe",
		.cb = &do_idescribe,
	}, {
		.name = "link",
		.cb = &do_link,
	}, {
		.name = "lock",
		.cb = &do_lock,
	}, {
		.name = "name",
		.cb = &do_name,
	}, {
		.name = "newpassword",
		.cb = &do_newpassword,
	}, {
		.name = "odrop",
		.cb = &do_odrop,
	}, {
		.name = "oecho",
		.cb = &do_oecho,
	/* }, { */
	/* 	.name = "ofail", */
	/* 	.cb = &do_ofail, */
	}, {
		.name = "open",
		.cb = &do_open,
	}, {
		.name = "osuccess",
		.cb = &do_osuccess,
	}, {
		.name = "owned",
		.cb = &do_owned,
	}, {
		.name = "password",
		.cb = &do_password,
	}, {
		.name = "pcreate",
		.cb = &do_pcreate,
	}, {
		.name = "pecho",
		.cb = &do_pecho,
	}, {
		.name = "propset",
		.cb = &do_propset,
	}, {
		.name = "recycle",
		.cb = &do_recycle,
	}, {
		.name = "relink",
		.cb = &do_relink,
	/* }, { */
	/* 	.name = "restrict", */
	/* 	.cb = &do_restrict, */
	/* }, { */
		/* sanity(player); */
		/* .name = "command", */
		/* .cb = &do_command, */
		/* sanechange(player, full_command); /1* sanchange *1/ */
	}, {
		/* sanfix(player); */
		.name = "set",
		.cb = &do_set,
		/* do_set(descr, player, arg1, arg2); */

		/* do_showextver(player); */
	}, {
		.name = "shutdown",
		.cb = &do_shutdown,
		/* do_shutdown(player); */
	}, {
		.name = "stats",
		.cb = &do_stats,
		/* do_stats(player, arg1); */
	}, {
		.name = "success",
		.cb = &do_success,
		/* do_success(descr, player, arg1, arg2); */
	}, {
		.name = "sweep",
		.cb = &do_sweep,
		/* do_sweep(descr, player, arg1); */
	}, {
		.name = "teleport",
		.cb = &do_teleport,
		/* do_teleport(descr, player, arg1, arg2); */
	}, {
		.name = "toad",
		.cb = &do_toad,
		/* do_toad(descr, player, arg1, arg2); */
	}, {
		.name = "trace",
		.cb = &do_trace,
		/* do_trace(descr, player, arg1, atoi(arg2)); */
	}, {
		.name = "unbless",
		.cb = &do_unbless,
		/* do_unbless(descr, player, arg1, arg2); */
	}, {
		.name = "unlink",
		.cb = &do_unlink,
		/* do_unlink(descr, player, arg1); */
	}, {
		.name = "unlock",
		.cb = &do_unlock,
		/* do_unlock(descr, player, arg1); */
	}, {
		.name = "usage",
		.cb = &do_usage,
		/* do_usage(player); */
	}, {
		.name = "version",
		.cb = &do_version,
		/* do_version(player); */
	}, {
		.name = "wall",
		.cb = &do_wall,
		/* do_wall(player, full_command); /1* rename *1/ */
	}, {
		.name = "buy",
		.cb = &do_buy,
		/* do_buy(descr, player, arg1, arg2); */
	}, {
		.name = "leave",
		.cb = &do_leave,
		/* do_leave(descr, player); /1* disembark *1/ */
	}, {
		.name = "drink",
		.cb = &do_drink,
		/* do_drink(descr, player, arg1); */
	}, {
		.name = "drop",
		.cb = &do_drop,
		/* do_drop(descr, player, arg1, arg2); */
	}, {
		.name = "eat",
		.cb = &do_eat,
		/* do_eat(descr, player, arg1); */
	}, {
		.name = "examine",
		.cb = &do_examine,
		/* do_examine(descr, player, arg1, arg2); */
	}, {
		.name = "equip",
		.cb = &do_equip,
		/* do_equip(descr, player, arg1); */
	}, {
		.name = "fill",
		.cb = &do_fill,
		/* do_fill(descr, player, arg1, arg2); */
	}, {
		.name = "get",
		.cb = &do_get,
		/* do_get(descr, player, arg1, arg2); */
	}, {
		.name = "give",
		.cb = &do_give,
		/* do_give(descr, player, arg1, atoi(arg2)); */
	/* }, { */
	/* 	.name = "move", */
	/* 	.cb = &do_move, */
	/* 	/1* do_move(descr, player, arg1, 0); *1/ */
	}, {
		.name = "gripe",
		.cb = &do_gripe,
		/* do_gripe(player, full_command); */
	}, {
		.name = "help",
		.cb = &do_help,
		/* do_help(player, arg1, arg2); */
	}, {
		.name = "inventory",
		.cb = &do_inventory,
		/* do_inventory(player); */
	}, {
		.name = "info",
		.cb = &do_info,
		/* do_info(player, arg1, arg2); */
	}, {
		.name = "kill",
		.cb = &do_kill,
		/* do_kill(descr, player, arg1); */
	}, {
		.name = "look_at",
		.cb = &do_look_at,
		/* do_look_at(descr, player, arg1, arg2); */
	}, {
		.name = "leave",
		.cb = &do_leave,
		/* do_leave(descr, player); */
	/* }, { */
	/* 	.name = "move", */
	/* 	.cb = &do_move, */
	/* 	/1* do_move(descr, player, arg1, 0); *1/ */
	/* }, { */
	/* 	.name = "motd", */
	/* 	.cb = &do_motd, */
	/* 	/1* /2* do_motd(player, full_command); *2/ *1/ */
	}, {
		.name = "view",
		.cb = &do_view,
		/* do_view(descr, player); /1* map *1/ */
	/* }, { */
	/* 	.name = "meme", */
	/* 	.cb = &do_meme, */
	/* 	/1* /2* do_meme(descr, player, arg1); *2/ *1/ */
	}, {
		.name = "man",
		.cb = &do_man,
		/* do_man(player, (!*arg1 && !*arg2 && arg1 != arg2) ? "=" : arg1, arg2); */
	}, {
		.name = "news",
		.cb = &do_news,
		/* do_news(player, arg1, arg2); */
	}, {
		.name = "page",
		.cb = &do_page,
		/* do_page(player, arg1, arg2); */
	}, {
		.name = "pose",
		.cb = &do_pose,
		/* do_pose(player, full_command); */
	}, {
		.name = "drop",
		.cb = &do_drop,
		/* do_drop(descr, player, arg1, arg2); /1* put *1/ */
	}, {
		.name = "look_at",
		.cb = &do_look_at,
		/* do_look_at(descr, player, arg1, arg2); /1* read (put alias) *1/ */
	}, {
		.name = "rob",
		.cb = &do_rob,
		/* do_rob(descr, player, arg1); */
	}, {
		.name = "say",
		.cb = &do_say,
		/* do_say(player, full_command); */
	}, {
		.name = "score",
		.cb = &do_score,
		/* do_score(player); */
	}, {
		.name = "sell",
		.cb = &do_sell,
	}, {
		.name = "select",
		.cb = &do_select,
	}, {
		.name = "shop",
		.cb = &do_shop,
	}, {
		.name = "sit",
		.cb = &do_sit,
	}, {
		.name = "stand",
		.cb = &do_stand,
	}, {
		.name = "status",
		.cb = &do_status,
	}, {
		.name = "get",
		.cb = &do_get,
	}, {
		.name = "drop",
		.cb = &do_drop,
	}, {
		.name = "train",
		.cb = &do_train,
	}, {
		.name = "unequip",
		.cb = &do_unequip,
	}, {
		.name = "whisper",
		.cb = &do_whisper,
	},
};

fd_set readfds, readfds_new, writefds;
descr_t *descriptor_list = NULL;

#define MAX_LISTEN_SOCKS 16

int shutdown_flag = 0;

static const char *create_fail =
		"Either there is already a player with that name, or that name is illegal.\r\n";

/* static const char *shutdown_message = "\r\nGoing down - Bye\r\n"; */

int resolver_sock[2];

static int sockfd, nextfd;
descr_t descr_map[FD_SETSIZE];

void
command_debug(command_t *cmd, char *label)
{
	char **arg;

	warn("command_debug '%s' %d", label, cmd->argc);
	for (arg = cmd->argv;
	     arg < cmd->argv + cmd->argc;
	     arg++)
	{
		warn(" '%s'", *arg);
	}
	warn("\n");
}

static command_t
command_new(descr_t *d, char *input, size_t len)
{
	command_t cmd;
	register char *p = input;

	p[len] = '\0';
	cmd.player = d->player;
	cmd.fd = d->fd;
	cmd.argc = 0;

	if (!*p)
		return cmd;

	cmd.argv[0] = p;
	cmd.argc++;

	for (; p < input + len; p++) {
		if (*p != ' ')
			continue;

		*p = '\0';

		cmd.argv[cmd.argc] = p + 1;
		cmd.argc ++;
	}

	for (int i = cmd.argc;
	     i < sizeof(cmd.argv) / sizeof(char *);
	     i++)

		cmd.argv[i] = "";

	command_debug(&cmd, "init");
	return cmd;
}

int
command_idx(command_t *cmd)
{
	noise_t h = uhash(cmd->argv[0], strlen(cmd->argv[0]), 88);
	return h & 0xff;
}

core_command_t *
command_match(command_t *cmd) {
	return cmds_hashed[command_idx(cmd)];
}

static void
commands_init() {
	int i;

	for (i = 0; i < sizeof(cmds) / sizeof(core_command_t); i++) {
		const char *name = cmds[i].name;
		noise_t h = uhash((void *) name, strlen(name), 88);
		cmds_hashed[h & 0xff] = &cmds[i];
	}
}

int shovechars();
void freeqs(descr_t *d);
void welcome_user(descr_t *d);
void close_sockets(const char *msg);
int boot_off(dbref player);
void boot_player_off(dbref player);
int make_socket(int);
descr_t *new_connection(int port, int sock);
void dump_users(descr_t *d, char *user);
void parse_connect(const char *msg, char *command, char *user, char *pass);
int descr_inband(descr_t *, const char *);
/* moves qd into qo */
int queue_write(descr_t *, const char *, int);
int process_output(descr_t *d);
void announce_connect(descr_t *, dbref);
void announce_disconnect(descr_t *);
char *time_format_1(long);
char *time_format_2(long);
void    remember_player_descr(dbref player, int);
void    update_desc_count_table();
int*    get_player_descrs(dbref player, int* count);
void    forget_player_descr(dbref player, int);
descr_t * descrdata_by_descr(int i);
int online_init(void);
dbref online_next(int *ptr);

extern FILE *input_file;
extern FILE *delta_infile;
extern FILE *delta_outfile;

short optflags = 0;
pid_t global_resolver_pid=0;
pid_t global_dumper_pid=0;
short global_dumpdone=0;

time_t sel_prof_start_time;
long sel_prof_idle_sec;
long sel_prof_idle_usec;
unsigned long sel_prof_idle_use;

void
show_program_usage(char *prog)
{
	fprintf(stderr, "Usage: %s [-sSyWv?] [-C [path]]", prog);
	fprintf(stderr, "    Options:\n");
	fprintf(stderr, "        -C PATH   changes directory to PATH before starting up.\n");
	fprintf(stderr, "        -S        don't do db sanity checks at startup time.\n");
	fprintf(stderr, "        -s        load db, then enter the interactive sanity editor.\n");
	fprintf(stderr, "        -y        attempt to auto-fix a corrupt db after loading.\n");
	fprintf(stderr, "        -W        only allow wizards to login.\n");
	fprintf(stderr, "        -v        display this server's version.\n");
	fprintf(stderr, "        -?        display this message.\n");
	exit(1);
}

/* NOTE: Will need to think about this more for unicode */
#define isinput( q ) isprint( (q) & 127 )

extern int sanity_violated;
int time_since_combat = 0;

int
main(int argc, char **argv)
{
	register char c;

	int i;
	for (i = 0; i < FD_SETSIZE; i++)
		memset(&descr_map[i], 0, sizeof(descr_t));

	while ((c = getopt(argc, argv, "dsyvSC:")) != -1) {
		switch (c) {
		case 'd':
			optflags |= OPT_DETACH;
			break;
		case 's':
			optflags |= OPT_SANITY_INTERACTIVE;
			break;
		case 'S':
			optflags |= OPT_NOSANITY;
			break;
		case 'y':
			optflags |= OPT_SANITY_AUTOFIX;
			break;
		case 'v':
			printf("%s\n", VERSION);
			break;
			
		case 'C':
			if (chdir(optarg)) {
				perror("chdir");
				return 4;
			}
			break;
		default:
			show_program_usage(*argv);
			return 1;

		case '?':
			show_program_usage(*argv);
			return 0;
		}
	}

	warn("INIT: TinyMUCK %s starting.", "version");
	warn("%s PID is: %d", argv[0], getpid());

	sel_prof_start_time = time(NULL); /* Set useful starting time */
	sel_prof_idle_sec = 0;
	sel_prof_idle_usec = 0;
	sel_prof_idle_use = 0;

	if (init_game() < 0) {
		warn("Couldn't load " STD_DB "!");
		return 2;
	}

	CBUG(map_init());
	commands_init();

	set_signals();

	sanity(AMBIGUOUS);
	errno = 0; // TODO why? sanity fails to access file

	if (sanity_violated) {
		optflags |= OPT_WIZONLY;
		if (optflags & OPT_SANITY_AUTOFIX)
			sanfix(AMBIGUOUS);
	}

	if (shovechars())
		return 1;

	map_close();
	mob_save();

	close_sockets("\r\nServer shutting down.\r\n");

	dump_database();

	return 0;
}

int notify_nolisten_level = 0;

int
notify_nolisten(dbref player, const char *msg, int isprivate)
{
	int retval = 0;
	char buf[BUFFER_LEN + 2];
	char buf2[BUFFER_LEN + 2];
	int firstpass = 1;
	char *ptr1;
	const char *ptr2;
	dbref ref;
	int di;
	int* darr;
	int dcount;

	ptr2 = msg;
	while (ptr2 && *ptr2) {
		ptr1 = buf;
		while (ptr2 && *ptr2 && *ptr2 != '\r')
			*(ptr1++) = *(ptr2++);
		*(ptr1++) = '\r';
		*(ptr1++) = '\n';
		*(ptr1++) = '\0';
		if (*ptr2 == '\r')
			ptr2++;

		darr = get_player_descrs(player, &dcount);
		for (di = 0; di < dcount; di++) {
			descr_inband(descrdata_by_descr(darr[di]), buf);
			if (firstpass) retval++;
		}

#if ZOMBIES
		if ((Typeof(player) == TYPE_THING) && (FLAGS(player) & ZOMBIE) &&
		    !(FLAGS(OWNER(player)) & ZOMBIE) &&
		    (!(FLAGS(player) & DARK) || Wizard(OWNER(player)))) {
			ref = getloc(player);
			if (Wizard(OWNER(player)) || ref == NOTHING ||
			    Typeof(ref) != TYPE_ROOM || !(FLAGS(ref) & ZOMBIE)) {
				if (isprivate || getloc(player) != getloc(OWNER(player))) {
					char pbuf[BUFFER_LEN];
					const char *prefix;
					char ch = *match_args;

					*match_args = '\0';

					if (notify_nolisten_level <= 0)
					{
						notify_nolisten_level++;

						prefix = do_parse_prop(-1, player, player, MESGPROP_PECHO, "(@Pecho)", pbuf, sizeof(pbuf), MPI_ISPRIVATE);

						notify_nolisten_level--;
					}
					else
						prefix = 0;

					*match_args = ch;

					if (!prefix || !*prefix) {
						prefix = NAME(player);
						snprintf(buf2, sizeof(buf2), "%s> %.*s", prefix,
							 (int)(BUFFER_LEN - (strlen(prefix) + 3)), buf);
					} else {
						snprintf(buf2, sizeof(buf2), "%s %.*s", prefix,
							 (int)(BUFFER_LEN - (strlen(prefix) + 2)), buf);
					}

					darr = get_player_descrs(OWNER(player), &dcount);
					for (di = 0; di < dcount; di++) {
						descr_inband(descrdata_by_descr(darr[di]), buf2);
						if (firstpass) retval++;
					}
				}
			}
		}
#endif
		firstpass = 0;
	}
	return retval;
}

int
notify_filtered(dbref from, dbref player, const char *msg, int isprivate)
{
	if (msg == 0)
		return 0;
	return notify_nolisten(player, msg, isprivate);
}

int
notify_from_echo(dbref from, dbref player, const char *msg, int isprivate)
{

#if LISTENERS
	const char *ptr=msg;
#if !LISTENERS_OBJ
	if (Typeof(player) == TYPE_ROOM)
#endif
		listenqueue(-1, from, getloc(from), player, player, NOTHING,
			    "_listen", ptr, LISTEN_MLEV, 1, 0);
	listenqueue(-1, from, getloc(from), player, player, NOTHING,
		    "~listen", ptr, LISTEN_MLEV, 1, 1);
	listenqueue(-1, from, getloc(from), player, player, NOTHING,
		    "~olisten", ptr, LISTEN_MLEV, 0, 1);
#endif

	if (Typeof(player) == TYPE_THING && (FLAGS(player) & VEHICLE) &&
		(!(FLAGS(player) & DARK) || Wizard(OWNER(player))))
	{
		dbref ref;

		ref = getloc(player);
		if (Wizard(OWNER(player)) || ref == NOTHING ||
			Typeof(ref) != TYPE_ROOM || !(FLAGS(ref) & VEHICLE)
				) {
			if (!isprivate && getloc(from) == getloc(player)) {
				char buf[BUFFER_LEN];
				char pbuf[BUFFER_LEN];
				const char *prefix;
				char ch = *match_args;

				*match_args = '\0';
				prefix = do_parse_prop(-1, from, player, MESGPROP_OECHO, "(@Oecho)", pbuf, sizeof(pbuf), MPI_ISPRIVATE);
				*match_args = ch;

				if (!prefix || !*prefix)
					prefix = "Outside>";
				snprintf(buf, sizeof(buf), "%s %.*s", prefix, (int)(BUFFER_LEN - (strlen(prefix) + 2)), msg);
				ref = DBFETCH(player)->contents;
				while (ref != NOTHING) {
					notify_filtered(from, ref, buf, isprivate);
					ref = DBFETCH(ref)->next;
				}
			}
		}
	}

	return notify_filtered(from, player, msg, isprivate);
}

int
notify_from(dbref from, dbref player, const char *msg)
{
	return notify_from_echo(from, player, msg, 1);
}

int
notify(dbref player, const char *msg)
{
	return notify_from_echo(player, player, msg, 1);
}

void
notify_fmt(dbref player, char *format, ...)
{
	va_list args;
	char bufr[BUFFER_LEN];

	va_start(args, format);
	vsnprintf(bufr, sizeof(bufr), format, args);
	bufr[sizeof(bufr)-1] = '\0';
	notify(player, bufr);
	va_end(args);
}

struct timeval
timeval_sub(struct timeval now, struct timeval then)
{
	now.tv_sec -= then.tv_sec;
	now.tv_usec -= then.tv_usec;
	if (now.tv_usec < 0) {
		now.tv_usec += 1000000;
		now.tv_sec--;
	}
	return now;
}

int
msec_diff(struct timeval now, struct timeval then)
{
	return ((now.tv_sec - then.tv_sec) * 1000 + (now.tv_usec - then.tv_usec) / 1000);
}

struct timeval
msec_add(struct timeval t, int x)
{
	t.tv_sec += x / 1000;
	t.tv_usec += (x % 1000) * 1000;
	if (t.tv_usec >= 1000000) {
		t.tv_sec += t.tv_usec / 1000000;
		t.tv_usec = t.tv_usec % 1000000;
	}
	return t;
}

/* struct timeval */
/* update_quotas(struct timeval last, struct timeval current) */
/* { */
/* 	int nslices; */
/* 	int cmds_per_time; */
/* 	descr_t *d; */
/* 	int td = msec_diff(current, last); */
/* 	time_since_combat += td; */

/* 	nslices = td / COMMAND_TIME_MSEC; */

/* 	if (nslices > 0) { */
/* 		for (d = descriptor_list; d; d = d->next) { */
/* 			if (d->flags & DF_CONNECTED) { */
/* 				cmds_per_time = ((FLAGS(d->player) & INTERACTIVE) */
/* 								 ? (COMMANDS_PER_TIME * 8) : COMMANDS_PER_TIME); */
/* 			} else { */
/* 				cmds_per_time = COMMANDS_PER_TIME; */
/* 			} */
/* 			d->quota += cmds_per_time * nslices; */
/* 			if (d->quota > COMMAND_BURST_SIZE) */
/* 				d->quota = COMMAND_BURST_SIZE; */
/* 		} */
/* 	} */
/* 	return msec_add(last, nslices * COMMAND_TIME_MSEC); */
/* } */

int
descr_write(descr_t *d, const char *data, size_t len)
{
	/* FD_SET(d->fd, &writefds); */
	return write(d->fd, data, len);

#if 0
	if (d->inband.output.p < d->inband.output.buf + QUEUE_MAX) {
		memcpy(d->inband.output.p, data, len);
		d->inband.output.len += len;
		return len;
	}

	return -1;
#endif
}

void
goodbye_user(descr_t *d)
{
	descr_inband(d, "\r\n" LEAVE_MESSAGE "\r\n\r\n");
}

/* static int con_players_max = 0;	/1* one of Cynbe's good ideas. *1/ */
/* static int con_players_curr = 0;	/1* for playermax checks. *1/ */
extern void purge_free_frames(void);

static void
do_tick()
{
	if (time_since_combat < 1000)
		return;

	time_since_combat = 0;
	mob_update();
	geo_update();
}

void
wall(const char *msg)
{
	descr_t *d;
	char buf[BUFSIZ];

	strlcpy(buf, msg, sizeof(buf));
	strlcat(buf, "\r\n", sizeof(buf));

	DESCR_ITER(d) descr_inband(d, buf);
}

unsigned
queue_size(queue_t *q) {
	return q->p - q->buf;
}

int
queue_read(descr_t *d, queue_t *q) {
	int ret = read(d->fd, q->p, QUEUE_MAX - q->len);
	if (ret <= 0)
		return ret;

	if (ret < 2)
		return -2;

	ret -= 2;
	q->len += ret;
	*(q->p = q->buf + q->len) = '\0';
	ret ++;
	/* q->p++; */
	/* q->len++; */
	warn("queue_read %d %ld bytes %ld+%d/%d -- %s\n", d->fd, q->len, q->len - ret, ret, QUEUE_MAX, q->buf);
	/* FD_CLR(d->fd, &readfds); */
	return ret;
}

void
descr_process(descr_t *d, char *input, size_t input_len)
{
	warn("descr_process %d\n", d->fd);

	command_t cmd = command_new(d, input, input_len);

	if (!cmd.argc)
		return;

	if (d->flags & DF_CONNECTED) {
		command_process(&cmd);
		return;
	}

	if (cmd.argc != 3)
		return;

	if (*cmd.argv[0] == 'c')
		auth(d->fd, cmd.argv[1], cmd.argv[2]);
	else
		welcome_user(d);
}

void queue_init(queue_t *q) {
	q->p = q->buf;
	q->len = 0;
}

int
descr_read(descr_t *d)
{
	queue_t *q = &d->inband.input;
	queue_init(q);
	switch (queue_read(d, q)) {
	case -1:
		if (errno == EAGAIN)
			return 0;

		perror("descr_read");
		return -1;
	case 0:
		return 0;
	}

	descr_process(d, q->buf, q->len);
	return q->len;
	/* return queue_read(d, &d->inband.input); */
}

int
auth(int descr, char *user, char *password)
{
	warn("auth %s %sx\n", user, password);
        int created = 0;
        dbref player = connect_player(user, password);
	descr_t *d = descrdata_by_descr(descr);

        if ((optflags & OPT_WIZONLY) && !TrueWizard(player)) {
                descr_inband(d, (optflags & OPT_WIZONLY)
                           ? "Sorry, but the game is in maintenance mode currently, and "
                           "only wizards are allowed to connect.  Try again later."
                           : PLAYERMAX_BOOTMESG);
                descr_inband(d, "\r\n");
		d->flags |= DF_BOOTED;
                return 1;
        }

        if (player == NOTHING) {
                player = create_player(user, password);

                if (player == NOTHING) {
                        descr_inband(d, create_fail);
                        /* if (d->proto.ws.ip) */
                        /*         web_logout(d->fd); */

                        warn("FAILED CREATE %s on fd %d", user, d->fd);
                        return 1;
                }

                warn("CREATED %s(%d) on fd %d",
                           NAME(player), player, d->fd);
                created = 1;
        } else
                warn("CONNECTED: %s(%d) on fd %d",
                           NAME(player), player, d->fd);
        d->flags = DF_CONNECTED;
        d->connected_at = time(NULL);
        d->player = player;
        /* update_desc_count_table(); */
        remember_player_descr(player, d->fd);
        /* cks: someone has to initialize this somewhere. */
        PLAYER_SET_BLOCK(d->player, 0);
        welcome_user(d);
        spit_file(player, MOTD_FILE);
        announce_connect(d, player);
        if (created) {
                /* TODO do_help(player, "begin", ""); */
                mob_put(player);
        } else {
                if (sanity_violated && Wizard(player))
                        notify(player,
                               "#########################################################################\n"
                               "## WARNING!  The DB appears to be corrupt!  Please repair immediately! ##\n"
                               "#########################################################################");
        }
        /* if (!(web_support(d->fd) && d->proto.ws.old)) */
                /* TODO do_view(d->fd, player); */

        look_room(d->fd, player, getloc(player), 0);

        return 0;
}

descr_t *
descr_next() {
	return &descr_map[nextfd];
}

descr_t *
descr_new()
{
	descr_t *d = descr_next();

	if (!d)
		return NULL;

	nextfd++;
	memset(d, 0, sizeof(descr_t));

	struct sockaddr_in addr;
	socklen_t addr_len;

	addr_len = (socklen_t)sizeof(addr);
	d->fd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);

	warn("accept %d\n", d->fd);

	/* FIXME */
	if (d->fd <= 0) {
		perror("descr_new");
		return NULL;
	}

	FD_SET(d->fd, &readfds_new);
	/* FD_SET(d->fd, &writefds); */

	d->flags = 0;
	d->player = -1;
	d->con_number = 0;
	d->connected_at = time(NULL);
	if (fcntl(d->fd, F_SETFL, O_NONBLOCK) == -1) {
		perror("make_nonblocking: fcntl");
		panic("O_NONBLOCK fcntl failed");
	}
	/* d->quota = COMMAND_BURST_SIZE; */
	d->last_time = d->connected_at;
	d->last_pinged_at = d->connected_at;
	/* FD_CLR(sockfd, &readfds); */
	return d;
}

void
descr_close(descr_t *d)
{
	if (d->flags & DF_CONNECTED) {
		warn("%d disconnects", d->fd);
		announce_disconnect(d);
	} else
		warn("%d never connected", d->fd);

	shutdown(d->fd, 2);
	close(d->fd);
	if (d)
		memset(d, 0, sizeof(descr_t));
}

int
shovechars()
{
	time_t now;
	struct timeval last_slice, current_time;
	struct timeval next_slice;
	struct timeval timeout, slice_timeout;
	descr_t *d;
	struct timeval sel_in, sel_out;
	int avail_descriptors;

	sockfd = make_socket(TINYPORT);

	if (sockfd <= 0) {
		perror("make_socket");
		return sockfd;
	}

	nextfd = sockfd + 1;
	FD_SET(sockfd, &readfds_new);
	warn("shovechars %d\n", sockfd);

	gettimeofday(&last_slice, (struct timezone *) 0);

	avail_descriptors = sysconf(_SC_OPEN_MAX) - 5;

	(void) time(&now);

	mob_init();

	/* Daemonize */
	if ((optflags & OPT_DETACH) && daemon(1, 1) != 0)
		_exit(0);

/* And here, we do the actual player-interaction loop */

	while (shutdown_flag == 0) {
		gettimeofday(&current_time, (struct timezone *) 0);
		/* last_slice = update_quotas(last_slice, current_time); */

		/* process_commands(); */
		do_tick();

		DESCR_ITER(d) {
			/* process_output(d); */
			if (d->flags & DF_BOOTED) {
				goodbye_user(d);
				d->flags ^= DF_BOOTED;
				descr_close(d);
			}
		}

		if (global_dumpdone != 0) {
			wall(DUMPING_MESG);
			global_dumpdone = 0;
		}
		purge_free_frames();
		untouchprops_incremental(1);

		if (shutdown_flag)
			break;

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		next_slice = msec_add(last_slice, COMMAND_TIME_MSEC);
		slice_timeout = timeval_sub(next_slice, current_time);

		gettimeofday(&sel_in,NULL);
		readfds = readfds_new;
		int select_n = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);

		switch (select_n) {
		case -1:
			switch (errno) {
			case EAGAIN:
				return 0;
			case EINTR:
				continue;
			}

			perror("select");
			return -1;
		case 0:
			continue;
		}

		warn("select_n %d\n", select_n);
		gettimeofday(&sel_out,NULL);
		if (sel_out.tv_usec < sel_in.tv_usec) {
			sel_out.tv_usec += 1000000;
			sel_out.tv_sec -= 1;
		}
		sel_out.tv_usec -= sel_in.tv_usec;
		sel_out.tv_sec -= sel_in.tv_sec;
		sel_prof_idle_sec += sel_out.tv_sec;
		sel_prof_idle_usec += sel_out.tv_usec;
		if (sel_prof_idle_usec >= 1000000) {
			sel_prof_idle_usec -= 1000000;
			sel_prof_idle_sec += 1;
		}
		sel_prof_idle_use++;
		(void) time(&now);

		for (d = descr_map;
		     d < descr_map + FD_SETSIZE;
		     d++) {
			if (!FD_ISSET(d->fd, &readfds))
				continue;

			if (d->fd == sockfd)
				descr_new();
			else 
				descr_read(d);

			/* FD_CLR(d->fd, &readfds); */

			/* if (queue_size(&d->inband.output)) */
			/* 	FD_SET(d->fd, &writefds); */
		}
	}

	/* End of the player processing loop */

	(void) time(&now);
	add_property((dbref) 0, "_sys/lastdumptime", NULL, (int) now);
	add_property((dbref) 0, "_sys/shutdowntime", NULL, (int) now);
	return 0;
}

void
wall_wizards(const char *msg)
{
	descr_t *d;
	char buf[BUFFER_LEN + 2];

	strlcpy(buf, msg, sizeof(buf));
	strlcat(buf, "\r\n", sizeof(buf));

	DESCR_ITER(d)
		if (Wizard(d->player))
			descr_inband(d, buf);
}

int
make_socket(int port)
{
	int opt;
	struct sockaddr_in server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("creating stream socket");
		exit(3);
	}

	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	/*
	opt = 240;
	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	*/

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding stream socket");
		close(sockfd);
		exit(4);
	}

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_SET(sockfd, &readfds);
	descr_map[0].fd = sockfd;

	listen(sockfd, 5);
	return sockfd;
}

int
descr_inband(descr_t *d, const char *s)
{
	/* warn("descr_inband %d %s", d->fd, s); */
	return descr_write(d, s, strlen(s));
}

void
parse_connect(const char *msg, char *command, char *user, char *pass)
{
	char *p;

	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = command;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = tolower(*msg++);
	*p = '\0';
	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = user;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = *msg++;
	*p = '\0';
	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = pass;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = *msg++;
	*p = '\0';
}

int
boot_off(dbref player)
{
    int* darr;
    int dcount;
	descr_t *last = NULL;

	darr = get_player_descrs(player, &dcount);
	if (darr) {
        last = descrdata_by_descr(darr[0]);
	}

	if (last) {
		last->flags |= DF_BOOTED;
		/* descr_close(last); */
		return 1;
	}
	return 0;
}

void
boot_player_off(dbref player)
{
    int di;
    int* darr;
    int dcount;
    descr_t *d;
 
	darr = get_player_descrs(player, &dcount);
    for (di = 0; di < dcount; di++) {
        d = descrdata_by_descr(darr[di]);
        if (d) {
            d->flags = DF_BOOTED;
        }
    }
}

void
close_sockets(const char *msg) {
	descr_t *d;

	DESCR_ITER(d) {
		forget_player_descr(d->player, d->fd);
		if (shutdown(d->fd, 2) < 0)
			perror("shutdown");
		close(d->fd);
	}
}

void
emergency_shutdown(void)
{
	close_sockets("\r\nEmergency shutdown due to server crash.");
}

void
dump_users(descr_t *e, char *user)
{
	descr_t *d;
	int wizard;
	time_t now;
	char buf[2048];
	char pbuf[64];
	char secchar = ' ';

	wizard = (e->flags & DF_CONNECTED) && Wizard(e->player);

	while (*user && (isspace(*user) || *user == '*'))
		user++;

	if (wizard)
		/* S/he is connected and not quelled. Okay; log it. */
		warn("WIZ: %s(%d) in %s(%d):  %s", NAME(e->player),
					(int) e->player, NAME(DBFETCH(e->player)->location),
					(int) DBFETCH(e->player)->location, "WHO");

	if (!*user)
		user = NULL;

	(void) time(&now);
	if (wizard) {
		descr_inband(e, "Player Name                Location     On For Idle   Host\r\n");
	} else {
		descr_inband(e, "Player Name           On For Idle\r\n");
	}

	DESCR_ITER(d) if ((
			   !WHO_HIDES_DARK || wizard
			   || !(FLAGS(d->player) & DARK))
			  && (!user || string_prefix(NAME(d->player), user))) {

		secchar = ' ';

		if (wizard) {
			/* don't print flags, to save space */
			snprintf(pbuf, sizeof(pbuf), "%.*s(#%d)", PLAYER_NAME_LIMIT + 1,
				 NAME(d->player), (int) d->player);
			snprintf(buf, sizeof(buf),
				 "%-*s [%6d] %10s %4s%c%c\r\n",
				 PLAYER_NAME_LIMIT + 10, pbuf,
				 (int) DBFETCH(d->player)->location,
				 time_format_1(now - d->connected_at),
				 time_format_2(now - d->last_time),
				 ((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
				 secchar);
		} else {
			snprintf(buf, sizeof(buf), "%-*s %10s %4s%c%c\r\n",
				 (int)(PLAYER_NAME_LIMIT + 1),
				 NAME(d->player),
				 time_format_1(now - d->connected_at),
				 time_format_2(now - d->last_time),
				 ((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
				 secchar);
		}
	}

	descr_inband(e, buf);
}

char *
time_format_1(long dt)
{
	register struct tm *delta;
	static char buf[64];

	delta = gmtime((time_t *) &dt);
	if (delta->tm_yday > 0)
		snprintf(buf, sizeof(buf), "%dd %02d:%02d", delta->tm_yday, delta->tm_hour, delta->tm_min);
	else
		snprintf(buf, sizeof(buf), "%02d:%02d", delta->tm_hour, delta->tm_min);
	return buf;
}

char *
time_format_2(long dt)
{
	register struct tm *delta;
	static char buf[64];

	delta = gmtime((time_t *) &dt);
	if (delta->tm_yday > 0)
		snprintf(buf, sizeof(buf), "%dd", delta->tm_yday);
	else if (delta->tm_hour > 0)
		snprintf(buf, sizeof(buf), "%dh", delta->tm_hour);
	else if (delta->tm_min > 0)
		snprintf(buf, sizeof(buf), "%dm", delta->tm_min);
	else
		snprintf(buf, sizeof(buf), "%ds", delta->tm_sec);
	return buf;
}

void
announce_puppets(dbref player, const char *msg, const char *prop)
{
	dbref what, where;
	const char *ptr, *msg2;
	char buf[BUFFER_LEN];

	for (what = 0; what < db_top; what++) {
		if (Typeof(what) == TYPE_THING && (FLAGS(what) & ZOMBIE)) {
			if (OWNER(what) == player) {
				where = getloc(what);
				if ((!Dark(where)) && (!Dark(player)) && (!Dark(what))) {
					msg2 = msg;
					if ((ptr = (char *) get_property_class(what, prop)) && *ptr)
						msg2 = ptr;
					snprintf(buf, sizeof(buf), "%.512s %.3000s", NAME(what), msg2);
					notify_except(DBFETCH(where)->contents, what, buf, what);
				}
			}
		}
	}
}

void
announce_connect(descr_t *d, dbref player)
{
	dbref loc;
	char buf[BUFFER_LEN];
	struct match_data md;
	dbref exit;
	int descr = d->fd;

	if ((loc = getloc(player)) == NOTHING)
		return;

	if ((!Dark(player)) && (!Dark(loc))) {
		snprintf(buf, sizeof(buf), "%s has connected.", NAME(player));
		notify_except(DBFETCH(loc)->contents, player, buf, player);
	}

	exit = NOTHING;
	if (PLAYER_DESCRCOUNT(player)) {
		init_match(descr, player, "connect", TYPE_EXIT, &md);	/* match for connect */
		md.match_level = 1;
		match_all_exits(&md);
		exit = match_result(&md);
		if (exit == AMBIGUOUS)
			exit = NOTHING;
	}

	if (exit == NOTHING || !(FLAGS(exit) & STICKY)) {
		if (can_move(descr, player, AUTOLOOK_CMD, 1)) {
			do_move(descr, player, AUTOLOOK_CMD, 1);
		} else {
			do_look_around(descr, player);
		}
	}

	/*
	 * See if there's a connect action.  If so, and the player is the first to
	 * connect, send the player through it.  If the connect action is set
	 * sticky, then suppress the normal look-around.
	 */

	if (exit != NOTHING)
		do_move(descr, player, "connect", 1);

	if (PLAYER_DESCRCOUNT(player) == 1) {
		announce_puppets(player, "wakes up.", "_/pcon");
	}

	/* queue up all _connect programs referred to by properties */
	envpropqueue(descr, player, getloc(player), NOTHING, player, NOTHING,
				 "_connect", "Connect", 1, 1);
	envpropqueue(descr, player, getloc(player), NOTHING, player, NOTHING,
				 "_oconnect", "Oconnect", 1, 0);

	ts_useobject(player);
	return;
}

void
announce_disconnect(descr_t *d)
{
	dbref player = d->player;
	dbref loc;
	char buf[BUFFER_LEN];

	if ((loc = getloc(player)) == NOTHING)
		return;

	if ((!Dark(player)) && (!Dark(loc))) {
		snprintf(buf, sizeof(buf), "%s has disconnected.", NAME(player));
		notify_except(DBFETCH(loc)->contents, player, buf, player);
	}

	/* trigger local disconnect action */
	if (PLAYER_DESCRCOUNT(player) == 1) {
		if (can_move(d->fd, player, "disconnect", 1)) {
			do_move(d->fd, player, "disconnect", 1);
		}
		announce_puppets(player, "falls asleep.", "_/pdcon");
	}

	d->flags = 0;
	d->player = NOTHING;

    forget_player_descr(player, d->fd);
    /* update_desc_count_table(); */

	/* queue up all _connect programs referred to by properties */
	envpropqueue(d->fd, player, getloc(player), NOTHING, player, NOTHING,
				 "_disconnect", "Disconnect", 1, 1);
	envpropqueue(d->fd, player, getloc(player), NOTHING, player, NOTHING,
				 "_odisconnect", "Odisconnect", 1, 0);

	ts_lastuseobject(player);
	DBDIRTY(player);
}

/***** O(1) Connection Optimizations *****/

/* void */
/* update_desc_count_table() */
/* { */
/* 	int c; */
/* 	descr_t *d; */

/* 	current_descr_count = 0; */
/* 	for (c = 0, d = descriptor_list; d; d = d->next) */
/* 	{ */
/* 		if (!(d->flags & DF_CONNECTED)) */
/* 			continue; */

/* 		d->con_number = c + 1; */
/* 		descr_count_table[c++] = d; */
/* 		current_descr_count++; */
/* 	} */
/* } */

int*
get_player_descrs(dbref player, int* count)
{
	int* darr;

	if (Typeof(player) == TYPE_PLAYER) {
		*count = PLAYER_DESCRCOUNT(player);
	    darr = PLAYER_DESCRS(player);
		if (!darr) {
			*count = 0;
		}
		return darr;
	} else {
		*count = 0;
		return NULL;
	}
}

void
remember_player_descr(dbref player, int descr)
{
	int  count = 0;
	int* arr   = NULL;

	if (Typeof(player) != TYPE_PLAYER)
		return;

	count = PLAYER_DESCRCOUNT(player);
	arr = PLAYER_DESCRS(player);

	if (!arr) {
		arr = (int*)malloc(sizeof(int));
		arr[0] = descr;
		count = 1;
	} else {
		arr = (int*)realloc(arr,sizeof(int) * (count+1));
		arr[count] = descr;
		count++;
	}
	PLAYER_SET_DESCRCOUNT(player, count);
	PLAYER_SET_DESCRS(player, arr);
}

void
forget_player_descr(dbref player, int descr)
{
	int  count = 0;
	int* arr   = NULL;

	if (Typeof(player) != TYPE_PLAYER)
		return;

	count = PLAYER_DESCRCOUNT(player);
	arr = PLAYER_DESCRS(player);

	if (!arr) {
		count = 0;
	} else if (count > 1) {
		int src, dest;
		for (src = dest = 0; src < count; src++) {
			if (arr[src] != descr) {
				if (src != dest) {
					arr[dest] = arr[src];
				}
				dest++;
			}
		}
		if (dest != count) {
			count = dest;
			arr = (int*)realloc(arr,sizeof(int) * count);
		}
	} else {
		free((void*)arr);
		arr = NULL;
		count = 0;
	}
	PLAYER_SET_DESCRCOUNT(player, count);
	PLAYER_SET_DESCRS(player, arr);
}

descr_t *
descrdata_by_descr(int c)
{
	if (c >= FD_SETSIZE || c < 0)
		return NULL;
	else
		return &descr_map[c];
}

/*** JME ***/

/*** Foxen ***/
int
least_idle_player_descr(dbref who)
{
	descr_t *d;
	descr_t *best_d = NULL;
	int dcount, di;
	int* darr;
	long best_time = 0;

	darr = get_player_descrs(who, &dcount);
	for (di = 0; di < dcount; di++) {
		d = descrdata_by_descr(darr[di]);
		if (d && (!best_time || d->last_time > best_time)) {
			best_d = d;
			best_time = d->last_time;
		}
	}
	if (best_d) {
		return best_d->con_number;
	}
	return 0;
}

int
most_idle_player_descr(dbref who)
{
	descr_t *d;
	descr_t *best_d = NULL;
	int dcount, di;
	int* darr;
	long best_time = 0;

	darr = get_player_descrs(who, &dcount);
	for (di = 0; di < dcount; di++) {
		d = descrdata_by_descr(darr[di]);
		if (d && (!best_time || d->last_time < best_time)) {
			best_d = d;
			best_time = d->last_time;
		}
	}
	if (best_d) {
		return best_d->con_number;
	}
	return 0;
}

int
dbref_first_descr(dbref c)
{
	int dcount;
	int* darr;

	darr = get_player_descrs(c, &dcount);
	if (dcount > 0) {
		return darr[dcount - 1];
	} else {
		return -1;
	}
}

dbref
partial_pmatch(const char *name)
{
	descr_t *d;
	dbref last = NOTHING;

	d = descriptor_list;
	DESCR_ITER(d) if (string_prefix(NAME(d->player), name)) {
		if (last != NOTHING) {
			last = AMBIGUOUS;
			break;
		}
		last = d->player;
	}

	return last;
}

void
art(int descr, const char *art)
{
	FILE *f;
	char *ptr;
	char buf[BUFFER_LEN];
	descr_t *d;

	if (*art == '/' || strstr(art, "..")
	    || (!(string_prefix(art, "bird/")
		|| string_prefix(art, "fish/")
                || !strchr(art, '/'))))

		return;
	
        d = descrdata_by_descr(descr);

	/* descr_inband(d, "\r\n"); */

        /* if (!web_art(descr, art, buf, sizeof(buf))) */
        /*         return; */

	snprintf(buf, sizeof(buf), "../art/%s.txt", art);

	if ((f = fopen(buf, "rb")) == NULL) 
		return;

	while (fgets(buf, sizeof(buf) - 3, f)) {
		ptr = index(buf, '\n');
		if (ptr && ptr > buf && *(ptr - 1) != '\r') {
			*ptr++ = '\r';
			*ptr++ = '\n';
			*ptr++ = '\0';
		}
		descr_inband(d, buf);
	}

	fclose(f);
	descr_inband(d, "\r\n");
}

void
mob_welcome(descr_t *d)
{
	struct obj const *o = mob_obj_random();
	if (o) {
		CBUG(*o->name == '\0');
		descr_inband(d, o->name);
		descr_inband(d, "\r\n\r\n");
		art(d->fd, o->art);
                if (*o->description) {
                        if (*o->description != '\0')
                                descr_inband(d, o->description);
                        descr_inband(d, "\r\n\r\n");
                }
	}
}

void
welcome_user(descr_t *d)
{
	FILE *f;
	char *ptr;
	char buf[BUFFER_LEN];

        /* if (!web_support(d->fd)) { */
                if ((f = fopen(WELC_FILE, "rb")) == NULL) {
                        descr_inband(d, DEFAULT_WELCOME_MESSAGE);
                        perror("spit_file: welcome.txt");
                } else {
                        while (fgets(buf, sizeof(buf) - 3, f)) {
                                ptr = index(buf, '\n');
                                if (ptr && ptr > buf && *(ptr - 1) != '\r') {
                                        *ptr++ = '\r';
                                        *ptr++ = '\n';
                                        *ptr++ = '\0';
                                }
                                descr_inband(d, buf);
                        }
                        fclose(f);
                }
        /* } */

        mob_welcome(d);

	if (optflags & OPT_WIZONLY) {
		descr_inband(d, "## The game is currently in maintenance mode, and only wizards will be able to connect.\r\n");
	}
#if PLAYERMAX
	else if (con_players_curr >= PLAYERMAX_LIMIT) {
		if (PLAYERMAX_WARNMESG && *PLAYERMAX_WARNMESG) {
			descr_inband(d, PLAYERMAX_WARNMESG);
			descr_inband(d, "\r\n");
		}
	}
#endif
}

void
dump_status(void)
{
	descr_t *d;
	time_t now;
	char buf[BUFFER_LEN];

	(void) time(&now);
	warn("STATUS REPORT:");
	DESCR_ITER(d) snprintf(buf, sizeof(buf),
			       "PLAYING fd %d player %s(%d) %s.\n",
			       d->fd, NAME(d->player), d->player,
			       (d->last_time) ? "idle %d seconds" : "never used");
}

static const char *interface_c_version = "$RCSfile$ $Revision: 1.127 $";
const char *get_interface_c_version(void) { return interface_c_version; }

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
#include "command.h"
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
#include "hash.h"

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
};

struct ws {
	int flags;
	unsigned ip;
	unsigned old;
};

typedef struct descr_st {
	int fd, flags;
	dbref player;
} descr_t;

#define CMD_HASH_SIZE 512

static hash_tab cmds_hashed[CMD_HASH_SIZE];

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
		.name = "describe",
		.cb = &do_describe,
	}, {
		.name = "doing",
		.cb = &do_doing,
	}, {
		.name = "drop_message",
		.cb = &do_drop_message,
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

		/* do_showextver(player); */
	}, {
		.name = "stats",
		.cb = &do_stats,
	}, {
		.name = "success",
		.cb = &do_success,
	}, {
		.name = "sweep",
		.cb = &do_sweep,
	}, {
		.name = "teleport",
		.cb = &do_teleport,
	}, {
		.name = "toad",
		.cb = &do_toad,
	}, {
		.name = "trace",
		.cb = &do_trace,
	}, {
		.name = "unbless",
		.cb = &do_unbless,
	}, {
		.name = "unlock",
		.cb = &do_unlock,
	}, {
		.name = "usage",
		.cb = &do_usage,
	}, {
		.name = "wall",
		.cb = &do_wall,
	}, {
		.name = "buy",
		.cb = &do_buy,
	}, {
		.name = "drink",
		.cb = &do_drink,
	}, {
		.name = "drop",
		.cb = &do_drop,
	}, {
		.name = "eat",
		.cb = &do_eat,
	}, {
		.name = "examine",
		.cb = &do_examine,
	}, {
		.name = "equip",
		.cb = &do_equip,
	}, {
		.name = "fill",
		.cb = &do_fill,
	}, {
		.name = "get",
		.cb = &do_get,
	}, {
		.name = "give",
		.cb = &do_give,
	}, {
		.name = "gripe",
		.cb = &do_gripe,
	}, {
		.name = "help",
		.cb = &do_help,
	}, {
		.name = "inventory",
		.cb = &do_inventory,
	}, {
		.name = "info",
		.cb = &do_info,
	}, {
		.name = "kill",
		.cb = &do_kill,
	}, {
		.name = "look",
		.cb = &do_look_at,
	}, {
		.name = "view",
		.cb = &do_view,
	/* }, { */
	/* 	.name = "meme", */
	/* 	.cb = &do_meme, */
	/* 	/1* /2* do_meme(descr, player, arg1); *2/ *1/ */
	}, {
		.name = "man",
		.cb = &do_man,
	}, {
		.name = "news",
		.cb = &do_news,
	}, {
		.name = "page",
		.cb = &do_page,
	}, {
		.name = "pose",
		.cb = &do_pose,
	}, {
		.name = "rob",
		.cb = &do_rob,
	}, {
		.name = "say",
		.cb = &do_say,
	}, {
		.name = "score",
		.cb = &do_score,
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

fd_set readfds, activefds, writefds;

int shutdown_flag = 0;

static int sockfd;
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

	for (; p < input + len && *p != ' '; p++)
		;

	if (*p == ' ') {
		*p = '\0';
		cmd.argv[cmd.argc] = p + 1;
		cmd.argc ++;

		for (; p < input + len; p++) {
			if (*p != '=')
				continue;

			*p = '\0';

			cmd.argv[cmd.argc] = p + 1;
			cmd.argc ++;
		}
	}

	for (int i = cmd.argc;
	     i < sizeof(cmd.argv) / sizeof(char *);
	     i++)

		cmd.argv[i] = "";

	return cmd;
}

core_command_t *
command_match(command_t *cmd) {
	hash_data *hd;

	if ((hd = find_hash(cmd->argv[0], cmds_hashed, CMD_HASH_SIZE)) == NULL)
		return NULL;

	return (core_command_t *) hd->pval;
}

command_t command_null(command_t *cmd) {
	command_t ret;
	ret.player = cmd->player;
	ret.fd = cmd->fd;
	ret.argc = 0;
	memset(ret.argv, '0', sizeof(ret.argv));
	return ret;
}

command_t command_new_null(int descr, ref_t player) {
	command_t ret;
	command_null(&ret);
	ret.player = player;
	ret.fd = descr;
	return ret;
}

static void
commands_init() {
	int i;

	for (i = 0; i < sizeof(cmds) / sizeof(core_command_t); i++) {
		hash_data hd;
		hd.pval = &cmds[i];

		add_hash(cmds[i].name, hd, cmds_hashed, CMD_HASH_SIZE);
	}
}

int shovechars();

short optflags = 0;

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

extern int sanity_violated;

void
close_sockets(const char *msg) {
	descr_t *d;

	DESCR_ITER(d) {
		/* forget_player_descr(d->player, d->fd); */
		if (shutdown(d->fd, 2) < 0)
			perror("shutdown");
		close(d->fd);
	}
}

void sig_shutdown(int i)
{
	warn("SHUTDOWN: via SIGNAL");
	shutdown_flag = 1;
}

int
init_game()
{
	FILE *f = fopen(STD_DB, "rb");

	if (f == NULL)
		return -1;

	db_free();
	srand(getpid());			/* init random number generator */

	if (db_read(f) < 0)
		return -1;

	return 0;
}

int
main(int argc, char **argv)
{
	register char c;

	memset(descr_map, 0, sizeof(descr_map));

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

	if (init_game() < 0) {
		warn("Couldn't load " STD_DB "!\n");
		return 2;
	}

	CBUG(map_init());
	commands_init();

	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGUSR1, SIG_DFL);
	signal(SIGTERM, sig_shutdown);

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

	FILE *f = fopen(STD_DB, "wb");

	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	db_write(f);
	fclose(f);
	sync();

	return 0;
}

int
descr_inband(int fd, const char *s)
{
	/* warn("descr_inband %d %s", d->fd, s); */
	return write(fd, s, strlen(s));
}

int
notify(dbref player, const char *msg)
{
	int retval = 0, fd;
	char buf[BUFFER_LEN + 2];
	int firstpass = 1;
	char *ptr1;
	const char *ptr2;

	/* CBUG(Typeof(player) != TYPE_PLAYER); */
	fd = PLAYER_FD(player);
	if (fd <= 0)
		return 0;

	/* descr_inband(fd, msg); */
	/* retval++; */

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

		descr_inband(fd, buf);
		if (firstpass)
			retval++;

		firstpass = 0;
	}
	return retval;
}

void
notifyf(dbref player, char *format, ...)
{
	va_list args;
	static char bufr[BUFFER_LEN];

	va_start(args, format);
	vsnprintf(bufr, sizeof(bufr), format, args);
	bufr[sizeof(bufr)-1] = '\0';
	notify(player, bufr);
	va_end(args);
}

void
wall(const char *msg)
{
	descr_t *d;
	char buf[BUFSIZ];

	strlcpy(buf, msg, sizeof(buf));
	strlcat(buf, "\r\n", sizeof(buf));

	DESCR_ITER(d) descr_inband(d->fd, buf);
}

void
mob_welcome(int fd)
{
	struct obj const *o = mob_obj_random();
	if (o) {
		CBUG(*o->name == '\0');
		descr_inband(fd, o->name);
		descr_inband(fd, "\r\n\r\n");
		art(fd, o->art);
                if (*o->description) {
                        if (*o->description != '\0')
                                descr_inband(fd, o->description);
                        descr_inband(fd, "\r\n\r\n");
                }
	}
}

void
welcome_user(int fd)
{
	FILE *f;
	char *ptr;
	char buf[BUFFER_LEN];

        /* if (!web_support(d->fd)) { */
                if ((f = fopen(WELC_FILE, "rb")) == NULL) {
                        descr_inband(fd, DEFAULT_WELCOME_MESSAGE);
                        perror("spit_file: welcome.txt");
                } else {
                        while (fgets(buf, sizeof(buf) - 3, f)) {
                                ptr = index(buf, '\n');
                                if (ptr && ptr > buf && *(ptr - 1) != '\r') {
                                        *ptr++ = '\r';
                                        *ptr++ = '\n';
                                        *ptr++ = '\0';
                                }
                                descr_inband(fd, buf);
                        }
                        fclose(f);
                }
        /* } */

        mob_welcome(fd);

	if (optflags & OPT_WIZONLY) {
		descr_inband(fd, "## The game is currently in maintenance mode, and only wizards will be able to connect.\r\n");
	}
#if PLAYERMAX
	else if (con_players_curr >= PLAYERMAX_LIMIT) {
		if (PLAYERMAX_WARNMESG && *PLAYERMAX_WARNMESG) {
			descr_inband(fd, PLAYERMAX_WARNMESG);
			descr_inband(fd, "\r\n");
		}
	}
#endif
}

int
auth(command_t *cmd)
{
	int fd = cmd->fd;
	char *user = cmd->argv[1];
	char *password = cmd->argv[2];
        int created = 0;
        dbref player = connect_player(user, password);
	descr_t *d = &descr_map[fd];

        if (player == NOTHING) {
                player = create_player(user, password);

                if (player == NOTHING) {
                        descr_inband(fd, "Either there is already a player with"
				     " that name, or that name is illegal.\r\n");
                        return 1;
                }

                created = 1;
                mob_put(player);
        } else {
		if (PLAYER_FD(player) > 0) {
			descr_inband(fd, "That player is already connected.\r\n");
			return 1;
		}
	}

        d->flags = DF_CONNECTED;
        d->player = cmd->player = player;
	CBUG(d->fd != fd);
	PLAYER_FD(player) = fd;
        spit_file(player, MOTD_FILE);
	do_look_around(cmd);
	do_view(cmd);
        return 0;
}

static inline void
do_v(command_t *cmd, char const *cmdstr)
{
	int ofs = 1;
	char const *s = cmdstr;

	for (; *s && ofs > 0; s += ofs) {
		ofs = geo_v(cmd, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(cmd, s);
	}
}

void
command_process(command_t *cmd)
{
	if (cmd->argc < 1)
		return;

	dbref player = cmd->player;
	int descr = cmd->fd;

	command_debug(cmd, "command_process");
	/* warn("command_process %s\n", command); */

        // set current descriptor (needed for death)
        CBUG(GETLID(player) < 0);
        mobi_t *liv = MOBI(player);
        liv->descr = descr;

	core_command_t *cmd_i = command_match(cmd);

	pos_t pos;
	map_where(pos, getloc(player));

	if (cmd_i)
		cmd_i->cb(cmd);
	else
		do_v(cmd, cmd->argv[0]);

	pos_t pos2;
	map_where(pos2, getloc(player));
	if (MORTON_READ(pos2) != MORTON_READ(pos))
		do_view(cmd);
}

int
descr_read(descr_t *d)
{
	char buf[BUFFER_LEN];

	int ret = read(d->fd, buf, BUFFER_LEN);
	switch (ret) {
	case -1:
		if (errno == EAGAIN)
			return 0;

		/* perror("descr_read"); */
		BUG("descr_read %s", strerror(errno));
	case 0:
	case 1:
		return -1;
	}
	ret -= 2;
	buf[ret] = '\0';

	command_t cmd = command_new(d, buf, ret);

	if (!cmd.argc)
		return 0;

	if (d->flags & DF_CONNECTED) {
		command_process(&cmd);
		return 0;
	}

	if (cmd.argc != 3)
		return 0;

	if (*cmd.argv[0] == 'c')
		auth(&cmd);

	return ret;
}

descr_t *
descr_new()
{
	struct sockaddr_in addr;
	socklen_t addr_len = (socklen_t)sizeof(addr);
	int fd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);
	descr_t *d;

	if (fd <= 0) {
		perror("descr_new");
		return NULL;
	}

	warn("accept %d\n", fd);

	FD_SET(fd, &activefds);

	d = &descr_map[fd];
	memset(d, 0, sizeof(descr_t));
	d->fd = fd;
	d->player = -1;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		perror("make_nonblocking: fcntl");
		abort();
	}

	welcome_user(fd);
	return d;
}

void
descr_close(descr_t *d)
{
	if (d->flags & DF_CONNECTED) {
		warn("%s(%d) disconnects on fd %d\n",
		     NAME(d->player), d->player, d->fd);
		PLAYER_SP(d->player)->fd = -1;
		DBDIRTY(d->player);
		d->flags = 0;
		d->player = NOTHING;
	} else
		warn("%d never connected\n", d->fd);

	shutdown(d->fd, 2);
	close(d->fd);
	FD_CLR(d->fd, &activefds);
	if (d)
		memset(d, 0, sizeof(descr_t));
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
shovechars()
{
	struct timeval timeout;
	descr_t *d;
	int avail_descriptors, i;

	sockfd = make_socket(TINYPORT);

	if (sockfd <= 0) {
		perror("make_socket");
		return sockfd;
	}

	FD_SET(sockfd, &activefds);

	avail_descriptors = sysconf(_SC_OPEN_MAX) - 5;

	mob_init();

	/* Daemonize */
	if ((optflags & OPT_DETACH) && daemon(1, 1) != 0)
		_exit(0);

	/* And here, we do the actual player-interaction loop */

	while (shutdown_flag == 0) {
		/* process_commands(); */
		mob_update();
		geo_update();

		untouchprops_incremental(1);

		if (shutdown_flag)
			break;

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		readfds = activefds;
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

		for (d = descr_map, i = 0;
		     d < descr_map + FD_SETSIZE;
		     d++, i++) {
			if (!FD_ISSET(i, &readfds))
				continue;

			if (i == sockfd) {
				descr_new();
			} else if (descr_read(d) < 0)
				descr_close(d);
		}
	}

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
			descr_inband(d->fd, buf);
}

int
boot_off(dbref player)
{
	int fd = PLAYER_SP(player)->fd;

	if (fd > 0) {
		descr_close(&descr_map[fd]);
		return 1;
	}

	return 0;
}

void
boot_player_off(dbref player)
{
	boot_off(player);
}

void
emergency_shutdown(void)
{
	close_sockets("\r\nEmergency shutdown due to server crash.");
}

void
art(int fd, const char *art)
{
	FILE *f;
	char buf[BUFFER_LEN];

	if (*art == '/' || strstr(art, "..")
	    || (!(string_prefix(art, "bird/")
		|| string_prefix(art, "fish/")
                || !strchr(art, '/'))))

		return;
	
	snprintf(buf, sizeof(buf), "../art/%s.txt", art);

	if ((f = fopen(buf, "rb")) == NULL) 
		return;

	while (fgets(buf, sizeof(buf) - 3, f))
		descr_inband(fd, buf);

	fclose(f);
	descr_inband(fd, "\r\n");
}

static const char *interface_c_version = "$RCSfile$ $Revision: 1.127 $";
const char *get_interface_c_version(void) { return interface_c_version; }

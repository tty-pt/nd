/* $Header$ */

/* Copyright 1992-2001 by Fuzzball Software */
/* Consider this code protected under the GNU public license, with explicit
 * permission to distribute when linked against openSSL. */

#include "copyright.h"
#include "config.h"
#include "match.h"
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

#include <err.h>

#include "descr.h"

#ifdef CONFIG_SECURE
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/socket.h>
#endif
#include <pwd.h>

#ifdef __OpenBSD__
#include <db4/db.h>
#else
#include <db.h>
#endif

#include "mdb.h"
#include "interface.h"
#include "command.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "mcp.h"
#include "externs.h"
#include "interp.h"
#include "kill.h"
#include "search.h"
#include "view.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#include "hash.h"
#include "ws.h"

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
	DF_WEBSOCKET = 4,
};

#define CMD_HASH_SIZE 512

static hash_tab cmds_hashed[CMD_HASH_SIZE];

#ifdef CONFIG_SECURE
SSL_CTX *sslctx;
#endif

void do_bio(command_t *cmd);
extern void do_auth(command_t *cmd);
extern void do_httpget(command_t *cmd);

core_command_t cmds[] = {
	{
		.name = "auth",
		.cb = &do_auth,
		.flags = CF_NOAUTH,
	}, {
		.name = "GET",
		.cb = &do_httpget,
		.flags = CF_NOAUTH | CF_NOARGS,
	}, {
		.name = "advitam",
		.cb = &do_advitam,
	}, {
		.name = "bless",
		.cb = &do_bless,
	}, {
		.name = "bio",
		.cb = &do_bio,
	}, {
		.name = "boot",
		.cb = &do_boot,
	}, {
		.name = "chown",
		.cb = &do_chown,
	}, {
		.name = "clone",
		.cb = &do_clone,
	}, {
		.name = "contents",
		.cb = &do_contents,
	}, {
		.name = "create",
		.cb = &do_create,
	}, {
		.name = "describe",
		.cb = &do_describe,
		/* sane_dump_object(player, arg1); /1* examine *1/ */
	}, {
		.name = "find",
		.cb = &do_find,
	/* }, { */
	/* 	.name = "force", */
	/* 	.cb = &do_force, */
	}, {
		.name = "heal",
		.cb = &do_heal,
	}, {
		.name = "name",
		.cb = &do_name,
	}, {
		.name = "owned",
		.cb = &do_owned,
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
		.name = "sweep",
		.cb = &do_sweep,
	}, {
		.name = "talk",
		.cb = &do_talk,
	}, {
		.name = "answer",
		.cb = &do_answer,
	}, {
		.name = "teleport",
		.cb = &do_teleport,
	}, {
		.name = "toad",
		.cb = &do_toad,
	}, {
		.name = "unbless",
		.cb = &do_unbless,
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
		.name = "drop",
		.cb = &do_drop,
	}, {
		.name = "consume",
		.cb = &do_consume,
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
	}, {
		.name = "meme",
		.cb = &do_meme,
	}, {
		.name = "man",
		.cb = &do_man,
	}, {
		.name = "news",
		.cb = &do_news,
	}, {
		.name = "pose",
		.cb = &do_pose,
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
	},
};

time_t start, now, tick;

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

	if (!*p || !isprint(*p))
		return cmd;

	cmd.argv[0] = p;
	cmd.argc++;

	for (; p < input + len && *p != ' '; p++)
		;

	if (*p == ' ') {
		*p = '\0';
		cmd.argv[cmd.argc] = p + 1;
		core_command_t *cmd_i = command_match(&cmd);
		cmd.argc ++;

		if (cmd_i && cmd_i->flags & CF_NOARGS)
			goto cleanup;

		for (; p < input + len; p++) {
			if (*p != '=')
				continue;

			*p = '\0';

			cmd.argv[cmd.argc] = p + 1;
			cmd.argc ++;
		}
	}

cleanup:
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
	fprintf(stderr, "Usage: %s [-dsSyWv?] [-C [path]]", prog);
	fprintf(stderr, "    Options:\n");
	fprintf(stderr, "        -C PATH   changes directory to PATH before starting up.\n");
	fprintf(stderr, "        -d        daemon mode\n");
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

	if (f == NULL) {
                warn("No such file");
                return -1;
        }

	db_free();
	srand(getpid());			/* init random number generator */

	if (db_read(f) < 0) {
                warn("Error in db_read");
		return -1;
        }

	return 0;
}

time_t get_tick() {
	return now - start;
}

int
main(int argc, char **argv)
{
	register char c;

	memset(descr_map, 0, sizeof(descr_map));

	while ((c = getopt(argc, argv, "dsyvS:")) != -1) {
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
			printf("0.0.1\n");
			break;
			
		default:
			show_program_usage(*argv);
			return 1;

		case '?':
			show_program_usage(*argv);
			return 0;
		}
	}

	if (geteuid())
		errx(1, "need root privileges");

#ifdef CONFIG_SECURE
	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	sslctx = SSL_CTX_new(TLS_server_method());
	SSL_CTX_set_ecdh_auto(sslctx, 1);
	SSL_CTX_use_certificate_file(sslctx, "/etc/ssl/tty.pt.crt" , SSL_FILETYPE_PEM);
	SSL_CTX_use_PrivateKey_file(sslctx, "/etc/ssl/private/tty.pt.key", SSL_FILETYPE_PEM);
	ERR_print_errors_fp(stderr);
#endif

	warn("INIT: TinyMUCK %s starting.", "version");
	warn("%s PID is: %d", argv[0], getpid());

	struct passwd *pw = getpwnam("www");
	if (pw == NULL)
		errx(1, "unknown user %s", "www");

	if (chroot("/var/www") != 0 || chdir("/htdocs/neverdark/game") != 0)
		err(1, "%s", "/var/www");

	if (setgroups(1, &pw->pw_gid) ||
			setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) ||
			setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid)) {
		fprintf(stderr, "%s: cannot drop privileges", __func__);
		exit(1);
	}

	if (init_game() < 0) {
		warn("Couldn't load " STD_DB "!\n");
		return 2;
	}

	int ret = map_init();
	if (ret) {
		warn("map_init %s", db_strerror(ret));
		exit(1);
	}
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
	/* return write(fd, s, strlen(s)); */
	size_t len = strlen(s);
	descr_t *d = &descr_map[fd];
	/* warn("descr_inband %d %s %d", d->fd, s, d->flags & DF_WEBSOCKET); */
	if (d->flags & DF_WEBSOCKET)
		return ws_write(d->fd, s, len);
	else
		return WRITE(d->fd, s, len);
}

int
notify(dbref player, const char *msg)
{
	int retval = 0, fd;
	char buf[BUFFER_LEN + 2];
	int firstpass = 1;
	char *ptr1;
	const char *ptr2;

	CBUG(Typeof(player) != TYPE_ENTITY);

	fd = ENTITY(player)->fd;
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
	struct object_skeleton const *o = mob_obj_random();
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

void
do_httpget(command_t *cmd)
{
	descr_t *d = &descr_map[cmd->fd];
	warn("httpget %d argc %d", cmd->fd, cmd->argc);
	if (ws_handshake(cmd->fd, cmd->argv[cmd->argc - 1])) {
		warn("websocket connection closed");
		return;
	}
	d->flags |= DF_WEBSOCKET;
	d->mcpframe.enabled = 1;
}

void
do_auth(command_t *cmd)
{
	char buf[BUFSIZ];
	int fd = cmd->fd;
	char *qsession = cmd->argv[1];
	FILE *fp;
	dbref player;
	descr_t *d = &descr_map[fd];

	snprintf(buf, sizeof(buf), "/sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp) {
		descr_inband(fd, "No such session\r\n");
		web_auth_fail(fd, 1);
		return;
	}

	fscanf(fp, "%s", buf);
	fclose(fp);

	warn("lookup_player '%s'\n", buf);

	player = lookup_player(buf);

	if (player == NOTHING) {
		player = create_player(buf);
		birth(player);
	} else if (ENTITY(player)->fd > 0) {
                descr_inband(fd, "That player is already connected.\r\n");
                web_auth_fail(fd, 2);
                return;
        }

	d->flags |= DF_CONNECTED;
	d->player = cmd->player = player;
	CBUG(d->fd != fd);
	ENTITY(player)->fd = fd;
	spit_file(player, MOTD_FILE);
        web_stats(player);
        web_bars(player);
        web_auth_success(fd, player);
        web_equipment(player);
	look_around(player);
	do_view(cmd);
}

static inline void
do_v(dbref player, char const *cmdstr)
{
	int ofs = 1;
	char const *s = cmdstr;

	for (; *s && ofs > 0; s += ofs) {
		ofs = geo_v(player, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(player, s);
	}
}

void
command_process(command_t *cmd)
{
	if (cmd->argc < 1)
		return;

	core_command_t *cmd_i = command_match(cmd);
	int descr = cmd->fd;

	descr_t *d = &descr_map[descr];

	if (!(d->flags & DF_CONNECTED)) {
		if (cmd_i && (cmd_i->flags & CF_NOAUTH))
			cmd_i->cb(cmd);
		return;
	}

	dbref player = cmd->player;

	command_debug(cmd, "command_process");

	// set current descriptor (needed for death)
	/* if (GETLID(player) < 0) */
	/* 	mob_put(player); */
	struct entity *mob = ENTITY(player);
	mob->fd = descr;

	pos_t pos;
	map_where(pos, getloc(player));

	if (cmd_i)
		cmd_i->cb(cmd);
	else
		do_v(cmd->player, cmd->argv[0]);

	pos_t pos2;
	map_where(pos2, getloc(player));
	if (MORTON_READ(pos2) != MORTON_READ(pos))
		do_view(cmd);
}

int
descr_read(descr_t *d)
{
	char buf[BUFFER_LEN];
	int ret;

	if (d->flags & DF_WEBSOCKET) {
		ret = ws_read(d->fd, buf, sizeof(buf));
		switch (ret) {
		case -1:
			if (errno == EAGAIN)
				return 0;

			perror("descr_read ws ");
		case 0: return -1;
		}
		ret -= 1;
	} else {
		ret = READ(d->fd, buf, BUFFER_LEN);
		switch (ret) {
		case -1:
			if (errno == EAGAIN)
				return 0;

			perror("descr_read");
			/* BUG("descr_read %s", strerror(errno)); */
		case 0:
		case 1:
			return -1;
		}
		ret -= 2;
	}
	buf[ret] = '\0';

	command_t cmd = command_new(d, buf, ret);

	if (!cmd.argc)
		return 0;

	command_process(&cmd);

	/* if (cmd.argc != 3) */
	/* 	return 0; */

	/* if (*cmd.argv[0] == 'c') */
	/* 	auth(&cmd); */

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

#ifdef CONFIG_SECURE
	d->cSSL = SSL_new(sslctx);
	SSL_set_fd(d->cSSL, fd);

	if (SSL_accept(d->cSSL) <= 0) {
		ERR_print_errors_fp(stderr);
		SSL_shutdown(d->cSSL);
		SSL_free(d->cSSL);
		close(fd);
		return NULL;
	}
#endif

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
		perror("make_nonblocking: fcntl");
		abort();
	}

	mcp_frame_init(&d->mcpframe, d);
	/* mcp_negotiation_start(&d->mcpframe); */
	/* welcome_user(fd); */
	return d;
}

void
SendText(McpFrame * mfr, const char *text)
{
	descr_t *d = mfr->descriptor;
	descr_inband(d->fd, text);
}

int
mcpframe_to_user(McpFrame * ptr)
{
	return ((descr_t *) ptr->descriptor)->player;
}

void
descr_close(descr_t *d)
{
	if (d->flags & DF_CONNECTED) {
		warn("%s(%d) disconnects on fd %d\n",
		     NAME(d->player), d->player, d->fd);
		dbref last_observed = ENTITY(d->player)->last_observed;
		if (last_observed != NOTHING)
			db_obs_remove(last_observed, d->player);

		ENTITY(d->player)->fd = -1;
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

time_t get_now() {
	time_t now;
	time(&now);
	return now;
}

int
shovechars()
{
	struct timeval timeout;
	descr_t *d;
	int i;

	sockfd = make_socket(TINYPORT);

	if (sockfd <= 0) {
		perror("make_socket");
		return sockfd;
	}

	FD_SET(sockfd, &activefds);

	/* Daemonize */
	if ((optflags & OPT_DETACH) && daemon(1, 1) != 0)
		_exit(0);

	start = get_now();
	/* And here, we do the actual player-interaction loop */

	while (shutdown_flag == 0) {
		/* process_commands(); */
		now = get_now();
                tick = now - start;
		objects_update();
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
	int fd = ENTITY(player)->fd;

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
	
        size_t len = strlen(art);

        if (!strcmp(art + len - 3, "txt")) {
                snprintf(buf, sizeof(buf), "../art/%s", art);

                if ((f = fopen(buf, "rb")) == NULL) 
                        return;

                while (fgets(buf, sizeof(buf) - 3, f))
                        descr_inband(fd, buf);

                fclose(f);
                descr_inband(fd, "\r\n");
        } else
                web_art(fd, art);
}

McpFrame *
descr_mcpframe(int fd)
{
	descr_t *d = &descr_map[fd];
	return &d->mcpframe;
}

static const char *interface_c_version = "$RCSfile$ $Revision: 1.127 $";
const char *get_interface_c_version(void) { return interface_c_version; }

/* Copyright 1992-2001 by Fuzzball Software */
/* Consider this code protected under the GNU public license, with explicit
 * permission to distribute when linked against openSSL. */

#include "io.h"

#include <string.h>
#ifndef __OpenBSD__
#include <bsd/string.h>
#endif


#include "copyright.h"
#include "object.h"
#include "config.h"

#include <sys/types.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

# define NEED_SOCKLEN_T
// do not include netinet6/in6.h directly, include netinet/in.h.  see RFC2553
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <openssl/ssl.h>

#include <err.h>

#ifdef CONFIG_SECURE
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sys/socket.h>
#endif
#include <pwd.h>
#include <grp.h>

#include "nddb.h"
#include "command.h"
#include "params.h"
#include "defaults.h"
#include "mcp.h"
#include "externs.h"
#include "map.h"
#include "view.h"
#include "mob.h"
#include "hash.h"
#include "ws.h"
#include "player.h"
#include "debug.h"
#include "utils.h"

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

enum opts {
	OPT_DETACH = 1,
};

DB *cmdsdb = NULL;

#ifdef CONFIG_SECURE
SSL_CTX *sslctx;
#endif

void do_bio(command_t *cmd);
extern void do_auth(command_t *cmd);
extern void do_avatar(command_t *cmd);
extern void do_httpget(command_t *cmd);
/* extern void do_gpt(command_t *cmd); */

int
carriage_return(char *output, char *input) {
	char *s = output, *i = input;
	int ret = 0;

	while (*i) {
		/* warn("i"); */
		if (*i == '\n') {
			/* warn("i"); */
			*s++ = '\r';
			ret ++;
		}
		*s = *i;
		s++;
		i++;
	}
	/* warn("\n"); */

	*s = '\0';

	return ret;
}

int twrite(int fd, const char *buf, size_t len);

char *
command(ENT *eplayer, char *prompt) {
	static char buf[BUFSIZ];
	static char carr[BUFSIZ * 2];
	struct popen2 child;
	ssize_t len = 0;
	int start = 1, cont = 0;
	CBUG(popen2(&child, prompt));
	close(child.in);
	if (descr_map[eplayer->fd].flags & DF_WEBSOCKET) {
		while ((len = read(child.out, buf, sizeof(buf) - 1)) > 0) {
			buf[len] = '\0';
			int cr = carriage_return(carr, buf);
			_ws_write(eplayer->fd, carr, len + cr - 1, start, cont, 0);
			start = 0;
			cont = 1;
		}
		_ws_write(eplayer->fd, "", 0, start, cont, 1);
	} else
		while ((len = read(child.out, buf, sizeof(buf) - 1)) > 0) {
			/* warn("READ %lu bytes\n", len); */
			buf[len] = '\0';
			int cr = carriage_return(carr, buf);
			twrite(eplayer->fd, carr, len + cr);
		}
	close(child.out);
	kill(child.pid, 0);
	return buf;
}

void
do_man(command_t *cmd) {
	char buf[BUFSIZ], *s;

	for (s = cmd->argv[1]; *s; s++) {
		if (isalnum(*s) || *s == '_')
			continue;
		notify(&cmd->player->sp.entity, "Hacking is invalid\n");
		return;
	}

#ifdef __OpenBSD__
	snprintf(buf, sizeof(buf), "man -c %s", cmd->argv[1]);
#else
	snprintf(buf, sizeof(buf), "man %s", cmd->argv[1]);
#endif
	warn("executing man '%s'\n", buf);
	command(&cmd->player->sp.entity, buf);
	/* res = carriage_return(command(buf)); */
	/* notify(&cmd->player->sp.entity, res); */
}

void
do_diff(command_t *cmd) {
	// TODO ssh in chroot?
	/* descr_inband(cmd->fd, command("git fetch origin master; git diff origin/master")); */

	// the fetch is not needed I think if it is
	// not updated we can do it manually or display
	// the status or the commit. It adds a lot of overhead to
	// fetch on every diff. Perhaps later, when the repo is
	// local, fetch will make no sense at all
	command(&cmd->player->sp.entity, "git diff origin/master");
}


core_command_t cmds[] = {
	{
		.name = "auth",
		.cb = &do_auth,
		.flags = CF_NOAUTH,
	}, {
		.name = "avatar",
		.cb = &do_avatar,
	}, {
		.name = "GET",
		.cb = &do_httpget,
		.flags = CF_NOAUTH | CF_NOARGS,
	/* }, { */
	/* 	.name = "gpt", */
	/* 	.cb = &do_gpt, */
	}, {
		.name = "advitam",
		.cb = &do_advitam,
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
		.name = "diff",
		.cb = &do_diff,
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
		.name = "recycle",
		.cb = &do_recycle,
	}, {
		.name = "teleport",
		.cb = &do_teleport,
	}, {
		.name = "toad",
		.cb = &do_toad,
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
		.name = "man",
		.cb = &do_man,
	}, {
		.name = "pose",
		.cb = &do_pose,
	}, {
		.name = "reroll",
		.cb = &do_reroll,
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

long long mstart, mnow, mlast, dt, macc = 0;

fd_set readfds, activefds, writefds;

int shutdown_flag = 0;

static int sockfd;
descr_t descr_map[FD_SETSIZE];

void
command_debug(command_t *cmd, char *label)
{
	char **arg;

	warn("acommand_debug '%s' %d", label, cmd->argc);
	for (arg = cmd->argv;
	     arg < cmd->argv + cmd->argc;
	     arg++)
	{
		warn(" '%s'", *arg);
	}
	warn("\n");
}

static inline core_command_t *
command_match(command_t *cmd) {
	return hash_get(cmdsdb, cmd->argv[0]);
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

static void
commands_init() {
	int i;

	for (i = 0; i < sizeof(cmds) / sizeof(core_command_t); i++)
		hash_put(cmdsdb, cmds[i].name, &cmds[i]);
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
	fprintf(stderr, "        -s        load db, then enter the interactive sanity editor.\n");
	fprintf(stderr, "        -W        only allow wizards to login.\n");
	fprintf(stderr, "        -v        display this server's version.\n");
	fprintf(stderr, "        -?        display this message.\n");
	exit(1);
}

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
	warn("SHUTDOWN: via SIGNAL\n");
	shutdown_flag = 1;
}

int
init_game()
{
	const char *name = "std.db";
	FILE *f;

	if (access(STD_DB, F_OK) != 0)
		name = STD_DB_OK;

	f = fopen(name, "rb");

	if (f == NULL) {
                warn("No such file\n");
                return -1;
        }

	objects_free();
	srand(getpid());			/* init random number generator */

	CBUG(!objects_read(f));

	return 0;
}

time_t get_tick() {
	return (mnow - mstart) / 1000;
}

int
main(int argc, char **argv)
{
	register char c;

	memset(descr_map, 0, sizeof(descr_map));

	while ((c = getopt(argc, argv, "dv")) != -1) {
		switch (c) {
		case 'd':
			optflags |= OPT_DETACH;
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


#ifdef CONFIG_ROOT
	if (geteuid())
		errx(1, "need root privileges");
#endif

#ifdef CONFIG_CHROOT
	char *user = "www";
	struct passwd *pw = getpwnam(user);

	if (pw == NULL)
		errx(1, "unknown user %s", user);

	if (chroot("/var/www") != 0 || chdir("/nd/game") != 0)
		err(1, "%s", "/var/www");

	if (setgroups(1, &pw->pw_gid) ||
			setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) ||
			setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid)) {
		fprintf(stderr, "%s: cannot drop privileges\n", __func__);
		exit(1);
	}
#else
	if (chdir("./game") != 0)
		err(1, "No dir ./game");
#endif

	players_init();
	hash_init(&cmdsdb);

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

	/* errno = 0; // TODO why? sanity fails to access file */

	if (shovechars())
		return 1;

	map_close();

	close_sockets("\r\nServer shutting down.\r\n");

	FILE *f = fopen(STD_DB, "wb");

	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	objects_write(f);
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
pprintf(OBJ *player, char *format, ...)
{
	ENT *eplayer = &player->sp.entity;
	va_list args;
	va_start(args, format);
	vdprintf(eplayer->fd, format, args);
	va_end(args);
	return 0;
}

int
twrite(int fd, const char *buf, size_t len)
{
	warn("twrite %lu bytes\n", len);
	const char *p = buf;
	size_t max_len = BUFSIZ;
	while (len >= max_len) {
		WRITE(fd, p, max_len);
		len -= max_len;
		p += max_len;
	}
	if (len)
		WRITE(fd, p, len);
	return 0;
}

int
writel(int fd, const char *line, size_t len)
{
	twrite(fd, line, len ? len : strlen(line));
	WRITE(fd, "\r\n", 2);
	return 0;
}

int
dwritelf(int fd, const char *format, va_list args)
{
	static char buf[BUFSIZ * 2];
	ssize_t len;
	len = vsnprintf(buf, sizeof(buf), format, args);
	return writel(fd, buf, len);
}

int
notify(ENT *eplayer, const char *msg)
{
	if (eplayer->fd <= 0)
		return 0;
	descr_t *d = &descr_map[eplayer->fd];
	return (d->flags & DF_WEBSOCKET)
		? ws_write(eplayer->fd, msg, strlen(msg))
		: writel(eplayer->fd, msg, 0);
}

int
notifyf(ENT *eplayer, const char *format, ...)
{
	if (eplayer->fd <= 0)
		return 0;
	descr_t *d = &descr_map[eplayer->fd];
	va_list args;
	va_start(args, format);
	int ret = (d->flags & DF_WEBSOCKET)
		? wsdprintf(eplayer->fd, format, args)
		: dwritelf(eplayer->fd, format, args);
	va_end(args);
	return ret;
}

static inline void
vnotifyfe(OBJ *first, OBJ *exception, const char *format, va_list va)
{
	FOR_LIST(first, first) {
		if (first->type != TYPE_ENTITY || first == exception)
			continue;

		ENT *efirst = &first->sp.entity;
		if (efirst->fd <= 0)
			continue;
		descr_t *d = &descr_map[efirst->fd];
		if (d->flags & DF_WEBSOCKET)
			wsdprintf(efirst->fd, format, va);
		else
			dwritelf(efirst->fd, format, va);
	}
}

static inline void
vnotifyfa(OBJ *first, const char *format, va_list va)
{
	return vnotifyfe(first, NULL, format, va);
}

static inline void
netnotifyfe(OBJ *first, OBJ *exception, const char *format, va_list va)
{
	FOR_LIST(first, first) {
		if (first->type != TYPE_ENTITY || first == exception)
			continue;
		ENT *efirst = &first->sp.entity;
		if (efirst->fd <= 0)
			continue;
		descr_t *d = &descr_map[efirst->fd];
		if (d->flags & DF_WEBSOCKET)
			continue;
		dwritelf(efirst->fd, format, va);
	}
}

static inline void
netnotifyfa(OBJ *first, const char *format, va_list va)
{
	netnotifyfe(first, NULL, format, va);
}

void
anotifyf(OBJ *room, char *format, ...)
{
	va_list args;
	va_start(args, format);
	vnotifyfa(room->contents, format, args);
	va_end(args);
	netnotifyfa(room->contents, "\n", args);
}

void
onotifyf(OBJ *player, char *format, ...)
{
	OBJ *loc = player->location;
	va_list args;
	va_start(args, format);
	vnotifyfe(loc->contents, player, format, args);
	va_end(args);
	/* netnotifyfe(loc->contents, player, "\n", args); */
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

static inline void
avatar(OBJ *player) {
	player->art_id = 1 + (random() % art_max("avatar"));
}

void
do_auth(command_t *cmd)
{
	char buf[BUFSIZ];
	int fd = cmd->fd;
	char *qsession = cmd->argv[1];
	FILE *fp;
	OBJ *player;
	descr_t *d = &descr_map[fd];


	if (!*qsession) {
		descr_inband(fd, "Please log in.");
		mcp_auth_fail(fd, 3);
		return;
	}

	snprintf(buf, sizeof(buf), "/sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp) {
		descr_inband(fd, "No such session\r\n");
		mcp_auth_fail(fd, 1);
		return;
	}

	fscanf(fp, "%s", buf);
	fclose(fp);

	warn("lookup_player '%s'\n", buf);

	player = player_get(buf);

	if (!player) {
		player = player_create(buf);
		birth(player);
		spells_birth(player);
		avatar(player);
	} else if (player->sp.entity.fd > 0) {
                descr_inband(fd, "That player is already connected.\r\n");
                mcp_auth_fail(fd, 2);
                return;
        }

	d->flags |= DF_CONNECTED;
	d->player = cmd->player = player;
	CBUG(d->fd != fd);
	ENT *eplayer = &player->sp.entity;
	eplayer->fd = fd;
        mcp_stats(player);
        mcp_bars(eplayer);
        mcp_auth_success(player);
        mcp_equipment(player);
	look_around(player);
	/* enter_room(player, E_ALL); */
	do_view(cmd);
}

void
do_avatar(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	avatar(player);
	notifyf(eplayer, "Selected avatar %u", player->art_id);
}

#if 0
void
do_gpt(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	char *reply = gpt(cmd->argv[1]);
	notifyf(eplayer, "%s", reply);
}
#endif

static inline void
do_v(OBJ *player, char const *cmdstr)
{
	int ofs = 1;
	char const *s = cmdstr;

	for (; *s && ofs > 0; s += ofs) {
		ofs = st_v(player, s);
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

	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;

	command_debug(cmd, "command_process");

	// set current descriptor (needed for death)
	eplayer->fd = descr;

	OBJ *here = player->location;

	pos_t pos;
	map_where(pos, here);

	if (cmd_i)
		cmd_i->cb(cmd);
	else
		do_v(player, cmd->argv[0]);

	pos_t pos2;
	OBJ *there = player->location;
	map_where(pos2, there);

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
		ret = READ(d->fd, buf, sizeof(buf));
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
	d->player = NULL;

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
	return d;
}

void
descr_close(descr_t *d)
{
	if (d->flags & DF_CONNECTED) {
		OBJ *player = d->player;
		ENT *eplayer = &player->sp.entity;
		warn("%s(%d) disconnects on fd %d\n",
		     player->name, object_ref(player), d->fd);
		OBJ *last_observed = eplayer->last_observed;
		if (last_observed)
			observer_remove(last_observed, player);

		eplayer->fd = -1;
		d->flags = 0;
		d->player = NULL;
	} else {
		d->flags = 0;
		d->player = NULL;
		warn("%d never connected\n", d->fd);
	}

	shutdown(d->fd, 2);
	close(d->fd);
	FD_CLR(d->fd, &activefds);
	FD_CLR(d->fd, &readfds);
	d->fd = -1;
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

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
								     // printf("milliseconds: %lld\n", milliseconds);
	return milliseconds;
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

	mstart = mnow = current_timestamp();
	/* And here, we do the actual player-interaction loop */

	warn("Done.\n");

	while (shutdown_flag == 0) {
		/* process_commands(); */
		mlast = mnow;
		mnow = current_timestamp();
		dt = mnow - mlast;
		macc += dt;
		if (macc > 1000) {
			objects_update();
			st_update();
			macc -= 1000;
		}

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
			case EBADF:
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

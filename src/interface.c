#include <ndc.h>
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
#include <arpa/telnet.h>
#include <sys/select.h>
#include <openssl/ssl.h>
#include <termios.h>

#include <err.h>

#include <pwd.h>
#include <grp.h>

#include "nddb.h"
#include "params.h"
#include "defaults.h"
#include "mcp.h"
#include "externs.h"
#include "map.h"
#include "view.h"
#include "mob.h"
#include "player.h"
#include "debug.h"
#include "utils.h"
#include "spacetime.h"

enum opts {
	OPT_DETACH = 1,
};

void do_bio(int fd, int argc, char *argv[]);
extern void do_avatar(int fd, int argc, char *argv[]);
extern void do_auth(int fd, int argc, char *argv[]);
extern void do_save(int fd, int argc, char *argv[]);
/* extern void do_gpt(int fd, int argc, char *argv[]); */

void
do_man(int fd, int argc, char *argv[]) {
	char *rargv[] = { "/usr/bin/man", "-s", "10", argv[1], NULL };

	ndc_pty(fd, rargv);
}

void
do_diff(int fd, int argc, char *argv[]) {
	char *command[] = { "git", "-C", "/items/nd", "diff", "origin/master", NULL };
	ndc_pty(fd, command);
}

struct cmd_slot cmds[] = {
	{
		.name = "sh",
		.cb = &do_sh,
	}, {
		.name = "GET",
		.cb = &do_GET,
		.flags = CF_NOAUTH | CF_NOTRIM,
	}, {
		.name = "POST",
		.cb = &do_POST,
		.flags = CF_NOAUTH | CF_NOTRIM,
	}, {
		.name = "auth",
		.cb = &do_auth,
		.flags = CF_NOAUTH,
	/* }, { */
	/* 	.name = "gpt", */
	/* 	.cb = &do_gpt, */
	}, {
		.name = "avatar",
		.cb = &do_avatar,
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
		.name = "inventory",
		.cb = &do_inventory,
	}, {
		.name = "fight",
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
		.name = "help",
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
		.name = "save",
		.cb = &do_save,
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
		.name = "stchown",
		.cb = &do_stchown,
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

short optflags = 0;
int euid = 0;
struct ndc_config nd_config = {
	.flags = 0,
};

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

int
main(int argc, char **argv)
{
	register char c;

	while ((c = getopt(argc, argv, "p:k:c:dvC:")) != -1) {
		switch (c) {
		case 'd':
			optflags |= OPT_DETACH;
			break;
		case 'v':
			printf("0.0.1\n");
			break;
		case 'p':
			nd_config.port = atoi(optarg);
			break;
		case 'k':
			nd_config.flags |= NDC_SSL;
			nd_config.ssl_key = strdup(optarg);
			break;
		case 'c':
			nd_config.flags |= NDC_SSL;
			nd_config.ssl_crt = strdup(optarg);
			break;
		case 'C':
			nd_config.chroot = optarg;
			break;
			
		default:
			show_program_usage(*argv);
			return 1;

		case '?':
			show_program_usage(*argv);
			return 0;
		}
	}

	ndc_init(&nd_config);
	euid = geteuid();
	if (euid && !nd_config.chroot)
		nd_config.chroot = ".";

	players_init();
	objects_init();
	int ret = map_init();
	if (ret) {
		warn("map_init %s", db_strerror(ret));
		exit(1);
	}
	st_init();

	/* errno = 0; // TODO why? sanity fails to access file */

	setenv("TERM", "xterm-256color", 1);
	fprintf(stderr, "Done.\n");
	ndc_main();

	st_close();
	map_close();
	objects_sync();
	sync();

	return 0;
}

void
nd_write(OBJ *player, char *str, size_t len) {
	if (!player->sp.entity.fds)
		return;

	struct hash_cursor c = hash_iter_start(player->sp.entity.fds);

	while (hash_iter_get(&c))
		ndc_write(* (int *) c.key, str, len);
}


void
nd_dwritef(OBJ *player, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	if (!player->sp.entity.fds)
		return;
	struct hash_cursor c = hash_iter_start(player->sp.entity.fds);

	while (hash_iter_get(&c))
		ndc_write(* (int *) c.key, buf, len);
}

void
nd_writef(OBJ *player, const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	nd_dwritef(player, fmt, va);
	va_end(va);
}

void
nd_rwrite(OBJ *room, OBJ *exception, char *str, size_t len) {
	OBJ *first = room->contents;
	FOR_LIST(first, first) {
		if (first->type != TYPE_ENTITY || first == exception)
			continue;
		nd_write(first, str, len);
	}
}

void
nd_rwritef(OBJ *room, OBJ *exception, char *format, ...)
{
	char buf[BUFFER_LEN];
	size_t len;
	va_list args;
	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	nd_rwrite(room, exception, buf, len);
}

void
nd_close(OBJ *player) {
	struct hash_cursor c = hash_iter_start(player->sp.entity.fds);

	while (hash_iter_get(&c))
		ndc_close(* (int *) c.key);
}

void
do_save(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);

	if (object_ref(player) != 1) {
		fprintf(stderr, "Only God can save\n");
		return;
	}

	objects_sync();
	st_sync();
	map_sync();
}

static inline void
avatar(OBJ *player) {
	player->art_id = 1 + (random() % art_max("avatar"));
}

void
do_avatar(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	avatar(player);
	mcp_content_out(player->location, player);
	mcp_content_in(player->location, player);
}

OBJ *
auth(int fd, char *qsession)
{
	char buf[BUFSIZ];
	FILE *fp;
	OBJ *player;

	if (!*qsession) {
		ndc_writef(fd, "No key provided.\n");
		mcp_auth_fail(fd, 3);
		return NULL;
	}

	warn("AUTH '%s'\n", qsession);
	snprintf(buf, sizeof(buf), "./sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp) {
		ndc_writef(fd, "No such session\n");
		mcp_auth_fail(fd, 1);
		return NULL;
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
		reroll(player, player);
		ENT *eplayer = &player->sp.entity;
		eplayer->hp = HP_MAX(eplayer);
		eplayer->mp = MP_MAX(eplayer);
        }

	FD_PLAYER(fd) = player;
	ndc_auth(fd, buf);
	ENT *eplayer = &player->sp.entity;
	if (!eplayer->fds)
		eplayer->fds = hash_init();
	hash_put(eplayer->fds, &fd, sizeof(fd), (void *) 1);
        mcp_stats(player);
        mcp_auth_success(player);
        mcp_equipment(player);
	look_around(player);
        mcp_bars(player);
	/* enter_room(player, E_ALL); */
	do_view(fd, 0, NULL);
	st_run(player, "auth");
	return player;
}

void
do_auth(int fd, int argc, char *argv[])
{
	auth(fd, argv[1]);
}

void
ndc_update(unsigned long long dt)
{
	double fdt = dt / 1000000.0;
	tick += fdt;
	if (tick > 1.0) {
		tick -= 1.0;
		objects_update(1.0);
		st_update(1.0);
	}
}

void ndc_vim(int fd, int argc, char *argv[]) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	OBJ *player = FD_PLAYER(fd);
	int ofs = 1;
	char const *s = argv[0];

	for (; *s && ofs > 0; s += ofs) {
		ofs = st_v(player, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(player, s);
	}
}

void ndc_view(int fd, int argc, char *argv[]) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	do_look_at(fd, argc, argv);
}

void ndc_connect(int fd) {
	static char *cookie;
	int headers = ndc_headers(fd);
	cookie = (char *) SHASH_GET(headers, "Cookie");
	if (!cookie)
		return;

	auth(fd, strchr(cookie, '=') + 1);
}

void ndc_disconnect(int fd) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	warn("%s(%d) disconnects on fd %d\n",
			player->name, object_ref(player), fd);
	OBJ *last_observed = eplayer->last_observed;
	if (last_observed)
		observer_remove(last_observed, player);

	FD_PLAYER(fd) = NULL;
	int no_fds = 1;
	struct hash_cursor c = hash_iter_start(eplayer->fds);
	while (hash_iter_get(&c)) {
		no_fds = 0;
		break;
	}
	hash_del(eplayer->fds, &fd, sizeof(fd));
	if (no_fds) {
		hash_close(eplayer->fds);
		eplayer->fds = 0;
	}
}

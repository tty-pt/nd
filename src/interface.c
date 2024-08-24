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
		.name = "streload",
		.cb = &do_streload,
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
		.name = NULL,
		.cb = NULL,
	},
};

short optflags = 0;
int euid = 0;
struct ndc_config nd_config = {
	.flags = 0,
};
long fds_hd = -1;
long player_hd = -1;

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

struct hash_cursor fds_iter(dbref player) {
	return hash_citer(fds_hd, &player, sizeof(player));
}

int fds_next(struct hash_cursor *c) {
	dbref key;
	int value;
	if (!hash_next(&key, &value, c))
		return 0;
	return value;
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

	fds_hd = hash_cinit(NULL, NULL, 0644, QH_DUP);

	ndc_init(&nd_config);
	euid = geteuid();
	if (euid && !nd_config.chroot)
		nd_config.chroot = ".";

	players_init();
	entities_init();
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
	struct hash_cursor c = fds_iter(object_ref(player));
	int fd;

	while ((fd = fds_next(&c)))
		ndc_write(fd, str, len);
}


void
nd_dwritef(OBJ *player, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fds_iter(object_ref(player));
	int fd;

	while ((fd = fds_next(&c)))
		ndc_write(fd, buf, len);
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
	OBJ *tmp;
	struct hash_cursor c = contents_iter(object_ref(room));
	while ((tmp = contents_next(&c)))
		if (tmp->type == TYPE_ENTITY && tmp != exception)
			nd_write(tmp, str, len);
}

void
nd_dowritef(OBJ *player, const char *fmt, va_list args) {
	char buf[BUFFER_LEN];
	size_t len;
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	nd_rwrite(object_get(player->location), player, buf, len);
}

void
nd_owritef(OBJ *player, char *format, ...)
{
	va_list args;
	va_start(args, format);
	nd_dowritef(player, format, args);
	va_end(args);
}

void
nd_close(OBJ *player) {
	struct hash_cursor c = fds_iter(object_ref(player));
	int fd;

	while ((fd = fds_next(&c)))
		ndc_close(fd);
}

void
do_save(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);

	if (object_ref(player) != 1) {
		nd_writef(player, "Only God can save\n");
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
	mcp_content_out(object_get(player->location), player);
	mcp_content_in(object_get(player->location), player);
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
		ENT eplayer;
		memset(&eplayer, 0, sizeof(eplayer));
		player = object_new();
		player->name = strdup(buf);
		player->location = eplayer.home = PLAYER_START;
		player->type = TYPE_ENTITY;
		player->owner = object_ref(player);
		player->value = START_PENNIES;
		eplayer.flags = EF_PLAYER;

		contents_put(PLAYER_START, object_ref(player));
		player_put(player);

		birth(&eplayer);
		spells_birth(&eplayer);
		avatar(player);
		reroll(player, player);
		eplayer.hp = HP_MAX(&eplayer);
		eplayer.mp = MP_MAX(&eplayer);
		ent_set(object_ref(player), &eplayer);
		st_start(player);

        }

	FD_PLAYER(fd) = player;
	ndc_auth(fd, buf);
	dbref ref = object_ref(player);
	hash_cput(fds_hd, &ref, sizeof(ref), &fd, sizeof(fd));
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

void ndc_connect(int fd) {
	static char *cookie;
	int headers = ndc_headers(fd);
	cookie = (char *) hash_sget(headers, "Cookie");
	if (!cookie)
		return;

	auth(fd, strchr(cookie, '=') + 1);
}

void ndc_disconnect(int fd) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	OBJ *player = FD_PLAYER(fd);
	warn("%s(%d) disconnects on fd %d\n",
			player->name, object_ref(player), fd);
	OBJ *last_observed = object_get(ent_get(object_ref(player)).last_observed);
	if (last_observed)
		observer_remove(last_observed, player);

	FD_PLAYER(fd) = NULL;
	dbref ref = object_ref(player);
	hash_vdel(contents_hd, &ref, sizeof(ref), &fd, sizeof(fd));
}

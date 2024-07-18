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

enum opts {
	OPT_DETACH = 1,
};

void do_bio(int fd, int argc, char *argv[]);
extern void do_auth(int fd, int argc, char *argv[]);
/* extern void do_gpt(int fd, int argc, char *argv[]); */

void
do_man(int fd, int argc, char *argv[]) {
	ndc_pty(fd, argv);
}

void
do_env(int fd, int argc, char *argv[]) {
	char *rargv[] = { "/usr/bin/env", NULL };
	ndc_pty(fd, rargv);
}

void
do_diff(int fd, int argc, char *argv[]) {
	char *command[] = { "git", "diff", "origin/master", NULL };
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
		.name = "kill",
		.cb = &do_kill,
	}, {
		.name = "look",
		.cb = &do_look_at,
	}, {
		.name = "view",
		.cb = &do_view,
	}, {
		.name = "env",
		.cb = &do_env,
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

short optflags = 0;
int euid = 0;
struct ndc_config nd_config = {
	.flags = 0,
	.serve = "/chroot_node_modules"
};
char std_db[BUFSIZ];
char std_db_ok[BUFSIZ];

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
init_game()
{

	snprintf(std_db, sizeof(std_db), "%s%s", euid ? nd_config.chroot : "", STD_DB);
	snprintf(std_db_ok, sizeof(std_db_ok), "%s%s", euid ? nd_config.chroot : "", STD_DB_OK);
	const char *name = std_db;
	FILE *f;

	if (access(std_db, F_OK) != 0)
		name = std_db_ok;

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

	if (init_game() < 0) {
		warn("Couldn't load %s!\n", std_db);
		return 2;
	}

	int ret = map_init();
	if (ret) {
		warn("map_init %s", db_strerror(ret));
		exit(1);
	}

	/* signal(SIGPIPE, SIG_IGN); */
	/* signal(SIGHUP, SIG_IGN); */
	/* signal(SIGUSR1, SIG_DFL); */

	/* errno = 0; // TODO why? sanity fails to access file */

	setenv("TERM", "xterm+256color", 1);
	fprintf(stderr, "Done.\n");
	ndc_main();

	map_close();

	FILE *f = fopen(std_db, "wb");

	if (f == NULL) {
		perror("fopen");
		return 1;
	}

	objects_write(f);
	fclose(f);
	sync();

	return 0;
}

static inline void
nd_ldwritef(OBJ *first, OBJ *exception, const char *format, va_list va) {
	FOR_LIST(first, first) {
		if (first->type != TYPE_ENTITY || first == exception)
			continue;
		ENT *efirst = &first->sp.entity;
		if (efirst->fd <= 0)
			continue;
		ndc_dwritef(efirst->fd, format, va);
	}
}

void
nd_rwritef(OBJ *room, OBJ *exception, char *format, ...)
{
	va_list args;
	va_start(args, format);
	nd_ldwritef(room->contents, exception, format, args);
	va_end(args);
}

static inline void
avatar(OBJ *player) {
	player->art_id = 1 + (random() % art_max("avatar"));
}

void
auth(int fd, char *qsession)
{
	char buf[BUFSIZ];
	FILE *fp;
	OBJ *player;

	if (!*qsession) {
		ndc_writef(fd, "Please log in.\n");
		mcp_auth_fail(fd, 3);
		return;
	}

	warn("AUTH '%s'\n", qsession);
	snprintf(buf, sizeof(buf), "./sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp) {
		ndc_writef(fd, "No such session\n");
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
                ndc_writef(fd, "That player is already connected.\n");
                mcp_auth_fail(fd, 2);
                return;
        }

	FD_PLAYER(fd) = player;
	ndc_auth(fd, buf);
	ENT *eplayer = &player->sp.entity;
	eplayer->fd = fd;
        mcp_stats(player);
        mcp_bars(eplayer);
        mcp_auth_success(player);
        mcp_equipment(player);
	look_around(player);
	/* enter_room(player, E_ALL); */
	do_view(fd, 0, NULL);
}

void
do_auth(int fd, int argc, char *argv[])
{
	auth(fd, argv[1]);
}

void
ndc_update()
{
	objects_update();
}

void ndc_vim(int fd, int argc, char *argv[]) {
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
	do_look_at(fd, argc, argv);
}

void ndc_connect(int fd) {
}

void ndc_ws_init(int fd) {
	char *cookie = getenv("HTTP_COOKIE");
	if (!cookie)
		return;
	auth(fd, strchr(cookie, '=') + 1);
}

void ndc_disconnect(int fd) {
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	warn("%s(%d) disconnects on fd %d\n",
			player->name, object_ref(player), fd);
	OBJ *last_observed = eplayer->last_observed;
	if (last_observed)
		observer_remove(last_observed, player);

	eplayer->fd = -1;
	FD_PLAYER(fd) = NULL;
}

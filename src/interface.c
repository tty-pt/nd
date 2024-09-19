#include "uapi/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ndc.h>

#include "config.h"
#include "mcp.h"
#include "player.h"
#include "st.h"
#include "uapi/entity.h"
#include "uapi/object.h"
#include "view.h"

#include "papi/nd.h"

enum opts {
	OPT_DETACH = 1,
};

void do_advitam(int fd, int argc, char *argv[]);
void do_auth(int fd, int argc, char *argv[]);
void do_avatar(int fd, int argc, char *argv[]);
void do_bio(int fd, int argc, char *argv[]);
void do_boot(int fd, int argc, char *argv[]);
void do_buy(int fd, int argc, char *argv[]);
void do_chown(int fd, int argc, char *argv[]);
void do_clone(int fd, int argc, char *argv[]);
void do_consume(int fd, int argc, char *argv[]);
void do_contents(int fd, int argc, char *argv[]);
void do_create(int fd, int argc, char *argv[]);
void do_drop(int fd, int argc, char *argv[]);
void do_equip(int fd, int argc, char *argv[]);
void do_examine(int fd, int argc, char *argv[]);
void do_fight(int fd, int argc, char *argv[]);
void do_fill(int fd, int argc, char *argv[]);
void do_get(int fd, int argc, char *argv[]);
void do_give(int fd, int argc, char *argv[]);
void do_heal(int fd, int argc, char *argv[]);
void do_inventory(int fd, int argc, char *argv[]);
void do_look_at(int fd, int argc, char *argv[]);
void do_name(int fd, int argc, char *argv[]);
void do_owned(int fd, int argc, char *argv[]);
void do_pose(int fd, int argc, char *argv[]);
void do_recycle(int fd, int argc, char *argv[]);
void do_reroll(int fd, int argc, char *argv[]);
void do_save(int fd, int argc, char *argv[]);
void do_say(int fd, int argc, char *argv[]);
void do_score(int fd, int argc, char *argv[]);
void do_select(int fd, int argc, char *argv[]);
void do_sell(int fd, int argc, char *argv[]);
void do_shop(int fd, int argc, char *argv[]);
void do_sit(int fd, int argc, char *argv[]);
void do_stand(int fd, int argc, char *argv[]);
void do_status(int fd, int argc, char *argv[]);
void do_teleport(int fd, int argc, char *argv[]);
void do_toad(int fd, int argc, char *argv[]);
void do_train(int fd, int argc, char *argv[]);
void do_unequip(int fd, int argc, char *argv[]);
void do_usage(int fd, int argc, char *argv[]);
void do_view(int fd, int argc, char *argv[]);
void do_wall(int fd, int argc, char *argv[]);

unsigned dplayer_hd = -1;

unsigned fd_player(int fd) {
	unsigned ret;
	hash_cget(dplayer_hd, &ret, &fd, sizeof(fd));
	return ret;
}

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
		.cb = &do_fight,
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
unsigned fds_hd = -1;
unsigned player_hd = -1;

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

struct hash_cursor fds_iter(unsigned player) {
	return hash_citer(fds_hd, &player, sizeof(player));
}

int fds_next(struct hash_cursor *c) {
	unsigned key;
	int value = 0;
	if (hash_next(&key, &value, c) < 0)
		return 0;
	return value;
}

void skel_init();
int objects_init();
void entities_init();
void objects_update(double dt);
void objects_sync();

void map_init();
int map_close(void);
int map_sync(void);

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

	dplayer_hd = hash_init();
	ndc_init(&nd_config);
	euid = geteuid();
	if (euid && !nd_config.chroot)
		nd_config.chroot = ".";

	fds_hd = hash_cinit(NULL, NULL, 0644, QH_DUP);
	skel_init();
	players_init();
	entities_init();
	objects_init();
	map_init();
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
nd_write(unsigned player_ref, char *str, size_t len) {
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		ndc_write(fd, str, len);
}


void
nd_dwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		ndc_write(fd, buf, len);
}

void
nd_rwrite(unsigned room_ref, unsigned exception_ref, char *str, size_t len) {
	unsigned tmp_ref;
	struct hash_cursor c = contents_iter(room_ref);
	while ((tmp_ref = contents_next(&c)) != NOTHING)
		if (tmp_ref != exception_ref && obj_get(exception_ref).type == TYPE_ENTITY)
			nd_write(tmp_ref, str, len);
}

void
nd_dowritef(unsigned player_ref, const char *fmt, va_list args) {
	char buf[BUFFER_LEN];
	size_t len;
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	nd_rwrite(obj_get(player_ref).location, player_ref, buf, len);
}

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, buf, len);
}

void nd_wwrite(unsigned player_ref, void *msg, size_t len) {
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		if ((ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, msg, len);
}

void
nd_close(unsigned player_ref) {
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		ndc_close(fd);
}

void
do_save(int fd, int argc, char *argv[]) {
	unsigned player_ref = fd_player(fd);

	if (player_ref != 1) {
		nd_writef(player_ref, "Only root can save\n");
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
	unsigned player_ref = fd_player(fd);
	OBJ player = obj_get(player_ref);
	avatar(&player);
	obj_set(player_ref, &player);
	mcp_content_out(player.location, player_ref);
	mcp_content_in(player.location, player_ref);
}

void reroll(unsigned player_ref, ENT *eplayer);

unsigned
auth(int fd, char *qsession)
{
	char buf[BUFSIZ];
	FILE *fp;
	OBJ player;

	if (!*qsession) {
		ndc_writef(fd, "No key provided.\n");
		mcp_auth_fail(fd, 3);
		return NOTHING;
	}

	fprintf(stderr, "AUTH '%s'\n", qsession);
	snprintf(buf, sizeof(buf), "./sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp) {
		ndc_writef(fd, "No such session\n");
		mcp_auth_fail(fd, 1);
		return NOTHING;
	}

	fscanf(fp, "%s", buf);
	fclose(fp);

	fprintf(stderr, "lookup_player '%s' (%d)\n", buf, fd);

	unsigned player_ref = player_get(buf);

	if (player_ref == NOTHING) {
		ENT eplayer;
		memset(&eplayer, 0, sizeof(eplayer));
		player_ref = object_new(&player);
		player.name = strdup(buf);
		player.location = 0;
		player.type = TYPE_ENTITY;
		player.owner = player_ref;
		player.value = 150;
		eplayer.flags = EF_PLAYER;

		player_put(buf, player_ref);
		hash_cput(fds_hd, &player_ref, sizeof(player_ref), &fd, sizeof(fd));

		birth(&eplayer);
		avatar(&player);
		reroll(player_ref, &eplayer);
		eplayer.hp = HP_MAX(&eplayer);
		eplayer.mp = MP_MAX(&eplayer);
		ent_set(player_ref, &eplayer);
		obj_set(player_ref, &player);
		st_start(player_ref);

        } else
		hash_cput(fds_hd, &player_ref, sizeof(player_ref), &fd, sizeof(fd));

	hash_cput(dplayer_hd, &fd, sizeof(fd), &player_ref, sizeof(player_ref));
	ndc_auth(fd, buf);
        mcp_stats(player_ref);
        mcp_auth_success(player_ref);
        mcp_equipment(player_ref);
	look_around(player_ref);
        mcp_bars(player_ref);
	do_view(fd, 0, NULL);
	st_run(player_ref, "ndst_auth");
	return player_ref;
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

int kill_v(unsigned player_ref, const char *cmdstr);

void ndc_vim(int fd, int argc, char *argv[]) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	unsigned player_ref = fd_player(fd);
	int ofs = 1;
	char const *s = argv[0];

	for (; *s && ofs > 0; s += ofs) {
		ofs = st_v(player_ref, s);
		if (ofs < 0)
			ofs = - ofs;
		s += ofs;
		ofs = kill_v(player_ref, s);
	}
}

void ndc_command(int fd, int argc, char *argv[]) {
	me = fd_player(fd);
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

	unsigned player_ref = fd_player(fd);
	fprintf(stderr, "%s(%u) disconnects on fd %d\n",
			obj_get(player_ref).name, player_ref, fd);
	hash_del(dplayer_hd, &fd, sizeof(fd));
}

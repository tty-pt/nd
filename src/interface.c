#include "uapi/io.h"

#include <signal.h>
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
#include "nddb.h"

#include "papi/nd.h"

enum opts {
	OPT_DETACH = 1,
};

void do_advitam(int fd, int argc, char *argv[]);
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

DB_TXN *txnid;

unsigned fd_player(unsigned fd) {
	unsigned ret;
	uhash_get(dplayer_hd, &ret, fd);
	return ret;
}

void
do_man(int fd, int argc __attribute__((unused)), char *argv[]) {
	char *rargv[] = { "/usr/bin/man", "-s", "10", argv[1], NULL };

	ndc_pty(fd, rargv);
}

void
do_diff(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
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
		.name = "PRI",
		.cb = &do_GET,
		.flags = CF_NOAUTH | CF_NOTRIM,
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

void skel_init(void);
void objects_init(void);
void entities_init(void);
void objects_update(double dt);

void map_init(void);
int map_close(unsigned flags);
int map_sync(void);
DB_ENV *env;

void close_all(int i) {
	unsigned flags = i ? DB_NOSYNC : 0;

	// temporary
	hash_close(dplayer_hd, flags);
	hash_close(fds_hd, flags);
	hash_close(skel_hd, flags);
	hash_close(drop_hd, flags);

	// permanent
	st_dlclose();
	hash_close(sl_hd, flags);
	hash_close(owner_hd, flags);
	map_close(flags);
	hash_close(ent_hd, flags);
	hash_close(player_hd, flags);
	hash_close(obj_hd, flags);
	hash_close(contents_hd, flags);
	hash_close(obs_hd, flags);
	hash_close(art_hd, flags);
	env->close(env, 0);
	closelog();
	sync();

	if (i)
		exit(i);
}

int
main(int argc, char **argv)
{
	register char c;

	openlog("nd", LOG_PID | LOG_CONS, LOG_DAEMON);

	ndc_pre_init();

	nd_config.flags |= NDC_DETACH;

	nd_config.port = 4201;

	while ((c = getopt(argc, argv, "p:K:k:dvC:")) != -1) switch (c) {
		case 'd': nd_config.flags &= ~NDC_DETACH; break;
		case 'v': printf("0.0.1\n"); break;
		case 'p': nd_config.port = atoi(optarg); break;
		case 'K': ndc_certs_add(optarg); break;
		case 'k': ndc_cert_add(optarg); break;
		case 'C': nd_config.chroot = optarg; break;
		default: show_program_usage(*argv); return 1;
		case '?': show_program_usage(*argv); return 0;
	}

	ndc_init(&nd_config);
	euid = geteuid();
	if (euid && !nd_config.chroot)
		nd_config.chroot = ".";

	dplayer_hd = hash_init();
	fds_hd = ahash_init();
	skel_init();
	db_env_create(&env, 0);
	/* env->set_verbose(env, DB_VERB_DEADLOCK, 1); */
	/* env->set_verbose(env, DB_VERB_WAITSFOR, 1); */
	/* env->set_verbose(env, DB_VERB_RECOVERY, 1); */
	env->set_flags(env, DB_AUTO_COMMIT, 1);
	env->set_lk_detect(env, DB_LOCK_OLDEST);
	/* env->set_timeout(env, 5000000, DB_SET_LOCK_TIMEOUT); */
	env->set_tx_max(env, 5 * 60);
	signal(SIGSEGV, close_all);
	env->open(env, "/var/nd/env", DB_CREATE | DB_RECOVER | DB_INIT_MPOOL | DB_INIT_TXN | DB_INIT_LOCK | DB_INIT_LOG | DB_THREAD, 0);

	hash_env_set(env);
	env->txn_begin(env, NULL, &txnid, 0);

	owner_hd = hash_cinit(STD_DB, "st", 0644, QH_TXN);
	sl_hd = hash_init();
	map_init();
	ent_hd = hash_cinit(STD_DB, "entity", 0644, QH_TXN);
	players_init();
	obj_hd = lhash_cinit(sizeof(OBJ), STD_DB, "obj", 0644, QH_TXN);
	contents_hd = ahash_init();
	obs_hd = ahash_init();
	art_hd = hash_init();

	txnid->commit(txnid, 0);
	txnid = NULL;

	st_init();
	objects_init();
	env->txn_checkpoint(env, 0, 0, 0);

	/* errno = 0; // TODO why? sanity fails to access file */

	setenv("TERM", "xterm-256color", 1);
	ndclog(LOG_INFO, "Done.");
	ndc_main();

	// temporary
	close_all(0);

	return 0;
}

void
nd_write(unsigned player_ref, char *str, size_t len) {
	struct hash_cursor c = hash_iter(fds_hd, &player_ref, sizeof(player_ref));
	unsigned fd;

	while (ahash_next(&fd, &c))
		ndc_write(fd, str, len);
}


void
nd_dwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fhash_iter(fds_hd, player_ref);
	unsigned fd;

	while (ahash_next(&fd, &c))
		ndc_write(fd, buf, len);
}

void
nd_rwrite(unsigned room_ref, unsigned exception_ref, char *str, size_t len) {
	unsigned tmp_ref;
	struct hash_cursor c = fhash_iter(contents_hd, room_ref);
	while (ahash_next(&tmp_ref, &c))
		if (tmp_ref == exception_ref)
			continue;
		else {
			OBJ tmp;
			lhash_get(obj_hd, &tmp, tmp_ref);
			if (tmp.type == TYPE_ENTITY)
				nd_write(tmp_ref, str, len);
		}
}

void
nd_dowritef(unsigned player_ref, const char *fmt, va_list args) {
	char buf[BUFFER_LEN];
	size_t len;
	OBJ player;
	len = vsnprintf(buf, sizeof(buf), fmt, args);
	lhash_get(obj_hd, &player, player_ref);
	nd_rwrite(player.location, player_ref, buf, len);
}

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fhash_iter(fds_hd, player_ref);
	unsigned fd;

	while (ahash_next(&fd, &c))
		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, buf, len);
}

void nd_wwrite(unsigned player_ref, void *msg, size_t len) {
	struct hash_cursor c = fhash_iter(fds_hd, player_ref);
	unsigned fd;

	while (ahash_next(&fd, &c))
		if ((ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, msg, len);
}

void
nd_close(unsigned player_ref) {
	struct hash_cursor c = fhash_iter(fds_hd, player_ref);
	unsigned fd;

	while (ahash_next(&fd, &c))
		ndc_close(fd);
}

void
do_save(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);

	if (player_ref != 1) {
		nd_writef(player_ref, "Only root can save\n");
		return;
	}

	hash_sync(owner_hd);
	map_sync();
	hash_sync(obj_hd);
	hash_sync(obj_hd);
	hash_sync(contents_hd);
	hash_sync(obs_hd);
	hash_sync(art_hd);
}

static inline void
avatar(OBJ *player) {
	player->art_id = 1 + (random() % art_max("avatar"));
}

void
do_avatar(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	avatar(&player);
	mcp_content_out(player.location, player_ref);
	mcp_content_in(player.location, player_ref);
}

void reroll(unsigned player_ref, ENT *eplayer);

char *ndc_auth_check(int fd) {
	static char user[BUFSIZ];
	char cookie[BUFSIZ], *eq;
	int headers = ndc_headers(fd);
	FILE *fp;

	if (shash_get(headers, cookie, "Cookie"))
		return NULL;

	eq = strchr(cookie, '=');
	if (!eq)
		return NULL;

	snprintf(user, sizeof(user), "./sessions/%s", eq + 1);
	fp = fopen(user, "r");

	if (!fp)
		return NULL;

	fscanf(fp, "%s", user);
	fclose(fp);

	return user;
}

unsigned
auth(unsigned fd)
{
	char *user = ndc_auth_check(fd);
	OBJ player;

	if (!user) {
		mcp_auth_fail(fd, 3);
		return 0;
	}

	env->txn_begin(env, NULL, &txnid, 0);
	unsigned player_ref = player_get(user);

	ndclog(LOG_INFO, "auth '%s' (%u/%u)", user, fd, player_ref);

	if (player_ref == NOTHING) {
		ENT eplayer;
		memset(&eplayer, 0, sizeof(eplayer));
		player_ref = object_new(&player);
		strcpy(player.name, user);
		player.location = 0;
		player.type = TYPE_ENTITY;
		player.owner = player_ref;
		player.value = 150;
		eplayer.flags = EF_PLAYER;

		player_put(user, player_ref);
		ahash_add(fds_hd, player_ref, fd);

		birth(&eplayer);
		avatar(&player);
		reroll(player_ref, &eplayer);
		eplayer.hp = HP_MAX(&eplayer);
		eplayer.mp = MP_MAX(&eplayer);
		ent_set(player_ref, &eplayer);
		lhash_put(obj_hd, player_ref, &player);
		st_start(player_ref);

        } else
		ahash_add(fds_hd, player_ref, fd);

	ahash_add(dplayer_hd, fd, player_ref);
	ndc_auth(fd, user);
        mcp_stats(player_ref);
        mcp_auth_success(player_ref);
        mcp_equipment(player_ref);
	look_around(player_ref);
        mcp_bars(player_ref);
	do_view(fd, 0, NULL);
	st_run(player_ref, "ndst_auth");
	txnid->commit(txnid, 0);
	txnid = NULL;
	return player_ref;
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

void ndc_vim(int fd, int argc __attribute__((unused)), char *argv[]) {
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

void ndc_command(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	me = fd_player(fd);
}

int ndc_connect(int fd) {
	return auth(fd);
}

void ndc_disconnect(int fd) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	unsigned player_ref = fd_player(fd);
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	ndclog(LOG_INFO, "%s(%u) disconnects on fd %d",
			player.name, player_ref, fd);
	uhash_del(dplayer_hd, fd);
	ahash_remove(fds_hd, player_ref, fd);
}

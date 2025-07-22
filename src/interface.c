#include "uapi/io.h"

#include <dlfcn.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ndc.h>

#include "config.h"
#include "mcp.h"
#include "player.h"
#include "st.h"
#include "uapi/entity.h"
#include "uapi/object.h"
#include "uapi/type.h"
#include "view.h"
#include "nddb.h"
#include "params.h"

#include "papi/nd.h"

struct ioc {
	char buf[BUFSIZ];
	size_t len;
	unsigned n;
} ioc[FD_SETSIZE];

typedef struct {
	char *label, *icon;
} action_info;

enum opts {
	OPT_DETACH = 1,
};

ROO room_zero_room = {
	.flags = RF_HAVEN,
	.doors = 0,
	.exits = 0,
	.floor = 0,
};

struct object room_zero = {
	.location = NOTHING,
	.owner = 1,
	.art_id = 0,
	.type = TYPE_ROOM,
	.value = 9999999,
	.flags = 0,
};

biome_skel_t void_biome_biome = {
	.bg = BLACK,
};

SKEL void_biome = {
	.name = "void",
	.type = TYPE_ROOM,
};

unsigned dplayer_hd, skel_hd, drop_hd,
	 adrop_hd, element_hd,
	 wts_hd, awts_hd, biome_hd, mod_hd,
	 mod_id_hd, type_hd, action_hd, vtf_hd,
	 situc_hd, sica_hd, bcp_hd, hd_hd;

struct nd nd;

void do_avatar(int fd, int argc, char *argv[]);
void do_bio(int fd, int argc, char *argv[]);
void do_ban(int fd, int argc, char *argv[]);
void do_chown(int fd, int argc, char *argv[]);
void do_clone(int fd, int argc, char *argv[]);
void do_create(int fd, int argc, char *argv[]);
void do_drop(int fd, int argc, char *argv[]);
void do_examine(int fd, int argc, char *argv[]);
void do_get(int fd, int argc, char *argv[]);
void do_inventory(int fd, int argc, char *argv[]);
void do_look_at(int fd, int argc, char *argv[]);
void do_name(int fd, int argc, char *argv[]);
void do_owned(int fd, int argc, char *argv[]);
void do_pose(int fd, int argc, char *argv[]);
void do_recycle(int fd, int argc, char *argv[]);
void do_save(int fd, int argc, char *argv[]);
void do_say(int fd, int argc, char *argv[]);
void do_select(int fd, int argc, char *argv[]);
void do_status(int fd, int argc, char *argv[]);
void do_teleport(int fd, int argc, char *argv[]);
void do_toad(int fd, int argc, char *argv[]);
void do_view(int fd, int argc, char *argv[]);
void do_wall(int fd, int argc, char *argv[]);

unsigned fd_player(unsigned fd) {
	unsigned ret;
	qdb_get(dplayer_hd, &ret, &fd);
	return ret;
}

void
do_man(int fd, int argc __attribute__((unused)), char *argv[]) {
	char *rargv[] = { "/usr/bin/man", "-s", "10", argv[1], NULL };

	ndc_pty(fd, rargv);
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
		.name = "bio",
		.cb = &do_bio,
	}, {
		.name = "ban",
		.cb = &do_ban,
	}, {
		.name = "chown",
		.cb = &do_chown,
	}, {
		.name = "clone",
		.cb = &do_clone,
	}, {
		.name = "create",
		.cb = &do_create,
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
		.name = "wall",
		.cb = &do_wall,
	}, {
		.name = "drop",
		.cb = &do_drop,
	}, {
		.name = "examine",
		.cb = &do_examine,
	}, {
		.name = "get",
		.cb = &do_get,
	}, {
		.name = "inventory",
		.cb = &do_inventory,
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
		.name = "say",
		.cb = &do_say,
	}, {
		.name = "save",
		.cb = &do_save,
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
		.name = "status",
		.cb = &do_status,
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

void objects_init(void);
void entities_init(void);
void objects_update(double dt);

void map_init(void);
int map_close(unsigned flags);
int map_sync(void);
DB_ENV *env;

void mod_close(void) {
	char buf[BUFSIZ];
	void *sl;
	qdb_cur_t c = qdb_iter(mod_id_hd, NULL);

	while (qdb_next(buf, &sl, &c))
		dlclose(sl);
}

void close_all(int i) {
	unsigned flags = i ? DB_NOSYNC : 0;

	mod_close();

	// temporary
	qdb_close(dplayer_hd, flags);
	qdb_close(fds_hd, flags);

	// permanent
	st_dlclose();
	qdb_close(owner_hd, flags);
	qdb_close(sl_hd, flags);
	map_close(flags);
	qdb_close(ent_hd, flags);
	qdb_close(player_hd, flags);

	qdb_close(obj_hd, flags);
	qdb_close(contents_hd, flags);
	qdb_close(obs_hd, flags);

	// skel (permanent)
	qdb_close(skel_hd, flags);
	qdb_close(drop_hd, flags);
	qdb_close(adrop_hd, flags);
	qdb_close(element_hd, flags);
	qdb_close(wts_hd, flags);
	qdb_close(awts_hd, flags);
	qdb_close(biome_hd, flags);

	qdb_close(mod_hd, flags);
	qdb_close(mod_id_hd, flags);

#if ENABLE_TRANSACTIONS
	env->close(env, 0);
#endif
	closelog();
	sync();

	if (i)
		exit(i);
}

typedef void (*mod_cb_t)(void);

int _mod_run(void *sl, char *symbol) {
	mod_cb_t cb = (mod_cb_t) dlsym(sl, symbol);
	if (!cb)
		return 1;
	cb();
	return 0;
}

void _mod_load(char *fname) {
	void *sl;

	sl = dlopen(fname, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);

	if (!sl) {
	    ndclog(LOG_ERR, "_mod_load failed loading '%s': %s\n", fname, dlerror());
	    return;
	}

	unsigned existed = qdb_exists(mod_hd, fname);
	struct nd *ind = dlsym(sl, "nd");
	if (ind)
		*ind = nd;

	// libnd versions and updating rules
	// and module so files and stuff like that
	char *symbol = existed ? "mod_open" : "mod_install";

	ndclog(LOG_INFO, "%s: '%s'\n", symbol, fname);
	unsigned id = qdb_put(mod_id_hd, NULL, &sl);
	qdb_put(mod_hd, fname, &id);
	_mod_run(sl, symbol);
}

void mod_load(char *fname) {
	if (qdb_exists(mod_hd, fname)) {
		ndclog(LOG_ERR, "mod_load: module '%s' already present\n", fname);
		return;
	}

	_mod_load(fname);
}

void mod_load_all(void) {
	char buf[BUFSIZ];
	void *ptr;
	qdb_cur_t c = qdb_iter(mod_id_hd, NULL);
	while (qdb_next(buf, &ptr, &c)) 
		_mod_load(buf);
}

void sic_call(void *retp, char *symbol, void *arg) {
	unsigned mod_id;
	void *ptr = NULL;
	qdb_cur_t c = qdb_iter(mod_id_hd, NULL);
	sic_adapter_t adapter;

	if (qdb_get(sica_hd, &adapter, symbol)) {
		fprintf(stderr, "No adapter registered for symbol '%s'\n", symbol);
		return;
	}

	while (qdb_next(&mod_id, &ptr, &c)) {
		struct nd *ndr = (struct nd *) dlsym(ptr, "nd");
		if (!ndr)
			continue;

		void *cb = dlsym(ptr, symbol);
		if (cb)
			adapter.call(retp, cb, arg);
	}
}

unsigned shared_put(unsigned hd, void *key, void *data) {
	return qdb_put(hd, key, data);
}

unsigned shared_get(unsigned hd, void *value, void *key) {
	return qdb_get(hd, value, key);
}

void nd_register(char *str, nd_cb_t *cb, unsigned flags) {
	ndc_register(str, cb, flags);
}

unsigned action_register(char *label, char *icon) {
	action_info ai = { .label = label, .icon = icon };
	return 1 << (qdb_put(action_hd, NULL, &ai));
}

unsigned vtf_register(char emp, enum color fg, unsigned flags) {
	vtf_t vtf = {
		.pi = { .fg = fg, .flags = flags },
		.emp = emp,
	};

	unsigned id = 1 << (qdb_put(vtf_hd, NULL, &vtf));
	vtf_max = id;
	return id;;
}

void *sic_iter(unsigned si_id, unsigned type) {
	static qdb_cur_t c;
	unsigned key[2] = { si_id, type };
	c = qdb_iter(situc_hd, key);
	return &c;
}

int sic_next(void **cb, void *c) {
	unsigned key[2];
	return qdb_next(key, cb, c);
}

void sic_put(unsigned si_id, unsigned type, void *cb) {
	unsigned key[2] = { si_id, type };
	qdb_put(situc_hd, key, &cb);
}

SIC_DEF(int, on_status, unsigned, player_ref)
SIC_DEF(int, on_examine, unsigned, player_ref, unsigned, ref, unsigned, type)
SIC_DEF(int, on_fbcp, char *, p, unsigned, ref)
SIC_DEF(int, on_add, unsigned, ref, unsigned, type, uint64_t, v)
SIC_DEF(unsigned short, on_view_flags, unsigned short, flags, unsigned, ref)
SIC_DEF(struct icon, on_icon, struct icon, i, unsigned, ref)
SIC_DEF(int, on_del, unsigned, ref)
SIC_DEF(int, on_clone, unsigned, orig_ref, unsigned, nu_ref)
SIC_DEF(int, on_update, unsigned, ref, unsigned, type, double, dt)
SIC_DEF(int, on_move, unsigned, ref, int, cant_move)

SIC_DEF(int, on_vim, unsigned, ref, sic_str_t, ss, int, ofs)

SIC_DEF(int, on_new_player, unsigned, player_ref)
SIC_DEF(int, on_auth, unsigned, player_ref)
SIC_DEF(int, on_before_leave, unsigned, ent_ref)
SIC_DEF(int, on_leave, unsigned, player_ref, unsigned, loc_ref)
SIC_DEF(int, on_enter, unsigned, player_ref, unsigned, loc_ref)
SIC_DEF(int, on_after_enter, unsigned, player_ref)
SIC_DEF(int, on_spawn, unsigned, player_ref, unsigned, loc_ref, struct bio, bio, uint64_t, v)
SIC_DEF(int, on_get, unsigned, player_ref, unsigned, ref)

SIC_DEF(struct bio, on_noise, struct bio, bio, uint32_t, he, uint32_t, w, uint32_t, tm, uint32_t, cl)
SIC_DEF(sic_str_t, on_empty_tile, view_tile_t, t, unsigned, side, sic_str_t, ss)

void sic_areg(char *name, sic_adapter_t *adapter) {
	qdb_put(sica_hd, name, adapter);
}

void shared_init(void) {
	nd.hds[HD_FD] = fds_hd;
	nd.hds[HD_SKEL] = skel_hd;
	nd.hds[HD_DROP] = drop_hd;
	nd.hds[HD_ADROP] = adrop_hd;
	nd.hds[HD_BIOME] = biome_hd;
	nd.hds[HD_WTS] = wts_hd;
	nd.hds[HD_RWTS] = wts_hd + 1;
	nd.hds[HD_OBJ] = obj_hd;
	nd.hds[HD_OBS] = obs_hd;
	nd.hds[HD_CONTENTS] = contents_hd;
	nd.hds[HD_TYPE] = type_hd;
	nd.hds[HD_RTYPE] = type_hd + 1;
	nd.hds[HD_BCP] = bcp_hd;
	nd.hds[HD_ELEMENT] = element_hd;
	nd.hds[HD_HD] = hd_hd + 1;
	nd.hd_hd = hd_hd;

	/* nd.fds_has = fds_has; */
	nd.nd_close = nd_close;
	nd.nd_write = nd_write;
	nd.nd_dwritef = nd_dwritef;
	nd.nd_rwrite = nd_rwrite;
	nd.nd_dowritef = nd_dowritef;
	nd.dnotify_wts = dnotify_wts;
	nd.dnotify_wts_to = dnotify_wts_to;
	nd.nd_tdwritef = nd_tdwritef;
	nd.nd_wwrite = nd_wwrite;

	nd.fd_player = fd_player;

	nd.map_has = map_has;
	nd.map_mwhere = map_mwhere;
	nd.map_where = map_where;
	nd.map_delete = map_delete;
	nd.map_get = map_get;

	nd.st_teleport = st_teleport;
	nd.st_run = st_run;

	nd.wts_plural = wts_plural;

	nd.obj_exists = obj_exists;
	nd.object_new = object_new;
	nd.object_copy = object_copy;
	nd.object_move = object_move;
	nd.object_add = object_add;
	nd.object_drop = object_drop;

	nd.object_icon = object_icon;
	nd.object_art = object_art;
	nd.unparse = unparse;

	nd.me_get = me_get;
	nd.ent_get = ent_get;
	nd.ent_set = ent_set;
	nd.ent_del = ent_del;
	nd.controls = controls;
	nd.payfor = payfor;
	nd.look_around = look_around;
	nd.enter = enter;
	nd.look_at = look_at;
	nd.room_clean = room_clean;

	nd.nd_put = shared_put;
	nd.nd_get = shared_get;
	nd.nd_iter = (nd_iter_t *) qdb_iter;
	nd.nd_next = (nd_next_t *) qdb_next;
	nd.nd_fin = (nd_fin_t *) qdb_fin;
	nd.nd_len_reg = qdb_reg;
	nd.nd_open = qdb_open;
	nd.nd_register = nd_register;

	nd.ematch_at = ematch_at;
	nd.ematch_player = ematch_player;
	nd.ematch_absolute = ematch_absolute;
	nd.ematch_me = ematch_me;
	nd.ematch_mine = ematch_mine;
	nd.ematch_near = ematch_near;
	nd.ematch_all = ematch_all;

	nd.mod_load = mod_load;

	nd.action_register = action_register;
	nd.vtf_register = vtf_register;
	nd.sic_areg = sic_areg;
	nd.sic_call = sic_call;

	nd.noise_point = noise_point;

	nd.fbcp_item = fbcp_item;
	nd.fbcp = fbcp;
	nd.mcp_content_in = mcp_content_in;
	nd.mcp_content_out = mcp_content_out;
	nd.mcp_bar = mcp_bar;
}

void base_actions_register(void);
void base_vtf_init(void);

int
main(int argc, char **argv)
{

	register char c;

	openlog("nd", LOG_PID | LOG_CONS | LOG_NDELAY, LOG_DAEMON);

	nd_config.flags |= NDC_DETACH;
	nd_config.port = 80;

	while ((c = getopt(argc, argv, "p:K:k:dvC:")) != -1) switch (c) {
		case 'd': nd_config.flags &= ~NDC_DETACH; break;
		case 'v': printf("0.0.1\n"); return 0;
		case 'p': nd_config.port = atoi(optarg); break;
		case 'C': nd_config.chroot = optarg; break;
		case 'K':
		case 'k': break;
		default: show_program_usage(*argv); return 1;
		case '?': show_program_usage(*argv); return 0;
	}

	qdb_init();
	qdb_reg("obj", sizeof(OBJ));
	qdb_reg("ent", sizeof(ENT));
	qdb_reg("skel", sizeof(SKEL));
	qdb_reg("drop", sizeof(DROP));
	qdb_reg("element", sizeof(element_t));
	qdb_reg("biome_map", sizeof(unsigned) * BIOME_MAX);
	qdb_reg("st", sizeof(struct st_key));
	qdb_reg("ai", sizeof(action_info));
	qdb_reg("vtf", sizeof(vtf_t));
	qdb_reg("sica", sizeof(sic_adapter_t));

	ndc_pre_init(&nd_config);
	ndclog(LOG_INFO, "nd booting.\n");

	optind = 1;
	while ((c = getopt(argc, argv, "p:K:k:dvC:")) != -1) switch (c) {
		case 'K': ndc_certs_add(optarg); break;
		case 'k': ndc_cert_add(optarg); break;
	}

	ndc_init();
	euid = geteuid();
	if (euid && !nd_config.chroot)
		nd_config.chroot = ".";

	dplayer_hd = qdb_open(NULL, "u", "u", 0); // keys are ints, actually
	fds_hd = qdb_open(NULL, "u", "u", QH_DUP); // a
	signal(SIGSEGV, close_all);

	qdb_config.file = STD_DB;

#if ENABLE_TRANSACTIONS
	qdb_config.flags = QH_TXN;

	env = qdb_env_create();
	qdb_env_open(env, "/var/nd/env", QH_TXN);
#endif

	qdb_begin();
	owner_hd = qdb_open("st", "st", "u", 0);
	sl_hd = qdb_open("sl", "st", "p", 0);

	action_hd = qdb_open("action", "u", "ai", QH_TMP | QH_AINDEX);
	type_hd = qdb_open("ndt", "u", "s", QH_TMP | QH_AINDEX | QH_THRICE);
	vtf_hd = qdb_open("vtf", "u", "vtf", QH_TMP | QH_AINDEX);
	situc_hd = qdb_open("situc", "ul", "p", QH_TMP);
	sica_hd = qdb_open("sica", "s", "sica", QH_TMP);
	bcp_hd = qdb_open("bcp", "u", "s", QH_TMP | QH_AINDEX);
	hd_hd = qdb_open("hd", "u", "s", QH_TMP | QH_AINDEX | QH_THRICE);

	ent_hd = qdb_open("entity", "u", "ent", 0);
	player_hd = qdb_open("player", "s", "u", 0);
	obj_hd = qdb_open("obj", "u", "obj", QH_AINDEX);
	contents_hd = qdb_open("contents", "u", "u", QH_DUP);
	obs_hd = qdb_open("obs", "u", "u", QH_DUP);
	map_init();

	// skel init
	skel_hd = qdb_open("skel", "u", "skel", QH_AINDEX);
	drop_hd = qdb_open("drop", "u", "drop", QH_AINDEX);
	adrop_hd = qdb_open("adrop", "u", "u", QH_DUP);
	element_hd = qdb_open("element", "u", "element", QH_AINDEX);
	wts_hd = qdb_open("wts", "u", "s", QH_AINDEX | QH_THRICE);
	awts_hd = qdb_open("awts", "u", "u", QH_DUP); // a
	biome_hd = qdb_open("biome", "u", "biome_map", 0);
	
	shared_init();

	mod_id_hd = qdb_open("module_id", "u", "p", QH_AINDEX);
	mod_hd = qdb_open("module", "s", "u", 0);

	SIC_AREG(on_status);
	SIC_AREG(on_examine);
	SIC_AREG(on_fbcp);
	SIC_AREG(on_add);
	SIC_AREG(on_view_flags);
	SIC_AREG(on_icon);
	SIC_AREG(on_del);
	SIC_AREG(on_clone);
	SIC_AREG(on_update);
	SIC_AREG(on_move);

	SIC_AREG(on_vim);

	SIC_AREG(on_new_player);
	SIC_AREG(on_auth);
	SIC_AREG(on_before_leave);
	SIC_AREG(on_leave);
	SIC_AREG(on_enter);
	SIC_AREG(on_after_enter);
	SIC_AREG(on_spawn);
	SIC_AREG(on_get);

	SIC_AREG(on_noise);
	SIC_AREG(on_empty_tile);

	qdb_put(type_hd, NULL, "room");
	qdb_put(type_hd, NULL, "entity");

	qdb_put(bcp_hd, NULL, "item");
	qdb_put(bcp_hd, NULL, "view");
	qdb_put(bcp_hd, NULL, "view_buffer");
	qdb_put(bcp_hd, NULL, "room");
	qdb_put(bcp_hd, NULL, "entity");
	qdb_put(bcp_hd, NULL, "auth_failure");
	qdb_put(bcp_hd, NULL, "auth_success");
	qdb_put(bcp_hd, NULL, "out");
	qdb_put(bcp_hd, NULL, "tod");

	base_actions_register();
	base_vtf_init();

	unsigned zero = 0, existed = 1;
	if (!qdb_exists(obj_hd, &zero)) {
		existed = 0;
		memcpy(room_zero.data, &room_zero_room, sizeof(room_zero_room));
		qdb_put(obj_hd, NULL, &room_zero);

		element_t spirit = {
			.color = MAGENTA,
			.weakness = ELM_SPIRIT,
		}, fire = {
			.color = RED,
			.weakness = ELM_WATER,
		}, water = {
			.color = BLUE,
			.weakness = ELM_FIRE,
		}, air = {
			.color = WHITE,
			.weakness = ELM_EARTH,
		}, earth = {
			.color = YELLOW,
			.weakness = ELM_AIR,
		}, physical = {
			.color = GREEN,
			.weakness = ELM_SPIRIT,
		};

		qdb_put(element_hd, NULL, &spirit);
		qdb_put(element_hd, NULL, &fire);
		qdb_put(element_hd, NULL, &water);
		qdb_put(element_hd, NULL, &air);
		qdb_put(element_hd, NULL, &earth);
		qdb_put(element_hd, NULL, &physical); // 5

		SENT adam_sent = {
			.y = 255,
		};

		SKEL adam = {
			.name = "human",
			.type = TYPE_ENTITY,
			.max_art = 300,
		};

		memcpy(adam.data, &adam_sent, sizeof(adam_sent));
		qdb_put(skel_hd, NULL, &adam); // 0
		qdb_put(wts_hd, NULL,  "punch"); // 0

		unsigned biome_map[BIOME_MAX];
		memcpy(void_biome.data, &void_biome_biome, sizeof(void_biome_biome));
		unsigned void_ref = qdb_put(skel_hd, NULL, &void_biome);
		for (unsigned i = 0; i < BIOME_MAX; i++)
			biome_map[i] = void_ref;
		void_ref = 16;
		qdb_put(biome_hd, &void_ref, biome_map);

		unsigned owner = 1;
		sthd_put(owner_hd, 0, 0, &owner);
	}

	objects_init();

	if (existed) {
		mod_load_all();
	} else {
		st_put(1, 0, 64);
		st_run(-1, "mod_init");
	}

	qdb_commit();

	srand(getpid());

	qdb_checkpoint(0, 0, 0);

	qdb_config.file = NULL;
	qdb_config.flags = 0;

	/* errno = 0; // TODO why? sanity fails to access file */

	setenv("TERM", "xterm-256color", 1);
	st_init();

	ndclog(LOG_INFO, "Done.\n");

	ndc_main();

	// temporary
	close_all(0);

	return 0;
}

void
ndc_flush(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	char buf[BUFSIZ + 32], *b = buf;

	if (!ioc[fd].len)
		return;

	if (ioc[fd].n > 1)
		b += snprintf(buf, sizeof(buf), "(%ux) ", ioc[fd].n);

	b += snprintf(b, sizeof(buf) - (b - buf), "%s", ioc[fd].buf);
	ndc_write(fd, buf, b - buf + 1);
	ioc[fd].len = 0;
	memset(ioc[fd].buf, 0, sizeof(ioc[fd].buf));
}

void
nd_flush(unsigned player_ref) {
	qdb_cur_t c = qdb_iter(fds_hd, &player_ref);
	unsigned fd;

	while (qdb_next(&player_ref, &fd, &c))
		ndc_flush(fd, 0, NULL);
}

void
nd_write(unsigned player_ref, char *str, size_t len) {
	qdb_cur_t c = qdb_iter(fds_hd, &player_ref);
	unsigned fd;

	while (qdb_next(&player_ref, &fd, &c)) {
		if (memcmp(str, ioc[fd].buf, len)) {
			ndc_flush(fd, 0, NULL);
			memcpy(ioc[fd].buf, str, len);
			ioc[fd].n = 1;
			ioc[fd].len = len;
		} else
			ioc[fd].n++;
	}
}

void
nd_dwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	nd_write(player_ref, buf, len);
}

void
nd_rwrite(unsigned room_ref, unsigned exception_ref, char *str, size_t len) {
	unsigned tmp_ref;
	qdb_cur_t c = qdb_iter(contents_hd, &room_ref);
	while (qdb_next(&room_ref, &tmp_ref, &c))
		if (tmp_ref == exception_ref)
			continue;
		else {
			OBJ tmp;
			qdb_get(obj_hd, &tmp, &tmp_ref);
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
	qdb_get(obj_hd, &player, &player_ref);
	nd_rwrite(player.location, player_ref, buf, len);
}

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	qdb_cur_t c = qdb_iter(fds_hd, &player_ref);
	unsigned fd;

	while (qdb_next(&player_ref, &fd, &c))
		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, buf, len);
}

void nd_wwrite(unsigned player_ref, void *msg, size_t len) {
	qdb_cur_t c = qdb_iter(fds_hd, &player_ref);
	unsigned fd;

	while (qdb_next(&player_ref, &fd, &c))
		if ((ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, msg, len);
}

void
nd_close(unsigned player_ref) {
	qdb_cur_t c = qdb_iter(fds_hd, &player_ref);
	unsigned fd;

	while (qdb_next(&player_ref, &fd, &c))
		ndc_close(fd);
}

void
do_save(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);

	if (player_ref != 1) {
		nd_writef(player_ref, "Only root can save\n");
		return;
	}

	qdb_sync(owner_hd);
	map_sync();
	qdb_sync(obj_hd);
	qdb_sync(obj_hd);
	qdb_sync(contents_hd);
	qdb_sync(obs_hd);
}

static inline void
avatar(OBJ *player) {
	SKEL skel;
	qdb_get(skel_hd, &skel, &player->skid);
	player->art_id = 1 + (random() % (skel.max_art ? skel.max_art : 1));
}

void
do_avatar(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	avatar(&player);
	mcp_content_out(player.location, player_ref);
	mcp_content_in(player.location, player_ref);
}

char *ndc_auth_check(int fd) {
	static char user[BUFSIZ];
	char cookie[BUFSIZ], *eq;
	int headers = ndc_headers(fd);
	FILE *fp;

	if (qdb_get(headers, cookie, "Cookie"))
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

	qdb_begin();
	unsigned player_ref = player_get(user);

	ndclog(LOG_INFO, "auth '%s' (%u/%u)\n", user, fd, player_ref);

	if (player_ref == NOTHING) {
		ENT ent;
		player_ref = object_add(&player, 0, 0, 0);
		ent = ent_get(player_ref);
		strlcpy(player.name, user, sizeof(player.name));
		player.value = 150;
		ent.flags = EF_PLAYER;
		player_put(user, player_ref);
		avatar(&player);

		qdb_put(fds_hd, &player_ref, &fd);
		ent_set(player_ref, &ent);
		qdb_put(obj_hd, &player_ref, &player);
		qdb_put(dplayer_hd, &fd, &player_ref);
		SIC_CALL(NULL, on_new_player, player_ref);
	} else {
		ENT eplayer = ent_get(player_ref);

		if (eplayer.flags & EF_BAN) {
			nd_writef(player_ref, "You are banned.\n");
			return 0;
		}

		qdb_put(fds_hd, &player_ref, &fd);
		qdb_put(dplayer_hd, &fd, &player_ref);
	}

	ndc_auth(fd, user);
	mcp_auth_success(player_ref);
	look_around(player_ref);
	do_view(fd, 0, NULL);
	if (day_n)
		mcp_tod(player_ref, 1);
	else
		mcp_tod(player_ref, 0);

	qdb_commit();
	SIC_CALL(NULL, on_auth, player_ref);
	return player_ref;
}

void
ndc_update(unsigned long long dt)
{
	double mul = 2.0;
	double fdt = dt / 1000000.0;
	tick += fdt;
	qdb_begin();
	if (tick > 1.0) {
		tick -= 1.0;
		objects_update(1.0 * mul);
		st_update(1.0 * mul);
	}
	qdb_commit();
}

void ndc_vim(int fd, int argc __attribute__((unused)), char *argv[]) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	unsigned player_ref = fd_player(fd);
	int ofs = 1;
	char const *s = argv[0];
	sic_str_t ss = { .str = "", .pos = 0 };
	strlcpy(ss.str, argv[0], sizeof(ss.str));

	for (; *s && ofs > 0; s += ofs) {
		int ret = 0;
		SIC_CALL(&ret, on_vim, player_ref, ss, ofs);
		s += ret;
		ofs = st_v(player_ref, s);
	}
}

void ndc_command(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	me = fd_player(fd);
	ioc[fd].n = 0;
}

int ndc_connect(int fd) {
	return auth(fd);
}

void ndc_disconnect(int fd) {
	if (!(ndc_flags(fd) & DF_AUTHENTICATED))
		return;

	unsigned player_ref = fd_player(fd);
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	ndclog(LOG_INFO, "%s(%u) disconnects on fd %d\n",
			player.name, player_ref, fd);
	qdb_del(dplayer_hd, &fd, NULL);
	qdb_del(fds_hd, &player_ref, &fd);
}

char *wts_plural(char *singular) {
	static char plural[BUFSIZ], *last;
	char *space = strchr(singular, ' ');
	size_t len = space ? space - singular : strlen(singular);
	last = singular + len - 1;
	strncpy(plural, singular, len + 1);

	switch (*last) {
		case 'y':
			*last = 'i';
		case 's':
		case 'x':
		case 'z':
		case 'h':
			strlcat(plural, "es", BUFSIZ);
			break;
		default:
			strlcat(plural, "s", BUFSIZ);
	}

	return plural;
}

unsigned *biome_map_get(uint64_t position) {
	static unsigned biome_map[BIOME_MAX];
	unsigned ref;

	for (int i = 0; i < 16; i++) {
		ref = ((position >> (48 + i)) << 4) | i;
		if (!qdb_get(biome_hd, biome_map, &ref))
			return biome_map;
	}

	ref = 16;
	qdb_get(biome_hd, biome_map, &ref);
	return biome_map;
}

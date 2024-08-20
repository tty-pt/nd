#include "spacetime.h"

#include "io.h"

#include <xxhash.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <qhash.h>
#include <pwd.h>
#include <sys/stat.h>

#include "nddb.h"
#include "debug.h"
#include "noise.h"
#include "map.h"
#include "entity.h"
#include "defaults.h"
#include "params.h"
#include "player.h"
#include "nd.h"

#define PRECOVERY

#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

static long owner_hd = -1, sl_hd = -1;

typedef void op_a_t(OBJ *player, enum exit e);
typedef int op_b_t(OBJ *player, struct cmd_dir cd);
typedef struct {
	union {
		op_a_t *a;
		op_b_t *b;
	} op;
	int type;
} op_t;
OBJ *g_player;

enum exit e_map[] = {
	[0 ... 254] = E_NULL,
	['h'] = E_WEST,
	['w'] = E_WEST,
	['j'] = E_SOUTH,
	['s'] = E_SOUTH,
	['k'] = E_NORTH,
	['n'] = E_NORTH,
	['l'] = E_EAST,
	['e'] = E_EAST,
	['K'] = E_UP,
	['u'] = E_UP,
	['J'] = E_DOWN,
	['d'] = E_DOWN,
};

exit_t exit_map[] = {
	[0 ... E_ALL] = {
		.simm = E_NULL,
		.name = "",
		.other = "",
		.dim = 5,
	},
	[E_EAST] = {
		.simm = E_WEST,
		.name = "east",
		.other = "wnsud",
		.dim = 1, .dis = 1,
	},
	[E_SOUTH] = {
		.simm = E_NORTH,
		.name = "south",
		.other = "ewudn",
		.dim = 0, .dis = 1,
	},
	[E_WEST] = {
		.simm = E_EAST,
		.name = "west",
		.other = "nsude",
		.dim = 1, .dis = -1,
	}, 
	[E_NORTH] = {
		.simm = E_SOUTH,
		.name = "north",
		.other = "sewud",
		.dim = 0, .dis = -1,
	},
	[E_UP] = {
		.simm = E_DOWN,
		.name = "up",
		.other = "dnsew",
		.dim = 2, .dis = 1,
	},
	[E_DOWN] = {
		.simm = E_UP,
		.name = "down",
		.other = "nsewu",
		.dim = 2, .dis = -1,
	},
};

unsigned short day_tick = 0;
double tick = 0;

static __inline__ ucoord_t
unsign(coord_t n)
{
	ucoord_t r = ((smorton_t) n + COORD_MAX);

	if (r == UCOORD_MAX)
		return 1;

	return r;
}

morton_t
pos_morton(pos_t p)
{
	upoint3D_t up;
	morton_t res = ((morton_t) p[3]) << 48;
	int i;

	POOP3D up[I] = unsign(p[I]);

	for (i = 0; i < 16; i ++) {
		POOP3D res |= ((morton_t) ((up[I] >> i) & 1)) << (I + (3 * i));
	}

	/* debug("encoded point %d %d %d %d -> x%llx", p[0], p[1], p[2], p[3], res); */
	return res;
}

static inline coord_t
sign(ucoord_t n)
{
	return (smorton_t) n - COORD_MAX;
}

void
morton_pos(pos_t p, morton_t code)
{
	morton_t up[3] = { 0, 0, 0 };
	int i;

	for (i = 0; i < 16; i ++) {
		POOP3D up[I] |= ((code >> (I + 3 * i)) & 1) << i;
	}

	POOP3D p[I] = sign(up[I]);
	p[3] = OBITS(code);
	/* debug("decoded point x%llx -> %d %d %d %d", code, p[0], p[1], p[2], p[3]); */
}

void
object_drop(OBJ *where, struct drop **drop)
{
	pos_t pos;
        register int i;

	map_where(pos, where->location);

	for (; *drop; drop++)
		if (random() < (RAND_MAX >> (*drop)->y)) {
			noise_t v2 = XXH32((const char *) pos, sizeof(pos_t), 3);
                        int yield = (*drop)->yield,
                            yield_v = (*drop)->yield_v;

                        if (!yield) {
                                object_add((*drop)->i, where, &v2);
                                continue;
                        }

                        yield += random() & yield_v;

                        for (i = 0; i < yield; i++) {
				v2 = XXH32((const char *) pos, sizeof(pos_t), 4 + i);
                                object_add((*drop)->i, where, &v2);
			}
                }
}

int
e_exit_can(OBJ *player, enum exit e) {
	return e_ground(player->location, e);
}

int
e_ground(dbref room, enum exit e)
{
	pos_t pos;

	if (e & (E_UP | E_DOWN))
		return 0;

	map_where(pos, room);
	return pos[2] == 0;
}

void
pos_move(pos_t d, pos_t o, enum exit e) {
	exit_t *ex = &exit_map[e];
	POOP4D d[I] = o[I];
	d[ex->dim] += ex->dis;
}

enum exit
dir_e(const char dir) {
	return e_map[(int) dir];
}

const char
e_dir(enum exit e) {
	return exit_map[e].name[0];
}

enum exit
e_simm(enum exit e) {
	return exit_map[e].simm;
}

const char *
e_name(enum exit e) {
	return exit_map[e].name;
}

const char *
e_other(enum exit e) {
	return exit_map[e].other;
}

morton_t
point_rel_idx(point_t p, point_t s, smorton_t w)
{
	smorton_t s0 = s[Y_COORD],
		s1 = s[X_COORD];
	if (s0 > p[Y_COORD])
		s0 -= UCOORD_MAX;
	if (s1 > p[X_COORD])
		s1 -= UCOORD_MAX;
	return (p[Y_COORD] - s0) * w + (p[X_COORD] - s1);
}

void
st_update(double dt)
{
	const char * msg = NULL;
	dbref i;
	day_tick += dt;

	if (day_tick == 0)
		msg = "The sun rises.\n";
	else if (day_tick == (1 << (DAY_Y - 1)))
		msg = "The sun sets.\n";
	else
		return;

	for (i = 0; i < db_top; i++) {
		OBJ *o = object_get(i);
		if (o->type == TYPE_ENTITY)
			nd_writef(o, msg);
	}
}

static inline void
st_pos(pos_t p, dbref loc, enum exit e)
{
	pos_t aux;
	map_where(aux, loc);
	pos_move(p, aux, e);
	CBUG(!*e_name(e));
	/* debug("st_pos %s 0x%llx -> 0x%llx\n", */
	/* 		e_name(e), */
	/* 		* (morton_t *) aux, */
	/* 		* (morton_t *) p); */
}

static OBJ *
st_there(dbref where, enum exit e)
{
	pos_t pos;
	/* CBUG(!*e_name(e)); */
	st_pos(pos, where, e);
	return map_get(pos);
}

static inline void
reward(OBJ *player, const char *msg, int amount)
{
	player->value += amount;
	nd_writef(player, "You %s. (+%dp)\n", msg, amount);
}

static inline int
fee_fail(OBJ *player, char *desc, char *info, int cost)
{
	int v = player->value;
	if (v < cost) {
		nd_writef(player, "You can't afford to %s. (%dp)\n", desc, cost);
		return 1;
	} else {
		player->value -= cost;
		nd_writef(player, "%s (-%dp). %s\n",
			   desc, cost, info);
		return 0;
	}
}

static inline int
fee_noname(OBJ *player) {
	return fee_fail(player, "claim a room", "", ROOM_COST);
}

static inline void
wall_around(OBJ *player, enum exit e)
{
	OBJ *here = object_get(player->location);
	ROO *rhere = &here->sp.room;
	const char *s;
	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		OBJ *there = st_there(player->location, e2);

		if ((rhere->exits & e2) && there)
			rhere->exits ^= e2;
	}
}

static int
st_claim(OBJ *player, OBJ *room) {
	ROO *rroom = &room->sp.room;

	if (!(rroom->flags & RF_TEMP)) {
		if (room->owner != object_ref(player)) {
			nd_writef(player, "You don't own this room\n");
			return 1;
		}

		return 0;
	}

	if (fee_noname(player))
		return 1;

	rroom->flags ^= RF_TEMP;
	CBUG(rroom->flags & RF_TEMP);
	room->owner = object_ref(player);

	return 0;
}

static inline void
exits_fix(OBJ *player, dbref there_ref, enum exit e)
{
	OBJ *there = object_get(there_ref);
	ROO *rthere = &there->sp.room;
	const char *s;

	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		OBJ *othere = st_there(there_ref, e2);

		if (!othere)
			continue;

		ROO *rothere = &othere->sp.room;

		if (rothere->flags & RF_TEMP) {
			continue;
		}

		if (!(rthere->exits & e2)) {
			if (!(rothere->exits & e_simm(e2)))
				continue;

			rthere->exits |= e2;
			continue;
		}

		if (!(rothere->exits & e_simm(e2)))
			rthere->exits &= ~e2;
	}
}

static void
exits_infer(OBJ *player, dbref here_ref)
{
	OBJ *here = object_get(here_ref);
	ROO *rhere = &here->sp.room;
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		OBJ *there = st_there(here_ref, e);

		if (!there) {
                        if (e != E_UP && e != E_DOWN)
				rhere->exits |= e;
			continue;
		}

		ROO *rthere = &there->sp.room;

		if (rthere->exits & e_simm(e)) {
			rhere->exits |= e;
			rhere->doors |= rthere->doors & e_simm(e);
		}
	}
}

// TODO make more add functions similar and easy to insert here

static inline void
stones_add(OBJ *where, struct bio *bio, pos_t p) {
	noise_t v = XXH32((const char *) p, sizeof(pos_t), 0);
	unsigned char n = v & 0x3, i;
	static struct object_skeleton stone = {
                .name = "stone",
                .description = "Solid stone(s)",
		.type = S_TYPE_MINERAL,
		.sp = {
			.mineral = { 0 } 
		},
        };

	if (bio->bio_idx == BIOME_WATER)
		return;

        if (!(n && (v & 0x18) && (v & 0x20)))
                return;

	noise_t v2 = XXH32((const char *) p, sizeof(pos_t), 0);

        for (i = 0; i < n; i++, v2 >>= 4)
                object_add(&stone, where, &v2);
}

static OBJ *
st_room_at(OBJ *player, pos_t pos)
{
	struct bio *bio;
	bio = noise_point(pos);
        OBJ *there = object_add(&biomes[bio->bio_idx], NULL, bio);
	ROO *rthere = &there->sp.room;
	map_put(pos, object_ref(there), DB_NOOVERWRITE);
	exits_infer(player, object_ref(there));

	if (pos[2] != 0)
		return there;

	rthere->floor = bio->bio_idx;
	entities_add(there, bio->bio_idx, bio->pd.n);
        stones_add(there, bio, pos);
	plants_add(there, bio, pos);
	return there;
}

void
do_bio(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	struct bio *bio;
	pos_t pos;
	map_where(pos, player->location);
	bio = noise_point(pos);
	nd_writef(player, "tmp %d rn %u bio %s(%d)\n",
		bio->tmp, bio->rn, biomes[bio->bio_idx].name, bio->bio_idx);
}

static OBJ *
st_room(OBJ *player, enum exit e)
{
	pos_t pos;
	OBJ *there;

	st_pos(pos, player->location, e);
	there = st_room_at(player, pos);

	return there;
}

static void
e_move(OBJ *player, enum exit e) {
	ENT *eplayer = &player->sp.entity;
	OBJ *loc = object_get(player->location),
	    *dest;
	ROO *rloc = &loc->sp.room;
	char const *dwts = "door";
	int door = 0;

	if (eplayer->klock) {
		nd_writef(player, "You can't move while being targeted.\n");
		return;
	}

	if (!map_has(player->location) || !(rloc->exits & e)) {
		nd_writef(player, "You can't go that way.\n");
		return;
	}

	stand_silent(player);

	if (rloc->doors & e) {
		if (e == E_UP || e == E_DOWN) {
			dwts = "hatch";
			door = 2;
		} else
			door = 1;

		nd_writef(player, "You open the %s.\n", dwts);
	}

	nd_writef(player, "You go %s%s%s.\n", ANSI_FG_BLUE ANSI_BOLD, e_name(e), ANSI_RESET);
	nd_owritef(player, "%s goes %s.\n", player->name, e_name(e));

	dest = st_there(player->location, e);
	if (!dest)
		dest = st_room(player, e);

	enter(player, dest, e);

	nd_owritef(player, "%s comes in from the %s.\n", player->name, e_name(e_simm(e)));

	if (door)
		nd_writef(player, "You close the %s.\n", dwts);
}

OBJ *
room_clean(OBJ *player, OBJ *here)
{
	OBJ *tmp;
	ROO *rhere = &here->sp.room;

	if (!(rhere->flags & RF_TEMP))
		return here;

	struct hash_cursor c = contents_iter(object_ref(here));
	while ((tmp = contents_next(&c))) {
		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;

		if (etmp->flags & EF_PLAYER)
			return here;
	}

	recycle(player, here);
	return NULL;
}

static void
walk(OBJ *player, enum exit e) {
	e_move(player, e);
}

static void
carve(OBJ *player, enum exit e)
{
	OBJ *there = NULL,
	    *here = object_get(player->location);
	ROO *rhere = &here->sp.room;
	int wall = 0;

	if (!e_ground(player->location, e)) {
		if (st_claim(player, here))
			return;

		if (player->value < ROOM_COST) {
			nd_writef(player, "You can't pay for that room.\n");
			return;
		}

		rhere->exits |= e;

		there = st_there(player->location, e);
		if (!there)
			there = st_room(player, e);
		/* wall_around(cmd, exit); */
		wall = 1;
	}

	e_move(player, e);
	there = object_get(player->location);

	if (here == there)
		return;

	st_claim(player, there);

	if (wall)
		wall_around(player, e_simm(e));
}

static void
uncarve(OBJ *player, enum exit e)
{
	const char *s0 = "is";
	OBJ *there,
	    *here = object_get(player->location);
	ROO *rhere = &here->sp.room;
	int ht, cd = e_ground(player->location, e);

	if (cd) {
		ht = rhere->flags & RF_TEMP;
		e_move(player, e);
		there = object_get(player->location);

		if (here == there)
			return;
	} else {
		if (!(rhere->exits & e)) {
                        nd_writef(player, "No exit there.\n");
                        return;
                }

		there = st_there(player->location, e);

		if (!there) {
			nd_writef(player, "No room there.\n");
			return;
		}
		s0 = "at";
	}

	ROO *rthere = &there->sp.room;

	if ((rthere->flags & RF_TEMP) || there->owner != object_ref(player)) {
		nd_writef(player, "You don't own th%s room.\n", s0);
		return;
	}

	rthere->flags ^= RF_TEMP;
	CBUG(!(rthere->flags & RF_TEMP));
	exits_infer(player, object_ref(there));
	if (cd) {
		if (ht && (rthere->doors & e_simm(e)))
			rthere->doors &= ~e_simm(e);
	} else
		room_clean(player, there);

	reward(player, "collect your materials", ROOM_COST);
}

static void
unwall(OBJ *player, enum exit e)
{
	int a, b, c, d;
	OBJ *there,
	    *here = object_get(player->location);
	ROO *rhere = &here->sp.room;

	a = here->owner == object_ref(player);
	b = rhere->flags & RF_TEMP;
	there = st_there(player->location, e);

	ROO *rthere = &there->sp.room;

	if (there) {
		c = there->owner == object_ref(player);
		d = rthere->flags & RF_TEMP;
	} else {
		c = 0;
		d = 1;
	}

	if (!((a && !b && (d || c))
	    || (c && !d && b))) {
		nd_writef(player, "You can't do that here.\n");
		return;
	}

	if (rhere->exits & e) {
		nd_writef(player, "There's an exit here already.\n");
		return;
	}

	debug("will create exit here %d there %d dir %c\n", object_ref(here), object_ref(there), e_dir(e));
	rhere->exits |= e;
	e_move(player, e);

	there = object_get(player->location);
	rthere = &there->sp.room;

	if (here == there)
		return;

	rthere->exits |= e_simm(e);
	nd_writef(player, "You tear down the wall.\n");
}

static inline int
gexit_claim(OBJ *player, enum exit e)
{
	int a, b, c, d;
	OBJ *here = st_there(player->location, e),
	    *there = object_get(player->location);
	ROO *rthere = &there->sp.room;

	a = here && here->owner == object_ref(player);
	c = rthere->flags & RF_TEMP;
	b = !c && there->owner == object_ref(player);
	d = e_ground(player->location, e);

	if (a && (b || c))
		return 0;

	if (!here || (here->sp.room.flags & RF_TEMP)) {
		if (b)
			return 0;
		if (d || c)
			return st_claim(player, there);
	}

	nd_writef(player, "You can't claim that exit.\n");
	return 1;
}

static inline int
gexit_claim_walk(OBJ *player, enum exit e)
{
	OBJ *here = object_get(player->location);
	ROO *rhere = &here->sp.room;

	if (!(rhere->exits & e)) {
		nd_writef(player, "No exit here.\n");
		return 1;
	}

	e_move(player, e);

	OBJ *there = object_get(player->location);

	if (here == there)
		return 1;

	ROO *rthere = &there->sp.room;

	CBUG(!(rthere->exits & e_simm(e)));

	return gexit_claim(player, e_simm(e));
}

static void
e_wall(OBJ *player, enum exit e)
{
	if (gexit_claim_walk(player, e))
		return;

	OBJ *here = object_get(player->location);
	ROO *rhere = &here->sp.room;

	rhere->exits &= ~e_simm(e);

	OBJ *there = st_there(player->location, e_simm(e));

	if (there) {
		ROO *rthere = &there->sp.room;
		rthere->exits &= ~e;
	}

	nd_writef(player, "You build a wall.\n");
}

static void
door(OBJ *player, enum exit e)
{
	if (gexit_claim_walk(player, e))
		return;

	OBJ *where = object_get(player->location);
	ROO *rwhere = &where->sp.room;

	rwhere->doors |= e_simm(e);

	where = st_there(player->location, e_simm(e));

	if (where) {
		rwhere = &where->sp.room;
		rwhere->doors |= e;
	}

	nd_writef(player, "You place a door.\n");
}

static void
undoor(OBJ *player, enum exit e)
{
	if (gexit_claim_walk(player, e))
		return;

	OBJ *where = object_get(player->location);
	ROO *rwhere = &where->sp.room;

	rwhere->doors &= ~e_simm(e);

	where = st_there(player->location, e_simm(e));

	if (where) {
		rwhere = &where->sp.room;
		rwhere->doors &= ~e;
	}

	nd_writef(player, "You remove a door.\n");
}

static int
tell_pos(OBJ *player, struct cmd_dir cd) {
	pos_t pos;
	OBJ *target = cd.rep == 1 ? player : object_get((dbref) cd.rep);
	int ret = 1;

	if (target->type != TYPE_ENTITY) {
		nd_writef(player, "Invalid object type.\n");
		return 0;
	}

	map_where(pos, target->location);
	nd_writef(player, "0x%llx\n", MORTON_READ(pos));
	return ret;
}

int
st_teleport(OBJ *player, struct cmd_dir cd)
{
	pos_t pos;
	OBJ *there;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '?') {
		// X6? teleport to chivas
		OBJ *target = object_get((dbref) cd.rep);
		if (target->type == TYPE_ENTITY) {
			map_where(pos, target->location);
			ret = 1;
		}
	} else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	there = map_get(pos);
	if (!there)
		there = st_room_at(player, pos);
	CBUG(!there);
	/* if (!eplayer->gpt) */
	nd_writef(player, "Teleporting to 0x%llx.\n", cd.rep);
	enter(player, there, E_NULL);
	return ret;
}

void st_start(OBJ *player) {
	struct cmd_dir cmd_dir = { .rep = 0 };
	st_teleport(player, cmd_dir);
}

static int
pull(OBJ *player, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	OBJ *there, *here, *what;

	if (e == E_NULL)
		return 0;

	here = object_get(player->location);
	what = object_get((dbref) cd.rep);

	ROO *rhere = &here->sp.room;

	if (!(rhere->exits & e)
	    || !what
	    || what->type != TYPE_ROOM
	    || what->owner != object_ref(player)
	    || ((there = st_there(player->location, e))
		&& room_clean(player, there) == there))
	{
		nd_writef(player, "You cannot do that.\n");
		return 1;
	}

	st_pos(pos, player->location, e);
	map_put(pos, cd.rep, 0);
	exits_fix(player, cd.rep, e);
	e_move(player, e);
	return 1;
}

op_t op_map[] = {
	['r'] = { .op.a = &carve },
	['R'] = { .op.a = &uncarve },
	['d'] = { .op.a = &door } ,
	['D'] = { .op.a = &undoor },
	['w'] = { .op.a = &e_wall },
	['W'] = { .op.a = &unwall },
	['x'] = { .op.b = &tell_pos, .type = 1 },
	['X'] = { .op.b = &st_teleport, .type = 1 },
	['#'] = { .op.b = &pull, .type = 1 },
};

static int
st_cmd_dir(struct cmd_dir *res, const char *cmd)
{
	int ofs = 0;
	morton_t rep = 1;
	char *end, sc;

	if (isdigit(cmd[0])) {
		rep = strtoull(cmd, &end, 0);
		ofs += end - &cmd[0];
	}

	res->dir = sc = cmd[ofs];
	res->e = dir_e(sc);
	res->rep = rep;

	return ofs;
}

int
st_v(OBJ *player, char const *opcs)
{
	struct cmd_dir cd;
	char const *s = opcs;

	for (;*s;) {
		int ofs = 0;
		op_t op = op_map[(int) *s]; // the op
		op_a_t *aop = op.type ? NULL : op.op.a; 
		ofs += !!(aop || op.type);
		ofs += st_cmd_dir(&cd, s + ofs);

		if (!(aop || op.op.b))
			aop = &walk;

		if (aop) {
			if (cd.e == E_NULL)
				return opcs - s;

			ofs ++;
			int j;
			for (j = 0; j < cd.rep; j++)
				aop(player, cd.e);
		} else
			ofs += op.op.b(player, cd);

		s += ofs;
	}

	look_around(player);
	return s - opcs;
}

struct st_key {
	uint64_t key;
	unsigned shift;
} __attribute__((packed));

void
echo(char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_dwritef(g_player, fmt, va);
	va_end(va);
};

void
oecho(char *format, ...) {
	va_list args;
	va_start(args, format);
	nd_owritef(g_player, format, args);
	va_end(args);
}

static void
st_open(struct st_key st_key, int owner)
{
	char filename[BUFSIZ];

	uint64_t short_key = st_key.key >> st_key.shift;
	snprintf(filename, sizeof(filename), "/var/nd/st/%u/%llu/libnd.so", st_key.shift, short_key);
	void *sl = dlopen(filename, RTLD_LAZY | RTLD_LOCAL);

	if (!sl)
		fprintf(stderr, "dlopen error for %d, %s: %s\n", owner, filename, dlerror());
	else {
		fprintf(stderr, "dlopen %d, %s\n", owner, filename);
		struct nd *ind = dlsym(sl, "nd");
		*ind = nd;
	}

	hash_cput(sl_hd, &st_key, sizeof(st_key), &sl, sizeof(void *));
	hash_cput(owner_hd, &st_key, sizeof(st_key), &owner, sizeof(owner));
}

struct st_key
st_key_new(uint64_t key, unsigned shift) {
	struct st_key st_key;
	st_key.key = key >> shift;
	st_key.shift = shift;
	return st_key;
}

void
st_put(int owner, uint64_t key, unsigned shift) {
	char buf[BUFSIZ];
	struct st_key st_key = st_key_new(key, shift);
	size_t len = snprintf(buf, sizeof(buf), "var/nd/st/%u/", shift);
	mkdir(buf, 0750);
	snprintf(buf + len, sizeof(buf) - len, "%llu", key >> shift);
	mkdir(buf, 0750);
	struct passwd *pw = getpwnam(object_get(owner)->name);
	chown(buf, pw->pw_uid, pw->pw_gid);
	st_open(st_key, owner);
}

void
st_init() {
	nd.echo = &echo;
	nd.oecho = &oecho;

	owner_hd = hash_cinit("/var/nd/st.db", NULL, 0644, 0);
	sl_hd = hash_init();
	struct hash_cursor c = hash_iter(owner_hd);
	struct st_key st_key;
	int owner;

	while (hash_next(&st_key, &owner, &c))
		st_open(st_key, owner);
}

void
st_sync() {
	hash_sync(owner_hd);
}

void
st_close() {
	struct hash_cursor c = hash_iter(sl_hd);
	struct st_key key;
	void *sl;

	while (hash_next(&key, &sl, &c))
		dlclose(sl);

	hash_close(sl_hd);
	hash_close(owner_hd);
}

typedef void (*st_run_cb)(void *player);

int
st_get(uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (hash_cget(owner_hd, &owner, &st_key, sizeof(st_key)) == -1)
		return -1;

	return owner;
}

inline static int
_st_can(int ref, uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (hash_cget(owner_hd, &owner, &st_key, sizeof(st_key)) == -1)
		return 0;

	return owner == ref;
}

static long int
st_hish_shift(OBJ *player, uint64_t position)
{
	int ref = object_ref(player);

	if (_st_can(ref, 0, 64))
		return 64;

	for (int i = 63; i >= 0; i--)
		if (_st_can(ref, position, i))
			return i;

	return -1;
}

inline static void
_st_run(OBJ *player, char *symbol, uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	void *sl;

	if (hash_cget(sl_hd, &sl, &st_key, sizeof(st_key)) == -1) {
		fprintf(stderr, ".");
		return;
	}

	st_run_cb cb = dlsym(sl, symbol);

	if (!cb) {
		fprintf(stderr, "x");
		return;
	}

	fprintf(stderr, "X");
	(*cb)(player);
}

void
st_run(OBJ *player, char *symbol) {
	uint64_t position = map_mwhere(player->location);
	int i;
	g_player = player;

	_st_run(player, symbol, 0, 64);

	for (i = 63; i >= 0; i--)
		_st_run(player, symbol, position, i);

	fprintf(stderr, "\n");
}

void
do_stchown(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);

	if (argc < 2) {
		nd_writef(player, "Requires at least an argument\n");
		return;
	}

	OBJ *who = player_get(argv[1]);

	if (!who || who->type != TYPE_ENTITY || !(who->sp.entity.flags & EF_PLAYER)) {
		nd_writef(player, "Invalid target\n");
		return;
	}

	uint64_t position = argc > 2
		? strtoull(argv[3], NULL, 10)
		: map_mwhere(player->location);

	long int high_shift = st_hish_shift(player, position);

	unsigned shift = argc < 3 ? high_shift : strtoul(argv[2], NULL, 10);

	long long unsigned key = shift > 63 ? 0 : position;

	if (high_shift < shift) {
		nd_writef(player, "Permission denied\n");
		return;
	}

	st_put(object_ref(who), key, shift);
	nd_writef(player, "Set shift %d ownership to %s\n", shift, who->name);
}

void
do_streload(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);

	uint64_t position = argc > 2
		? strtoull(argv[2], NULL, 10)
		: map_mwhere(player->location);

	long int high_shift = st_hish_shift(player, position);

	unsigned shift = argc < 2 ? high_shift : strtoul(argv[1], NULL, 10);

	long long unsigned key = shift > 63 ? 0 : position;

	struct st_key st_key = st_key_new(key, shift);
	int owner;
	void *sl;

	if (hash_cget(owner_hd, &owner, &st_key, sizeof(st_key)) == -1
			|| owner != object_ref(player))
	{
		nd_writef(player, "Permission denied\n");
		return;
	}

	if (hash_cget(sl_hd, &sl, &st_key, sizeof(st_key)) == -1) {
		nd_writef(player, "Not open\n");
		return;
	}

	dlclose(sl);
	st_open(st_key, owner);
}

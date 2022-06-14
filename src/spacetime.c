#include "spacetime.h"

#include <stdlib.h>
#include <db4/db.h>
#include "io.h"
#include "debug.h"
#include "noise.h"
#include "map.h"
#include "entity.h"
#include "props.h"
#include "defaults.h"

#define PRECOVERY

#define MESGPROP_MARK	"@/mark"

#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

typedef void op_a_t(OBJ *player, enum exit e);
typedef int op_b_t(OBJ *player, struct cmd_dir cd);
typedef struct {
	union {
		op_a_t *a;
		op_b_t *b;
	} op;
	int type;
} op_t;

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
		.fname = "",
		.other = "",
		.dim = 5,
	},
	[E_EAST] = {
		.simm = E_WEST,
		.name = "east",
		.fname = "e;east",
		.other = "wnsud",
		.dim = 1, .dis = 1,
	},
	[E_SOUTH] = {
		.simm = E_NORTH,
		.name = "south",
		.fname = "s;south",
		.other = "ewudn",
		.dim = 0, .dis = 1,
	},
	[E_WEST] = {
		.simm = E_EAST,
		.name = "west",
		.fname = "w;west",
		.other = "nsude",
		.dim = 1, .dis = -1,
	}, 
	[E_NORTH] = {
		.simm = E_SOUTH,
		.name = "north",
		.fname = "n;north",
		.other = "sewud",
		.dim = 0, .dis = -1,
	},
	[E_UP] = {
		.simm = E_DOWN,
		.name = "up",
		.fname = "u;up",
		.other = "dnsew",
		.dim = 2, .dis = 1,
	},
	[E_DOWN] = {
		.simm = E_UP,
		.name = "down",
		.fname = "d;down",
		.other = "nsewu",
		.dim = 2, .dis = -1,
	},
};

unsigned short day_tick = 0;

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

static inline int
rarity_get() {
	register int r = random();
	if (r > RAND_MAX >> 1)
		return 0; // POOR
	if (r > RAND_MAX >> 2)
		return 1; // COMMON
	if (r > RAND_MAX >> 6)
		return 2; // UNCOMMON
	if (r > RAND_MAX >> 10)
		return 3; // RARE
	if (r > RAND_MAX >> 14)
		return 4; // EPIC
	return 5; // MYTHICAL
}

OBJ *
object_add(SKEL *sk, OBJ *where)
{
	OBJ *nu = object_new();
	nu->name = strdup(sk->name);
	nu->description = strdup(sk->description);
	nu->art = strdup(sk->art);
	nu->avatar = strdup(sk->avatar);
	nu->location = where;
	nu->owner = object_get(GOD);
	nu->type = TYPE_THING;
	if (where)
		PUSH(nu, where->contents);

	switch (sk->type) {
	case S_TYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = sk->sp.equipment.eqw;
			enu->msv = sk->sp.equipment.msv;
			enu->rare = rarity_get();
			CBUG(!where || where->type != TYPE_ENTITY);
			ENT *ewhere = &where->sp.entity;
			if (ewhere->fd > 0 && equip(where, nu))
				;
		}

		break;
	case S_TYPE_CONSUMABLE:
		{
			CON *cnu = &nu->sp.consumable;
			nu->type = TYPE_CONSUMABLE;
			cnu->food = sk->sp.consumable.food;
			cnu->drink = sk->sp.consumable.drink;
		}

		break;
	case S_TYPE_ENTITY:
		{
			ENT *enu = &nu->sp.entity;
			nu->type = TYPE_ENTITY;
			stats_init(enu, &sk->sp.entity);
			enu->flags = sk->sp.entity.flags;
			enu->wtso = sk->sp.entity.wt;
			birth(nu);
			object_drop(nu, sk->sp.entity.drop);
			enu->home = where;
		}

		break;
	case S_TYPE_PLANT:
		nu->type = TYPE_PLANT;
		object_drop(nu, sk->sp.plant.drop);
		nu->owner = object_get(GOD);
		break;
        case S_TYPE_BIOME:
		{
			ROO *rnu = &nu->sp.room;
			nu->type = TYPE_ROOM;
			rnu->exits = rnu->doors = 0;
			rnu->dropto = NULL;
			rnu->flags = RF_TEMP;
		}

		break;
	case S_TYPE_OTHER:
		break;
	}

	if (sk->type != S_TYPE_BIOME)
		mcp_content_in(where, nu);

	return nu;
}

void
object_drop(OBJ *where, struct drop **drop)
{
        register int i;

	for (; *drop; drop++)
		if (random() < (RAND_MAX >> (*drop)->y)) {
                        int yield = (*drop)->yield,
                            yield_v = (*drop)->yield_v;

                        if (!yield) {
                                object_add((*drop)->i, where);
                                continue;
                        }

                        yield += random() & yield_v;

                        for (i = 0; i < yield; i++)
                                object_add((*drop)->i, where);
                }
}

int
e_exit_can(OBJ *player, enum exit e) {
	return e_ground(player->location, e);
}

int
e_ground(OBJ *room, enum exit e)
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
e_fname(enum exit e) {
	return exit_map[e].fname;
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
st_update()
{
	const char * msg = NULL;
	dbref i;
	day_tick += DAYTICK;

	if (day_tick == 0)
		msg = "The sun rises.";
	else if (day_tick == (1 << (DAY_Y - 1)))
		msg = "The sun sets.";
	else
		return;

	for (i = 0; i < db_top; i++) {
		OBJ *o = object_get(i);
		if (o->type == TYPE_ENTITY) {
			ENT *eo = &o->sp.entity;
			notify(eo, msg);
		}
	}
}

static inline void
st_pos(pos_t p, OBJ *loc, enum exit e)
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
st_there(OBJ *where, enum exit e)
{
	pos_t pos;
	/* CBUG(!*e_name(e)); */
	st_pos(pos, where, e);
	return map_get(pos);
}

static inline void
reward(OBJ *player, const char *msg, int amount)
{
	ENT *eplayer = &player->sp.entity;
	player->value += amount;
	notifyf(eplayer, "You %s. (+%dp)", msg, amount);
}

static inline int
fee_fail(OBJ *player, char *desc, char *info, int cost)
{
	ENT *eplayer = &player->sp.entity;
	int v = player->value;
	if (v < cost) {
		notifyf(eplayer, "You can't afford to %s. (%dp)", desc, cost);
		return 1;
	} else {
		player->value -= cost;
		notifyf(eplayer, "%s (-%dp). %s",
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
	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;
	const char *s;
	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		OBJ *there = st_there(here, e2);

		if ((rhere->exits & e2) && there)
			rhere->exits ^= e2;
	}
}

static int
st_claim(OBJ *player, OBJ *room) {
	ENT *eplayer = &player->sp.entity;
	ROO *rroom = &room->sp.room;

	if (!(rroom->flags & RF_TEMP)) {
		if (room->owner != player) {
			notify(eplayer, "You don't own this room");
			return 1;
		}

		return 0;
	}

	if (fee_noname(player))
		return 1;

	rroom->flags ^= RF_TEMP;
	CBUG(rroom->flags & RF_TEMP);
	room->owner = player;

	return 0;
}

static inline void
exits_fix(OBJ *player, OBJ *there, enum exit e)
{
	ROO *rthere = &there->sp.room;
	const char *s;

	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		OBJ *othere = st_there(there, e2);

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
exits_infer(OBJ *player, OBJ *here)
{
	ROO *rhere = &here->sp.room;
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		OBJ *there = st_there(here, e);

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
stones_add(OBJ *where, enum biome b, pos_t p) {
	noise_t v = uhash((const char *) p, sizeof(pos_t), 0);
	unsigned char n = v & 0x3, i;
	static struct object_skeleton stone = {
                .name = "stone",
                .art = "stones.jpg",
                .description = "Solid stone(s)",
                .avatar = "stones_avatar.jpg"
        };
	if (b == BIOME_WATER)
		return;

        if (!(n && (v & 0x18) && (v & 0x20)))
                return;

        for (i = 0; i < n; i++)
                object_add(&stone, where);
}

static inline void
others_add(OBJ *where, enum biome b, pos_t p)
{
        stones_add(where, b, p);
}

static OBJ *
st_room_at(OBJ *player, pos_t pos)
{
	struct bio *bio;
	bio = noise_point(pos);
        OBJ *there = object_add(&biomes[bio->bio_idx], NULL);
	ROO *rthere = &there->sp.room;
	map_put(pos, there, DB_NOOVERWRITE);
	exits_infer(player, there);

	if (pos[2] != 0)
		return there;

	rthere->floor = bio->bio_idx;
	entities_add(there, bio->bio_idx, bio->pd.n);
	others_add(there, bio->bio_idx, pos);
	plants_add(there,
			&bio->pd, bio->ty,
			bio->tmp, bio->rn);
	return there;
}

void
do_bio(command_t *cmd) {
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	struct bio *bio;
	pos_t pos;
	map_where(pos, player->location);
	bio = noise_point(pos);
	notifyf(eplayer, "tmp %d rn %u bio %s(%d)",
		bio->tmp, bio->rn, biomes[bio->bio_idx].name, bio->bio_idx);
}

static OBJ *
st_room(OBJ *player, enum exit e)
{
	pos_t pos;
	OBJ *here = player->location,
	    *there;

	st_pos(pos, here, e);
	there = st_room_at(player, pos);

	return there;
}

static void
e_move(OBJ *player, enum exit e) {
	ENT *eplayer = &player->sp.entity;
	OBJ *loc = player->location,
	    *dest;
	ROO *rloc = &loc->sp.room;
	char const *dwts = "door";
	int door = 0;

	if (!(rloc->exits & e)) {
		notify(eplayer, "You can't go that way.");
		return;
	}

	stand_silent(player);

	if (rloc->doors & e) {
		if (e == E_UP || e == E_DOWN) {
			dwts = "hatch";
			door = 2;
		} else
			door = 1;

		notifyf(eplayer, "You open the %s.", dwts);
	}

	dest = st_there(loc, e);
	notifyf(eplayer, "You go %s.", e_name(e));

	if (!dest)
		dest = st_room(player, e);

	enter(player, dest);

	if (door)
		notifyf(eplayer, "You close the %s.", dwts);
}

OBJ *
room_clean(OBJ *player, OBJ *here)
{
	ROO *rhere = &here->sp.room;
	OBJ *tmp;

	if (!(rhere->flags & RF_TEMP))
		return here;

	tmp = here->contents;
	FOR_LIST(tmp, tmp) {
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
	ENT *eplayer = &player->sp.entity;
	OBJ *there = NULL,
	    *here = player->location;
	ROO *rhere = &here->sp.room;
	int wall = 0;

	if (!e_ground(here, e)) {
		if (st_claim(player, here))
			return;

		if (player->value < ROOM_COST) {
			notify(eplayer, "You can't pay for that room");
			return;
		}

		rhere->exits |= e;

		there = st_there(here, e);
		if (!there)
			there = st_room(player, e);
		/* wall_around(cmd, exit); */
		wall = 1;
	}

	e_move(player, e);
	there = player->location;

	if (here == there)
		return;

	st_claim(player, there);

	if (wall)
		wall_around(player, e_simm(e));
}

static void
uncarve(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;
	const char *s0 = "is";
	OBJ *there,
	    *here = player->location;
	ROO *rhere = &here->sp.room;
	int ht, cd = e_ground(here, e);

	if (cd) {
		ht = rhere->flags & RF_TEMP;
		e_move(player, e);
		there = player->location;

		if (here == there)
			return;
	} else {
		if (!(rhere->exits & e)) {
                        notifyf(eplayer, "No exit there");
                        return;
                }

		there = st_there(here, e);

		if (!there) {
			notify(eplayer, "No room there");
			return;
		}
		s0 = "at";
	}

	ROO *rthere = &there->sp.room;

	if ((rthere->flags & RF_TEMP) || there->owner != player) {
		notifyf(eplayer, "You don't own th%s room.", s0);
		return;
	}

	rthere->flags ^= RF_TEMP;
	CBUG(!(rthere->flags & RF_TEMP));
	exits_infer(player, there);
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
	ENT *eplayer = &player->sp.entity;
	int a, b, c, d;
	OBJ *there,
	    *here = player->location;
	ROO *rhere = &here->sp.room;

	a = here->owner == player;
	b = rhere->flags & RF_TEMP;
	there = st_there(here, e);

	ROO *rthere = &there->sp.room;

	if (there) {
		c = there->owner == player;
		d = rthere->flags & RF_TEMP;
	} else {
		c = 0;
		d = 1;
	}

	if (!((a && !b && (d || c))
	    || (c && !d && b))) {
		notify(eplayer, "You can't do that here.");
		return;
	}

	if (rhere->exits & e) {
		notify(eplayer, "There's an exit here already.");
		return;
	}

	debug("will create exit here %d there %d dir %c\n", object_ref(here), object_ref(there), e_dir(e));
	rhere->exits |= e;
	e_move(player, e);

	there = player->location;
	rthere = &there->sp.room;

	if (here == there)
		return;

	rthere->exits |= e_simm(e);
	notify(eplayer, "You tear down the wall.");
}

static inline int
gexit_claim(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;
	int a, b, c, d;
	OBJ *here = st_there(player, e),
	    *there = player->location;
	ROO *rthere = &there->sp.room;

	a = here && here->owner == player;
	c = rthere->flags & RF_TEMP;
	b = !c && there->owner == player;
	d = e_ground(there, e);

	if (a && (b || c))
		return 0;

	if (!here || (here->sp.room.flags & RF_TEMP)) {
		if (b)
			return 0;
		if (d || c)
			return st_claim(player, there);
	}

	notify(eplayer, "You can't claim that exit");
	return 1;
}

static inline int
gexit_claim_walk(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;

	if (!(rhere->exits & e)) {
		notify(eplayer, "No exit here");
		return 1;
	}

	e_move(player, e);

	OBJ *there = player->location;

	if (here == there)
		return 1;

	ROO *rthere = &there->sp.room;

	CBUG(!(rthere->exits & e_simm(e)));

	return gexit_claim(player, e_simm(e));
}

static void
e_wall(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;
	if (gexit_claim_walk(player, e))
		return;

	OBJ *here = player->location;
	ROO *rhere = &here->sp.room;

	rhere->exits &= ~e_simm(e);

	OBJ *there = st_there(here, e_simm(e));

	if (there) {
		ROO *rthere = &there->sp.room;
		rthere->exits &= ~e;
	}

	notify(eplayer, "You build a wall.");
}

static void
door(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;

	if (gexit_claim_walk(player, e))
		return;

	OBJ *where = player->location;
	ROO *rwhere = &where->sp.room;

	rwhere->doors |= e_simm(e);

	where = st_there(where, e_simm(e));

	if (where) {
		rwhere = &where->sp.room;
		rwhere->doors |= e;
	}

	notify(eplayer, "You place a door.");
}

static void
undoor(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;

	if (gexit_claim_walk(player, e))
		return;

	OBJ *where = player->location;
	ROO *rwhere = &where->sp.room;

	rwhere->doors &= ~e_simm(e);

	where = st_there(where, e_simm(e));

	if (where) {
		rwhere = &where->sp.room;
		rwhere->doors &= ~e;
	}

	notify(eplayer, "You remove a door.");
}

static int
tell_pos(OBJ *player, struct cmd_dir cd) {
	ENT *eplayer = &player->sp.entity;
	pos_t pos;
	OBJ *target = cd.rep == 1 ? player : object_get((dbref) cd.rep);
	int ret = 1;

	if (target->type != TYPE_ENTITY) {
		notify(eplayer, "Invalid object type.");
		return 0;
	}

	map_where(pos, target->location);
	notifyf(eplayer, "0x%llx", MORTON_READ(pos));
	return ret;
}

int
st_teleport(OBJ *player, struct cmd_dir cd)
{
	ENT *eplayer = &player->sp.entity;
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
	notifyf(eplayer, "Teleporting to 0x%llx.", cd.rep);
	enter(player, there);
	return ret;
}

static int
mark(OBJ *player, struct cmd_dir cd)
{
	pos_t pos;
	char value[32];
	if (cd.rep == 1)
		map_where(pos, player->location);
	else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	snprintf(value, 32, "0x%llx", MORTON_READ(pos));
	set_property_mark(player, MESGPROP_MARK, cd.dir, value);
	return 1;
}

static int
recall(OBJ *player, struct cmd_dir cd)
{
	const char *xs = get_property_mark(player, MESGPROP_MARK, cd.dir);
	if (!xs)
		return 0;
	cd.rep = strtoull(xs, NULL, 0);
	cd.dir = '\0';
	st_teleport(player, cd);
	return 1;
}

static int
pull(OBJ *player, struct cmd_dir cd)
{
	ENT *eplayer = &player->sp.entity;
	pos_t pos;
	enum exit e = cd.e;
	OBJ *there, *here, *what;

	if (e == E_NULL)
		return 0;

	here = player->location;
	what = object_get((dbref) cd.rep);

	ROO *rhere = &here->sp.room;

	if (!(rhere->exits & e)
	    || !what
	    || what->type != TYPE_ROOM
	    || what->owner != player
	    || ((there = st_there(here, e))
		&& room_clean(player, there) == there))
	{
		notify(eplayer, "You cannot do that.");
		return 1;
	}

	st_pos(pos, here, e);
	map_put(pos, what, 0);
	exits_fix(player, what, e);
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
	['m'] = { .op.b = &mark, .type = 1 },
	['"'] = { .op.b = &recall, .type = 1 },
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

	return s - opcs;
}

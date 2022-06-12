#include "io.h"
#include "entity.h"
#include "geography.h"
#include "mdb.h"
#include "props.h"
#include "match.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include <stdlib.h>
#ifdef __OpenBSD__
#include <db4/db.h>
#else
#include <db.h>
#endif
#include "kill.h"
#include "externs.h"
#include "map.h"
#include "noise.h"
#include "hash.h"
#include "view.h"
#include "mob.h"

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

unsigned short day_tick = 0;

/* }}} */

void
geo_update()
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

/* OPS {{{ */

/* Basic {{{ */

static inline void
geo_pos(pos_t p, OBJ *loc, enum exit e)
{
	pos_t aux;
	map_where(aux, loc);
	pos_move(p, aux, e);
	CBUG(!*e_name(e));
	/* debug("geo_pos %s 0x%llx -> 0x%llx\n", */
	/* 		e_name(e), */
	/* 		* (morton_t *) aux, */
	/* 		* (morton_t *) p); */
}

OBJ *
geo_there(OBJ *where, enum exit e)
{
	pos_t pos;
	/* CBUG(!*e_name(e)); */
	geo_pos(pos, where, e);
	return map_get(pos);
}

/* }}} */

/* New room {{{ */

// TODO fee -> use legacy system (and improve it)

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
		OBJ *there = geo_there(here, e2);

		if ((rhere->exits & e2) && there)
			rhere->exits ^= e2;
	}
}

int
geo_claim(OBJ *player, OBJ *room) {
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
		OBJ *othere = geo_there(there, e2);

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
		OBJ *there = geo_there(here, e);

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
geo_room_at(OBJ *player, pos_t pos)
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
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	struct bio *bio;
	pos_t pos;
	map_where(pos, player->location);
	bio = noise_point(pos);
	notifyf(eplayer, "tmp %d rn %u bio %s(%d)",
		bio->tmp, bio->rn, biomes[bio->bio_idx].name, bio->bio_idx);
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

	dest = geo_there(loc, e);
	notifyf(eplayer, "You go %s.", e_name(e));

	if (!dest)
		dest = geo_room(player, e);

	enter(player, dest);

	if (door)
		notifyf(eplayer, "You close the %s.", dwts);
}

// exclusively called by trigger() and carve, in vertical situations
OBJ *
geo_room(OBJ *player, enum exit e)
{
	pos_t pos;
	OBJ *here = player->location,
	    *there;

	geo_pos(pos, here, e);
	there = geo_room_at(player, pos);

	return there;
}

/* }}} */

/* OPS {{{ */

/* used only on enter_room */

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
		if (geo_claim(player, here))
			return;

		if (player->value < ROOM_COST) {
			notify(eplayer, "You can't pay for that room");
			return;
		}

		rhere->exits |= e;

		there = geo_there(here, e);
		if (!there)
			there = geo_room(player, e);
		/* wall_around(cmd, exit); */
		wall = 1;
	}

	e_move(player, e);
	there = player->location;

	if (here == there)
		return;

	geo_claim(player, there);

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

		there = geo_there(here, e);

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
	there = geo_there(here, e);

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
	OBJ *here = geo_there(player, e),
	    *there = player->location;
	ROO *rthere = &there->sp.room;

	a = here && here->owner == player;
	c = rthere->flags & RF_TEMP;
	b = !c && there->owner == player;
	d = e_ground(there, e_dir(e));

	if (a && (b || c))
		return 0;

	if (!here || (here->sp.room.flags & RF_TEMP)) {
		if (b)
			return 0;
		if (d || c)
			return geo_claim(player, there);
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

	OBJ *there = geo_there(here, e_simm(e));

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

	where = geo_there(where, e_simm(e));

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

	where = geo_there(where, e_simm(e));

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
geo_teleport(OBJ *player, struct cmd_dir cd)
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
		there = geo_room_at(player, pos);
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
	geo_teleport(player, cd);
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
	    || ((there = geo_there(here, e))
		&& room_clean(player, there) == there))
	{
		notify(eplayer, "You cannot do that.");
		return 1;
	}

	geo_pos(pos, here, e);
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
	['X'] = { .op.b = &geo_teleport, .type = 1 },
	['m'] = { .op.b = &mark, .type = 1 },
	['"'] = { .op.b = &recall, .type = 1 },
	['#'] = { .op.b = &pull, .type = 1 },
};

/* }}} */

/* }}} */

/* PARSE {{{ */

static int
geo_cmd_dir(struct cmd_dir *res, const char *cmd)
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
geo_v(OBJ *player, char const *opcs)
{
	struct cmd_dir cd;
	char const *s = opcs;

	for (;*s;) {
		int ofs = 0;
		op_t op = op_map[(int) *s]; // the op
		op_a_t *aop = op.type ? NULL : op.op.a; 
		ofs += !!(aop || op.type);
		ofs += geo_cmd_dir(&cd, s + ofs);

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

/* }}} */

int gexits(ROO *rwhere) {
        int i, ret;
        register char *s;
        for (s = "wsnedu", ret = 0, i = 1; *s; s++, i <<= 1) {
		if (!(rwhere->exits & dir_e(*s)))
                        continue;
                ret |= i;
        }
        return ret;
}

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
#include "web.h"
#include "search.h"
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

typedef void op_a_t(dbref player, enum exit e);
typedef int op_b_t(dbref player, struct cmd_dir cd);
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

	for (i = 0; i < db_top; i++)
		if (Typeof(i) == TYPE_ENTITY)
			notify(i, msg);
}

/* OPS {{{ */

/* Basic {{{ */

static inline void
geo_pos(pos_t p, dbref loc, enum exit e)
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

dbref
geo_there(dbref where, enum exit e)
{
	pos_t pos;
	/* CBUG(!*e_name(e)); */
	geo_pos(pos, where, e);
	return map_get(pos);
}

/* }}} */

/* exit {{{ */

static dbref
gexit(dbref loc, dbref loc2, enum exit e)
{
	dbref ref;

	ref = object_new();

	/* Initialize everything */
	NAME(ref) = alloc_string(e_fname(e));
	db[ref].location = loc;
	OWNER(ref) = 1;
	FLAGS(ref) = TYPE_EXIT;

	/* link it in */
	PUSH(ref, db[loc].sp.room.exits);

	db[ref].sp.exit.ndest = 1;
	db[ref].sp.exit.dest = (dbref *) malloc(sizeof(dbref));
	e_exit_dest_set(ref, loc2);

	return ref;
}

/* }}} */

/* New room {{{ */

// TODO fee -> use legacy system (and improve it)

static inline void
reward(dbref player, const char *msg, int amount)
{
	SETVALUE(player, GETVALUE(player) + amount);
	notifyf(player, "You %s. (+%dp)", msg, amount);
}

static inline int
fee_fail(dbref player, char *desc, char *info, int cost)
{
	int v = GETVALUE(player);
	if (v < cost) {
		notifyf(player, "You can't afford to %s. (%dp)", desc, cost);
		return 1;
	} else {
		SETVALUE(player, v - cost);
		notifyf(player, "%s (-%dp). %s",
			   desc, cost, info);
		return 0;
	}
}

static inline int
fee_noname(dbref player) {
	return fee_fail(player, "claim a room", "", ROOM_COST);
}

static inline void
wall_around(dbref player, dbref exit)
{
	dbref here = getloc(exit);
	const char *s;
	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit = e_exit_where(player, here, e);
		dbref there = geo_there(here, e);

		if (oexit >= 0 && there < 0)
			recycle(player, oexit);
	}
}

int
geo_claim(dbref player, dbref room) {
	if (!(ROOM(room)->flags & RF_TEMP)) {
		if (OWNER(room) != player) {
			notify(player, "You don't own this room");
			return 1;
		}

		return 0;
	}

	if (fee_noname(player))
		return 1;

	ROOM(room)->flags ^= RF_TEMP;
	CBUG(ROOM(room)->flags & RF_TEMP);
	OWNER(room) = player;
	room = db[room].contents;

	DOLIST(room, room)
		if (Typeof(room) == TYPE_EXIT && e_exit_is(room))
			OWNER(room) = player;

	return 0;
}

static inline void
exits_fix(dbref player, dbref there, dbref exit)
{
	const char *s;

	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref othere_exit,
		      oexit = e_exit_where(player, there, e),
		      othere = geo_there(there, e);

		if (othere < 0 || (ROOM(othere)->flags & RF_TEMP)) {
			if (oexit < 0)
				continue;
			e_exit_dest_set(oexit, NOTHING);
			continue;
		}

		othere_exit = e_exit_where(player, othere, e_simm(e));

		if (oexit < 0) {
			if (othere_exit < 0)
				continue;

			gexit(there, othere, e);
			e_exit_dest_set(othere_exit, there);
			continue;
		}

		if (othere_exit < 0)
			recycle(player, oexit);

		else {
			e_exit_dest_set(oexit, othere);
			e_exit_dest_set(othere_exit, there);
		}
	}

	e_exit_dest_set(exit, there);
}

static void
exits_infer(dbref player, dbref here)
{
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit, exit_there, there = geo_there(here, e);

		if (there < 0) {
                        if (*s != 'u' && *s != 'd')
                                gexit(here, NOTHING, e);
			continue;
		}

		exit_there = e_exit_where(player, there, e_simm(e));
		if (exit_there < 0)
			continue;

		oexit = gexit(here, there, e);
		/* if (there > 0 && !GETTMP(there)) */
		SETDOOR(oexit, GETDOOR(exit_there));
		e_exit_dest_set(exit_there, here);
	}
}

// TODO make more add functions similar and easy to insert here

static inline void
stones_add(dbref where, enum biome b, pos_t p) {
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
                object_add(stone, where);
}

static inline void
others_add(dbref where, enum biome b, pos_t p)
{
        stones_add(where, b, p);
}

static dbref
geo_room_at(dbref player, pos_t pos)
{
	struct bio *bio;
	dbref there;
	bio = noise_point(pos);
        there = object_add(biomes[bio->bio_idx], -1);
	map_put(pos, there, DB_NOOVERWRITE);
	db[there].sp.room.exits = NOTHING;
	db[there].sp.room.dropto = NOTHING;
	/* FLAGS(there) = TYPE_ROOM | (FLAGS(cmd->player) & JUMP_OK); */
	CBUG(there <= 0);
	exits_infer(player, there);

	if (pos[2] != 0)
		return there;

	db[there].sp.room.floor = bio->bio_idx;
	entities_add(there, bio->bio_idx, bio->pd.n);
	others_add(there, bio->bio_idx, pos);
	plants_add(player, there,
			&bio->pd, bio->ty,
			bio->tmp, bio->rn);
	return there;
}

void
do_bio(command_t *cmd) {
	struct bio *bio;
	pos_t pos;
	map_where(pos, getloc(cmd->player));
	bio = noise_point(pos);
	notifyf(cmd->player, "tmp %d rn %u bio %s(%d)",
		bio->tmp, bio->rn, biomes[bio->bio_idx].name, bio->bio_idx);
}

static void
e_move(dbref player, enum exit e) {
	const char dirs[] = { e_dir(e), '\0' };
	dbref exit = ematch_exit_at(player, getloc(player), dirs);;

	switch (exit) {
	case NOTHING:
		notify(player, "You can't go that way.");
		break;
	default:
		CBUG(!e_exit_is(exit));

		/* we got one */
		/* check to see if we got through */
		if (!can_doit(player, exit, "You can't go that way."))
			break;

		{
			dbref dest;
			union specific sp;
			sp = db[exit].sp;

			if (sp.exit.ndest && sp.exit.dest[0] != NOTHING)
				dest = sp.exit.dest[0];
			else
				dest = geo_there(getloc(exit), exit_e(exit));

			if (dest > 0)
				enter_room(player, dest, exit);
			else {
				dest = geo_room(player, exit);
				enter_room(player, dest, exit);
			}
		}
		break;
	}
}

// exclusively called by trigger() and carve, in vertical situations
dbref
geo_room(dbref player, dbref exit)
{
	pos_t pos;
	dbref here = getloc(exit), there;
	enum exit e = exit_e(exit);

	geo_pos(pos, here, e);
	there = geo_room_at(player, pos);

	return there;
}

/* }}} */

/* OPS {{{ */

/* used only on enter_room */

dbref
geo_clean(dbref player, dbref here)
{
	dbref tmp;

	if (!(ROOM(here)->flags & RF_TEMP))
		return here;

	tmp = db[here].contents;
	DOLIST(tmp, tmp)
		if (Typeof(tmp) == TYPE_ENTITY && (ENTITY(tmp)->flags & EF_PLAYER)) {
			/* CBUG(tmp == cmd->player); */
			return here;
		}

	recycle(player, here);
	return NOTHING;
}

void /* called on recycle */
gexit_snull(dbref player, dbref exit)
{
	enum exit e = exit_e(exit);
	dbref oexit,
	      room = getloc(exit),
	      there = geo_there(room, e);

	if (there < 0)
		return;

	oexit = e_exit_where(player, there, e_simm(e));
	if (oexit >= 0)
		e_exit_dest_set(oexit, NOTHING);
}

static void
walk(dbref player, enum exit e) {
#if 1
	dbref exit = e_exit_here(player, e),
	      there;

	if (exit >= 0) {
		there = e_exit_dest(exit);
		if (there > 0) {
			pos_t pos;
			geo_pos(pos, getloc(player), e);
			map_put(pos, there, 0);
			exits_fix(player, there, exit);
		}
	}
#endif
	e_move(player, e);
}

static void
carve(dbref player, enum exit e)
{
	dbref there = NOTHING,
	      here = getloc(player),
	      exit = e_exit_here(player, e);
	int wall = 0;

	if (!e_ground(getloc(player), e)) {
		if (geo_claim(player, here))
			return;
		if (GETVALUE(player) < ROOM_COST) {
			notify(player, "You can't pay for that room");
			return;
		}
		exit = e_exit_here(player, e);
		if (exit < 0)
			exit = gexit(here, there, e);
		there = geo_there(here, e);
		if (there < 0) {
			there = geo_room(player, exit);
			e_exit_dest_set(exit, there);
		}
		/* wall_around(cmd, exit); */
		wall = 1;
	}

	e_move(player, e);
	there = getloc(player);
	if (here == there)
		return;
	geo_claim(player, there);
	if (wall) {
		exit = e_exit_here(player, e_simm(e));
		wall_around(player, exit);
	}
}

static void
uncarve(dbref player, enum exit e)
{
	const char *s0 = "is";
	dbref there, here = getloc(player),
	      exit = e_exit_here(player, e);
	int ht, cd = e_ground(here, e);

	if (cd) {
		ht = ROOM(here)->flags & RF_TEMP;
		e_move(player, e);
		there = getloc(player);

		if (here == there)
			return;
	} else {
                if (exit < 0) {
                        notifyf(player, "No exit there");
                        return;
                }

		there = e_exit_dest(exit);
		if (there < 0) {
			notify(player, "No room there");
			return;
		}
		s0 = "at";
	}

	if ((ROOM(there)->flags & RF_TEMP) || OWNER(there) != player) {
		notifyf(player, "You don't own th%s room.", s0);
		return;
	}

	ROOM(there)->flags ^= RF_TEMP;
	CBUG(!(ROOM(there)->flags & RF_TEMP));
	exits_infer(player, there);
	if (cd) {
		exit = e_exit_here(player, e_simm(e));
		if (ht && GETDOOR(exit))
			SETDOOR(exit, 0);
	} else
		geo_clean(player, there);

	reward(player, "collect your materials", ROOM_COST);
}

static void
unwall(dbref player, enum exit e)
{
	int a, b, c, d;
	dbref exit, there,
	      here = getloc(player);

	a = OWNER(here) == player;
	b = ROOM(here)->flags & RF_TEMP;
	there = geo_there(here, e);

	if (there > 0) {
		c = OWNER(there) == player;
		d = ROOM(there)->flags & RF_TEMP;
	} else {
		c = 0;
		d = 1;
	}

	if (!((a && !b && (d || c))
	    || (c && !d && b))) {
		notify(player, "You can't do that here.");
		return;
	}

	exit = e_exit_here(player, e);

	if (exit >= 0) {
		notify(player, "There's an exit here already.");
		return;
	}

	debug("will create exit here %d there %d dir %c\n", here, there, e_dir(e));
	exit = gexit(here, there, e);
	e_move(player, e);

	there = getloc(player);
	if (here == there)
		return;

        if (Typeof(here) != TYPE_ROOM)
                here = NOTHING;

	gexit(there, here, e_simm(e));
	notify(player, "You tear down the wall.");
}

static inline int
gexit_claim(dbref player, dbref exit, dbref exit_there)
{
	int a, b, c, d;
	dbref here = getloc(exit),
	      there = getloc(exit_there);
	const char dir = NAME(exit)[0];

	a = here > 0 && OWNER(here) == player && OWNER(exit) == player;
	c = ROOM(there)->flags & RF_TEMP;
	b = !c && OWNER(there) == player && OWNER(exit_there) == player;
	d = e_ground(getloc(player), dir);

	if ( a && (b || c))
		return 0;

	if (here < 0 || (ROOM(here)->flags & RF_TEMP)) {
		if (b)
			return 0;
		if (d || c)
			return geo_claim(player, there);
	}

	notify(player, "You can't claim that exit");
	return 1;
}

static inline int
gexit_claim_walk(dbref player, dbref *exit_r,
		 dbref *exit_there_r, enum exit e)
{
	dbref here = getloc(player),
	      exit = e_exit_here(player, e), exit_there;

	if (exit < 0) {
		notify(player, "No exit here");
		return 1;
	}

	e_move(player, e);

	if (here == getloc(player))
		return 1;

	exit_there = e_exit_here(player, e_simm(e));
	CBUG(exit_there < 0);

	*exit_r = exit;
	*exit_there_r = exit_there;
	return gexit_claim(player, exit, exit_there);
}

static void
e_wall(dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(player, &exit, &exit_there, e))
		return;
	recycle(player, exit_there);
	if (Typeof(exit) != TYPE_GARBAGE)
		recycle(player, exit);
	notify(player, "You build a wall.");
}

static void
door(dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(player, &exit, &exit_there, e))
		return;
	SETDOOR(exit, 1);
	SETDOOR(exit_there, 1);
	notify(player, "You place a door.");
}

static void
undoor(dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(player, &exit, &exit_there, e))
		return;
	SETDOOR(exit_there, 0);
	SETDOOR(exit, 0);
	notify(player, "You remove a door.");
}

static int
tell_pos(dbref player, struct cmd_dir cd) {
	pos_t pos;
	dbref who = cd.rep == 1 ? player : (dbref) cd.rep;
	int ret = 1;

	if (Typeof(who) != TYPE_ENTITY) {
		notify(player, "Invalid object type.");
		return 0;
	}

	map_where(pos, getloc(who));
	notifyf(player, "0x%llx", MORTON_READ(pos));
	return ret;
}

int
geo_teleport(dbref player, struct cmd_dir cd)
{
	pos_t pos;
	dbref there;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '?') {
		// X6? teleport to chivas
		dbref who = (dbref) cd.rep;
		if (Typeof(who) == TYPE_ENTITY) {
			map_where(pos, getloc(who));
			ret = 1;
		}
	} else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	there = map_get(pos);
	if (there < 0)
		there = geo_room_at(player, pos);
	CBUG(there < 0);
	notifyf(player, "Teleporting to 0x%llx.", cd.rep);
	enter_room(player, there, NOTHING);
	return ret;
}

static int
mark(dbref player, struct cmd_dir cd)
{
	pos_t pos;
	char value[32];
	if (cd.rep == 1)
		map_where(pos, getloc(player));
	else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	snprintf(value, 32, "0x%llx", MORTON_READ(pos));
	set_property_mark(player, MESGPROP_MARK, cd.dir, value);
	return 1;
}

static int
recall(dbref player, struct cmd_dir cd)
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
pull(dbref player, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	dbref there, here = getloc(player),
	      exit = e_exit_here(player, e),
	      what = (dbref) cd.rep;

	if (e == E_NULL)
		return 0;

	here = getloc(player),
	exit = e_exit_here(player, e),
	what = (dbref) cd.rep;

	if (exit < 0 || what < 0
	    || Typeof(what) != TYPE_ROOM
	    || OWNER(what) != player
	    || ((there = e_exit_dest(exit)) > 0
		&& geo_clean(player, there) == there))
	{
		notify(player, "You cannot do that.");
		return 1;
	}

	geo_pos(pos, here, e);
	map_put(pos, what, 0);
	exits_fix(player, what, exit);
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
geo_v(dbref player, char const *opcs)
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

int gexits(dbref player, dbref where) {
        int i, ret;
        register char *s;
        for (s = "wsnedu", ret = 0, i = 1; *s; s++, i <<= 1) {
                dbref tmp = e_exit_where(player, where, dir_e(*s));
                if (tmp < 0)
                        continue;
                ret |= i;
        }
        return ret;
}

void geo_notify(int descr, dbref player) {

        dbref o[GEON_M], *o_p;
	pos_t pos;
	char buf[BUFFER_LEN + 2];
	McpMesg msg;

	map_where(pos, getloc(player));
        map_search(o, pos, GEON_RADIUS);
	mcp_mesg_init(&msg, MCP_WEB_PKG, "enter");
	snprintf(buf, sizeof(buf), "0x%llx", MORTON_READ(pos));
	mcp_mesg_arg_append(&msg, "x", buf);

	for (o_p = o; o_p < o + GEON_BDI; o_p++) {
		dbref loc = *o_p;
		if (loc < 0 || Typeof(loc) != TYPE_ROOM)
			continue;

		web_room_mcp(loc, &msg);
	}

	mcp_mesg_clear(&msg);
}

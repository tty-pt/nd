#include "geography.h"
#include "db.h"
#include "props.h"
#include "match.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include <stdlib.h>
#include <db4/db.h>
#include "kill.h"
#include "externs.h"
#include "web.h"
#include "map.h"
#include "view.h"
#undef NDEBUG
#include "debug.h"

#define PRECOVERY

#define MESGPROP_MARK	"@/mark"

#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

typedef void op_a_t(int descr, dbref player, enum exit e);
typedef int op_b_t(int descr, dbref player, struct cmd_dir cd);
typedef struct {
	union {
		op_a_t *a;
		op_b_t *b;
	} op;
	int type;
} op_t;

unsigned short day_tick = 0;

static inline void
others_add(int descr, dbref player, struct bio *b, dbref where, pos_t p)
{
	noise_t v = uhash(&p, sizeof(pos_t), 0);
	unsigned char n = v & 0x7;
	static struct obj stone = { "stone", "", "" };
	if (b->bio_idx == BIOME_WATER)
		return;
	if (n && (v & 0x18))
		obj_stack_add(stone, where, n);
}

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
		if (Typeof(i) == TYPE_PLAYER)
			notify_from(1, i, msg);
}

/* OPS {{{ */

/* Basic {{{ */

int
gexit_is(dbref exit)
{ 
	char const *fname = e_fname(exit_e(exit));
	/* debug("gexit_is? %s\n", fname); */
	return *fname && !strncmp(NAME(exit), fname, 4);
}

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

dbref
gexit_where(int descr, dbref player, dbref loc, enum exit e)
{
	struct match_data md;
	init_match_remote(descr, player, loc, e_name(e), TYPE_EXIT, &md),
	match_room_exits(loc, &md);
	return match_result(&md);
}

static inline dbref
gexit_dest(dbref exit)
{
#ifdef PRECOVERY
	if (!DBFETCH(exit)->sp.exit.ndest)
		return NOTHING;

	else
		return DBFETCH(exit)->sp.exit.dest[0];
#else
	CBUG(!DBFETCH(exit)->sp.exit.ndest);
	return DBFETCH(exit)->sp.exit.dest[0];
#endif
}

void
gexit_dest_set(dbref exit, dbref dest)
{
	union specific *sp = &DBFETCH(exit)->sp;
#ifdef PRECOVERY
	if (!sp->exit.ndest) {
		sp->exit.dest = (dbref *)malloc(sizeof(dbref));
		sp->exit.ndest = 1;
	}
#else
	CBUG(!sp->exit.ndest);
#endif
	sp->exit.dest[0] = dest;
}

static dbref
gexit(int descr, dbref player, dbref loc, dbref loc2, enum exit e)
{
	dbref ref;

	ref = new_object();

	/* Initialize everything */
	NAME(ref) = alloc_string(e_fname(e));
	DBFETCH(ref)->location = loc;
	OWNER(ref) = 1;
	FLAGS(ref) = TYPE_EXIT;

	/* link it in */
	PUSH(ref, DBFETCH(loc)->exits);
	DBDIRTY(loc);

	DBFETCH(ref)->sp.exit.ndest = 1;
	DBFETCH(ref)->sp.exit.dest = (dbref *) malloc(sizeof(dbref));
	gexit_dest_set(ref, loc2);
	DBDIRTY(ref);

	return ref;
}

static inline dbref
gexit_here(int descr, dbref player, enum exit e)
{
	return gexit_where(descr, player, getloc(player), e);
}

/* }}} */

/* New room {{{ */

// TODO fee -> use legacy system (and improve it)

static inline void
reward(dbref player, const char *msg, int amount)
{
	SETVALUE(player, GETVALUE(player) + amount);
	notify_fmt(player, "You %s. (+%dp)", msg, amount);
}

static inline int
fee_fail(dbref player, char *desc, char *info, int cost)
{
	int v = GETVALUE(player);
	if (v < cost) {
		notify_fmt(player, "You can't afford to %s. (%dp)", desc, cost);
		return 1;
	} else {
		SETVALUE(player, v - cost);
		notify_fmt(player, "%s (-%dp). %s",
			   desc, cost, info);
		return 0;
	}
}

static inline int
fee_noname(dbref player) {
	return fee_fail(player, "claim a room", "", ROOM_COST);
}

static inline void
wall_around(int descr, dbref player, dbref exit)
{
	dbref here = getloc(exit);
	const char *s;
	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit = gexit_where(descr, player, here, e);
		dbref there = geo_there(here, e);

		if (oexit >= 0 && there < 0)
			recycle(descr, player, oexit);
	}
}

int
geo_claim(int descr, dbref player, dbref room) {
	if (!GETTMP(room)) {
		if (OWNER(room) != player) {
			notify(player, "You don't own this room");
			return 1;
		}

		return 0;
	}

	if (fee_noname(player))
		return 1;

	SETTMP(room, 0);
	OWNER(room) = player;
	room = DBFETCH(room)->contents;

	DOLIST(room, room)
		if (Typeof(room) == TYPE_EXIT && gexit_is(room))
			OWNER(room) = player;

	return 0;
}

static inline void
exits_fix(int descr, dbref player, dbref there, dbref exit)
{
	const char *s;

	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref othere_exit,
		      oexit = gexit_where(descr, player, there, e),
		      othere = geo_there(there, e);

		if (othere < 0 || GETTMP(othere)) {
			if (oexit < 0)
				continue;
			gexit_dest_set(oexit, NOTHING);
			continue;
		}

		othere_exit = gexit_where(descr, player, othere, e_simm(e));

		if (oexit < 0) {
			if (othere_exit < 0)
				continue;

			gexit(descr, player, there, othere, e);
			gexit_dest_set(othere_exit, there);
			continue;
		}

		if (othere_exit < 0)
			recycle(descr, player, oexit);

		else {
			gexit_dest_set(oexit, othere);
			gexit_dest_set(othere_exit, there);
		}
	}

	gexit_dest_set(exit, there);
}

static void
exits_infer(int descr, dbref player, dbref here)
{
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit, exit_there, there = geo_there(here, e);

		if (there < 0) {
                        if (*s != 'u' && *s != 'd')
                                gexit(descr, player, here, NOTHING, e);
			continue;
		}

		exit_there = gexit_where(descr, player, there, e_simm(e));
		if (exit_there < 0)
			continue;

		oexit = gexit(descr, player, here, there, e);
		/* if (there > 0 && !GETTMP(there)) */
		SETDOOR(oexit, GETDOOR(exit_there));
		gexit_dest_set(exit_there, here);
	}
}

static dbref
geo_room_at(int descr, dbref player, pos_t pos)
{
	struct bio *bio;
	static const dbref loc = 0;
	dbref there = new_object();
	CBUG(there <= 0);
	map_put(pos, there, DB_NOOVERWRITE);
	NAME(there) = alloc_string("No name");
	DBFETCH(there)->location = loc;
	OWNER(there) = 1;
	DBFETCH(there)->exits = NOTHING;
	DBFETCH(there)->sp.room.dropto = NOTHING;
	FLAGS(there) = TYPE_ROOM | (FLAGS(player) & JUMP_OK);
	PUSH(there, DBFETCH(loc)->contents);
	DBDIRTY(there);
	DBDIRTY(loc);
	bio = noise_point(pos);
	CBUG(there <= 0);
	exits_infer(descr, player, there);
	SETTMP(there, 1);

	if (pos[2] != 0)
		return there;

	SETTREE(there, bio->pd.n);
	SETFLOOR(there, bio->bio_idx);
	mobs_add(bio, there);
	others_add(descr, player, bio, there, pos);
	plants_add(descr, player, there,
			&bio->pd, bio->ty,
			bio->tmp, bio->rn);
	DBDIRTY(there);
	DBDIRTY(loc);
	return there;
}

static void
e_move(int descr, dbref player, enum exit e) {
	const char dirs[] = { e_dir(e), '\0' };
	do_move(descr, player, dirs, 0);
}

// exclusively called by trigger() and carve, in vertical situations
dbref
geo_room(int descr, dbref player, dbref exit)
{
	pos_t pos;
	dbref here = getloc(exit), there;
	enum exit e = exit_e(exit);

	geo_pos(pos, here, e);
	there = geo_room_at(descr, player, pos);

	return there;
}

/* }}} */

/* OPS {{{ */

/* used only on enter_room */

dbref
geo_clean(int descr, dbref player, dbref here)
{
	dbref tmp;

	if (!GETTMP(here))
		return here;

	tmp = DBFETCH(here)->contents;
	DOLIST(tmp, tmp)
		if (Typeof(tmp) == TYPE_PLAYER) {
			CBUG(tmp == player);
			return here;
		}

	recycle(descr, player, here);
	return NOTHING;
}

void /* called on recycle */
gexit_snull(int descr, dbref player, dbref exit)
{
	enum exit e = exit_e(exit);
	dbref oexit,
	      room = getloc(exit),
	      there = geo_there(room, e);

	if (there < 0)
		return;

	oexit = gexit_where(descr, player, there, e_simm(e));
	if (oexit >= 0)
		gexit_dest_set(oexit, NOTHING);
}

static void
walk(int descr, dbref player, enum exit e) {
#if defined(PRECOVERY) && 0
	dbref exit = gexit_here(descr, player, e),
	      there;

	if (exit >= 0) {
		there = gexit_dest(exit);
		if (there > 0) {
			pos_t pos;
			geo_pos(pos, getloc(player), e);
			map_put(pos, there, 0);
			exits_fix(descr, player, there, exit);
		}
	}
#endif
	e_move(descr, player, e);
}

/* used in predicates.c {{{ */

int
geo_lock(dbref room, enum exit e)
{
	pos_t pos;

	if (e & (E_UP | E_DOWN))
		return 0;

	map_where(pos, room);
	return pos[2] == 0;
}

int
gexit_can(dbref player, dbref exit) {
	enum exit e = exit_e(exit);
	CBUG(exit < 0);
	CBUG(gexit_dest(exit) >= 0);
	return geo_lock(getloc(player), e);
}

/* }}} */

static void
carve(int descr, dbref player, enum exit e)
{
	dbref there = NOTHING,
	      here = getloc(player),
	      exit = gexit_here(descr, player, e);
	int wall = 0;

	if (!geo_lock(getloc(player), e)) {
		if (geo_claim(descr, player, here))
			return;
		if (GETVALUE(player) < ROOM_COST) {
			notify(player, "You can't pay for that room");
			return;
		}
		exit = gexit_here(descr, player, e);
		if (exit < 0)
			exit = gexit(descr, player, here, there, e);
		there = geo_there(here, e);
		if (there < 0) {
			there = geo_room(descr, player, exit);
			gexit_dest_set(exit, there);
		}
		wall_around(descr, player, exit);
		wall = 1;
	}

	e_move(descr, player, e);
	there = getloc(player);
	if (here == there)
		return;
	geo_claim(descr, player, there);
	if (wall) {
		exit = gexit_here(descr, player, e_simm(e));
		wall_around(descr, player, exit);
	}
}

static void
uncarve(int descr, dbref player, enum exit e)
{
	const char *s0 = "is";
	dbref there, here = getloc(player),
	      exit = gexit_here(descr, player, e);
	int ht, cd = geo_lock(here, e);

	if (cd) {
		ht = GETTMP(here);
		e_move(descr, player, e);
		there = getloc(player);

		if (here == there)
			return;
	} else {
		there = gexit_dest(exit);
		if (there < 0) {
			notify(player, "No room there");
			return;
		}
		s0 = "at";
	}

	if (GETTMP(there) || OWNER(there) != player) {
		notify_fmt(player, "You don't own th%s room.", s0);
		return;
	}

	SETTMP(there, 1);
	exits_infer(descr, player, there);
	if (cd) {
		exit = gexit_here(descr, player, e_simm(e));
		if (ht && GETDOOR(exit))
			SETDOOR(exit, 0);
	} else
		geo_clean(descr, player, there);

	reward(player, "collect your materials", ROOM_COST);
}

static void
unwall(int descr, dbref player, enum exit e)
{
	int a, b, c, d;
	dbref exit, there,
	      here = getloc(player);

	a = OWNER(here) == player;
	b = GETTMP(here);
	there = geo_there(here, e);

	if (there > 0) {
		c = OWNER(there) == player;
		d = GETTMP(there);
	} else {
		c = 0;
		d = 1;
	}

	if (!((a && !b && (d || c))
	    || (c && !d && b))) {
		notify(player, "You can't do that here.");
		return;
	}

	exit = gexit_here(descr, player, e);

	if (exit >= 0) {
		notify(player, "There's an exit here already.");
		return;
	}

	debug("will create exit here %d there %d dir %c\n", here, there, e_dir(e));
	exit = gexit(descr, player, here, there, e);
	e_move(descr, player, e);

	there = getloc(player);
	if (here == there)
		return;

	gexit(descr, player, there, here, e_simm(e));
	notify(player, "You tear down the wall.");
}

static inline int
gexit_claim(int descr, dbref player, dbref exit, dbref exit_there)
{
	int a, b, c, d;
	dbref here = getloc(exit),
	      there = getloc(exit_there);
	const char dir = NAME(exit)[0];

	a = here > 0 && OWNER(here) == player && OWNER(exit) == player;
	c = GETTMP(there);
	b = !c && OWNER(there) == player && OWNER(exit_there) == player;
	d = geo_lock(getloc(player), dir);

	if ( a && (b || c))
		return 0;

	if (here < 0 || GETTMP(here)) {
		if (b)
			return 0;
		if (d || c)
			return geo_claim(descr, player, there);
	}

	notify(player, "You can't claim that exit");
	return 1;
}

static inline int
gexit_claim_walk(dbref *exit_r, dbref *exit_there_r,
		 int descr, dbref player, enum exit e)
{
	dbref here = getloc(player),
	      exit = gexit_here(descr, player, e), exit_there;

	if (exit < 0) {
		notify(player, "No exit here");
		return 1;
	}

	e_move(descr, player, e);

	if (here == getloc(player))
		return 1;

	exit_there = gexit_here(descr, player, e_simm(e));
	CBUG(exit_there < 0);

	*exit_r = exit;
	*exit_there_r = exit_there;
	return gexit_claim(descr, player, exit, exit_there);
}

static void
wall(int descr, dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, e))
		return;
	recycle(descr, player, exit_there);
	recycle(descr, player, exit);
	notify(player, "You build a wall.");
}

static void
door(int descr, dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, e))
		return;
	SETDOOR(exit, 1);
	SETDOOR(exit_there, 1);
	notify(player, "You place a door.");
}

static void
undoor(int descr, dbref player, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, e))
		return;
	SETDOOR(exit_there, 0);
	SETDOOR(exit, 0);
	notify(player, "You remove a door.");
}

static int
tell_pos(int descr, dbref player, struct cmd_dir cd) {
	pos_t pos;
	dbref who = cd.rep == 1 ? player : (dbref) cd.rep;
	int ret = 1;

	if (Typeof(who) != TYPE_PLAYER) {
		notify(player, "Invalid object type.");
		return 0;
	}

	map_where(pos, getloc(who));
	notify_fmt(player, "0x%llx", pos);
	return ret;
}

static int
teleport(int descr, dbref player, struct cmd_dir cd)
{
	pos_t pos;
	dbref there;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '!') {
		memcpy(pos, &cd.rep, sizeof(cd.rep));
		ret = 1;
	} else if (cd.dir == '?') {
		dbref who = (dbref) cd.rep;
		if (Typeof(who) == TYPE_PLAYER) {
			map_where(pos, getloc(who));
			ret = 1;
		}
	} else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	there = map_get(pos);
	if (there < 0)
		there = geo_room_at(descr, player, pos);
	CBUG(there < 0);
	notify_fmt(player, "Teleporting to 0x%llx.", cd.rep);
	enter_room(descr, player, there, NOTHING);
	return ret;
}

static int
mark(int descr, dbref player, struct cmd_dir cd)
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
recall(int descr, dbref player, struct cmd_dir cd)
{
	const char *xs = get_property_mark(player, MESGPROP_MARK, cd.dir);
	if (!xs)
		return 0;
	cd.rep = strtoull(xs, NULL, 0);
	cd.dir = '\0';
	teleport(descr, player, cd);
	return 1;
}

static int
pull(int descr, dbref player, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	dbref there, here = getloc(player),
	      exit = gexit_here(descr, player, e),
	      what = (dbref) cd.rep;

	if (e == E_NULL)
		return 0;

	here = getloc(player),
	exit = gexit_here(descr, player, e),
	what = (dbref) cd.rep;

	if (exit < 0 || what < 0
	    || Typeof(what) != TYPE_ROOM
	    || OWNER(what) != player
	    || ((there = gexit_dest(exit)) > 0
		&& geo_clean(descr, player, there) == there))
	{
		notify(player, "You cannot do that.");
		return 1;
	}

	geo_pos(pos, here, e);
	map_put(pos, what, 0);
	exits_fix(descr, player, what, exit);
	e_move(descr, player, e);
	return 1;
}

op_t op_map[] = {
	['r'] = { .op.a = &carve },
	['R'] = { .op.a = &uncarve },
	['d'] = { .op.a = &door } ,
	['D'] = { .op.a = &undoor },
	['w'] = { .op.a = &wall },
	['W'] = { .op.a = &unwall },
	['x'] = { .op.b = &tell_pos, .type = 1 },
	['X'] = { .op.b = &teleport, .type = 1 },
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
geo_v(int descr, dbref player, char const *opcs)
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
				aop(descr, player, cd.e);
		} else
			ofs += op.op.b(descr, player, cd);

		s += ofs;
	}

	return s - opcs;
}

/* }}} */

int gexits(int descr, dbref player, dbref where) {
        int i, ret;
        register char *s;
        for (s = "wsnedu", ret = 0, i = 1; *s; s++, i <<= 1) {
                dbref tmp = gexit_where(descr, player, where, dir_e(*s));
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

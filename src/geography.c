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

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

typedef void op_a_t(command_t *cmd, enum exit e);
typedef int op_b_t(command_t *cmd, struct cmd_dir cd);
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
		if (Typeof(i) == TYPE_PLAYER)
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
gexit(command_t *cmd, dbref loc, dbref loc2, enum exit e)
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
	e_exit_dest_set(ref, loc2);
	DBDIRTY(ref);

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
wall_around(command_t *cmd, dbref exit)
{
	dbref here = getloc(exit);
	const char *s;
	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit = e_exit_where(cmd, here, e);
		dbref there = geo_there(here, e);

		if (oexit >= 0 && there < 0)
			recycle(cmd, oexit);
	}
}

int
geo_claim(command_t *cmd, dbref room) {
	dbref player = cmd->player;

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
		if (Typeof(room) == TYPE_EXIT && e_exit_is(room))
			OWNER(room) = player;

	return 0;
}

static inline void
exits_fix(command_t *cmd, dbref there, dbref exit)
{
	const char *s;

	for (s = e_other(exit_e(exit)); *s; s++) {
		enum exit e = dir_e(*s);
		dbref othere_exit,
		      oexit = e_exit_where(cmd, there, e),
		      othere = geo_there(there, e);

		if (othere < 0 || GETTMP(othere)) {
			if (oexit < 0)
				continue;
			e_exit_dest_set(oexit, NOTHING);
			continue;
		}

		othere_exit = e_exit_where(cmd, othere, e_simm(e));

		if (oexit < 0) {
			if (othere_exit < 0)
				continue;

			gexit(cmd, there, othere, e);
			e_exit_dest_set(othere_exit, there);
			continue;
		}

		if (othere_exit < 0)
			recycle(cmd, oexit);

		else {
			e_exit_dest_set(oexit, othere);
			e_exit_dest_set(othere_exit, there);
		}
	}

	e_exit_dest_set(exit, there);
}

static void
exits_infer(command_t *cmd, dbref here)
{
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		dbref oexit, exit_there, there = geo_there(here, e);

		if (there < 0) {
                        if (*s != 'u' && *s != 'd')
                                gexit(cmd, here, NOTHING, e);
			continue;
		}

		exit_there = e_exit_where(cmd, there, e_simm(e));
		if (exit_there < 0)
			continue;

		oexit = gexit(cmd, here, there, e);
		/* if (there > 0 && !GETTMP(there)) */
		SETDOOR(oexit, GETDOOR(exit_there));
		e_exit_dest_set(exit_there, here);
	}
}

// TODO make more add functions similar and easy to insert here

static inline void
others_add(command_t *cmd, dbref where, enum biome b, pos_t p)
{
	noise_t v = uhash((const char *) &p, sizeof(pos_t), 0);
	unsigned char n = v & 0x7;
	static struct obj stone = { "stone", "", "" };
	if (b == BIOME_WATER)
		return;
	if (n && (v & 0x18))
		obj_stack_add(stone, where, n);
}

static dbref
geo_room_at(command_t *cmd, pos_t pos)
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
	FLAGS(there) = TYPE_ROOM | (FLAGS(cmd->player) & JUMP_OK);
	PUSH(there, DBFETCH(loc)->contents);
	DBDIRTY(there);
	DBDIRTY(loc);
	bio = noise_point(pos);
	CBUG(there <= 0);
	exits_infer(cmd, there);
	SETTMP(there, 1);

	if (pos[2] != 0)
		return there;

	SETTREE(there, bio->pd.n);
	SETFLOOR(there, bio->bio_idx);
	mobs_add(there, bio->bio_idx, bio->pd.n);
	others_add(cmd, there, bio->bio_idx, pos);
	plants_add(cmd, there,
			&bio->pd, bio->ty,
			bio->tmp, bio->rn);
	DBDIRTY(there);
	DBDIRTY(loc);
	return there;
}

static void
e_move(command_t *cmd, enum exit e) {
	command_t new_cmd = *cmd;
	const char dirs[] = { e_dir(e), '\0' };
	new_cmd.argv[1] = (char *) dirs;
	do_move(&new_cmd);
}

// exclusively called by trigger() and carve, in vertical situations
dbref
geo_room(command_t *cmd, dbref exit)
{
	pos_t pos;
	dbref here = getloc(exit), there;
	enum exit e = exit_e(exit);

	geo_pos(pos, here, e);
	there = geo_room_at(cmd, pos);

	return there;
}

/* }}} */

/* OPS {{{ */

/* used only on enter_room */

dbref
geo_clean(command_t *cmd, dbref here)
{
	dbref tmp;

	if (!GETTMP(here))
		return here;

	tmp = DBFETCH(here)->contents;
	DOLIST(tmp, tmp)
		if (Typeof(tmp) == TYPE_PLAYER) {
			CBUG(tmp == cmd->player);
			return here;
		}

	recycle(cmd, here);
	return NOTHING;
}

void /* called on recycle */
gexit_snull(command_t *cmd, dbref exit)
{
	enum exit e = exit_e(exit);
	dbref oexit,
	      room = getloc(exit),
	      there = geo_there(room, e);

	if (there < 0)
		return;

	oexit = e_exit_where(cmd, there, e_simm(e));
	if (oexit >= 0)
		e_exit_dest_set(oexit, NOTHING);
}

static void
walk(command_t *cmd, enum exit e) {

#if 1
	dbref player = cmd->player;
	dbref exit = e_exit_here(cmd, e),
	      there;

	if (exit >= 0) {
		there = e_exit_dest(exit);
		if (there > 0) {
			pos_t pos;
			geo_pos(pos, getloc(player), e);
			map_put(pos, there, 0);
			exits_fix(cmd, there, exit);
		}
	}
#endif
	e_move(cmd, e);
}

static void
carve(command_t *cmd, enum exit e)
{
	dbref player = cmd->player;
	dbref there = NOTHING,
	      here = getloc(player),
	      exit = e_exit_here(cmd, e);
	int wall = 0;

	if (!e_ground(getloc(player), e)) {
		if (geo_claim(cmd, here))
			return;
		if (GETVALUE(player) < ROOM_COST) {
			notify(player, "You can't pay for that room");
			return;
		}
		exit = e_exit_here(cmd, e);
		if (exit < 0)
			exit = gexit(cmd, here, there, e);
		there = geo_there(here, e);
		if (there < 0) {
			there = geo_room(cmd, exit);
			e_exit_dest_set(exit, there);
		}
		/* wall_around(cmd, exit); */
		wall = 1;
	}

	e_move(cmd, e);
	there = getloc(player);
	if (here == there)
		return;
	geo_claim(cmd, there);
	if (wall) {
		exit = e_exit_here(cmd, e_simm(e));
		wall_around(cmd, exit);
	}
}

static void
uncarve(command_t *cmd, enum exit e)
{
	dbref player = cmd->player;
	const char *s0 = "is";
	dbref there, here = getloc(player),
	      exit = e_exit_here(cmd, e);
	int ht, cd = e_ground(here, e);

	if (cd) {
		ht = GETTMP(here);
		e_move(cmd, e);
		there = getloc(player);

		if (here == there)
			return;
	} else {
		there = e_exit_dest(exit);
		if (there < 0) {
			notify(player, "No room there");
			return;
		}
		s0 = "at";
	}

	if (GETTMP(there) || OWNER(there) != player) {
		notifyf(player, "You don't own th%s room.", s0);
		return;
	}

	SETTMP(there, 1);
	exits_infer(cmd, there);
	if (cd) {
		exit = e_exit_here(cmd, e_simm(e));
		if (ht && GETDOOR(exit))
			SETDOOR(exit, 0);
	} else
		geo_clean(cmd, there);

	reward(player, "collect your materials", ROOM_COST);
}

static void
unwall(command_t *cmd, enum exit e)
{
	int a, b, c, d;
	dbref player = cmd->player,
	      exit, there,
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

	exit = e_exit_here(cmd, e);

	if (exit >= 0) {
		notify(player, "There's an exit here already.");
		return;
	}

	debug("will create exit here %d there %d dir %c\n", here, there, e_dir(e));
	exit = gexit(cmd, here, there, e);
	e_move(cmd, e);

	there = getloc(player);
	if (here == there)
		return;

	gexit(cmd, there, here, e_simm(e));
	notify(player, "You tear down the wall.");
}

static inline int
gexit_claim(command_t *cmd, dbref exit, dbref exit_there)
{
	int a, b, c, d;
	dbref player = cmd->player,
	      here = getloc(exit),
	      there = getloc(exit_there);
	const char dir = NAME(exit)[0];

	a = here > 0 && OWNER(here) == player && OWNER(exit) == player;
	c = GETTMP(there);
	b = !c && OWNER(there) == player && OWNER(exit_there) == player;
	d = e_ground(getloc(player), dir);

	if ( a && (b || c))
		return 0;

	if (here < 0 || GETTMP(here)) {
		if (b)
			return 0;
		if (d || c)
			return geo_claim(cmd, there);
	}

	notify(player, "You can't claim that exit");
	return 1;
}

static inline int
gexit_claim_walk(command_t *cmd, dbref *exit_r,
		 dbref *exit_there_r, enum exit e)
{
	dbref player = cmd->player, here = getloc(player),
	      exit = e_exit_here(cmd, e), exit_there;

	if (exit < 0) {
		notify(player, "No exit here");
		return 1;
	}

	e_move(cmd, e);

	if (here == getloc(player))
		return 1;

	exit_there = e_exit_here(cmd, e_simm(e));
	CBUG(exit_there < 0);

	*exit_r = exit;
	*exit_there_r = exit_there;
	return gexit_claim(cmd, exit, exit_there);
}

static void
e_wall(command_t *cmd, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(cmd, &exit, &exit_there, e))
		return;
	recycle(cmd, exit_there);
	recycle(cmd, exit);
	notify(cmd->player, "You build a wall.");
}

static void
door(command_t *cmd, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(cmd, &exit, &exit_there, e))
		return;
	SETDOOR(exit, 1);
	SETDOOR(exit_there, 1);
	notify(cmd->player, "You place a door.");
}

static void
undoor(command_t *cmd, enum exit e)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(cmd, &exit, &exit_there, e))
		return;
	SETDOOR(exit_there, 0);
	SETDOOR(exit, 0);
	notify(cmd->player, "You remove a door.");
}

static int
tell_pos(command_t *cmd, struct cmd_dir cd) {
	pos_t pos;
	dbref who = cd.rep == 1 ? cmd->player : (dbref) cd.rep;
	int ret = 1;

	if (Typeof(who) != TYPE_PLAYER) {
		notify(cmd->player, "Invalid object type.");
		return 0;
	}

	map_where(pos, getloc(who));
	notifyf(cmd->player, "0x%llx", MORTON_READ(pos));
	return ret;
}

static int
teleport(command_t *cmd, struct cmd_dir cd)
{
	pos_t pos;
	dbref there;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '?') {
		// X6? teleport to chivas
		dbref who = (dbref) cd.rep;
		if (Typeof(who) == TYPE_PLAYER) {
			map_where(pos, getloc(who));
			ret = 1;
		}
	} else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	there = map_get(pos);
	if (there < 0)
		there = geo_room_at(cmd, pos);
	CBUG(there < 0);
	notifyf(cmd->player, "Teleporting to 0x%llx.", cd.rep);
	enter_room(cmd, there, NOTHING);
	return ret;
}

static int
mark(command_t *cmd, struct cmd_dir cd)
{
	pos_t pos;
	char value[32];
	if (cd.rep == 1)
		map_where(pos, getloc(cmd->player));
	else
		memcpy(pos, &cd.rep, sizeof(cd.rep));
	snprintf(value, 32, "0x%llx", MORTON_READ(pos));
	set_property_mark(cmd->player, MESGPROP_MARK, cd.dir, value);
	return 1;
}

static int
recall(command_t *cmd, struct cmd_dir cd)
{
	const char *xs = get_property_mark(cmd->player, MESGPROP_MARK, cd.dir);
	if (!xs)
		return 0;
	cd.rep = strtoull(xs, NULL, 0);
	cd.dir = '\0';
	teleport(cmd, cd);
	return 1;
}

static int
pull(command_t *cmd, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	dbref there, here = getloc(cmd->player),
	      exit = e_exit_here(cmd, e),
	      what = (dbref) cd.rep;

	if (e == E_NULL)
		return 0;

	here = getloc(cmd->player),
	exit = e_exit_here(cmd, e),
	what = (dbref) cd.rep;

	if (exit < 0 || what < 0
	    || Typeof(what) != TYPE_ROOM
	    || OWNER(what) != cmd->player
	    || ((there = e_exit_dest(exit)) > 0
		&& geo_clean(cmd, there) == there))
	{
		notify(cmd->player, "You cannot do that.");
		return 1;
	}

	geo_pos(pos, here, e);
	map_put(pos, what, 0);
	exits_fix(cmd, what, exit);
	e_move(cmd, e);
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
geo_v(command_t *cmd, char const *opcs)
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
				aop(cmd, cd.e);
		} else
			ofs += op.op.b(cmd, cd);

		s += ofs;
	}

	return s - opcs;
}

/* }}} */

int gexits(command_t *cmd, dbref where) {
        int i, ret;
        register char *s;
        for (s = "wsnedu", ret = 0, i = 1; *s; s++, i <<= 1) {
                dbref tmp = e_exit_where(cmd, where, dir_e(*s));
                if (tmp < 0)
                        continue;
                ret |= i;
        }
        return ret;
}

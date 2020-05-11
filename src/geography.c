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
#undef NDEBUG
#include "debug.h"

#define MESGPROP_TREE	"_/tree"
#define GETTREE(x)	get_property_value(x, MESGPROP_TREE)
#define SETTREE(x, y)	set_property_value(x, MESGPROP_TREE, y)

#define BIOME_BG(i)	(NIGHT_IS ? ANSI_RESET : biome_bgs[i])

#define PRECOVERY

#define MESGPROP_MARK	"@/mark"

#define GNAMEO(c) (c - 'A')
#define GOTHEO(c) (c + 'a')
#define GNAME(c) geo_map[GNAMEO(c)]
#define GOTHER(c) geo_map[GOTHEO(c)]
#define GSIMM(c) (*GOTHER(c))

#define VIEW_BDI (VIEW_SIZE * (VIEW_SIZE - 1))
#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

typedef struct {
	dbref what;
	morton_t where;
} geo_range_t;

struct cmd_dir {
	char dir, sdir;
	morton_t rep;
};

typedef void op_a_t(int descr, dbref player, const char dir);
typedef int op_b_t(int descr, dbref player, struct cmd_dir cd);
typedef struct {
	union {
		op_a_t *a;
		op_b_t *b;
	} op;
	int type;
} op_t;

// see http://www.vision-tools.com/h-tropf/multidimensionalrangequery.pdf

static DB_ENV *dbe = NULL;
static DB *pdb, *ipdb;
static const morton_t m1 = 0x111111111111ULL;
static const morton_t m0 = 0x800000000000ULL;

// global buffer for map? // FIXME d bio_limit
static char map_buf[8 * BUFSIZ];

static const char * v = "|";
static const char * l = ANSI_FG_WHITE "+";
static const char *h_open	= "   ";
static const char *h_closed	= "---";
/* BIOME_BGS {{{ */
char const *biome_bgs[] = {
	// Water BOLD WHITE "~~~"
	ANSI_BG_BLUE,

	// ----

	// Cold dry desert, Pile of frozen rocks, BOLD WHITE "o" "O"
	ANSI_BG_BLUE,
	// Cold stone, Block of ice, BOLD GREEN "o" "O"
	ANSI_BG_CYAN,
	// Cold snowy rock, Bear, BOLD YELLOW " 8 "
	ANSI_BG_WHITE,

	// Tundra, frozen pine BOLD WHITE "x" "X"
	ANSI_BG_CYAN,
	// Taiga
	ANSI_BG_GREEN,
	// Temperate rain forest
	ANSI_BG_GREEN,

	// Woodland / Grassland / Shrubland
	ANSI_BG_GREEN,
	// ?
	ANSI_BG_GREEN,
	// Temperate forest
	ANSI_BG_GREEN,

	// desert
	ANSI_BG_YELLOW,
	// Savannah
	ANSI_BG_YELLOW,
	// Temperate Seasonal forest, "Red Tree" RED "x" "X"
	ANSI_RESET,

	// Volcanic, Puddle of lava, BOLD RED "o" "O"
	ANSI_RESET,
};
/* }}} */
const char *geo_map[] = {
	[0 ... 254] = "",
	['h'] = "w",
	['j'] = "s",
	['k'] = "n",
	['l'] = "e",
	['K'] = "u",
	['J'] = "d",
	['n'] = "north",
	['s'] = "south",
	['e'] = "east",
	['w'] = "west",
	['u'] = "up",
	['d'] = "down",
	[GNAMEO('n')] = "n;north",
	[GNAMEO('s')] = "s;south",
	[GNAMEO('e')] = "e;east",
	[GNAMEO('w')] = "w;west",
	[GNAMEO('u')] = "u;up",
	[GNAMEO('d')] = "d;down",
	[GOTHEO('n')] = "sewud",
	[GOTHEO('s')] = "newud",
	[GOTHEO('e')] = "wnsud",
	[GOTHEO('w')] = "ensud",
	[GOTHEO('u')] = "dnsew",
	[GOTHEO('d')] = "unsew",
};

unsigned short day_tick;

/* POINT DB {{{ */
static int
geo_mki_code(DB *sec, const DBT *key, const DBT *data, DBT *result)
{
	result->size = sizeof(morton_t);
	result->data = data->data;
	return 0;
}

static int
geo_cmp(DB *sec, const DBT *a_r, const DBT *b_r)
{
	morton_t a = * (morton_t*) a_r->data,
		 b = * (morton_t*) b_r->data;

	return b > a ? -1 : (a > b ? 1 : 0);
}

int
geo_close()
{
	return pdb->close(pdb, 0) || ipdb->close(ipdb, 0);
}

int
geo_init()
{
	int ret = -1;

	if (db_create(&ipdb, dbe, 0)
	    || ipdb->open(ipdb, NULL, "geo.db", "dp", DB_HASH, DB_CREATE, 0664))
		return ret;

	if (db_create(&pdb, dbe, 0))
		goto err;

	if (pdb->set_bt_compare(pdb, geo_cmp)
	    || pdb->open(pdb, NULL, "geo.db", "pd", DB_BTREE, DB_CREATE, 0664)
	    || ipdb->associate(ipdb, NULL, pdb, geo_mki_code, DB_CREATE))
	
		pdb->close(pdb, 0);
	else {
		day_tick = 0;
		return 0;
	}

err:	ipdb->close(ipdb, 0);
	return ret;
}

static void
geo_put(morton_t code, dbref thing, int flags)
{
	DBT key;
	DBT data;
	int ret;

	CBUG(Typeof(thing) != TYPE_ROOM);

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.data = &thing;
	key.size = sizeof(thing);
	data.data = &code;
	data.size = sizeof(code);

	ret = ipdb->put(ipdb, NULL, &key, &data, flags);
	CBUG(ret);
}

morton_t
geo_where(dbref room)
{
	int bad;
	DBT key;
	DBT data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &room;
	key.size = sizeof(room);

	if ((bad = ipdb->get(ipdb, NULL, &key, &data, 0))) {
#if 1
		static morton_t code = 130056652770671ULL;
		debug("room %d db get: %s", room, db_strerror(bad));
#else
		BUG("room %d db get: %s", room, db_strerror(bad));
#ifdef PRECOVERY
		if (bad == DB_NOTFOUND) {
			geo_put(code, room, DB_NOOVERWRITE);
			return code;
		}
#endif
#endif
		return code;
	}

	return * (morton_t *) data.data;
}

static dbref
geo_get(morton_t at)
{
	DBC *cur;
	DBT pkey;
	DBT data;
	DBT key;
	dbref ref;
	int res;

	if (pdb->cursor(pdb, NULL, &cur, 0))
		return NOTHING;

	memset(&pkey, 0, sizeof(DBT));
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.size = sizeof(at);
	key.data = &at;

	while (1) {
		res = cur->c_pget(cur, &key, &pkey, &data, DB_SET);
		switch (res) {
		case 0:
			ref = * (dbref *) pkey.data;
#ifdef PRECOVERY
			if (Typeof(ref) == TYPE_GARBAGE) {
				debug("GARBAGE %d REMOVED ;)", ref);
				cur->c_del(cur, 0);
				continue;
			}
#endif
			CBUG(Typeof(ref) != TYPE_ROOM);
			return ref;
		case DB_NOTFOUND:
			cur->close(cur);
			return NOTHING;
		}
	}
}

int
geo_delete(dbref what)
{
	DBT key;
	morton_t code = geo_where(what);
	memset(&key, 0, sizeof(key));
	key.data = &code;
	key.size = sizeof(code);
	return pdb->del(pdb, NULL, &key, 0);
}

static inline morton_t
qload_0(morton_t c, morton_t lm0, morton_t lm1) // LOAD(0111...
{
	return (c & (~ lm0)) | lm1;
}

static inline morton_t
qload_1(morton_t c, morton_t lm0, morton_t lm1) // LOAD(1000...
{
	return (c | lm0) & (~ lm1);
}

static inline int
inrange(morton_t dr, point3D_t min, point3D_t max)
{
	point3D_t drp;

	// TODO use ffs
	morton_decode(drp, dr);

	POOP3D if (drp[I] < min[I] || drp[I] >= max[I])
		return 0;

	return 1;
}

static inline void
compute_bmlm(morton_t *bm, morton_t *lm,
	     morton_t dr, morton_t min, morton_t max)
{
	register morton_t lm0 = m0, lm1 = m1;

	for (; lm0; lm0 >>= 1, lm1 >>= 1)
	{
		register morton_t a = dr & lm0,
			 b = min & lm0,
			 c = max & lm0;

		if (b) { // ? 1 ?
			if (!a && c) { // 0 1 1
				*bm = min;
				break;
			}

		} else if (a) // 1 0 ?
			if (c) { // 1 0 1
				*lm = qload_0(max, lm0, lm1);
				min = qload_1(min, lm0, lm1);
			} else { // 1 0 0
				*lm = max;
				break;
			}

		else if (c) { // 0 0 1
			// BIGMIN
			*bm = qload_1(min, lm0, lm1);
			max = qload_0(max, lm0, lm1);
		}
	}
}

static inline int
geo_range_unsafe(geo_range_t *res,
		     size_t n,
		     struct rect3D *rect,
		     ucoord_t flags)
{
	point3D_t rect_e = {
		rect->s[0] + rect->l[0],
		rect->s[1] + rect->l[1],
		rect->s[2] + rect->l[2],
	};

	morton_t rmin = morton_encode(rect->s, flags),
		 rmax = morton_encode(rect_e, flags),
		 lm = 0,
		 code;

	geo_range_t *r = res;
	DBC *cur;
	DBT key, pkey, data;
	size_t nn = n;
	int ret = 0;
	int dbflags;

	if (pdb->cursor(pdb, NULL, &cur, 0))
		return -1;

	memset(&data, 0, sizeof(DBT));
	memset(&pkey, 0, sizeof(DBT));

	dbflags = DB_SET_RANGE;
	memset(&key, 0, sizeof(DBT));
	key.data = &rmin;
	key.size = sizeof(rmin);

next:	ret = cur->c_pget(cur, &key, &pkey, &data, dbflags);
	dbflags = DB_NEXT;

	switch (ret) {
	case 0: break;
	case DB_NOTFOUND: ret = 0;
	default: goto out;
	}

	code = *(morton_t*) key.data;

	if (code > rmax)
		goto out;

	if (inrange(code, rect->s, rect_e)) {
		r->what = * (dbref *) pkey.data;
		r->where = code;
		r++;
		nn--;
		if (nn == 0)
			goto out;
	} else
		compute_bmlm(&rmin, &lm, code, rmin, rmax);

	goto next;

out:	if (cur->close(cur) || ret)
		return -1;

	return r - res;
}

/* this version accounts for type limits,
 * and wraps around them in each direction, if necessary
 * TODO iterative form
 */

static int
geo_range_safe(geo_range_t *res,
		   size_t n,
		   struct rect3D *rect,
		   ucoord_t flags,
		   int dim)
{
	geo_range_t *re = res;
	int i, aux;
	size_t nn = n;
	struct rect3D r = *rect;

	for (i = dim; i < DIM; i++) {
		if (rect->s[i] + rect->l[i] <= COORD_MAX)
			continue;

		r.l[i] = COORD_MAX - rect->s[i];
		r.s[i] = rect->s[i];
		aux = geo_range_safe(re, nn, &r, flags, i + 1);

		if (aux < 0)
			return -1;

		nn -= aux;
		re += aux;

		r.l[i] = rect->s[i] + rect->l[i] - COORD_MAX - 1;
		r.s[i] = COORD_MIN + 1;
		aux = geo_range_safe(re, nn, &r, flags, i + 1); 

		if (aux < 0)
			return -1;

		nn -= aux;

		return n - nn;
	}

	return geo_range_unsafe(re, nn, rect, flags);
}

/* }}} */

/* STUFF THAT SHOULD BE IN item.c {{{ */

dbref
obj_stack_add(struct obj o, dbref where, unsigned char n)
{
	CBUG(n <= 0);
	dbref nu = obj_add(o, where);
	if (n > 1)
		SETSTACK(nu, n);
	return nu;
}

static inline unsigned
plant_yield(struct plant *pl, struct bio *b, unsigned char n)
{
	register unsigned ptmin = pl->tmp_min;
	return random() % (1 + n * (b->tmp - ptmin) * pl->yield / (pl->tmp_max - ptmin));
}

// FIXME Y ???
static inline int
fruit_size(struct plant *pl, struct bio *b)
{
	return 3;
}

static inline void
plant_add(int descr, dbref player, dbref where, struct bio *b, unsigned char plid, unsigned char n)
{
	if (n == 0)
		return;
	struct plant *pl = &plants[plid];
	dbref plant = obj_stack_add(pl->o, where, n);
	dbref fruit = obj_add(pl->fruit, plant);
	struct boolexp *key = parse_boolexp(descr, player, NAME(player), 0);
	unsigned yield = plant_yield(pl, b, n);
	SETCONLOCK(plant, key);
        SETPLID(plant, plid);
	SETFOOD(fruit, fruit_size(pl, b));
	if (yield > 1)
		SETSTACK(fruit, yield);
}

static inline void
_plants_add(int descr, dbref player, dbref where, struct bio *b,
            unsigned char plid[3], unsigned char pln)
{
	register int i, aux;
        for (i = 0; i < 3; i++) {
                aux = TREE_N(pln, i);
                if (aux)
                        plant_add(descr, player, where, b, plid[i], aux);
        }
}

static inline void
plants_add(int descr, dbref player, struct bio *b, dbref where)
{
	unsigned char e_plid[EXTRA_TREE], e_pln;

        if (b->pln)
                _plants_add(descr, player, where, b, b->plid, b->pln);
	noise_rplants(e_plid, &e_pln, b);
        if (e_pln)
                _plants_add(descr, player, where, b, e_plid, e_pln);
}

static inline void
others_add(int descr, dbref player, struct bio *b, dbref where, morton_t p)
{
	noise_t v = XXH32(&p, sizeof(morton_t), 0);
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
	char const *gname = GNAME(NAME(exit)[0]);
	return *gname && !strncmp(NAME(exit), gname, 4);
}

static inline morton_t
geo_x(dbref loc, const char dir)
{
	morton_t x = geo_where(loc);
	point3D_t p;

	morton_decode(p, x);

	switch (dir) {
	case 'n': p[Y_COORD]--; break;
	case 's': p[Y_COORD]++; break;
	case 'e': p[X_COORD]++; break;
	case 'w': p[X_COORD]--; break;
	case 'u': p[2]++; break;
	case 'd': p[2]--; break;
	}

	return morton_encode(p, OBITS(x));
}

static inline dbref
geo_there(dbref where, const char dir)
{
	return geo_get(geo_x(where, dir));
}

/* }}} */

/* exit {{{ */

static inline dbref
gexit_where(int descr, dbref player, dbref loc, const char dir)
{
	struct match_data md;
	init_match_remote(descr, player, loc, GEXPAND(dir), TYPE_EXIT, &md),
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
gexit(int descr, dbref player, dbref loc, dbref loc2, const char dir)
{
	dbref ref;

	ref = new_object();

	/* Initialize everything */
	NAME(ref) = alloc_string(GNAME(dir));
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
gexit_here(int descr, dbref player, const char dir)
{
	return gexit_where(descr, player, getloc(player), dir);
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
	for (s = GOTHER(NAME(exit)[0]); *s; s++) {
		dbref oexit = gexit_where(descr, player, here, *s);
		dbref there = geo_there(here, *s);

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
	const char *s, dir = NAME(exit)[0];

	for (s = GOTHER(dir); *s; s++) {
		dbref othere_exit,
		      oexit = gexit_where(descr, player, there, *s),
		      othere = geo_there(there, *s);

		if (othere < 0 || GETTMP(othere)) {
			if (oexit < 0)
				continue;
			gexit_dest_set(oexit, NOTHING);
			continue;
		}

		othere_exit = gexit_where(descr, player, othere, GSIMM(*s));

		if (oexit < 0) {
			if (othere_exit < 0)
				continue;

			gexit(descr, player, there, othere, *s);
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
		dbref oexit, exit_there, there = geo_there(here, *s);

		if (there < 0) {
                        if (*s != 'u' && *s != 'd')
                                gexit(descr, player, here, NOTHING, *s);
			continue;
		}

		exit_there = gexit_where(descr, player, there, GSIMM(*s));
		if (exit_there < 0)
			continue;

		oexit = gexit(descr, player, here, there, *s);
		/* if (there > 0 && !GETTMP(there)) */
		SETDOOR(oexit, GETDOOR(exit_there));
		gexit_dest_set(exit_there, here);
	}
}

static dbref
geo_room_at(int descr, dbref player, morton_t x)
{
	struct bio *bio;
	static const dbref loc = 0;
	dbref there = new_object();
	CBUG(there <= 0);
	geo_put(x, there, DB_NOOVERWRITE);
	NAME(there) = alloc_string("No name");
	DBFETCH(there)->location = loc;
	OWNER(there) = 1;
	DBFETCH(there)->exits = NOTHING;
	DBFETCH(there)->sp.room.dropto = NOTHING;
	FLAGS(there) = TYPE_ROOM | (FLAGS(player) & JUMP_OK);
	PUSH(there, DBFETCH(loc)->contents);
	DBDIRTY(there);
	DBDIRTY(loc);
	bio = noise_point(x);
	CBUG(there <= 0);
	exits_infer(descr, player, there);
	SETTMP(there, 1);
	{
		point3D_t pos;
		morton_decode(pos, x);
		if (pos[2] != 0)
			return there;
		SETTREE(there, bio->pln);
		SETFLOOR(there, bio->bio_idx);
		mobs_add(bio, there);
		others_add(descr, player, bio, there, x);
                plants_add(descr, player, bio, there);
	}
	DBDIRTY(there);
	DBDIRTY(loc);
	return there;
}

// exclusively called by trigger() and carve, in vertical situations
dbref
geo_room(int descr, dbref player, dbref exit)
{
	morton_t x;
	const char dir = NAME(exit)[0];
	dbref here = getloc(exit), there;

	x = geo_x(here, dir);
	there = geo_room_at(descr, player, x);

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
	const char dir = NAME(exit)[0];
	dbref oexit,
	      room = getloc(exit),
	      there = geo_there(room, dir);

	if (there < 0)
		return;

	oexit = gexit_where(descr, player, there, GSIMM(dir));
	if (oexit >= 0)
		gexit_dest_set(oexit, NOTHING);
}

static void
walk(int descr, dbref player, const char dir) {
	const char dirs[] = { dir, '\0' };
	dbref exit = gexit_here(descr, player, dir),
	      there;

#ifdef PRECOVERY
	if (exit >= 0) {
		there = gexit_dest(exit);
		if (there > 0) {
			morton_t x = geo_x(getloc(player), dir);
			geo_put(x, there, 0);
			exits_fix(descr, player, there, exit);
		}
	}
#endif
	do_move(descr, player, dirs, 0);
}

/* used in predicates.c {{{ */

static inline int
morton_z_null(morton_t x)
{
	point3D_t p;
	morton_decode(p, x);
	return p[2] == 0;
}

int
geo_lock(dbref room, const char dir)
{
	if (dir == 'u' || dir == 'd')
		return 0;

	return morton_z_null(geo_where(room));
}

int
gexit_can(dbref player, dbref exit) {
	const char dir = NAME(exit)[0];
	CBUG(exit < 0);
	CBUG(gexit_dest(exit) >= 0);
	return geo_lock(getloc(player), dir);
}

/* }}} */

static void
carve(int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' };
	dbref there = NOTHING,
	      here = getloc(player),
	      exit = gexit_here(descr, player, dir);
	int wall = 0;

	if (!geo_lock(getloc(player), dir)) {
		if (geo_claim(descr, player, here))
			return;
		if (GETVALUE(player) < ROOM_COST) {
			notify(player, "You can't pay for that room");
			return;
		}
		exit = gexit_here(descr, player, dir);
		if (exit < 0)
			exit = gexit(descr, player, here, there, dir);
		there = geo_there(here, dir);
		if (there < 0) {
			there = geo_room(descr, player, exit);
			gexit_dest_set(exit, there);
		}
		wall_around(descr, player, exit);
		wall = 1;
	}

	do_move(descr, player, dirs, 0);
	there = getloc(player);
	if (here == there)
		return;
	geo_claim(descr, player, there);
	if (wall) {
		exit = gexit_here(descr, player, GSIMM(dir));
		wall_around(descr, player, exit);
	}
}

static void
uncarve(int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' }, *s0 = "is";
	dbref there, here = getloc(player),
	      exit = gexit_here(descr, player, dir);
	int ht, cd = geo_lock(here, dir);

	if (cd) {
		ht = GETTMP(here);
		do_move(descr, player, dirs, 0);
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
		exit = gexit_here(descr, player, GSIMM(dir));
		if (ht && GETDOOR(exit))
			SETDOOR(exit, 0);
	} else
		geo_clean(descr, player, there);

	reward(player, "collect your materials", ROOM_COST);
}

static void
unwall(int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' };
	int a, b, c, d;
	dbref exit, there,
	      here = getloc(player);

	a = OWNER(here) == player;
	b = GETTMP(here);
	there = geo_there(here, dir);

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

	exit = gexit_here(descr, player, dir);

	if (exit >= 0) {
		notify(player, "There's an exit here already.");
		return;
	}

	exit = gexit(descr, player, here, there, dir);
	do_move(descr, player, dirs, 0);

	there = getloc(player);
	if (here == there)
		return;

	gexit(descr, player, there, here, GSIMM(dir));
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
		 int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' };
	dbref here = getloc(player),
	      exit = gexit_here(descr, player, dir), exit_there;

	if (exit < 0) {
		notify(player, "No exit here");
		return 1;
	}

	do_move(descr, player, dirs, 0);

	if (here == getloc(player))
		return 1;

	exit_there = gexit_here(descr, player, GSIMM(dir));
	CBUG(exit_there < 0);

	*exit_r = exit;
	*exit_there_r = exit_there;
	return gexit_claim(descr, player, exit, exit_there);
}

static void
wall(int descr, dbref player, const char dir)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, dir))
		return;
	recycle(descr, player, exit_there);
	recycle(descr, player, exit);
	notify(player, "You build a wall.");
}

static void
door(int descr, dbref player, const char dir)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, dir))
		return;
	SETDOOR(exit, 1);
	SETDOOR(exit_there, 1);
	notify(player, "You place a door.");
}

static void
undoor(int descr, dbref player, const char dir)
{
	dbref exit, exit_there;
	if (gexit_claim_walk(&exit, &exit_there, descr, player, dir))
		return;
	SETDOOR(exit_there, 0);
	SETDOOR(exit, 0);
	notify(player, "You remove a door.");
}

static const morton_t obits_shift = sizeof(coord_t) * 8 * 3;

static morton_t
morton_write(morton_t x)
{
	coord_t p[4];
	memcpy(p, &x, sizeof(x));
	return morton_encode(p, p[3]);
}

static morton_t
morton_read(morton_t x)
{
	coord_t p[4];
	coord_t obits = OBITS(x);
	morton_decode(p, x);
	return (* (morton_t *) p) | ((morton_t) obits << obits_shift);
}

static int
pos(int descr, dbref player, struct cmd_dir cd) {
	morton_t x;
	dbref who = cd.rep == 1 ? player : (dbref) cd.rep;
	int ret = 1;

	if (Typeof(who) != TYPE_PLAYER) {
		notify(player, "Invalid object type.");
		return 0;
	}

	x = geo_where(getloc(who));

	if (cd.dir != '!') {
		x = morton_read(x);
		ret = 0;
	}

	notify_fmt(player, "0x%llx", x);
	return ret;
}

static int
teleport(int descr, dbref player, struct cmd_dir cd)
{
	morton_t x;
	dbref there;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '!') {
		x = cd.rep;
		ret = 1;
	} else if (cd.dir == '?') {
		dbref who = (dbref) cd.rep;
		x = Typeof(who) == TYPE_PLAYER
			? geo_where(getloc(who))
			: 0;
		ret = 1;
	} else
		x = morton_write(cd.rep);
	there = geo_get(x);
	if (there < 0)
		there = geo_room_at(descr, player, x);
	CBUG(there < 0);
	notify_fmt(player, "Teleporting to 0x%llx.", cd.rep);
	enter_room(descr, player, there, NOTHING);
	return ret;
}

static int
mark(int descr, dbref player, struct cmd_dir cd)
{
	morton_t x = cd.rep;
	char value[32];
	if (x == 1)
		x = geo_where(getloc(player));
	x = morton_read(x);
	snprintf(value, 32, "0x%llx", x);
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
	const char dir = cd.sdir,
	      dirs[] = { dir, '\0' };
	dbref there, here = getloc(player),
	      exit = gexit_here(descr, player, dir),
	      what = (dbref) cd.rep;
	morton_t x;

	if (dir == '\0')
		return 0;

	here = getloc(player),
	exit = gexit_here(descr, player, dir),
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

	x = geo_x(here, dir);
	geo_put(x, what, 0);
	exits_fix(descr, player, what, exit);
	do_move(descr, player, dirs, 0);
	return 1;
}

op_t op_map[] = {
	['r'] = { .op.a = &carve },
	['R'] = { .op.a = &uncarve },
	['d'] = { .op.a = &door } ,
	['D'] = { .op.a = &undoor },
	['w'] = { .op.a = &wall },
	['W'] = { .op.a = &unwall },
	['x'] = { .op.b = &pos, .type = 1 },
	['X'] = { .op.b = &teleport, .type = 1 },
	['m'] = { .op.b = &mark, .type = 1 },
	['"'] = { .op.b = &recall, .type = 1 },
	['#'] = { .op.b = &pull, .type = 1 },
};

/* }}} */

/* }}} */

/* MAP {{{ */

static inline char *
dr_room(int descr, dbref player, struct bio *n, dbref here, char *buf, const char *bg)
{
	register char *b = buf;
	int tree = GETTREE(here);

	dbref tmp;

	tmp = gexit_where(descr, player, here, 'd');
	if (tmp > 0 && DBFETCH(tmp)->sp.exit.ndest
	    && DBFETCH(tmp)->sp.exit.dest[0] >= 0)
		b = stpcpy(b, ANSI_FG_WHITE "<");
	else if (tree & 3) {
		struct plant *pl = &plants[n->plid[0]];
		b = stpcpy(b, pl->pre);
		*b++ = (tree & 3) > TREE_HALF ? pl->big : pl->small;
	} else
		*b++ = ' ';

        tree >>= 2;

	{
		int max_prio = 0;
		const char *pre = "";
		char emp = ' ';
		dbref tmp = DBFETCH(here)->contents;

		DOLIST(tmp, tmp) switch (Typeof(tmp)) {
		case TYPE_PLAYER:
			if (tmp == player) {
				max_prio = 3;
				pre = ANSI_BOLD ANSI_FG_RED;
				emp = '#';
				break;
			}

			if (!max_prio) {
				pre = ANSI_BOLD ANSI_FG_BLUE;
				emp = '#';
			}

			break;

		case TYPE_THING:
			if (max_prio >= 2)
				break;

			if (GETSHOP(tmp)) {
				pre = ANSI_BOLD ANSI_FG_GREEN;
				emp = '$';
				max_prio = 2;
			} else if (max_prio < 1) {
				max_prio = 1;
				if (GETLID(tmp) >= 0) {
					pre = ANSI_BOLD ANSI_FG_YELLOW;
					emp = '!';
				} else if (GETDRINK(tmp) >= 0) {
					pre = ANSI_BOLD ANSI_FG_BLUE;
					emp = '~';
				} else
					max_prio = 0;
			}
			break;
		}

		if (emp == ' ' && (tree & 3)) {
			struct plant *pl = &plants[n->plid[1]];
			pre = pl->pre;
			emp = (tree & 3) > TREE_HALF
                                ? pl->big : pl->small;
		}

		b = stpcpy(b, ANSI_RESET);
		b = stpcpy(b, bg);
		b = stpcpy(b, pre);
		*b++ = emp;
	}

        tree >>= 2;
	tmp = gexit_where(descr, player, here, 'u');
	if (tmp > 0 && DBFETCH(tmp)->sp.exit.ndest
	    && DBFETCH(tmp)->sp.exit.dest[0] >= 0)
		b = stpcpy(b, ANSI_FG_WHITE ">");
	else if (tree & 3) {
		struct plant *pl = &plants[n->plid[2]];
		b = stpcpy(b, pl->pre);
		*b++ = (tree & 3) > TREE_HALF ? pl->big : pl->small;
	} else
		*b++ = ' ';

	b = stpcpy(b, ANSI_RESET);
	b = stpcpy(b, bg);

	return b;
}

static inline const char *
wall_paint(int floor)
{
	switch (floor) {
	case -8:
		return ANSI_FG_RED;
	default:
		return ANSI_FG_YELLOW;
	}
}

static inline char *
dr_v(char *b, dbref exit, const char *wp) {
	if (exit < 0) {
		b = stpcpy(b, wp);
		return stpcpy(b, v);
	} else if (GETDOOR(exit))
		return stpcpy(b, l);
	else
		return stpcpy(b, " ");
}

static inline unsigned
floor_get(dbref what) {
	unsigned floor = GETFLOOR(what);
	if (floor > BIOME_VOLCANIC)
		return BIOME_VOLCANIC;
	else
		return floor;
}

static char *
dr_vs(int descr, dbref player, struct bio *n, dbref *g, char *b)
{
	const char *bg, *wp;
	dbref *g_max = g + VIEW_SIZE;
	dbref prev;
	unsigned floor;

	if (*g < 0) {
		floor = n->bio_idx;
		wp = "";
	} else {
		floor = floor_get(*g);
		wp = wall_paint(floor);
	}

	bg = BIOME_BG(floor);
	b = stpcpy(b, " ");
	for (;;) {
		b = stpcpy(b, bg);

		if (*g >= 0)
			b = dr_room(descr, player, n, *g, b, bg);
		else {
			if (TREE_N(n->pln, 0)) {
				struct plant *pl = &plants[n->plid[0]];
				b = stpcpy(b, pl->pre);
				*b++ = TREE_N(n->pln, 0) > TREE_HALF ? pl->big : pl->small;
			} else
				*b++ = ' ';

			if (TREE_N(n->pln, 1)) {
				struct plant *pl = &plants[n->plid[1]];
				b = stpcpy(b, pl->pre);
				*b++ = TREE_N(n->pln, 1) > TREE_HALF ? pl->big : pl->small;
			} else
				*b++ = ' ';

			if (TREE_N(n->pln, 2)) {
				struct plant *pl = &plants[n->plid[2]];
				b = stpcpy(b, pl->pre);
				*b++ = TREE_N(n->pln, 2) > TREE_HALF ? pl->big : pl->small;
			} else
				*b++ = ' ';
		}

		prev = *g;
		g++;
		n++;

		if (g >= g_max)
			break;

		if (*g < 0 || GETTMP(*g)) {
			floor = n->bio_idx;
			bg = BIOME_BG(floor);
		} else {
			floor = floor_get(*g);
			wp = wall_paint(floor);
			bg = BIOME_BG(floor);
			b = stpcpy(b, bg);
		}

		if (prev >= 0)
			prev = gexit_where(descr, player, prev, 'e');
		else if (*g >= 0)
			prev = gexit_where(descr, player, *g, 'w');
		else {
			*b++ = ' ';
			continue;
		}

		b = dr_v(b, prev, wp);
	}

        b = stpcpy(b, ANSI_RESET"\n");
        return b;
}

static inline char *
dr_h(char *b, char const *bg, char const *wp, char *w, int toggle)
{
	if (toggle) {
		b = stpcpy(b, " ");
		b = stpcpy(b, bg);
	} else {
		b = stpcpy(b, bg);
		b = stpcpy(b, " ");
	}
	b = stpcpy(b, wp);
	b = stpcpy(b, w);
	return b;
}

static inline char *
dr_hs_n(int descr, dbref player, struct bio *n, dbref *g, char *b)
{
	dbref *g_max = g + VIEW_SIZE;
	const char *wp, *bg;
	char wb[32], *w;
	unsigned floor;
	int toggle;

	for (wp = "",
	     bg = BIOME_BG(*g < 0 ? n->bio_idx : floor_get(*g)) ;
	     g < g_max ;
	     bg = BIOME_BG(floor),
	     b = dr_h(b, bg, wp, w, toggle),
	     n++, g++)
	{
		dbref curr = *g,
		      next = g[VIEW_SIZE];

		char dir = 's';

		if (next >= 0) {
			if (GETTMP(next)) {
				if (curr < 0 || GETTMP(curr)) {
					floor = n->bio_idx;
					toggle = 1;
				} else {
					floor = floor_get(curr);
					toggle = 0;
				}
			} else {
				floor = floor_get(next);
				toggle = 0;
			}
			dir = 'n';
			curr = next;
		} else if (curr >= 0 && !GETTMP(curr)) {
			floor = floor_get(curr);
			toggle = 0;
		} else {
			floor = n->bio_idx;
			w = (char *) h_open;
			toggle = 1;
			continue;
		}

		wp = wall_paint(floor);
		curr = gexit_where(descr, player, curr, dir);

		if (curr < 0)
			w = (char *) h_closed;
		else if (GETDOOR(curr)) {
			snprintf(wb, sizeof(wb), "-"ANSI_FG_WHITE"+%s-", wp);
			w = wb;
		} else
			w = (char *) h_open;
	}

        b = stpcpy(b, ANSI_RESET"\n");
        return b;
}

static inline void
map_search(dbref *mat, point3D_t pos, ucoord_t flags, unsigned radius)
{
	const unsigned side = 2 * radius + 1;
	const unsigned m = side * side;

	struct rect3D vpr3D = {
		{ pos[0] - radius, pos[1] - radius, pos[2] },
		{ side, side, 1 },
	};

	/* static const size_t m = 49; */
	geo_range_t buf[m];
	size_t n;
	int i;

	n =  geo_range_safe(buf, m, &vpr3D, flags, 0);
	memset(mat, -1, sizeof(dbref) * m);

	for (i = 0; i < n; i++) {
		point3D_t p;
		morton_decode(p, buf[i].where);
		mat[point_rel_idx(p, vpr3D.s, side)] = buf[i].what;
	}
}

/* used at the end of every command that
 * moves the player (not every move) */
void
geo_view(int descr, dbref player)
{
	morton_t code = geo_where(getloc(player));
	struct bio bd[VIEW_M], *n_max = bd + VIEW_BDI, *n_p = bd;
        char *p = map_buf;
        /* size_t l = sizeof(map_buf); */
	dbref o[VIEW_M], *o_p;
	point3D_t pos;
	ucoord_t obits = OBITS(code);

        /* memset(map_buf, 0, sizeof(map_buf)); */
	morton_decode(pos, code);
	noise_view(bd, pos, obits);
	map_search(o, pos, obits, VIEW_AROUND);

	o_p = o;

	p = dr_vs(descr, player, n_p, o_p, p);

	for (; n_p < n_max;) {
		p = dr_hs_n(descr, player, n_p, o_p, p);
		n_p += VIEW_SIZE;
		o_p += VIEW_SIZE;
		p = dr_vs(descr, player, n_p, o_p, p);
	}

        if (web_geo_view(descr, map_buf))
                notify(player, map_buf);
}

/* }}} */

/* PARSE {{{ */

static inline char
geo_dir_sanity(register char c)
{
	c = geo_map[(int) c][0];
	return (c == 'w' || c == 'e' || c == 'n'
		|| c == 's' || c == 'u' || c == 'd')  
		? c : '\0';
}

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
	res->sdir = geo_dir_sanity(sc);
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
			if (cd.sdir == '\0')
				return opcs - s;

			ofs ++;
			int j;
			for (j = 0; j < cd.rep; j++)
				aop(descr, player, cd.sdir);
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
                dbref tmp = gexit_where(descr, player, where, *s);
                if (tmp < 0)
                        continue;
                ret |= i;
        }
        return ret;
}

void geo_notify(int descr, dbref player) {

        dbref o[GEON_M], *o_p;
        point3D_t pos;
	morton_t x = geo_where(getloc(player));
	char buf[BUFFER_LEN + 2];
        morton_decode(pos, x);
        map_search(o, pos, OBITS(x), GEON_RADIUS);
	McpMesg msg;
	mcp_mesg_init(&msg, MCP_WEB_PKG, "enter");
	snprintf(buf, sizeof(buf), "0x%llx", x);
	mcp_mesg_arg_append(&msg, "x", buf);
	for (o_p = o; o_p < o + GEON_BDI; o_p++) {
		dbref loc = *o_p;
		if (loc < 0 || Typeof(loc) != TYPE_ROOM)
			continue;

		web_room_mcp(loc, &msg);
	}
	mcp_mesg_clear(&msg);
}

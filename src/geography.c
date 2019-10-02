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
#undef NDEBUG
#include "debug.h"

#define MESGPROP_TREE	"_/tree"
#define GETTREE(x)	get_property_value(x, MESGPROP_TREE)
#define SETTREE(x, y)	set_property_value(x, MESGPROP_TREE, y)

#define BIOME_BG(i)	(NIGHT_IS ? ANSI_RESET : biome_bgs[i])

#define PRECOVERY

#define MESGPROP_MARK	"@/mark/%c"

#define GNAMEO(c) (c - 'A')
#define GOTHEO(c) (c + 'a')

typedef struct {
	dbref what;
	morton_t where;
} pointdb_range_t;

struct cmd_dir {
	char dir;
	morton_t rep;
};

typedef void op_t(int descr, dbref player, const char dir);

// see http://www.vision-tools.com/h-tropf/multidimensionalrangequery.pdf

static DB_ENV *dbe = NULL;
static DB *pdb, *ipdb;
static const morton_t m1 = 0x111111111111ULL;
static const morton_t m0 = 0x800000000000ULL;

// global buffer for map? // FIXME d bio_limit
static char map_buf[256];

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
const char *map[] = {
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

/* PDB {{{ */
static int
pointdb_mki_code(DB *sec, const DBT *key, const DBT *data, DBT *result)
{
	result->size = sizeof(morton_t);
	result->data = data->data;
	return 0;
}

static int
pointdb_cmp(DB *sec, const DBT *a_r, const DBT *b_r)
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

	if (pdb->set_bt_compare(pdb, pointdb_cmp)
	    || pdb->open(pdb, NULL, "geo.db", "pd", DB_BTREE, DB_CREATE, 0664)
	    || ipdb->associate(ipdb, NULL, pdb, pointdb_mki_code, DB_CREATE))
	{
		pdb->close(pdb, 0);
	} else {
		ret = 0;
		day_tick = 0;
		return 0;
	}

err:	ipdb->close(ipdb, 0);
	return ret;
}

static void
pdb_put(morton_t code, dbref thing, int flags)
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

static inline void
pdb_move(dbref thing, morton_t code)
{
	pdb_put(code, thing, 0);
}

morton_t
pdb_where(dbref room)
{
	int bad;
	DBT key;
	DBT data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &room;
	key.size = sizeof(room);

	if ((bad = ipdb->get(ipdb, NULL, &key, &data, 0))) {
		BUG("room %d db get: %s", room, db_strerror(bad));
#ifdef PRECOVERY
		static morton_t code = 130056652770671ULL;
		if (bad == DB_NOTFOUND) {
			pdb_put(code, room, DB_NOOVERWRITE);
			return code;
		}
#endif
	}

	return * (morton_t *) data.data;
}

static dbref
pdb_get(morton_t at)
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
	morton_t code = pdb_where(what);
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
	morton3D_decode(drp, dr);

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
pointdb_range_unsafe(pointdb_range_t *res,
		     size_t n,
		     struct rect3D *rect,
		     ucoord_t flags)
{
	point3D_t rect_e = {
		rect->s[0] + rect->l[0],
		rect->s[1] + rect->l[1],
		rect->s[2] + rect->l[2],
	};

	morton_t rmin = morton3D_encode(rect->s, flags),
		 rmax = morton3D_encode(rect_e, flags),
		 lm = 0,
		 code;

	pointdb_range_t *r = res;
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
pointdb_range_safe(pointdb_range_t *res,
		   size_t n,
		   struct rect3D *rect,
		   ucoord_t flags,
		   int dim)
{
	pointdb_range_t *re = res;
	int i, aux;
	size_t nn = n;
	struct rect3D r = *rect;

	for (i = dim; i < DIM; i++) {
		if (rect->s[i] + rect->l[i] <= COORD_MAX)
			continue;

		r.l[i] = COORD_MAX - rect->s[i];
		r.s[i] = rect->s[i];
		aux = pointdb_range_safe(re, nn, &r, flags, i + 1);

		if (aux < 0)
			return -1;

		nn -= aux;
		re += aux;

		r.l[i] = rect->s[i] + rect->l[i] - COORD_MAX - 1;
		r.s[i] = COORD_MIN + 1;
		aux = pointdb_range_safe(re, nn, &r, flags, i + 1); 

		if (aux < 0)
			return -1;

		nn -= aux;

		return n - nn;
	}

	return pointdb_range_unsafe(re, nn, rect, flags);
}

/* }}} */

/* BIO ENTER {{{ */

dbref
obj_stack_add(struct obj o, dbref where, unsigned char n)
{
	CBUG(n <= 0);
	dbref nu = obj_add(o, where);
	/* DBFETCH(nu)->exits = NOTHING; */
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
plant_add(int descr, dbref player, struct bio *b, dbref where, unsigned char plid, unsigned char n)
{
	if (n == 0)
		return;
	struct plant *pl = &plants[plid];
	dbref plant = obj_stack_add(pl->o, where, n);
	dbref fruit = obj_add(pl->fruit, plant);
	struct boolexp *key = parse_boolexp(descr, player, NAME(player), 0);
	unsigned yield = plant_yield(pl, b, n);
	SETCONLOCK(plant, key);
	SETFOOD(fruit, fruit_size(pl, b));
	if (yield > 1)
		SETSTACK(fruit, yield);
}

static inline void
plants_add(int descr, dbref player, struct bio *b, dbref where)
{
	unsigned char plid[EXTRA_TREE], pln[EXTRA_TREE];
	int i;

	for (i = 0; i < 3; i++)
		if (b->pln[i])
			plant_add(descr, player, b, where, b->plid[i], b->pln[i]);

	noise_rplants(plid, pln, b);

	for (i = 0; i < EXTRA_TREE; i++)
		if (pln[i])
			plant_add(descr, player, b, where, plid[i], pln[i]);
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
	plants_add(descr, player, b, where);
}

/* }}} */

int
night_is()
{
	return day_tick >= (1 << (DAY_Y - 1));
}

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

static inline int
dir_valid(const char c)
{
	char sc = map[(int) c][0];
	switch (sc) {
	case 'w':
	case 'e':
	case 'n':
	case 's':
	case 'u':
	case 'd':
		return 1;
	default:
		return 0;
	}
}

const char *
geo_expand(char c)
{
	return map[(int) c];
}

static inline const char *
geo_name(const char c)
{
	return map[GNAMEO(c)];
}

static inline const char *
geo_other(const char c)
{
	return map[GOTHEO(c)];
}

static inline char
geo_simm(const char c)
{
	return *geo_other(c);
}

static inline morton_t
geo_move(morton_t code, char g)
{
	point3D_t p;
	morton3D_decode(p, code);
	switch (g) {
	case 'n': p[Y_COORD]--; break;
	case 's': p[Y_COORD]++; break;
	case 'e': p[X_COORD]++; break;
	case 'w': p[X_COORD]--; break;
	case 'u': p[2]++; break;
	case 'd': p[2]--; break;
	}
	return morton3D_encode(p, OBITS(code));
}

int
geo_is(dbref exit)
{ 
	char const *gname = geo_name(NAME(exit)[0]);
	return *gname && !strncmp(NAME(exit), gname, 4);
}

static inline morton_t
geo_x(dbref loc, const char dir)
{
	return geo_move(pdb_where(loc), dir);
}

static inline dbref
geo_there(dbref where, const char dir)
{
	return pdb_get(geo_x(where, dir));
}

/* }}} */

/* exit {{{ */

static inline dbref
geo_exit_where(int descr, dbref player, dbref loc, const char dir)
{
	struct match_data md;
	init_match_remote(descr, player, loc, geo_expand(dir), TYPE_EXIT, &md),
	match_room_exits(loc, &md);
	return match_result(&md);
}

static inline dbref
exit_dest(dbref exit)
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
exit_dest_set(dbref exit, dbref dest)
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
geo_exit(int descr, dbref player, dbref loc, dbref loc2, const char dir)
{
	dbref ref;

	ref = new_object();

	/* Initialize everything */
	NAME(ref) = alloc_string(geo_name(dir));
	DBFETCH(ref)->location = loc;
	OWNER(ref) = player;
	FLAGS(ref) = TYPE_EXIT;

	/* link it in */
	PUSH(ref, DBFETCH(loc)->exits);
	DBDIRTY(loc);

	DBFETCH(ref)->sp.exit.ndest = 1;
	DBFETCH(ref)->sp.exit.dest = (dbref *) malloc(sizeof(dbref));
	exit_dest_set(ref, loc2);
	DBDIRTY(ref);

	return ref;
}

static inline dbref
geo_exit_here(int descr, dbref player, const char dir)
{
	return geo_exit_where(descr, player, getloc(player), dir);
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
	for (s = geo_other(NAME(exit)[0]); *s; s++) {
		dbref oexit = geo_exit_where(descr, player, here, *s);
		dbref there = geo_there(here, *s);

		if (oexit >= 0 && there < 0)
			recycle(descr, player, oexit);
	}
}

int
room_claim(int descr, dbref player, dbref room) {
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
		if (Typeof(room) == TYPE_EXIT && geo_is(room))
			OWNER(room) = player;

	return 0;
}

static void
exits_infer(int descr, dbref player, dbref here)
{
	const char *s = "wedsun";

	for (; *s; s++) {
		dbref oexit, exit_there, there = geo_there(here, *s);

		if (there < 0) {
			geo_exit(descr, player, here, NOTHING, *s);
			continue;
		}

		exit_there = geo_exit_where(descr, player, there, geo_simm(*s));
		if (exit_there < 0)
			continue;

		oexit = geo_exit(descr, player, here, there, *s);
		/* if (there > 0 && !GETTMP(there)) */
		SETDOOR(oexit, GETDOOR(exit_there));
		exit_dest_set(exit_there, here);
	}
}

static dbref
geo_room_at(int descr, dbref player, morton_t x)
{
	struct bio *bio;
	static const dbref loc = 0;
	dbref there = new_object();
	CBUG(there <= 0);
	pdb_put(x, there, DB_NOOVERWRITE);
	NAME(there) = alloc_string("No name");
	DBFETCH(there)->location = loc;
	OWNER(there) = player;
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
		morton3D_decode(pos, x);
		if (pos[2] != 0)
			return there;
		SETTREE(there, (!!(bio->pln[0]))
			| ((!!(bio->pln[1])) << 1)
			| ((!!(bio->pln[2])) << 2));
		SETFLOOR(there, bio->bio_idx);
		mobs_add(bio, there);
		others_add(descr, player, bio, there, x);
	}
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
untmp_clean(int descr, dbref player, dbref here)
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

	oexit = geo_exit_where(descr, player, there, geo_simm(dir));
	if (oexit >= 0)
		exit_dest_set(oexit, NOTHING);
}

static void
walk(int descr, dbref player, const char dir) {
	const char dirs[] = { dir, '\0' };
	dbref exit = geo_exit_here(descr, player, dir),
	      there;

#ifdef PRECOVERY
	if (exit >= 0) {
		there = exit_dest(exit);
		if (there > 0) {
			morton_t x = geo_x(getloc(player), dir);
			pdb_put(x, there, 0);
		}
	}
#endif
	do_move(descr, player, dirs, 0);
}

static inline int
morton_z_null(morton_t x)
{
	point3D_t p;
	morton3D_decode(p, x);
	return p[2] == 0;
}

int
geo_lock(dbref room, const char dir)
{
	if (dir == 'u' || dir == 'd')
		return 0;

	return morton_z_null(pdb_where(room));
}

int /* used in can_doit */
gexit_can(dbref player, dbref exit) {
	const char dir = NAME(exit)[0];
	CBUG(exit < 0);
	CBUG(exit_dest(exit) >= 0);
	return geo_lock(getloc(player), dir);
}

static void
carve(int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' };
	dbref there = NOTHING,
	      here = getloc(player),
	      exit = geo_exit_here(descr, player, dir);
	int wall = 0;

	if (!geo_lock(getloc(player), dir)) {
		if (room_claim(descr, player, here))
			return;
		if (GETVALUE(player) < ROOM_COST) {
			notify(player, "You can't pay for that room");
			return;
		}
		exit = geo_exit_here(descr, player, dir);
		if (exit < 0)
			exit = geo_exit(descr, player, here, there, dir);
		there = geo_there(here, dir);
		if (there < 0) {
			there = geo_room(descr, player, exit);
			exit_dest_set(exit, there);
		}
		wall_around(descr, player, exit);
		wall = 1;
	}

	do_move(descr, player, dirs, 0);
	there = getloc(player);
	if (here == there)
		return;
	room_claim(descr, player, there);
	if (wall) {
		exit = geo_exit_here(descr, player, geo_simm(dir));
		wall_around(descr, player, exit);
	}
}

static void
uncarve(int descr, dbref player, const char dir)
{
	const char dirs[] = { dir, '\0' }, *s0 = "is";
	dbref there, here = getloc(player),
	      exit = geo_exit_here(descr, player, dir);
	int ht, cd = geo_lock(here, dir);

	if (cd) {
		ht = GETTMP(here);
		do_move(descr, player, dirs, 0);
		there = getloc(player);

		if (here == there)
			return;
	} else {
		there = exit_dest(exit);
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
		exit = geo_exit_here(descr, player, geo_simm(dir));
		if (ht && GETDOOR(exit))
			SETDOOR(exit, 0);
	} else
		untmp_clean(descr, player, there);

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

	exit = geo_exit_here(descr, player, dir);

	if (exit >= 0) {
		notify(player, "There's an exit here already.");
		return;
	}

	exit = geo_exit(descr, player, here, there, dir);
	do_move(descr, player, dirs, 0);

	there = getloc(player);
	if (here == there)
		return;

	geo_exit(descr, player, there, here, geo_simm(dir));
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
			return room_claim(descr, player, there);
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
	      exit = geo_exit_here(descr, player, dir), exit_there;

	if (exit < 0) {
		notify(player, "No exit here");
		return 1;
	}

	do_move(descr, player, dirs, 0);

	if (here == getloc(player))
		return 1;

	exit_there = geo_exit_here(descr, player, geo_simm(dir));
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

op_t *op_map[] = {
	['r'] = &carve,
	['R'] = &uncarve,
	['d'] = &door,
	['D'] = &undoor,
	['w'] = &wall,
	['W'] = &unwall,
};

/* }}} */

static int
teleport(int descr, dbref player, morton_t x)
{
	dbref there = pdb_get(x);
	if (there < 0)
		there = geo_room_at(descr, player, x);
	CBUG(there < 0);
	enter_room(descr, player, there, NOTHING);
	return 0;
}

static inline void
mark_set(dbref player, const char opc, morton_t x)
{
	char value[32];
	snprintf(value, 32, "0x%llx", x); // FIXME
	set_property_mark(player, MESGPROP_MARK, opc, value);
}

static inline morton_t
mark_get(dbref player, const char opc)
{
	const char *x = get_property_mark(player, MESGPROP_MARK, opc);
	if (!x)
		return -1ULL;
	else
		return strtoull(x, NULL, 0);
}

/* }}} */

/* MAP {{{ */

static inline char *
dr_room(int descr, dbref player, struct bio *n, dbref here, char *buf, const char *bg)
{
	register char *b = buf;
	int tree = GETTREE(here);

	dbref tmp;

	tmp = geo_exit_where(descr, player, here, 'd');
	if (tmp > 0 && DBFETCH(tmp)->sp.exit.ndest
	    && DBFETCH(tmp)->sp.exit.dest[0] >= 0)
		b = stpcpy(b, ANSI_FG_WHITE "<");
	else if (tree & 1) {
		struct plant *pl = &plants[n->plid[0]];
		b = stpcpy(b, pl->pre);
		*b++ = n->pln[0] > TREE_HALF ? pl->big : pl->small;
	} else
		*b++ = ' ';

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

		if (emp == ' ' && tree & 2) {
			struct plant *pl = &plants[n->plid[1]];
			pre = pl->pre;
			emp = n->pln[1] > TREE_HALF ? pl->big : pl->small;
		}

		b = stpcpy(b, ANSI_RESET);
		b = stpcpy(b, bg);
		b = stpcpy(b, pre);
		*b++ = emp;
	}

	tmp = geo_exit_where(descr, player, here, 'u');
	if (tmp > 0 && DBFETCH(tmp)->sp.exit.ndest
	    && DBFETCH(tmp)->sp.exit.dest[0] >= 0)
		b = stpcpy(b, ANSI_FG_WHITE ">");
	else if (tree & 4) {
		struct plant *pl = &plants[n->plid[2]];
		b = stpcpy(b, pl->pre);
		*b++ = n->pln[2] > TREE_HALF ? pl->big : pl->small;
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

static void
dr_vs(int descr, dbref player, struct bio *n, dbref *g)
{
	const char *bg, *wp;
	char *b = map_buf;
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
			if (n->pln[0]) {
				struct plant *pl = &plants[n->plid[0]];
				b = stpcpy(b, pl->pre);
				*b++ = n->pln[0] > TREE_HALF ? pl->big : pl->small;
			} else
				*b++ = ' ';

			if (n->pln[1]) {
				struct plant *pl = &plants[n->plid[1]];
				b = stpcpy(b, pl->pre);
				*b++ = n->pln[1] > TREE_HALF ? pl->big : pl->small;
			} else
				*b++ = ' ';

			if (n->pln[2]) {
				struct plant *pl = &plants[n->plid[2]];
				b = stpcpy(b, pl->pre);
				*b++ = n->pln[2] > TREE_HALF ? pl->big : pl->small;
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
			prev = geo_exit_where(descr, player, prev, 'e');
		else if (*g >= 0)
			prev = geo_exit_where(descr, player, *g, 'w');
		else {
			*b++ = ' ';
			continue;
		}

		b = dr_v(b, prev, wp);
	}

	*b = '\0';
	notify(player, map_buf);
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

static inline void
dr_hs_n(int descr, dbref player, struct bio *n, dbref *g)
{
	dbref *g_max = g + VIEW_SIZE;
	const char *wp, *bg;
	char *b = map_buf;
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
		curr = geo_exit_where(descr, player, curr, dir);

		if (curr < 0)
			w = (char *) h_closed;
		else if (GETDOOR(curr)) {
			snprintf(wb, sizeof(wb), "-"ANSI_FG_WHITE"+%s-", wp);
			w = wb;
		} else
			w = (char *) h_open;
	}

	*b = '\0';
	notify(player, map_buf);
}

static inline void
map_search(dbref *mat, point3D_t pos, ucoord_t flags)
{
	struct rect3D vpr3D = {
		{ pos[0] - VIEW_AROUND, pos[1] - VIEW_AROUND, pos[2] },
		{ VIEW_SIZE, VIEW_SIZE, 1 },
	};

	static const size_t m = 49;
	pointdb_range_t buf[m];
	size_t n;
	int i;

	n =  pointdb_range_safe(buf, m, &vpr3D, flags, 0);
	memset(mat, -1, sizeof(dbref) * m);

	for (i = 0; i < n; i++) {
		point3D_t p;
		morton3D_decode(p, buf[i].where);
		mat[point_rel_idx(p, vpr3D.s, VIEW_SIZE)] = buf[i].what;
	}
}

/* used at the end of every command */
void
do_map(int descr, dbref player)
{
	morton_t code = pdb_where(getloc(player));
	static const size_t bdi = VIEW_SIZE * (VIEW_SIZE - 1);
	struct bio bd[VIEW_M], *n_max = bd + bdi, *n_p = bd;
	dbref o[VIEW_M], *o_p;
	point3D_t pos;
	ucoord_t obits = OBITS(code);

	morton3D_decode(pos, code);
	noise_view(bd, pos, obits);
	map_search(o, pos, obits);

	o_p = o;

	dr_vs(descr, player, n_p, o_p);

	for (; n_p < n_max;) {
		dr_hs_n(descr, player, n_p, o_p);
		n_p += VIEW_SIZE;
		o_p += VIEW_SIZE;
		dr_vs(descr, player, n_p, o_p);
	}
}

/* }}} */

/* PARSE {{{ */

static int
geo_cmd_dir(struct cmd_dir *res, const char *cmd)
{
	int ofs = 0;
	morton_t rep = 1;
	char *end;

	if (isdigit(cmd[0])) {
		rep = strtoull(cmd, &end, 0);
		ofs += end - &cmd[0];
	}

	res->dir = cmd[ofs];
	res->rep = rep;
	if (dir_valid(res->dir))
		return ofs + 1;
	else {
		res->dir = '\0';
		return 0;
	}
}

static inline int
room_put_here(int descr, dbref player, char const *s)
{
	morton_t x;
	char *end;
	dbref here = getloc(player),
	      there = strtol(s + 1, &end, 0);

	x = pdb_where(here);

	if (OWNER(here) != player) {
		notify_fmt(player, "You don't own this room.");
		return s - end;
	}

	if (!GETTMP(here))
		SETTMP(here, 1);

	enter_room(descr, player, there, NOTHING);
	pdb_move(there, x);
	return end - s;
}

static inline int
teleport_human(int descr, dbref player, const char *cmd)
{
	morton_t x;
	point3D_t p;
	char *end = (char*) cmd;
	ucoord_t obits = 0;

	memset(p, 0, sizeof(p));

	if (*end == '\0')
		goto skip;

	p[0] = strtol(end, &end, 0);
	if (*end == '\0')
		goto skip;

	end++;
	p[1] = strtol(end, &end, 0);
	if (*end == '\0')
		goto skip;

	end++;
	p[2] = strtol(end, &end, 0);
	if (*end == '\0')
		goto skip;

	end++;
	obits = strtol(end, &end, 0);

skip:	x = morton3D_encode(p, obits);
	x = teleport(descr, player, x);
	CBUG(x);
	return end - cmd;
}

static inline int
geo_location_x(int descr, dbref player, char const *cmd) {
	dbref who = player;
	char const *s = cmd;
	dbref i;

	if (*s == '/') {
		s++;
		who = NOTHING;
		if (!payfor(player, LOOKUP_COST))
			return s - cmd;

		for (i = 0; i < db_top; i++)
			if (Typeof(i) == TYPE_PLAYER && string_prefix(NAME(i), s))
				who = i;
		if (who < 0)
			return s - cmd;

		s += strlen(s);
	}

	notify_fmt(player, "0x%llx", pdb_where(getloc(who)));
	return s - cmd;
}

int
geo_v(int descr, dbref player, char const *opcs)
{
	struct cmd_dir cd;
	morton_t x;
	char const *s = opcs;
	int cont = 1;

	for (; cont;) switch(*s++) {
	case '\0':
		cont = 0;
		break;
	case 'x':
		s += geo_location_x(descr, player, s);
		break;
	case 'X':
		s += geo_cmd_dir(&cd, s);
		x = cd.rep;
		notify_fmt(player, "Teleporting to 0x%llx.", x);
		teleport(descr, player, x);
		break;
	case 't':
		x = pdb_where(getloc(player));
		{
			point3D_t p;
			morton3D_decode(p, x);
			notify_fmt(player, "y: %d, x: %d, z: %d, extra: 0x%x", p[Y_COORD], p[X_COORD], p[2], OBITS(x));
		}
		break;
	case 'T':
		s += teleport_human(descr, player, s);
		break;
	case 'm':
		x = pdb_where(getloc(player));
		mark_set(player, *s, x);
		s++;
		break;
	case '"':
		x = mark_get(player, *s);
		if (x == -1ULL)
			break;
		teleport(descr, player, x);
		s++;
		break;
	case '#':
		s += room_put_here(descr, player, s);
		break;
	default: {
		int ofs = 0;
		s --;
		op_t *op = op_map[(int) *s];
		ofs += !!op;
		ofs += geo_cmd_dir(&cd, s + ofs);

		if (cd.dir == '\0')
			return opcs - s;

		if (!op)
			op = &walk;

		int j;
		for (j = 0; j < cd.rep; j++)
			op(descr, player, map[(int) cd.dir][0]);
		s += ofs;
	}}

	return s - opcs;
}

/* }}} */

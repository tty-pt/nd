#include "st.h"

#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pwd.h>
#include <qdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <xxhash.h>

#include "config.h"
#include "nddb.h"
#include "noise.h"
#include "params.h"
#include "player.h"
#include "view.h"
#include "mcp.h"
#include "papi/nd.h"

#define PRECOVERY

#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

#define ROOM_COST 80

unsigned owner_hd = -1, sl_hd = -1;

typedef void op_a_t(unsigned player_ref, enum exit e);
typedef int op_b_t(unsigned player_ref, struct cmd_dir cd);
typedef struct {
	union {
		op_a_t *a;
		op_b_t *b;
	} op;
	int type;
} op_t;
unsigned g_player_ref;
OBJ *g_player;
extern struct nd nd;
static unsigned *biome_map;

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

unsigned long long day_tick = 0;
unsigned short day_n = 0;
double tick = 0;

static __inline__ ucoord_t
unsign(coord_t n)
{
	ucoord_t r = ((smorton_t) n + COORD_MAX);

	if (r == UCOORD_MAX)
		return 1;

	return r;
}


/* spread3(x):
 *   Take x ∈ [0..0xFFFF] and produce a 64-bit word where
 *   its bit-i goes to bit-(3*i) in the result.
 *
 * Part of a Morton-3D encode:  code = spread3(x)
 *                                  | spread3(y)<<1
 *                                  | spread3(z)<<2
 */
static inline uint64_t spread3(uint32_t x)
{
    uint64_t v = x & 0xFFFFu;  /* keep only low 16 bits */
    v = (v | (v << 32)) & 0x1F00000000FFFFULL; /* make room for high triples */
    v = (v | (v << 16)) & 0x1F0000FF0000FFULL; /* down to 8-bit chunks */
    v = (v | (v << 8)) & 0x100F00F00F00F00FULL; /* down to 4-bit groups */
    v = (v | (v << 4)) & 0x10C30C30C30C30C3ULL; /* down to 2-bit groups */
    v = (v | (v << 2)) & 0x1249249249249249ULL; /* final 3 bit interleave */
    return v;
}

static inline uint64_t morton3_pack_u16(
		uint16_t x,
		uint16_t y,
		uint16_t z,
		uint16_t world)
{
    return spread3(x)
         | (spread3(y) << 1)
         | (spread3(z) << 2)
         | ((uint64_t)world << 48);
}

morton_t
pos_morton(pos_t p)
{
	upoint3D_t up;
	up[0] = unsign(p[0]);
	up[1] = unsign(p[1]);
	up[2] = unsign(p[2]);
	return morton3_pack_u16(up[0], up[1], up[2], 0) | ((morton_t) p[3] << 48);
}

static inline coord_t
sign(ucoord_t n)
{
	return (smorton_t) n - COORD_MAX;
}

/* compact_axis(): collect one out of every 3 bits from 'code',
 * starting at 'shift' (0 = x, 1 = y, 2 = z).  Returns low-order
 * 21 bits containing that coordinate.                         */
static inline uint32_t compact_axis(uint64_t code, unsigned shift)
{
    code >>= shift; /* align the desired series to LSB */
    /* first keep only 1---1---1 pattern → mask 0x1249249249249… */
    code &= 0x1249249249249249ULL;

    /* Now collapse gaps:  3→2 → 2→1 → 1→0 */
    code = (code ^ (code >> 2))  & 0x10C30C30C30C30C3ULL;
    code = (code ^ (code >> 4))  & 0x100F00F00F00F00FULL;
    code = (code ^ (code >> 8))  & 0x1F0000FF0000FFULL;
    code = (code ^ (code >> 16)) & 0x1F00000000FFFFULL;
    code = (code ^ (code >> 32)) & 0x00000000001FFFFFULL;

    return (uint32_t) code; /* low 21 bits hold the axis value */
}

static inline void decode3(uint64_t code,
                           uint32_t *x, uint32_t *y, uint32_t *z)
{
    *x = compact_axis(code, 0);   /* bits 0,3,6,…   */
    *y = compact_axis(code, 1);   /* bits 1,4,7,…   */
    *z = compact_axis(code, 2);   /* bits 2,5,8,…   */
}

void
morton_pos(pos_t p, morton_t code)
{
	static const morton_t mask_off = 0x0000FFFFFFFFFFFFULL;
	uint32_t uup[3] = { 0, 0, 0 };
	decode3(code & mask_off, &uup[0], &uup[1], &uup[2]);
	p[0] = sign(uup[0]);
	p[1] = sign(uup[1]);
	p[2] = sign(uup[2]);
	p[3] = OBITS(code);
}

void
object_drop(unsigned where_ref, unsigned skel_id)
{
	pos_t pos;
        register int i;
	unsigned drop_id;
	OBJ where;
	unsigned c = qmap_iter(adrop_hd, &skel_id);

	qmap_get(obj_hd, &where, &where_ref);
	map_where(pos, where.location);

	while (qmap_next(&skel_id, &drop_id, c)) {
		DROP drop;
		qmap_get(drop_hd, &drop, &drop_id);
		if (random() < (RAND_MAX >> drop.y)) {
			uint32_t v2 = XXH32((const char *) pos, sizeof(pos_t), 3);
                        int yield = drop.yield,
                            yield_v = drop.yield_v;

                        if (!yield) {
				OBJ obj;
				unsigned obj_ref = object_add(&obj, drop.skel, where_ref, v2, 0);
				qmap_put(obj_hd, &obj_ref, &obj);
                                continue;
                        }

                        yield += random() & yield_v;

                        for (i = 0; i < yield; i++) {
				v2 = XXH32((const char *) pos, sizeof(pos_t), 4 + i);
				OBJ obj;
                                unsigned obj_ref = object_add(&obj, drop.skel, where_ref, v2, 0);
				qmap_put(obj_hd, &obj_ref, &obj);
			}
                }
	}
}

int
e_exit_can(OBJ *player, enum exit e) {
	return e_ground(player->location, e);
}

int
e_ground(unsigned room, enum exit e)
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
	memcpy(d, o, sizeof(coord_t) * 4);
	d[ex->dim] += ex->dis;
}

enum exit
dir_e(const char dir) {
	return e_map[(int) dir];
}

char
e_dir(enum exit e) {
	return exit_map[e].name[0];
}

enum exit
e_simm(enum exit e) {
	return exit_map[e].simm;
}

char *
e_name(enum exit e) {
	return (char *) exit_map[e].name;
}

char *
e_other(enum exit e) {
	return (char *) exit_map[e].other;
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
	day_tick += dt;
	if (day_tick > (1ULL << DAYTICK_Y))
		day_tick = 0;

	if (day_tick >= (1ULL << (DAYTICK_Y - 1))) {
		if (day_n)
			return;
		msg = "The sun sets.\n";
		day_n = 1;
	} else if (day_n) {
		msg = "The sun rises.\n";
		day_n = 0;
	} else
		return;

	unsigned c = qmap_iter(obj_hd, NULL);
	OBJ iobj;
	unsigned iobj_ref;

	while (qmap_next(&iobj_ref, &iobj, c))
		if (iobj.type == TYPE_ENTITY) {
			view(iobj_ref);
			mcp_tod(iobj_ref, day_n);
			nd_writef(iobj_ref, msg);
		}
}

static inline void
st_pos(pos_t p, unsigned loc, enum exit e)
{
	pos_t aux;
	map_where(aux, loc);
	pos_move(p, aux, e);
}

static unsigned
st_there(unsigned where, enum exit e)
{
	pos_t pos;
	st_pos(pos, where, e);
	return map_get(pos);
}

static inline int
fee_fail(unsigned player_ref, OBJ *player, char *desc, char *info, unsigned cost)
{
	if (player->value < cost) {
		nd_writef(player_ref, "You can't afford to %s. (%dp)\n", desc, cost);
		return 1;
	} else {
		player->value -= cost;
		nd_writef(player_ref, "%s (-%dp). %s\n",
			   desc, cost, info);
		return 0;
	}
}

static int
st_claim(unsigned player_ref, OBJ *room) {
	ROO *rroom = (ROO *) &room->data;

	if (!(rroom->flags & RF_TEMP)) {
		if (room->owner != player_ref) {
			nd_writef(player_ref, "You don't own this room\n");
			return 1;
		}

		return 0;
	}

	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	if (fee_fail(player_ref, &player, "claim a room", "", ROOM_COST))
		return 1;

	qmap_put(obj_hd, &player_ref, &player);
	rroom->flags ^= RF_TEMP;
	room->owner = player_ref;

	return 0;
}

static inline void
exits_fix(unsigned there_ref, enum exit e)
{
	OBJ there;
	qmap_get(obj_hd, &there, &there_ref);
	ROO *rthere = (ROO *) &there.data;
	const char *s;

	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		unsigned othere_ref = st_there(there_ref, e2);

		if (othere_ref == NOTHING)
			continue;

		OBJ othere;
		qmap_get(obj_hd, &othere, &othere_ref);
		ROO *rothere = (ROO *) &othere.data;

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

	qmap_put(obj_hd, &there_ref, &there);
}

static void
exits_infer(unsigned here_ref, ROO *rhere)
{
	const char *s = "wsnedu";

	for (; *s; s++) {
		enum exit e = dir_e(*s);
		unsigned there_ref = st_there(here_ref, e);

		if (there_ref == NOTHING) {
                        if (e != E_UP && e != E_DOWN)
				rhere->exits |= e;
			continue;
		}

		OBJ there;
		qmap_get(obj_hd, &there, &there_ref);
		ROO *rthere = (ROO *) &there.data;

		if (rthere->exits & e_simm(e)) {
			rhere->exits |= e;
			rhere->doors |= rthere->doors & e_simm(e);
		}
	}
}

void map_put(pos_t p, unsigned thing, int flags);

static unsigned
st_room_at(unsigned player_ref, pos_t pos)
{
	struct bio bio = noise_point(pos);
	OBJ there;
	biome_map = biome_map_get(* (uint64_t *) pos);
	unsigned there_ref = object_add(&there, biome_map[bio.bio_idx], 0, (uint64_t) &bio, 0);
	ROO *rthere = (ROO *) &there.data;
	map_put(pos, there_ref, DB_NOOVERWRITE);
	exits_infer(there_ref, rthere);

	fprintf(stderr, "st_room_at %u\n", player_ref);

	if (pos[2] != 0) {
		qmap_put(obj_hd, &there_ref, &there);
		return there_ref;
	}

	rthere->floor = bio.bio_idx;
	qmap_put(obj_hd, &there_ref, &there);
	uint32_t v = XXH32((const char *) pos, sizeof(pos_t), 1);
	call_on_spawn(player_ref, there_ref, bio, v);
	return there_ref;
}

void
do_bio(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);
	struct bio bio;
	pos_t pos;
	OBJ player;

	qmap_get(obj_hd, &player, &player_ref);
	map_where(pos, player.location);
	bio = noise_point(pos);
	SKEL biome;
	qmap_get(skel_hd, &biome, &biome_map[bio.bio_idx]);
	nd_writef(player_ref, "tmp %d rn %u bio %s(%d)\n",
		bio.tmp, bio.rn, biome.name, bio.bio_idx);
}

static unsigned
st_room(unsigned player_ref, unsigned location, enum exit e)
{
	pos_t pos;
	st_pos(pos, location, e);
	return st_room_at(player_ref, pos);
}

static unsigned
e_move(unsigned player_ref, enum exit e) {
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	OBJ loc;
	qmap_get(obj_hd, &loc, &player.location);
	unsigned dest_ref;
	ROO *rloc = (ROO *) &loc.data;
	char const *dwts = "door";
	int door = 0;

	int cant_move = call_on_move(player_ref);
	if (cant_move)
		return NOTHING;

	if (!map_has(player.location) || !(rloc->exits & e)) {
		nd_writef(player_ref, "You can't go that way.\n");
		return NOTHING;
	}

	if (rloc->doors & e) {
		if (e == E_UP || e == E_DOWN) {
			dwts = "hatch";
			door = 2;
		} else
			door = 1;

		nd_writef(player_ref, "You open the %s.\n", dwts);
	}

	dest_ref = st_there(player.location, e);
	if (dest_ref == NOTHING)
		dest_ref = st_room(player_ref, player.location, e);

	enter(player_ref, dest_ref, e);

	if (door)
		nd_writef(player_ref, "You close the %s.\n", dwts);

	return dest_ref;
}

unsigned
room_clean(unsigned here_ref)
{
	unsigned tmp_ref;
	OBJ here;

	qmap_get(obj_hd, &here, &here_ref);

	if (!(((ROO *) here.data)->flags & RF_TEMP))
		return here_ref;

	unsigned c = qmap_iter(contents_hd, &here_ref);
	while (qmap_next(&here_ref, &tmp_ref, c)) {
		OBJ tmp;
		qmap_get(obj_hd, &tmp, &tmp_ref);

		if (tmp.type != TYPE_ENTITY)
			continue;

		if (tmp.flags & OF_PLAYER) {
			qmap_fin(c);
			return here_ref;
		}
	}

	object_move(here_ref, NOTHING);
	return NOTHING;
}

static void
walk(unsigned player_ref, enum exit e) {
	e_move(player_ref, e);
}

static void
carve(unsigned player_ref, enum exit e)
{
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned here_ref = player.location, there_ref = here_ref;
	OBJ here;
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;
	int wall = 0;

	if (!e_ground(here_ref, e)) {
		if (st_claim(player_ref, &here))
			return;

		rhere->exits |= e;
		qmap_put(obj_hd, &here_ref, &here);

		there_ref = st_there(here_ref, e);
		if (there_ref == NOTHING)
			there_ref = st_room(player_ref, here_ref, e);
		wall = 1;
	}

	there_ref = e_move(player_ref, e);
	if (there_ref == NOTHING)
		return;

	OBJ there;
	qmap_get(obj_hd, &there, &there_ref);
	st_claim(player_ref, &there); // FIXME check success in advance

	if (wall) {
		ROO *rthere = (ROO *) &there.data;
		const char *s;
		for (s = e_other(e_simm(e)); *s; s++) {
			enum exit e2 = dir_e(*s);

			if ((rthere->exits & e2) &&
					st_there(there_ref, e2) != NOTHING)

				rthere->exits ^= e2;
		}
	}

	qmap_put(obj_hd, &there_ref, &there);
}

static void
uncarve(unsigned player_ref, enum exit e)
{
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	const char *s0 = "is";
	unsigned here_ref = player.location, there_ref;
	OBJ here;
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;
	int ht, cd = e_ground(here_ref, e);

	if (cd) {
		ht = rhere->flags & RF_TEMP;
		there_ref = e_move(player_ref, e);
		if (there_ref == NOTHING)
			return;
	} else {
		if (!(rhere->exits & e)) {
                        nd_writef(player_ref, "No exit there.\n");
                        return;
                }

		there_ref = st_there(here_ref, e);

		if (there_ref == NOTHING) {
			nd_writef(player_ref, "No room there.\n");
			return;
		}
		s0 = "at";
	}

	OBJ there;
	qmap_get(obj_hd, &there, &there_ref);
	ROO *rthere = (ROO *) &there.data;

	if ((rthere->flags & RF_TEMP) || there.owner != player_ref) {
		nd_writef(player_ref, "You don't own th%s room.\n", s0);
		return;
	}

	rthere->flags ^= RF_TEMP;
	exits_infer(there_ref, rthere);
	if (cd) {
		if (ht && (rthere->doors & e_simm(e)))
			rthere->doors &= ~e_simm(e);
		qmap_put(obj_hd, &there_ref, &there);
	} else
		room_clean(there_ref);

	qmap_get(obj_hd, &player, &player_ref);
	player.value += ROOM_COST;
	nd_writef(player_ref, "You collect your materials. (+%dp)\n", ROOM_COST);
	qmap_put(obj_hd, &player_ref, &player);
}

static void
unwall(unsigned player_ref, enum exit e)
{
	int a, b, c, d;
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned there_ref, here_ref = player.location;
	OBJ here;
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;

	a = here.owner == player_ref;
	b = rhere->flags & RF_TEMP;
	there_ref = st_there(here_ref, e);

	OBJ there;
	qmap_get(obj_hd, &there, &there_ref);
	ROO *rthere = (ROO *) &there.data;

	if (there_ref != NOTHING) {
		c = there.owner == player_ref;
		d = rthere->flags & RF_TEMP;
	} else {
		c = 0;
		d = 1;
	}

	if (!((a && !b && (d || c))
	    || (c && !d && b))) {
		nd_writef(player_ref, "You can't do that here.\n");
		return;
	}

	if (rhere->exits & e) {
		nd_writef(player_ref, "There's an exit here already.\n");
		return;
	}

	rhere->exits |= e;
	there_ref = e_move(player_ref, e);
	if (there_ref == NOTHING)
		return;

	qmap_get(obj_hd, &there, &there_ref);
	rthere = (ROO *) &there.data;
	rthere->exits |= e_simm(e);
	qmap_put(obj_hd, &there_ref, &there);
	nd_writef(player_ref, "You tear down the wall.\n");
}

static inline int
gexit_claim(unsigned player_ref, enum exit e)
{
	int a, b, c, d;
	OBJ player, here, there;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned here_ref = player.location,
	      there_ref = st_there(player.location, e);
	qmap_get(obj_hd, &here, &here_ref);
	qmap_get(obj_hd, &there, &there_ref);
	ROO *rthere = (ROO *) &there.data;

	a = here_ref != NOTHING && here.owner == player_ref;
	c = rthere->flags & RF_TEMP;
	b = !c && there.owner == player_ref;
	d = e_ground(here_ref, e);

	if (a && (b || c))
		return 0;

	if (here_ref != NOTHING || (((ROO *) here.data)->flags & RF_TEMP)) {
		if (b)
			return 0;
		if (d || c) // FIXME
			return st_claim(player_ref, &there);
	}

	nd_writef(player_ref, "You can't claim that exit.\n");
	return 1;
}

static inline int
gexit_claim_walk(unsigned player_ref, enum exit e)
{
	OBJ player, here;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned here_ref = player.location;
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;

	if (!(rhere->exits & e)) {
		nd_writef(player_ref, "No exit here.\n");
		return 1;
	}

	unsigned there_ref = e_move(player_ref, e);
	if (there_ref == NOTHING)
		return 1;

	player.location = there_ref;
	qmap_put(obj_hd, &player_ref, &player);

	return gexit_claim(player_ref, e_simm(e));
}

static void
e_wall(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, here;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned here_ref = player.location;
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;

	rhere->exits &= ~e_simm(e);
	qmap_put(obj_hd, &here_ref, &here);

	unsigned there_ref = st_there(here_ref, e_simm(e));

	if (there_ref != NOTHING) {
		OBJ there;
		qmap_get(obj_hd, &there, &there_ref);
		ROO *rthere = (ROO *) &there.data;
		rthere->exits &= ~e;
		qmap_put(obj_hd, &there_ref, &there);
	}

	nd_writef(player_ref, "You build a wall.\n");
}

static void
door(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, where;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned where_ref = player.location;
	qmap_get(obj_hd, &where, &where_ref);
	ROO *rwhere = (ROO *) &where.data;
	rwhere->doors |= e_simm(e);
	qmap_put(obj_hd, &where_ref, &where);

	where_ref = st_there(where_ref, e_simm(e));

	if (where_ref == NOTHING) {
		qmap_get(obj_hd, &where, &where_ref);
		rwhere = (ROO *) &where.data;
		rwhere->doors |= e;
		qmap_put(obj_hd, &where_ref, &where);
	}

	nd_writef(player_ref, "You place a door.\n");
}

static void
undoor(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, where;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned where_ref = player.location;
	qmap_get(obj_hd, &where, &where_ref);
	ROO *rwhere = (ROO *) &where.data;
	rwhere->doors &= ~e_simm(e);
	qmap_put(obj_hd, &where_ref, &where);

	where_ref = st_there(where_ref, e_simm(e));

	if (where_ref != NOTHING) {
		qmap_get(obj_hd, &where, &where_ref);
		rwhere = (ROO *) &where.data;
		rwhere->doors &= ~e;
		qmap_put(obj_hd, &where_ref, &where);
	}

	nd_writef(player_ref, "You remove a door.\n");
}

static int
tell_pos(unsigned player_ref, struct cmd_dir cd) {
	pos_t pos;
	unsigned target_ref = cd.rep == 1 ? player_ref : cd.rep;
	OBJ target;
	qmap_get(obj_hd, &target, &target_ref);
	int ret = 1;

	if (target.type != TYPE_ENTITY) {
		nd_writef(player_ref, "Invalid object type.\n");
		return 0;
	}

	map_where(pos, target.location);
	nd_writef(player_ref, "0x%llx\n", MORTON_READ(pos));
	return ret;
}

void
st_teleport(unsigned player_ref, uint64_t mpos) {
	pos_t pos;
	memcpy(pos, &mpos, sizeof(mpos));
	unsigned there_ref = map_get(pos);
	biome_map = biome_map_get(* (uint16_t *) pos);
	if (there_ref == NOTHING)
		there_ref = st_room_at(player_ref, pos);
	enter(player_ref, there_ref, E_NULL);
}

int
vim_teleport(unsigned player_ref, struct cmd_dir cd)
{
	pos_t pos;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '?') {
		// X6? teleport to chivas
		unsigned target_ref = cd.rep;
		OBJ target;
		qmap_get(obj_hd, &target, &target_ref);
		if (target.type == TYPE_ENTITY) {
			map_where(pos, target.location);
			ret = 1;
		}
		memcpy(&cd.rep, pos, sizeof(cd.rep));
	}

	st_teleport(player_ref, cd.rep);
	return ret;
}

static int
pull(unsigned player_ref, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	unsigned here_ref = player.location, there_ref;

	if (e == E_NULL)
		return 0;

	unsigned what_ref = cd.rep;

	if (what_ref == NOTHING) {
		nd_writef(player_ref, "You cannot do that.\n");
		return 1;
	}

	OBJ what, here;
	qmap_get(obj_hd, &what, &what_ref);
	qmap_get(obj_hd, &here, &here_ref);
	ROO *rhere = (ROO *) &here.data;

	if (!(rhere->exits & e)
	    || what.type != TYPE_ROOM
	    || what.owner != player_ref
	    || ((there_ref = st_there(here_ref, e))
		&& room_clean(there_ref) == there_ref))
	{
		nd_writef(player_ref, "You cannot do that.\n");
		return 1;
	}

	st_pos(pos, player.location, e);
	map_put(pos, cd.rep, 0);
	exits_fix(cd.rep, e);
	e_move(player_ref, e);
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
	['X'] = { .op.b = &vim_teleport, .type = 1 },
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

static void may_look(unsigned player_ref, morton_t old_loc) {
	OBJ player;
	morton_t new_loc;

	qmap_get(obj_hd, &player, &player_ref);
	new_loc = map_mwhere(player.location);

	if (old_loc == new_loc)
		return;

	look_at(player_ref, NOTHING);
	view(player_ref);
	nd_flush(player_ref);
}

int
st_v(unsigned player_ref, char const *opcs)
{
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	morton_t old_loc = map_mwhere(player.location);
	pos_t old_pos;
	morton_pos(old_pos, old_loc);
	biome_map = biome_map_get(* (uint64_t *) old_pos);
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
			if (cd.e == E_NULL) {
				may_look(player_ref, old_loc);
				return opcs - s;
			}

			ofs ++;
			morton_t j;
			for (j = 0; j < cd.rep; j++)
				aop(player_ref, cd.e);
		} else
			ofs += op.op.b(player_ref, cd);

		s += ofs;
	}

	may_look(player_ref, old_loc);
	return s - opcs;
}

void
echo(char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_dwritef(g_player_ref, fmt, va);
	va_end(va);
}

void
oecho(char *format, ...) {
	va_list args;
	va_start(args, format);
	nd_owritef(g_player_ref, format, args);
	va_end(args);
}

// load a spacetime shared library and put it in effect
static void
st_open(struct st_key st_key, int owner)
{
	char filename[BUFSIZ];

	uint64_t short_key = st_key.key >> st_key.shift;
	snprintf(filename, sizeof(filename), "/var/nd/st/%u/%llu/libnd.so", st_key.shift, short_key);
	void *sl = dlopen(filename, RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);

	if (!sl)
		syslog(LOG_ERR, "dlopen error for %d, %s: %s", owner, filename, dlerror());
	else {
		dlerror();
		syslog(LOG_INFO, "dlopen %d, %s", owner, filename);
		struct nd *ind = dlsym(sl, "nd");
		if (ind)
			*ind = nd;
		else
			fprintf(stderr, "%u's st module didn't link agains libnd\n", owner);
		qmap_put(sl_hd, &st_key, &sl);
	}

	/* hash_put(owner_hd, &st_key, sizeof(st_key), &owner, sizeof(owner)); */
}

void
st_put(unsigned owner_ref, uint64_t key, unsigned shift) {
	char buf[BUFSIZ];
	struct st_key st_key = st_key_new(key, shift);
	size_t len = snprintf(buf, sizeof(buf), "/var/nd/st/%u/", shift);
	mkdir(buf, 0750);
	snprintf(buf + len, sizeof(buf) - len, "%llu", key >> shift);
	mkdir(buf, 0750);
	st_open(st_key, owner_ref);
}

void
st_init(void) {
	unsigned c = qmap_iter(owner_hd, NULL);
	struct st_key st_key;
	int owner;

	while (qmap_next(&st_key, &owner, c))
		st_open(st_key, owner);
}

void st_dlclose(void) {
	unsigned c = qmap_iter(sl_hd, NULL);
	struct st_key key;
	void *sl;

	while (qmap_next(&key, &sl, c))
		dlclose(sl);
}

typedef void (*st_run_cb)(unsigned player_ref);

int
st_get(uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (qmap_get(owner_hd, &owner, &st_key))
		return -1;

	return owner;
}

inline static int
_st_can(int ref, uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (qmap_get(owner_hd, &owner, &st_key))
		return 0;

	return owner == ref;
}

static long int
st_high_shift(unsigned player_ref, uint64_t position)
{
	int ref = player_ref;

	if (_st_can(ref, 0, 64))
		return 64;

	for (int i = 63; i >= 0; i--)
		if (_st_can(ref, position, i))
			return i;

	return -1;
}

inline static int
_st_run(unsigned player_ref, char *symbol, uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	void *sl;

	if (qmap_get(sl_hd, &sl, &st_key))
		return 0;

	st_run_cb cb = (st_run_cb) dlsym(sl, symbol);

	if (!cb)
		return 0;

	(*cb)(player_ref);
	return 1;
}

void
st_run(unsigned player_ref, char *symbol) {
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);
	uint64_t position = map_mwhere(player.location), mask = 0;
	int i;
	g_player_ref = player_ref;

	_st_run(player_ref, symbol, 0, 64);

	for (i = 63; i >= 0; i--)
		mask |= _st_run(player_ref, symbol, position, i) << i;

	syslog(LOG_INFO, "st_run %s %llx", symbol, mask);
}

void
do_stchown(int fd, int argc, char *argv[]) {
	unsigned player_ref = fd_player(fd);

	if (argc < 2) {
		nd_writef(player_ref, "Requires at least an argument\n");
		return;
	}

	unsigned who_ref = player_get(argv[1]);

	if (who_ref == NOTHING) {
		nd_writef(player_ref, "Invalid target\n");
		return;
	}

	OBJ who;
	qmap_get(obj_hd, &who, &who_ref);

	if (!(who.flags & OF_PLAYER)) {
		nd_writef(player_ref, "Invalid target\n");
		return;
	}

	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);

	uint64_t position = argc > 2
		? strtoull(argv[3], NULL, 10)
		: map_mwhere(player.location);

	long int high_shift = st_high_shift(player_ref, position);

	unsigned shift = argc < 3 ? high_shift : strtoul(argv[2], NULL, 10);

	long long unsigned key = shift > 63 ? 0 : position;

	if (high_shift < shift) {
		nd_writef(player_ref, "Permission denied\n");
		return;
	}

	st_put(who_ref, key, shift);
	nd_writef(player_ref, "Set shift %d ownership to %s\n", shift, who.name);
}

void
do_streload(int fd, int argc, char *argv[]) {
	unsigned player_ref = fd_player(fd);
	OBJ player;
	qmap_get(obj_hd, &player, &player_ref);

	uint64_t position = argc > 2
		? strtoull(argv[2], NULL, 10)
		: map_mwhere(player.location);

	long int high_shift = st_high_shift(player_ref, position);

	unsigned shift = argc < 2 ? high_shift : strtoul(argv[1], NULL, 10);

	long long unsigned key = shift > 63 ? 0 : position;

	struct st_key st_key = st_key_new(key, shift);
	unsigned owner;
	void *sl;

	if (qmap_get(owner_hd, &owner, &st_key) || owner != player_ref)
	{
		nd_writef(player_ref, "Permission denied\n");
		return;
	}

	if (qmap_get(sl_hd, &sl, &st_key)) {
		nd_writef(player_ref, "Not open\n");
		return;
	}

	dlclose(sl);
	st_open(st_key, owner);
}

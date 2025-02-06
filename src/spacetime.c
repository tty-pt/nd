#include "st.h"

#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <pwd.h>
#include <qhash.h>
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
#include "papi/nd.h"

#define PRECOVERY

#define GEON_RADIUS (VIEW_AROUND + 1)
#define GEON_SIZE (GEON_RADIUS * 2 + 1)
#define GEON_M (GEON_SIZE * GEON_SIZE)
#define GEON_BDI (GEON_SIZE * (GEON_SIZE - 1))

#define ROOM_COST 80

unsigned owner_hd = -1, sl_hd = -1, stone_skel_id;

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
struct nd nd;

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
}

void
object_drop(unsigned where_ref, unsigned skel_id)
{
	pos_t pos;
        register int i;
	unsigned drop_id;
	struct hash_cursor c = fhash_iter(adrop_hd, skel_id);
	OBJ where;
	lhash_get(obj_hd, &where, where_ref);

	map_where(pos, where.location);

	while (ahash_next(&drop_id, &c)) {
		DROP drop;
		lhash_get(drop_hd, &drop, drop_id);
		if (random() < (RAND_MAX >> drop.y)) {
			noise_t v2 = XXH32((const char *) pos, sizeof(pos_t), 3);
                        int yield = drop.yield,
                            yield_v = drop.yield_v;

                        if (!yield) {
				OBJ obj;
				unsigned obj_ref = object_add(&obj, drop.skel, where_ref, &v2);
				lhash_put(obj_hd, obj_ref, &obj);
                                continue;
                        }

                        yield += random() & yield_v;

                        for (i = 0; i < yield; i++) {
				v2 = XXH32((const char *) pos, sizeof(pos_t), 4 + i);
				OBJ obj;
                                unsigned obj_ref = object_add(&obj, drop.skel, where_ref, &v2);
				lhash_put(obj_hd, obj_ref, &obj);
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
	POOP4D d[I] = o[I];
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

	struct hash_cursor c = lhash_iter(obj_hd);
	OBJ iobj;
	unsigned iobj_ref;

	while (lhash_next(&iobj_ref, &iobj, &c))
		if (iobj.type == TYPE_ENTITY) {
			view(iobj_ref);
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
	ROO *rroom = &room->sp.room;

	if (!(rroom->flags & RF_TEMP)) {
		if (room->owner != player_ref) {
			nd_writef(player_ref, "You don't own this room\n");
			return 1;
		}

		return 0;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	if (fee_fail(player_ref, &player, "claim a room", "", ROOM_COST))
		return 1;

	lhash_put(obj_hd, player_ref, &player);
	rroom->flags ^= RF_TEMP;
	room->owner = player_ref;

	return 0;
}

static inline void
exits_fix(unsigned there_ref, enum exit e)
{
	OBJ there;
	lhash_get(obj_hd, &there, there_ref);
	ROO *rthere = &there.sp.room;
	const char *s;

	for (s = e_other(e); *s; s++) {
		enum exit e2 = dir_e(*s);
		unsigned othere_ref = st_there(there_ref, e2);

		if (othere_ref == NOTHING)
			continue;

		OBJ othere;
		lhash_get(obj_hd, &othere, othere_ref);
		ROO *rothere = &othere.sp.room;

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

	lhash_put(obj_hd, there_ref, &there);
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
		lhash_get(obj_hd, &there, there_ref);
		ROO *rthere = &there.sp.room;

		if (rthere->exits & e_simm(e)) {
			rhere->exits |= e;
			rhere->doors |= rthere->doors & e_simm(e);
		}
	}
}

// TODO make more add functions similar and easy to insert here

static inline void
stones_add(unsigned where_ref, struct bio *bio, pos_t p) {
	noise_t v = XXH32((const char *) p, sizeof(pos_t), 0);
	unsigned char n = v & 0x3, i;

	if (bio->bio_idx == BIOME_WATER)
		return;

        if (!(n && (v & 0x18) && (v & 0x20)))
                return;

	noise_t v2 = XXH32((const char *) p, sizeof(pos_t), 0);

        for (i = 0; i < n; i++, v2 >>= 4) {
		OBJ stone;
                unsigned stone_ref = object_add(&stone, stone_skel_id, where_ref, &v2);
		lhash_put(obj_hd, stone_ref, &stone);
	}
}

struct bio * noise_point(pos_t p);
void plants_add(unsigned where_ref, void *bio, pos_t pos);
void mobs_add(unsigned where_ref, enum biome, long long pdn);
void map_put(pos_t p, unsigned thing, int flags);

static unsigned
st_room_at(pos_t pos)
{
	struct bio *bio;
	bio = noise_point(pos);
	OBJ there;
	unsigned there_ref = object_add(&there, biome_refs[bio->bio_idx], NOTHING, bio);
	ROO *rthere = &there.sp.room;
	map_put(pos, there_ref, DB_NOOVERWRITE);
	exits_infer(there_ref, rthere);

	if (pos[2] != 0) {
		lhash_put(obj_hd, there_ref, &there);
		return there_ref;
	}

	rthere->floor = bio->bio_idx;
	lhash_put(obj_hd, there_ref, &there);
	mobs_add(there_ref, bio->bio_idx, bio->pd.n);
        stones_add(there_ref, bio, pos);
	plants_add(there_ref, bio, pos);
	return there_ref;
}

void
do_bio(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);
	struct bio *bio;
	pos_t pos;
	OBJ player;

	lhash_get(obj_hd, &player, player_ref);
	map_where(pos, player.location);
	bio = noise_point(pos);
	SKEL biome;
	lhash_get(skel_hd, &biome, biome_refs[bio->bio_idx]);
	nd_writef(player_ref, "tmp %d rn %u bio %s(%d)\n",
		bio->tmp, bio->rn, biome.name, bio->bio_idx);
}

static unsigned
st_room(unsigned location, enum exit e)
{
	pos_t pos;
	st_pos(pos, location, e);
	return st_room_at(pos);
}

static unsigned
e_move(unsigned player_ref, enum exit e) {
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	ENT eplayer = ent_get(player_ref);
	OBJ loc;
	lhash_get(obj_hd, &loc, player.location);
	unsigned dest_ref;
	ROO *rloc = &loc.sp.room;
	char const *dwts = "door";
	int door = 0;

	if (eplayer.klock) {
		nd_writef(player_ref, "You can't move while being targeted.\n");
		return NOTHING;
	}

	if (!map_has(player.location) || !(rloc->exits & e)) {
		nd_writef(player_ref, "You can't go that way.\n");
		return NOTHING;
	}

	stand_silent(player_ref, &eplayer);
	ent_set(player_ref, &eplayer);

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
		dest_ref = st_room(player.location, e);

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

	lhash_get(obj_hd, &here, here_ref);

	if (!(here.sp.room.flags & RF_TEMP))
		return here_ref;

	struct hash_cursor c = fhash_iter(contents_hd, here_ref);
	while (ahash_next(&tmp_ref, &c)) {
		OBJ tmp;
		lhash_get(obj_hd, &tmp, tmp_ref);

		if (tmp.type != TYPE_ENTITY)
			continue;

		if (ent_get(tmp_ref).flags & EF_PLAYER) {
			hash_fin(&c);
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
	lhash_get(obj_hd, &player, player_ref);
	unsigned here_ref = player.location, there_ref = here_ref;
	OBJ here;
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;
	int wall = 0;

	if (!e_ground(here_ref, e)) {
		if (st_claim(player_ref, &here))
			return;

		rhere->exits |= e;
		lhash_put(obj_hd, here_ref, &here);

		there_ref = st_there(here_ref, e);
		if (there_ref == NOTHING)
			there_ref = st_room(here_ref, e);
		wall = 1;
	}

	there_ref = e_move(player_ref, e);
	if (there_ref == NOTHING)
		return;

	OBJ there;
	lhash_get(obj_hd, &there, there_ref);
	st_claim(player_ref, &there); // FIXME check success in advance

	if (wall) {
		ROO *rthere = &there.sp.room;
		const char *s;
		for (s = e_other(e_simm(e)); *s; s++) {
			enum exit e2 = dir_e(*s);

			if ((rthere->exits & e2) &&
					st_there(there_ref, e2) != NOTHING)

				rthere->exits ^= e2;
		}
	}

	lhash_put(obj_hd, there_ref, &there);
}

static void
uncarve(unsigned player_ref, enum exit e)
{
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	const char *s0 = "is";
	unsigned here_ref = player.location, there_ref;
	OBJ here;
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;
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
	lhash_get(obj_hd, &there, there_ref);
	ROO *rthere = &there.sp.room;

	if ((rthere->flags & RF_TEMP) || there.owner != player_ref) {
		nd_writef(player_ref, "You don't own th%s room.\n", s0);
		return;
	}

	rthere->flags ^= RF_TEMP;
	exits_infer(there_ref, rthere);
	if (cd) {
		if (ht && (rthere->doors & e_simm(e)))
			rthere->doors &= ~e_simm(e);
		lhash_put(obj_hd, there_ref, &there);
	} else
		room_clean(there_ref);

	lhash_get(obj_hd, &player, player_ref);
	player.value += ROOM_COST;
	nd_writef(player_ref, "You collect your materials. (+%dp)\n", ROOM_COST);
	lhash_put(obj_hd, player_ref, &player);
}

static void
unwall(unsigned player_ref, enum exit e)
{
	int a, b, c, d;
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	unsigned there_ref, here_ref = player.location;
	OBJ here;
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;

	a = here.owner == player_ref;
	b = rhere->flags & RF_TEMP;
	there_ref = st_there(here_ref, e);

	OBJ there;
	lhash_get(obj_hd, &there, there_ref);
	ROO *rthere = &there.sp.room;

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

	lhash_get(obj_hd, &there, there_ref);
	rthere = &there.sp.room;
	rthere->exits |= e_simm(e);
	lhash_put(obj_hd, there_ref, &there);
	nd_writef(player_ref, "You tear down the wall.\n");
}

static inline int
gexit_claim(unsigned player_ref, enum exit e)
{
	int a, b, c, d;
	OBJ player, here, there;
	lhash_get(obj_hd, &player, player_ref);
	unsigned here_ref = player.location,
	      there_ref = st_there(player.location, e);
	lhash_get(obj_hd, &here, here_ref);
	lhash_get(obj_hd, &there, there_ref);
	ROO *rthere = &there.sp.room;

	a = here_ref != NOTHING && here.owner == player_ref;
	c = rthere->flags & RF_TEMP;
	b = !c && there.owner == player_ref;
	d = e_ground(here_ref, e);

	if (a && (b || c))
		return 0;

	if (here_ref != NOTHING || (here.sp.room.flags & RF_TEMP)) {
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
	lhash_get(obj_hd, &player, player_ref);
	unsigned here_ref = player.location;
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;

	if (!(rhere->exits & e)) {
		nd_writef(player_ref, "No exit here.\n");
		return 1;
	}

	unsigned there_ref = e_move(player_ref, e);
	if (there_ref == NOTHING)
		return 1;

	player.location = there_ref;
	lhash_put(obj_hd, player_ref, &player);

	return gexit_claim(player_ref, e_simm(e));
}

static void
e_wall(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, here;
	lhash_get(obj_hd, &player, player_ref);
	unsigned here_ref = player.location;
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;

	rhere->exits &= ~e_simm(e);
	lhash_put(obj_hd, here_ref, &here);

	unsigned there_ref = st_there(here_ref, e_simm(e));

	if (there_ref != NOTHING) {
		OBJ there;
		lhash_get(obj_hd, &there, there_ref);
		ROO *rthere = &there.sp.room;
		rthere->exits &= ~e;
		lhash_put(obj_hd, there_ref, &there);
	}

	nd_writef(player_ref, "You build a wall.\n");
}

static void
door(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, where;
	lhash_get(obj_hd, &player, player_ref);
	unsigned where_ref = player.location;
	lhash_get(obj_hd, &where, where_ref);
	ROO *rwhere = &where.sp.room;
	rwhere->doors |= e_simm(e);
	lhash_put(obj_hd, where_ref, &where);

	where_ref = st_there(where_ref, e_simm(e));

	if (where_ref == NOTHING) {
		lhash_get(obj_hd, &where, where_ref);
		rwhere = &where.sp.room;
		rwhere->doors |= e;
		lhash_put(obj_hd, where_ref, &where);
	}

	nd_writef(player_ref, "You place a door.\n");
}

static void
undoor(unsigned player_ref, enum exit e)
{
	if (gexit_claim_walk(player_ref, e))
		return;

	OBJ player, where;
	lhash_get(obj_hd, &player, player_ref);
	unsigned where_ref = player.location;
	lhash_get(obj_hd, &where, where_ref);
	ROO *rwhere = &where.sp.room;
	rwhere->doors &= ~e_simm(e);
	lhash_put(obj_hd, where_ref, &where);

	where_ref = st_there(where_ref, e_simm(e));

	if (where_ref != NOTHING) {
		lhash_get(obj_hd, &where, where_ref);
		rwhere = &where.sp.room;
		rwhere->doors &= ~e;
		lhash_put(obj_hd, where_ref, &where);
	}

	nd_writef(player_ref, "You remove a door.\n");
}

static int
tell_pos(unsigned player_ref, struct cmd_dir cd) {
	pos_t pos;
	unsigned target_ref = cd.rep == 1 ? player_ref : cd.rep;
	OBJ target;
	lhash_get(obj_hd, &target, target_ref);
	int ret = 1;

	if (target.type != TYPE_ENTITY) {
		nd_writef(player_ref, "Invalid object type.\n");
		return 0;
	}

	map_where(pos, target.location);
	nd_writef(player_ref, "0x%llx\n", MORTON_READ(pos));
	return ret;
}

int
st_teleport(unsigned player_ref, struct cmd_dir cd)
{
	pos_t pos;
	int ret = 0;
	if (cd.rep == 1)
		cd.rep = 0;
	if (cd.dir == '?') {
		// X6? teleport to chivas
		unsigned target_ref = cd.rep;
		OBJ target;
		lhash_get(obj_hd, &target, target_ref);
		if (target.type == TYPE_ENTITY) {
			map_where(pos, target.location);
			ret = 1;
		}
	} else
		memcpy(pos, &cd.rep, sizeof(cd.rep));

	unsigned there_ref = map_get(pos);
	if (there_ref == NOTHING)
		there_ref = st_room_at(pos);

	/* if (!eplayer->gpt) */
	nd_writef(player_ref, "Teleporting to 0x%llx.\n", cd.rep);
	enter(player_ref, there_ref, E_NULL);
	return ret;
}

void st_start(unsigned player_ref) {
	struct cmd_dir cmd_dir = { .rep = 0 };
	st_teleport(player_ref, cmd_dir);
}

static int
pull(unsigned player_ref, struct cmd_dir cd)
{
	pos_t pos;
	enum exit e = cd.e;
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	unsigned here_ref = player.location, there_ref;

	if (e == E_NULL)
		return 0;

	unsigned what_ref = cd.rep;

	if (what_ref == NOTHING) {
		nd_writef(player_ref, "You cannot do that.\n");
		return 1;
	}

	OBJ what, here;
	lhash_get(obj_hd, &what, what_ref);
	lhash_get(obj_hd, &here, here_ref);
	ROO *rhere = &here.sp.room;

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

static void may_look(unsigned player_ref, morton_t old_loc) {
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	morton_t new_loc = map_mwhere(player.location);
	if (old_loc == new_loc)
		return;
	look_around(player_ref);
	view(player_ref);
}

int
st_v(unsigned player_ref, char const *opcs)
{
	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	morton_t old_loc = map_mwhere(player.location);
	struct cmd_dir cd;
	char const *s = opcs;

	env->txn_begin(env, NULL, &txnid, 0);

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
				if (txnid)
					txnid->commit(txnid, 0);
				txnid = NULL;
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
	if (txnid)
		txnid->commit(txnid, 0);
	txnid = NULL;
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
		*ind = nd;
	}

	hash_put(sl_hd, &st_key, sizeof(st_key), &sl, sizeof(void *));
	hash_put(owner_hd, &st_key, sizeof(st_key), &owner, sizeof(owner));
}

struct st_key
st_key_new(uint64_t key, unsigned shift) {
	struct st_key st_key;
	st_key.key = key >> shift;
	st_key.shift = shift;
	return st_key;
}

void
st_put(unsigned owner_ref, uint64_t key, unsigned shift) {
	char buf[BUFSIZ];
	struct st_key st_key = st_key_new(key, shift);
	size_t len = snprintf(buf, sizeof(buf), "var/nd/st/%u/", shift);
	mkdir(buf, 0750);
	snprintf(buf + len, sizeof(buf) - len, "%llu", key >> shift);
	mkdir(buf, 0750);
	OBJ owner;
	lhash_get(obj_hd, &owner, owner_ref);
	struct passwd *pw = getpwnam(owner.name);
	chown(buf, pw->pw_uid, pw->pw_gid);
	st_open(st_key, owner_ref);
}

void
st_init(void) {
	SKEL skel = {
		.name = "stone",
		.description = "Solid stone(s)",
		.type = STYPE_MINERAL,
		.sp = {
			.mineral = { 0 } 
		},
	};

	stone_skel_id = lhash_new(skel_hd, &skel);

	/* st_put(1, 0, 64); */

	nd.fds_hd = fds_hd;
	/* nd.fds_has = fds_has; */
	nd.nd_close = nd_close;
	nd.nd_write = nd_write;
	nd.nd_dwritef = nd_dwritef;
	nd.nd_rwrite = nd_rwrite;
	nd.nd_dowritef = nd_dowritef;
	nd.dnotify_wts = dnotify_wts;
	nd.dnotify_wts_to = dnotify_wts_to;
	nd.notify_attack = notify_attack;
	nd.nd_tdwritef = nd_tdwritef;
	nd.nd_wwrite = nd_wwrite;

	nd.map_has = map_has;
	nd.map_mwhere = map_mwhere;
	nd.map_where = map_where;
	nd.map_delete = map_delete;
	nd.map_get = map_get;

	nd.st_teleport = st_teleport;
	nd.st_run = st_run;

	nd.wts_plural = wts_plural;

	nd.skel_hd = skel_hd;
	nd.drop_hd = drop_hd;
	nd.adrop_hd = adrop_hd;

	nd.obj_hd = obj_hd;
	nd.obj_exists = obj_exists;
	nd.object_new = object_new;
	nd.object_copy = object_copy;
	nd.object_move = object_move;
	nd.object_add = object_add;
	nd.object_drop = object_drop;

	nd.obs_hd = obs_hd;

	nd.contents_hd = contents_hd;

	nd.object_icon = object_icon;
	nd.art_max = art_max;
	nd.object_art = object_art;
	nd.unparse = unparse;

	nd.me_get = me_get;
	nd.ent_get = ent_get;
	nd.ent_set = ent_set;
	nd.ent_del = ent_del;
	nd.ent_reset = ent_reset;
	nd.birth = birth;
	nd.sit = sit;
	nd.stand_silent = stand_silent;
	nd.stand = stand;
	nd.controls = controls;
	nd.payfor = payfor;
	nd.look_around = look_around;
	nd.equip_affect = equip_affect;
	nd.equip = equip;
	nd.unequip = unequip;
	nd.mask_element = mask_element;
	nd.entity_damage = entity_damage;
	nd.enter = enter;
	nd.kill_dodge = kill_dodge;
	nd.kill_dmg = kill_dmg;
	nd.spell_cast = spell_cast;
	nd.debufs_end = debufs_end;

	struct hash_cursor c = hash_iter(owner_hd, NULL, 0);
	struct st_key st_key;
	int owner;

	while (hash_next(&st_key, &owner, &c))
		st_open(st_key, owner);
}

void st_dlclose(void) {
	struct hash_cursor c = hash_iter(sl_hd, NULL, 0);
	struct st_key key;
	void *sl;

	while (hash_next(&key, &sl, &c))
		dlclose(sl);
}

typedef void (*st_run_cb)(unsigned player_ref);

int
st_get(uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (hash_get(owner_hd, &owner, &st_key, sizeof(st_key)))
		return -1;

	return owner;
}

inline static int
_st_can(int ref, uint64_t key, unsigned shift) {
	struct st_key st_key = st_key_new(key, shift);
	int owner;

	if (hash_get(owner_hd, &owner, &st_key, sizeof(st_key)))
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

	if (hash_get(sl_hd, &sl, &st_key, sizeof(st_key)))
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
	lhash_get(obj_hd, &player, player_ref);
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
	lhash_get(obj_hd, &who, who_ref);

	if (who.type != TYPE_ENTITY || !(ent_get(who_ref).flags & EF_PLAYER)) {
		nd_writef(player_ref, "Invalid target\n");
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

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
	lhash_get(obj_hd, &player, player_ref);

	uint64_t position = argc > 2
		? strtoull(argv[2], NULL, 10)
		: map_mwhere(player.location);

	long int high_shift = st_high_shift(player_ref, position);

	unsigned shift = argc < 2 ? high_shift : strtoul(argv[1], NULL, 10);

	long long unsigned key = shift > 63 ? 0 : position;

	struct st_key st_key = st_key_new(key, shift);
	unsigned owner;
	void *sl;

	if (hash_get(owner_hd, &owner, &st_key, sizeof(st_key)) || owner != player_ref)
	{
		nd_writef(player_ref, "Permission denied\n");
		return;
	}

	if (hash_get(sl_hd, &sl, &st_key, sizeof(st_key))) {
		nd_writef(player_ref, "Not open\n");
		return;
	}

	dlclose(sl);
	st_open(st_key, owner);
}

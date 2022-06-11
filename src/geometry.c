#include "geometry.h"
#include "mdb.h"
#include "props.h"
#include "externs.h"
#include "search.h"
#include "web.h"

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

/* https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN */

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
object_add(struct object_skeleton sk, OBJ *where)
{
	OBJ *nu = object_new();
	nu->name = alloc_string(sk.name);
	nu->description = alloc_string(sk.description);
	nu->art = alloc_string(sk.art);
	nu->avatar = alloc_string(sk.avatar);
	nu->location = where;
	nu->owner = object_get(GOD);
	nu->type = TYPE_THING;
	if (where)
		PUSH(nu, where->contents);

	switch (sk.type) {
	case S_TYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = sk.sp.equipment.eqw;
			enu->msv = sk.sp.equipment.msv;
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
			cnu->food = sk.sp.consumable.food;
			cnu->drink = sk.sp.consumable.drink;
		}

		break;
	case S_TYPE_ENTITY:
		{
			ENT *enu = &nu->sp.entity;
			nu->type = TYPE_ENTITY;
			mob_add_stats(&sk, nu);
			enu->flags = sk.sp.entity.flags;
			enu->wtso = sk.sp.entity.wt;
			birth(nu);
			object_drop(nu, sk.sp.entity.drop);
			enu->home = where;
		}

		break;
	case S_TYPE_PLANT:
		nu->type = TYPE_PLANT;
		object_drop(nu, sk.sp.plant.drop);
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

	if (sk.type != S_TYPE_BIOME)
		web_content_in(where, nu);

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
                                object_add(*(*drop)->i, where);
                                continue;
                        }

                        yield += random() & yield_v;

                        for (i = 0; i < yield; i++)
                                object_add(*(*drop)->i, where);
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

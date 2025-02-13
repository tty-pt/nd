#include "uapi/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qhash.h>

#include "config.h"
#include "mcp.h"
#include "noise.h"
#include "params.h"
#include "player.h"
#include "nddb.h"
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/map.h"

enum actions {
        ACT_LOOK = 1,
        ACT_KILL = 2,
        ACT_SHOP = 4,
        ACT_CONSUME = 8,
        ACT_OPEN = 16,
        ACT_CHOP = 32,
        ACT_FILL = 64,
        ACT_GET = 128,
        ACT_TALK = 256,
};

struct object room_zero = {
	.location = NOTHING,
	.owner = 1,
	.art_id = 0,
	.type = TYPE_ROOM,
	.value = 9999999,
	.flags = 0,
	.sp.room = {
		.flags = RF_HAVEN,
		.doors = 0,
		.exits = 0,
		.floor = 0,
	},
};

char std_db[BUFSIZ];
char std_db_ok[BUFSIZ];
unsigned obj_hd, contents_hd, obs_hd;

int obj_exists(unsigned ref) {
	return uhash_exists(obj_hd, ref);
}

unsigned
object_new(OBJ *newobj)
{
	memset(newobj, 0, sizeof(OBJ));
	newobj->location = NOTHING;
	newobj->owner = ROOT;
	return lhash_new(obj_hd, newobj);
}

static inline int
rarity_get(void) {
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

ucoord_t biome_rain_floor[16] = {
	0, 0, 0, 0,
	128, 128, 128, 128,
	256, 256, 256, 256,
	384, 384, 384, 384,
};

ucoord_t biome_rain_ceil[16] = {
	128, 128, 128, 128,
	256, 256, 256, 256,
	384, 384, 384, 384,
	512, 512, 512, 512,    
};

coord_t biome_temp_floor[16] = {
	-41, -41, -41, -41,
	11, 11, 11, 11,
	64, 64, 64, 64,
	117, 117, 117, 117,
};

coord_t biome_temp_ceil[16] = {
	11, 11, 11, 11,
	64, 64, 64, 64,
	117, 117, 117, 117,
	170, 170, 170, 170,
};

unsigned _bio_idx(coord_t tmp_f, coord_t tmp_c, ucoord_t rain_f, ucoord_t rain_c, coord_t tmp, ucoord_t rain);

static inline unsigned
biome_art_idx(struct bio *bio) {
	return 1 + _bio_idx(biome_rain_floor[bio->bio_idx],
		biome_rain_ceil[bio->bio_idx],
		biome_temp_floor[bio->bio_idx],
		biome_temp_ceil[bio->bio_idx],
		bio->rn,
		bio->tmp);
}

struct core_art {
	unsigned max;
	char *name;
};

struct core_art core_art[] = {
	// ENTITIES
	{ 1, "dolphin" },
	{ 6, "eagle" },
	{ 5, "firebird" },
	{ 2, "goldfish" },
	{ 1, "icebird" },
	{ 3, "koifish" },
	{ 9, "owl" },
	{ 8, "parrot" },
	{ 6, "rainbowfish" },
	{ 6, "salmon" },
	{ 7, "shark" },
	{ 15, "sparrow" },
	{ 16, "starling" },
	{ 9, "swallow" },
	{ 13, "tuna" },
	{ 2, "vulture" },
	{ 6, "wolf" },
	{ 7, "woodpecker" },
	{ 300, "avatar" },

	// PLANTS
	{ 10, "abies alba" },
	{ 12, "acacia senegal" },
	{ 14, "betula pendula" },
	{ 6, "daucus carota" },
	{ 19, "pinus sylvestris" },
	{ 21, "pseudotsuga menziesii" },
	{ 9, "arthrocereus rondonianus" },
	{ 11, "solanum lycopersicum" },
	{ 7, "linum usitatissimum" },

	// MINERALS
	{ 17, "stone" },

	// OTHER
	{ 14, "stick" },
};

unsigned art_hd = -1;

unsigned art_max(char *name) {
	unsigned max;
	shash_get(art_hd, &max, name);
	return max;
}

static inline unsigned
art_idx(OBJ *obj) {
	return 1 + (random() % art_max(obj->name));
}

void stats_init(ENT *enu, SENT *sk);

unsigned
object_add(OBJ *nu, unsigned skel_id, unsigned where_ref, void *arg)
{
	SKEL skel;
	lhash_get(skel_hd, &skel, skel_id);
	unsigned nu_ref = object_new(nu);
	strcpy(nu->name, skel.name);
	nu->location = where_ref;
	nu->owner = ROOT;
	nu->type = TYPE_THING;
	if (where_ref != NOTHING)
		ahash_add(contents_hd, nu->location, nu_ref);

	switch (skel.type) {
	case STYPE_SPELL:
		break;
	case STYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = skel.sp.equipment.eqw;
			enu->msv = skel.sp.equipment.msv;
			enu->rare = rarity_get();
			equip(where_ref, nu_ref);
		}

		break;
	case STYPE_CONSUMABLE:
		{
			CON *cnu = &nu->sp.consumable;
			nu->type = TYPE_CONSUMABLE;
			cnu->food = skel.sp.consumable.food;
			cnu->drink = skel.sp.consumable.drink;
		}

		break;
	case STYPE_ENTITY:
		{
			ENT ent;
			memset(&ent, 0, sizeof(ent));
			nu->type = TYPE_ENTITY;
			stats_init(&ent, &skel.sp.entity);
			ent.flags = skel.sp.entity.flags;
			ent.wtso = skel.sp.entity.wt;
			ent.sat = NOTHING;
			ent.last_observed = NOTHING;
			birth(&ent);
			object_drop(nu_ref, skel_id);
			ent.home = where_ref;
			ent_set(nu_ref, &ent);
			nu->art_id = art_idx(nu);
		}

		break;
	case STYPE_PLANT:
		{
			noise_t v = * (noise_t *) arg;
			nu->type = TYPE_PLANT;
			object_drop(nu_ref, skel_id);
			nu->owner = ROOT;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}

		break;
        case STYPE_BIOME:
		{
			struct bio *bio = arg;
			ROO *rnu = &nu->sp.room;
			nu->type = TYPE_ROOM;
			rnu->exits = rnu->doors = 0;
			rnu->flags = RF_TEMP;
			nu->art_id = biome_art_idx(bio);
		}

		break;
	case STYPE_MINERAL:
		{
			noise_t v = * (noise_t *) arg;
			nu->type = TYPE_MINERAL;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}

		break;

	case STYPE_OTHER:
		if (arg) {
			noise_t v = * (noise_t *) arg;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}
		nu->type = TYPE_THING;

		break;
	}

	lhash_put(obj_hd, nu_ref, nu);
	if (skel.type != STYPE_BIOME)
		mcp_content_in(where_ref, nu_ref);

	return nu_ref;
}

char *
object_art(unsigned thing_ref)
{
	OBJ thing;
	lhash_get(obj_hd, &thing, thing_ref);
	static char art[BUFSIZ];
	char *type = NULL;

	switch (thing.type) {
	case TYPE_ENTITY:
		type = "entity";
		snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.art_id && ent_get(thing_ref).flags & EF_PLAYER ? "avatar" : thing.name, thing.art_id);
		return art;
	case TYPE_ROOM: type = "biome"; break;
	case TYPE_PLANT: type = "plant"; break;
	case TYPE_MINERAL: type = "mineral"; break;
	default: type = "other"; break;
	}

	snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.name, thing.art_id);
	return art;
}

void
objects_init(void)
{
	unsigned ref;

	srand(getpid());

	size_t hash_i = 0;
	for (; hash_i < sizeof(core_art) / sizeof(struct core_art); hash_i++) {
		struct core_art *art = &core_art[hash_i];
		suhash_put(art_hd, art->name, art->max);
	}

	struct hash_cursor c = lhash_iter(obj_hd);
	OBJ oi;
	while (lhash_next(&ref, &oi, &c)) {
		ahash_add(contents_hd, oi.location, ref);
		if (oi.type != TYPE_ENTITY)
			continue;

		ENT ent = ent_get(ref);
		if (ent.flags & EF_PLAYER)
			player_put(oi.name, ref);
	}

	if (!uhash_exists(obj_hd, 0))
		lhash_new(obj_hd, &room_zero);
}

unsigned
object_copy(OBJ *nu, unsigned old_ref)
{
	OBJ old;
	lhash_get(obj_hd, &old, old_ref);
	unsigned ref = object_new(nu);
	strcpy(nu->name, old.name);
	nu->location = NOTHING;
	nu->owner = old.owner;
        return ref;
}

void entity_update(unsigned player_ref, double dt);

void
objects_update(double dt)
{
	OBJ obj;
	unsigned obj_ref;
	struct hash_cursor c = lhash_iter(obj_hd);
	while (lhash_next(&obj_ref, &obj, &c))
		if (!*obj.name)
			continue;
		else if (obj.type == TYPE_ENTITY)
			entity_update(obj_ref, dt);
}

void
object_move(unsigned what_ref, unsigned where_ref)
{
	OBJ what;
	lhash_get(obj_hd, &what, what_ref);

        if (what.location != NOTHING) {
                mcp_content_out(what.location, what_ref);
		ahash_remove(contents_hd, what.location, what_ref);
        }

	struct hash_cursor c = fhash_iter(obs_hd, what_ref);
	unsigned first_ref;
	while (ahash_next(&first_ref, &c)) {
		ENT efirst = ent_get(first_ref);
		efirst.last_observed = what.location;
		ahash_remove(obs_hd, what_ref, first_ref);
		ent_set(first_ref, &efirst);
	}

	/* test for special cases */
	if (where_ref == NOTHING) {
		unsigned first_ref;

		struct hash_cursor c = fhash_iter(contents_hd, what_ref);
		while (ahash_next(&first_ref, &c))
			object_move(first_ref, NOTHING);

		switch (what.type) {
		case TYPE_ENTITY:
			ent_del(what_ref);
			break;
		case TYPE_ROOM:
			map_delete(what_ref);
		}
		lhash_del(obj_hd, what_ref);
		return;
	}

	what.location = where_ref;
	lhash_put(obj_hd, what_ref, &what);

	if (what.type == TYPE_ENTITY) {
		ENT ewhat = ent_get(what_ref);
		if (ewhat.last_observed != NOTHING)
			ahash_remove(obs_hd, ewhat.last_observed, what_ref);
		if ((ewhat.flags & EF_SITTING)) {
			stand(what_ref, &ewhat);
			ent_set(what_ref, &ewhat);
		}
	}

	ahash_add(contents_hd, where_ref, what_ref);
	mcp_content_in(where_ref, what_ref);
}

struct icon
object_icon(unsigned what_ref)
{
	OBJ what;
	lhash_get(obj_hd, &what, what_ref);

        static char buf[BUFSIZ];
        struct icon ret = {
                .actions = ACT_LOOK,
                .icon = ANSI_RESET ANSI_BOLD "?",
        };
        unsigned aux;
        switch (what.type) {
        case TYPE_ROOM:
                ret.icon = ANSI_FG_YELLOW "-";
                break;
        case TYPE_ENTITY:
		ret.actions |= ACT_KILL;
		ret.icon = ANSI_BOLD ANSI_FG_YELLOW "!";
		if (ent_get(what_ref).flags & EF_SHOP) {
			ret.actions |= ACT_SHOP;
			ret.icon = ANSI_BOLD ANSI_FG_GREEN "$";
		}
                break;
	case TYPE_CONSUMABLE:
		{
			CON *cwhat = &what.sp.consumable;
			if (cwhat->drink) {
				ret.actions |= ACT_FILL;
				ret.icon = ANSI_BOLD ANSI_FG_BLUE "~";
			} else {
				ret.icon = ANSI_BOLD ANSI_FG_RED "o";
			}
			ret.actions |= ACT_CONSUME;
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pwhat = &what.sp.plant;
			aux = pwhat->plid;
			SKEL skel;
			lhash_get(skel_hd, &skel, aux);
			SPLA *pl = &skel.sp.plant;

			ret.actions |= ACT_CHOP;
			snprintf(buf, sizeof(buf), "%s%c%s", pl->pre,
					pwhat->size > PLANT_HALF ? pl->big : pl->small,
					pl->post); 

			// use the icon immediately
			ret.icon = buf;
		}
		break;
	default:
                ret.actions |= ACT_GET;
                break;
        }
        return ret;
}

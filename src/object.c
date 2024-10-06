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
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/map.h"

#define STD_DB "/var/nd/std.db"
#define STD_DB_OK "/var/nd/std.db.ok"

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

struct object *db = 0;
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
	newobj->name = NULL;
	newobj->location = NOTHING;
	newobj->owner = ROOT;
	return lhash_new(obj_hd, newobj);
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
	nu->name = strdup(skel.name);
	nu->description = strdup(skel.description);
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
putref(FILE * f, unsigned ref)
{
	if (fprintf(f, "%u\n", ref) < 0) {
		abort();
	}
}

static void
putstring(FILE * f, const char *s)
{
	if (s) {
		if (fputs(s, f) == EOF) {
			abort();
		}
	}
	if (putc('\n', f) == EOF) {
		abort();
	}
}

static void
object_write(FILE * f, unsigned ref)
{
	int j;
	OBJ obj;
	lhash_get(obj_hd, &obj, ref);
	putstring(f, obj.name);
	putref(f, obj.location);
	putref(f, NOTHING);
	putref(f, NOTHING);
	putref(f, obj.value);
	putref(f, obj.type);
	putref(f, obj.flags);
	putref(f, obj.art_id);

	switch (obj.type) {
	case TYPE_ENTITY:
		{
			ENT ent = ent_get(ref);
			putref(f, ent.home);
			putref(f, ent.flags);
			putref(f, ent.lvl);
			putref(f, ent.cxp);
			putref(f, ent.spend);
			for (j = 0; j < ATTR_MAX; j++)
				putref(f, ent.attr[j]);
			putref(f, ent.wtso);
			putref(f, ent.sat);
			for (j = 0; j < 8; j++)
				putref(f, ent.spells[j].skel);
		}
		break;

	case TYPE_PLANT:
		{
			PLA *pla = &obj.sp.plant;
			putref(f, pla->plid);
			putref(f, pla->size);
		}
		break;

	case TYPE_SEAT:
		{
			SEA *sea = &obj.sp.seat;
			putref(f, sea->quantity);
			putref(f, sea->capacity);
		}
		break;

	case TYPE_CONSUMABLE:
		{
			CON *con = &obj.sp.consumable;
			putref(f, con->food);
			putref(f, con->drink);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *equ = &obj.sp.equipment;
			putref(f, equ->eqw);
			putref(f, equ->msv);
		}
		break;
	case TYPE_THING:
		putref(f, obj.owner);
		break;

	case TYPE_ROOM:
		{
			ROO *roo = &obj.sp.room;
			putref(f, NOTHING);
			putref(f, roo->flags);
			putref(f, roo->exits);
			putref(f, roo->doors);
			putref(f, roo->floor);
			putref(f, obj.owner);
		}
		break;
	}
}

static inline void
db_write_list(FILE * f)
{
	struct hash_cursor c = lhash_iter(obj_hd);
	unsigned ref;
	OBJ oi;

	while (hash_next(&ref, &oi, &c)) {
		if (fprintf(f, "#%u\n", ref) < 0)
			abort();
		object_write(f, ref);
	}
}

static unsigned objects_read(FILE *f);

int
objects_init()
{
	obj_hd = lhash_init(sizeof(OBJ));
	contents_hd = ahash_init();
	obs_hd = ahash_init();
	art_hd = hash_init();

	snprintf(std_db, sizeof(std_db), "%s%s", euid ? nd_config.chroot : "", STD_DB);
	snprintf(std_db_ok, sizeof(std_db_ok), "%s%s", euid ? nd_config.chroot : "", STD_DB_OK);
	const char *name = std_db;
	FILE *f;

	if (access(std_db, F_OK) != 0)
		name = std_db_ok;

	f = fopen(name, "rb");

	if (f == NULL)
                return -1;

	srand(getpid());			/* init random number generator */

	objects_read(f);
	unsigned ref;
	struct hash_cursor c = lhash_iter(obj_hd);
	OBJ oi;
	while (hash_next(&ref, &oi, &c))
		ahash_add(contents_hd, oi.location, ref);

	return 0;
}

void
objects_sync()
{
	FILE *f = fopen(std_db, "wb");

	if (f == NULL) {
		perror("fopen");
		return;
	}

	putref(f, 3);

	db_write_list(f);

	fseek(f, 0L, 2);
	putstring(f, "***END OF DUMP***");

	fflush(f);
	fclose(f);
}

#define STRING_READ(x) strdup(string_read(x))

unsigned
ref_read(FILE * f)
{
	char buf[BUFSIZ];
	fgets(buf, sizeof(buf), f);
	return (atol(buf));
}

static const char *
string_read(FILE * f)
{
	static char xyzzybuf[BUFSIZ];
	char *p;
	char c;

	if (fgets(xyzzybuf, sizeof(xyzzybuf), f) == NULL) {
		xyzzybuf[0] = '\0';
		return xyzzybuf;
	}

	if (strlen(xyzzybuf) == BUFSIZ - 1) {
		/* ignore whatever comes after */
		if (xyzzybuf[BUFSIZ - 2] != '\n')
			while ((c = fgetc(f)) != '\n') ;
	}
	for (p = xyzzybuf; *p; p++) {
		if (*p == '\n') {
			*p = '\0';
			break;
		}
	}

	return xyzzybuf;
}

static void
object_read(FILE * f)
{
	OBJ o;
	memset(&o, 0, sizeof(struct object));
	unsigned ref = lhash_new(obj_hd, &o);
	int j = 0;

	ref_read(f);

	o.owner = ROOT;
	o.name = STRING_READ(f);
	o.location = ref_read(f);
	ref_read(f);
	ref_read(f);
	o.value = ref_read(f);
	o.type = ref_read(f);
	o.flags = ref_read(f);
	o.art_id = ref_read(f);

	switch (o.type) {
	case TYPE_PLANT:
		{
			PLA *po = &o.sp.plant;
			po->plid = ref_read(f);
			po->size = ref_read(f);
		}
		break;
	case TYPE_SEAT:
		{
			SEA *ps = &o.sp.seat;
			ps->quantity = ref_read(f);
			ps->capacity = ref_read(f);
		}
		break;
	case TYPE_CONSUMABLE:
		{
			CON *co = &o.sp.consumable;
			co->food = ref_read(f);
			co->drink = ref_read(f);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *eo = &o.sp.equipment;
			eo->eqw = ref_read(f);
			eo->msv = ref_read(f);
		}
		break;
	case TYPE_THING:
		o.owner = ref_read(f);
		break;
	case TYPE_ROOM:
		{
			ROO *ro = &o.sp.room;
			ref_read(f);
			ro->flags = ref_read(f);
			ro->exits = ref_read(f);
			ro->doors = ref_read(f);
			ro->floor = ref_read(f);
			o.owner = ref_read(f);
		}
		break;
	case TYPE_ENTITY:
		{
			ENT ent;
			memset(&ent, 0, sizeof(ent));
			ent.home = ref_read(f);
			ent_reset(&ent);
			ent.flags = ref_read(f);
			ent.lvl = ref_read(f);
			ent.cxp = ref_read(f);
			ent.spend = ref_read(f);
			for (j = 0; j < ATTR_MAX; j++)
				ent.attr[j] = ref_read(f);
			ent.wtso = ref_read(f);
			ent.sat = ref_read(f);
			for (j = 0; j < 8; j++) {
				struct spell *sp = &ent.spells[j];
				sp->skel = ref_read(f);
			}
			o.owner = ref;
			birth(&ent);
			if (ent.flags & EF_PLAYER)
				player_put(o.name, ref);
			ent_set(ref, &ent);
		}
	}

	lhash_put(obj_hd, ref, &o);
}

static unsigned
objects_read(FILE * f)
{
	const char *special;
	char c;

	int hash_i = 0;
	for (; hash_i < sizeof(core_art) / sizeof(struct core_art); hash_i++) {
		struct core_art *art = &core_art[hash_i];
		suhash_put(art_hd, art->name, art->max);
	}

	/* Parse the header */
	ref_read(f);

	c = getc(f);			/* get next char */
	for (;;) {
		switch (c) {
		case NUMBER_TOKEN:
			/* another entry, yawn */
			/* read it in */
			object_read(f);
			break;
		case '*':
			special = STRING_READ(f);
			if (strcmp(special, "**END OF DUMP***")) {
				free((void *) special);
				return NOTHING;
			} else {
				free((void *) special);
				special = STRING_READ(f);
				if (special)
					free((void *) special);
				return 1;
			}
			break;
		default:
			return NOTHING;
			/* break; */
		}
		c = getc(f);
	}							/* for */
}								/* db_read */

unsigned
object_copy(OBJ *nu, unsigned old_ref)
{
	OBJ old;
	lhash_get(obj_hd, &old, old_ref);
	unsigned ref = object_new(nu);
	nu->name = strdup(old.name);
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
	while (hash_next(&obj_ref, &obj, &c))
		if (!obj.name)
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
		if (what.name)
			free((void *) what.name);
		if (what.description)
			free((void *) what.description);
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

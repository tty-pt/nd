/* $Header$ */

#include "copyright.h"
#include "config.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/queue.h>

#include "params.h"
#include "defaults.h"

#include "spell.h"
#include "player.h"
#include "debug.h"
#include "plant.h"
#include "noise.h"
#include "mcp.h"
#include "map.h"
#include "io.h"
#include <qhash.h>

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

long obj_hd = -1, observable_hd = -1;
long contents_hd = -1;
struct object *db = 0;
dbref db_top = 0;
char std_db[BUFSIZ];
char std_db_ok[BUFSIZ];

struct free_id {
        dbref value;
        SLIST_ENTRY(free_id) entry;
};

SLIST_HEAD(free_list_head, free_id);
struct free_list_head free_ids;

#ifndef DB_INITIAL_SIZE
#define DB_INITIAL_SIZE 10000
#endif							/* DB_INITIAL_SIZE */

dbref db_size = DB_INITIAL_SIZE;

int obj_exists(dbref ref) {
	return !!hash_get(obj_hd, &ref, sizeof(ref));
}

OBJ obj_get(dbref ref) {
	OBJ obj;
	hash_cget(obj_hd, &obj, &ref, sizeof(ref));
	return obj;
}

void obj_set(dbref ref, OBJ *obj) {
	hash_cput(obj_hd, &ref, sizeof(ref), obj, sizeof(OBJ));
}

struct hash_cursor obj_iter() {
	return hash_iter(obj_hd);
}

dbref obj_next(OBJ *iobj, struct hash_cursor *c) {
	dbref iobj_ref;

	if (hash_next(&iobj_ref, iobj, c) <= 0)
		return NOTHING;
	else
		return iobj_ref;
}

dbref
object_new(OBJ *newobj)
{
	memset(newobj, 0, sizeof(OBJ));
	/* clear it out */
	memset(newobj, 0, sizeof(struct object));
	newobj->name = NULL;
	newobj->location = NOTHING;
	newobj->owner = ROOT;

	struct free_id *new_id = SLIST_FIRST(&free_ids);
	if (new_id) {
		dbref ret = new_id->value;
		SLIST_REMOVE_HEAD(&free_ids, entry);
		obj_set(ret, newobj);
		return ret;
	} else
		return ++db_top;
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

long art_hd = -1;

unsigned art_max(char *name) {
	unsigned max;
	hash_cget(art_hd, &max, name, strlen(name));
	return max;
}

static inline unsigned
art_idx(OBJ *obj) {
	return 1 + (random() % art_max(obj->name));
}

static inline void contents_put(dbref parent, dbref child) {
	hash_cput(contents_hd, &parent, sizeof(parent), &child, sizeof(child));
}

dbref
object_add(OBJ *nu, SKEL *sk, dbref where_ref, void *arg)
{
	dbref nu_ref = object_new(nu);
	nu->name = strdup(sk->name);
	nu->description = strdup(sk->description);
	nu->location = where_ref;
	nu->owner = ROOT;
	nu->type = TYPE_THING;
	if (where_ref != NOTHING)
		contents_put(nu->location, nu_ref);

	switch (sk->type) {
	case S_TYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = sk->sp.equipment.eqw;
			enu->msv = sk->sp.equipment.msv;
			enu->rare = rarity_get();
			CBUG(where_ref == NOTHING || obj_get(where_ref).type != TYPE_ENTITY);
			equip(where_ref, nu_ref);
		}

		break;
	case S_TYPE_CONSUMABLE:
		{
			CON *cnu = &nu->sp.consumable;
			nu->type = TYPE_CONSUMABLE;
			cnu->food = sk->sp.consumable.food;
			cnu->drink = sk->sp.consumable.drink;
		}

		break;
	case S_TYPE_ENTITY:
		{
			ENT ent;
			memset(&ent, 0, sizeof(ent));
			nu->type = TYPE_ENTITY;
			stats_init(&ent, &sk->sp.entity);
			ent.flags = sk->sp.entity.flags;
			ent.wtso = sk->sp.entity.wt;
			ent.sat = NOTHING;
			ent.last_observed = NOTHING;
			birth(&ent);
			spells_birth(&ent);
			object_drop(nu_ref, sk->sp.entity.drop);
			ent.home = where_ref;
			ent_set(nu_ref, &ent);
			nu->art_id = art_idx(nu);
		}

		break;
	case S_TYPE_PLANT:
		{
			noise_t v = * (noise_t *) arg;
			nu->type = TYPE_PLANT;
			object_drop(nu_ref, sk->sp.plant.drop);
			nu->owner = ROOT;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}

		break;
        case S_TYPE_BIOME:
		{
			struct bio *bio = arg;
			ROO *rnu = &nu->sp.room;
			nu->type = TYPE_ROOM;
			rnu->exits = rnu->doors = 0;
			rnu->flags = RF_TEMP;
			nu->art_id = biome_art_idx(bio);
		}

		break;
	case S_TYPE_MINERAL:
		{
			noise_t v = * (noise_t *) arg;
			nu->type = TYPE_MINERAL;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}

		break;

	case S_TYPE_OTHER:
		if (arg) {
			noise_t v = * (noise_t *) arg;
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}
		nu->type = TYPE_THING;

		break;
	}

	obj_set(nu_ref, nu);
	if (sk->type != S_TYPE_BIOME)
		mcp_content_in(where_ref, nu_ref);

	return nu_ref;
}

char *
object_art(dbref thing_ref)
{
	OBJ thing = obj_get(thing_ref);
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
putref(FILE * f, dbref ref)
{
	if (fprintf(f, "%d\n", ref) < 0) {
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
object_write(FILE * f, dbref ref)
{
	int j;
	OBJ obj = obj_get(ref);
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
	dbref ref;

	struct hash_cursor c = obj_iter();
	OBJ oi;
	while ((ref = obj_next(&oi, &c)) != NOTHING) {
		if (fprintf(f, "#%d\n", ref) < 0)
			abort();
		object_write(f, ref);
	}
}

static dbref objects_read(FILE *f);

int
objects_init()
{
	SLIST_INIT(&free_ids);
	obj_hd = hash_init();
	contents_hd = hash_cinit(NULL, NULL, 0644, QH_DUP);
	observable_hd = hash_init();
	art_hd = hash_init();

	snprintf(std_db, sizeof(std_db), "%s%s", euid ? nd_config.chroot : "", STD_DB);
	snprintf(std_db_ok, sizeof(std_db_ok), "%s%s", euid ? nd_config.chroot : "", STD_DB_OK);
	const char *name = std_db;
	FILE *f;

	if (access(std_db, F_OK) != 0)
		name = std_db_ok;

	f = fopen(name, "rb");

	if (f == NULL) {
                warn("No such file\n");
                return -1;
        }

	srand(getpid());			/* init random number generator */

	objects_read(f);
	dbref ref;
	struct hash_cursor c = obj_iter();
	OBJ oi;
	while ((ref = obj_next(&oi, &c)) != NOTHING)
		contents_put(oi.location, ref);

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

	putref(f, db_top);

	db_write_list(f);

	fseek(f, 0L, 2);
	putstring(f, "***END OF DUMP***");

	fflush(f);
	fclose(f);
}

#define STRING_READ(x) strdup(string_read(x))

struct hash_cursor obs_iter(dbref observable_ref) {
	return hash_citer(observable_hd, &observable_ref, sizeof(observable_ref));
}

dbref obs_next(struct hash_cursor *c) {
	dbref key, value;
	if (!hash_next(&key, &value, c))
		return NOTHING;
	return value;
}

void
observer_add(dbref observable_ref, dbref observer_ref) {
	hash_cput(observable_hd, &observable_ref, sizeof(observable_ref), &observer_ref, sizeof(observer_ref));
}

int
observer_remove(dbref observable_ref, dbref observer_ref) {
	hash_vdel(observable_hd, &observable_ref, sizeof(observable_ref), &observer_ref, sizeof(observer_ref));
	return 0;
}

dbref
ref_read(FILE * f)
{
	char buf[BUFSIZ];
	CBUG(!fgets(buf, sizeof(buf), f));
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
	dbref ref = ref_read(f);
	OBJ o;
	int j = 0;

	memset(&o, 0, sizeof(struct object));
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
			/* warn("entity home\n"); */
			/* eo->gpt = NULL; */
			ent_tmp_reset(&ent);
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
				int ref = ref_read(f);
				struct spell_skeleton *_sp = SPELL_SKELETON(ref);
				/* warn("entity! flags %d ref %d sp %p\n", eo->flags, ref); */
				sp->skel = _sp - spell_skeleton_map;
				sp->val = SPELL_DMG(&ent, _sp);
				sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
			}
			/* warn("entity! flags %d ref %d\n", eo->flags, ref_read(f)); */
			o.owner = ref;
			birth(&ent);
			if (ent.flags & EF_PLAYER)
				player_put(o.name, ref);
			ent_set(ref, &ent);
			/* warn("ENTITY\n"); */
		}
	}

	obj_set(ref, &o);
}

static dbref
objects_read(FILE * f)
{
	const char *special;
	char c;

	int hash_i = 0;
	for (; hash_i < sizeof(core_art) / sizeof(struct core_art); hash_i++) {
		struct core_art *art = &core_art[hash_i];
		hash_cput(art_hd, art->name, strlen(art->name), &art->max, sizeof(art->max));
	}

	/* Parse the header */
	db_top = ref_read(f);

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
				warn("found end of dump\n");
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
			warn("default '%c'\n", c);
			return NOTHING;
			/* break; */
		}
		c = getc(f);
	}							/* for */
}								/* db_read */

dbref
object_copy(OBJ *nu, dbref old_ref)
{
	OBJ old = obj_get(old_ref);
	dbref ref = object_new(nu);
	nu->name = strdup(old.name);
	nu->location = NOTHING;
	nu->owner = old.owner;
        return ref;
}

void
objects_update(double dt)
{
	OBJ obj;
	dbref obj_ref;
	struct hash_cursor c = obj_iter();
	while ((obj_ref = obj_next(&obj, &c)) != NOTHING)
		if (obj.type == TYPE_ENTITY)
			entity_update(obj_ref, dt);
}

void
object_move(dbref what_ref, dbref where_ref)
{
	CBUG(what_ref == NOTHING);
	OBJ what = obj_get(what_ref);

        if (what.location != NOTHING) {
                mcp_content_out(what.location, what_ref);
		hash_vdel(contents_hd, &what.location, sizeof(what.location), &what_ref, sizeof(what_ref));
        }

	struct hash_cursor c = obs_iter(what_ref);
	dbref first_ref;
	while ((first_ref = obs_next(&c)) != NOTHING) {
		ENT efirst = ent_get(first_ref);
		efirst.last_observed = what.location;
		observer_remove(what_ref, first_ref);
		ent_set(first_ref, &efirst);
	}

	/* test for special cases */
	if (where_ref == NOTHING) {
		dbref first_ref;

		struct hash_cursor c = contents_iter(what_ref);
		while ((first_ref = contents_next(&c)) != NOTHING)
			object_move(first_ref, NOTHING);

		switch (what.type) {
		case TYPE_ENTITY:
			ent_del(what_ref);
			break;
		case TYPE_ROOM:
			map_delete(what_ref);
		}
		free((void *) what.name);
		if (what.description)
			free((void *) what.description);
		hash_del(obj_hd, &what_ref, sizeof(what_ref));

		struct free_id *new_id = malloc(sizeof(struct free_id));
		new_id->value = what_ref;
		SLIST_INSERT_HEAD(&free_ids, new_id, entry);
		return;
	}

	what.location = where_ref;
	obj_set(what_ref, &what);

	if (what.type == TYPE_ENTITY) {
		ENT ewhat = ent_get(what_ref);
		if (ewhat.last_observed != NOTHING)
			observer_remove(ewhat.last_observed, what_ref);
		if ((ewhat.flags & EF_SITTING)) {
			stand(what_ref, &ewhat);
			ent_set(what_ref, &ewhat);
		}
	}

	contents_put(where_ref, what_ref);
	mcp_content_in(where_ref, what_ref);
}

struct icon
object_icon(dbref what_ref)
{
	OBJ what = obj_get(what_ref);
        static char buf[BUFSIZ];
        struct icon ret = {
                .actions = ACT_LOOK,
                .icon = ANSI_RESET ANSI_BOLD "?",
        };
        dbref aux;
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
			struct object_skeleton *obj_skel = PLANT_SKELETON(aux);
			struct plant_skeleton *pl = &obj_skel->sp.plant;

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

struct hash_cursor contents_iter(dbref parent) {
	return hash_citer(contents_hd, &parent, sizeof(parent));
}

dbref contents_next(struct hash_cursor *c) {
	dbref key, value;
	if (!hash_next(&key, &value, c))
		return NOTHING;
	return value;
}

void obj_del(dbref obj_ref) {
	object_move(obj_ref, NOTHING);
}

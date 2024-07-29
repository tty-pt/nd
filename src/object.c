/* $Header$ */

#include "copyright.h"
#include "config.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "params.h"
#include "defaults.h"

#include "spell.h"
#include "player.h"
#include "debug.h"
#include "plant.h"
#include "noise.h"
#include "mcp.h"

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
dbref db_top = 0;
OBJ *recyclable = NULL;

#ifndef DB_INITIAL_SIZE
#define DB_INITIAL_SIZE 10000
#endif							/* DB_INITIAL_SIZE */

dbref db_size = DB_INITIAL_SIZE;

static inline OBJ *
_object_parent(OBJ *obj)
{
	if (!obj)
		return NULL;
	return obj->location;
}

OBJ *
object_parent(OBJ *obj)
{
	OBJ *ptr, *oldptr;

	ptr = _object_parent(obj);
	do {
		obj = _object_parent(obj);
	} while (obj != (oldptr = ptr = _object_parent(ptr)) &&
		 obj != (ptr = _object_parent(ptr)) &&
		 obj && object_item(obj));
	if (obj && (obj == oldptr || obj == ptr)) {
		obj = object_get(GLOBAL_ENVIRONMENT);
	}
	return obj;
}


static void
objects_grow(dbref newtop)
{
	struct object *newdb;

	if (newtop > db_top) {
		db_top = newtop;
		if (!db) {
			/* make the initial one */
			db_size = DB_INITIAL_SIZE;
			while (db_top > db_size)
				db_size += 1000;
			if ((db = (struct object *) malloc(db_size * sizeof(struct object))) == 0) {
				abort();
			}
		}
		/* maybe grow it */
		if (db_top > db_size) {
			/* make sure it's big enough */
			while (db_top > db_size)
				db_size += 1000;
			if ((newdb = (struct object *) realloc((void *) db,
												   db_size * sizeof(struct object))) == 0) {
				abort();
			}
			db = newdb;
		}
	}
}

void
object_clear(OBJ *o)
{
	memset(o, 0, sizeof(struct object));

	o->name = 0;
	o->first_observer = NULL;
	o->contents = o->location = o->next = NULL;
	o->owner = object_get(GOD);
}

OBJ *
object_new(void)
{
	OBJ *newobj;

	if (recyclable) {
		newobj = recyclable;
		recyclable = newobj->next;
		object_free(newobj);
	} else {
		newobj = object_get(db_top);
		objects_grow(db_top + 1);
	}

	/* clear it out */
	object_clear(newobj);
	return newobj;
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

int art_hd = -1;

unsigned art_max(char *name) {
	struct core_art *core_art_item = (struct core_art *) SHASH_GET(art_hd, name);
	return core_art_item->max;
}

static inline unsigned
art_idx(OBJ *obj) {
	return 1 + (random() % art_max(obj->name));
}

OBJ *
object_add(SKEL *sk, OBJ *where, void *arg)
{
	OBJ *nu = object_new();
	nu->name = strdup(sk->name);
	nu->description = strdup(sk->description);
	nu->location = where;
	nu->owner = object_get(GOD);
	nu->type = TYPE_THING;
	if (where)
		PUSH(nu, where->contents);

	switch (sk->type) {
	case S_TYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = sk->sp.equipment.eqw;
			enu->msv = sk->sp.equipment.msv;
			enu->rare = rarity_get();
			CBUG(!where || where->type != TYPE_ENTITY);
			equip(where, nu)
				;
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
			ENT *enu = &nu->sp.entity;
			nu->type = TYPE_ENTITY;
			stats_init(enu, &sk->sp.entity);
			enu->flags = sk->sp.entity.flags;
			enu->wtso = sk->sp.entity.wt;
			birth(nu);
			spells_birth(nu);
			object_drop(nu, sk->sp.entity.drop);
			enu->home = where;
			nu->art_id = art_idx(nu);
		}

		break;
	case S_TYPE_PLANT:
		{
			noise_t v = * (noise_t *) arg;
			nu->type = TYPE_PLANT;
			object_drop(nu, sk->sp.plant.drop);
			nu->owner = object_get(GOD);
			nu->art_id = 1 + (v & 0xf) % art_max(nu->name);
		}

		break;
        case S_TYPE_BIOME:
		{
			struct bio *bio = arg;
			ROO *rnu = &nu->sp.room;
			nu->type = TYPE_ROOM;
			rnu->exits = rnu->doors = 0;
			rnu->dropto = NULL;
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

	if (sk->type != S_TYPE_BIOME)
		mcp_content_in(where, nu);

	return nu;
}

char *
object_art(OBJ *thing)
{
	static char art[BUFSIZ];
	char *type = NULL;

	switch (thing->type) {
	case TYPE_ENTITY:
		type = "entity";
		snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing->art_id && thing->sp.entity.flags & EF_PLAYER ? "avatar" : thing->name, thing->art_id);
		return art;
	case TYPE_ROOM: type = "biome"; break;
	case TYPE_PLANT: type = "plant"; break;
	case TYPE_MINERAL: type = "mineral"; break;
	default: type = "other"; break;
	}

	snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing->name, thing->art_id);
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
object_write(FILE * f, OBJ *obj)
{
	int j;
	putstring(f, obj->name);
	putref(f, object_ref(obj->location));
	putref(f, object_ref(obj->contents));
	putref(f, object_ref(obj->next));
	putref(f, obj->value);
	putref(f, obj->type);
	putref(f, obj->flags);
	putref(f, obj->art_id);

	switch (obj->type) {
	case TYPE_ENTITY:
		{
			ENT *ent = &obj->sp.entity;
			putref(f, object_ref(ent->home));
			putref(f, ent->flags);
			putref(f, ent->lvl);
			putref(f, ent->cxp);
			putref(f, ent->spend);
			for (j = 0; j < ATTR_MAX; j++)
				putref(f, ent->attr[j]);
			putref(f, ent->wtso);
			putref(f, object_ref(ent->sat));
			for (j = 0; j < 8; j++)
				putref(f, ent->spells[j]._sp - spell_skeleton_map);
		}
		break;

	case TYPE_PLANT:
		{
			PLA *pla = &obj->sp.plant;
			putref(f, pla->plid);
			putref(f, pla->size);
		}
		break;

	case TYPE_SEAT:
		{
			SEA *sea = &obj->sp.seat;
			putref(f, sea->quantity);
			putref(f, sea->capacity);
		}
		break;

	case TYPE_CONSUMABLE:
		{
			CON *con = &obj->sp.consumable;
			putref(f, con->food);
			putref(f, con->drink);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *equ = &obj->sp.equipment;
			putref(f, equ->eqw);
			putref(f, equ->msv);
		}
		break;
	case TYPE_THING:
		putref(f, object_ref(obj->owner));
		break;

	case TYPE_ROOM:
		{
			ROO *roo = &obj->sp.room;
			putref(f, object_ref(roo->dropto));
			putref(f, roo->flags);
			putref(f, roo->exits);
			putref(f, roo->doors);
			putref(f, roo->floor);
			putref(f, object_ref(obj->owner));
		}
		break;
	}
}

static inline void
db_write_list(FILE * f)
{
	OBJ *obj;

	FOR_ALL(obj) {
		if (fprintf(f, "#%d\n", object_ref(obj)) < 0)
			abort();
		object_write(f, obj);
	}
}


OBJ *
objects_write(FILE * f)
{
	putref(f, db_top);

	db_write_list(f);

	fseek(f, 0L, 2);
	putstring(f, "***END OF DUMP***");

	fflush(f);
	return object_get(db_top);
}

#define STRING_READ(x) strdup(string_read(x))

void
object_free(OBJ *o)
{
	if (o->name)
		free((void *) o->name);

	if (o->description)
		free((void *) o->description);

	if (o->type != TYPE_ROOM) {
		struct observer_node *obs, *aux;
		for (obs = o->first_observer; obs; obs = aux) {
			aux = obs->next;
			free(obs);
		}
	}
}

void
observer_add(OBJ *observable, OBJ *observer) {
	struct observer_node *node = (struct observer_node *)
		malloc(sizeof(struct observer_node));
	node->next = observable->first_observer;
	node->who = observer;
	observable->first_observer = node;
}

int
observer_remove(OBJ *observable, OBJ *observer) {
	struct observer_node *cur, *prev = NULL;

	for (cur = observable->first_observer; cur; ) {
		if (cur->who == observer) {
			if (prev) {
				prev->next = cur->next;
				free(cur);
				return 0;
			}
		}
		prev = cur;
		cur = cur->next;
	}

	return 1;
}

void
objects_free(void)
{
	OBJ *obj;

	if (db) {
		FOR_ALL(obj)
			object_free(obj);
		free((void *) db);
		db = 0;
		db_top = 0;
	}
	recyclable = NULL;
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
	OBJ *o = object_get(ref);
	int j = 0;

	object_clear(o);

	/* make space */
	objects_grow(ref + 1);

	o->name = STRING_READ(f);
	o->location = object_get(ref_read(f));
	o->contents = object_get(ref_read(f));
	o->next = object_get(ref_read(f));
	o->value = ref_read(f);
	o->type = ref_read(f);
	o->flags = ref_read(f);
	o->art_id = ref_read(f);
	/* warn("db_read_object_foxen %d %s location %d contents %d next %d type %d\n", ref, o->name, */
	/* 		object_ref(o->location), */
	/* 		object_ref(o->contents), */
	/* 		object_ref(o->next), */
	/* 		o->type); */

	switch (o->type) {
	case TYPE_PLANT:
		{
			PLA *po = &o->sp.plant;
			po->plid = ref_read(f);
			po->size = ref_read(f);
		}
		return;
	case TYPE_SEAT:
		{
			SEA *ps = &o->sp.seat;
			ps->quantity = ref_read(f);
			ps->capacity = ref_read(f);
		}
		return;
	case TYPE_CONSUMABLE:
		{
			CON *co = &o->sp.consumable;
			co->food = ref_read(f);
			co->drink = ref_read(f);
		}
		return;
	case TYPE_EQUIPMENT:
		{
			EQU *eo = &o->sp.equipment;
			eo->eqw = ref_read(f);
			eo->msv = ref_read(f);
		}
		return;
	case TYPE_THING:
		o->owner = object_get(ref_read(f));
		return;
	case TYPE_ROOM:
		{
			ROO *ro = &o->sp.room;
			ro->dropto = object_get(ref_read(f));
			ro->flags = ref_read(f);
			ro->exits = ref_read(f);
			ro->doors = ref_read(f);
			ro->floor = ref_read(f);
			o->owner = object_get(ref_read(f));
		}
		return;
	case TYPE_ENTITY:
		{
			ENT *eo = &o->sp.entity;
			eo->home = object_get(ref_read(f));
			/* warn("entity home\n"); */
			eo->last_observed = NULL;
			eo->gpt = NULL;
			eo->flags = ref_read(f);
			fprintf(stderr, "READ ENT %s %d\n", o->name, eo->flags);
			eo->lvl = ref_read(f);
			eo->cxp = ref_read(f);
			eo->spend = ref_read(f);
			for (j = 0; j < ATTR_MAX; j++)
				eo->attr[j] = ref_read(f);
			eo->wtso = ref_read(f);
			eo->sat = object_get(ref_read(f));
			for (j = 0; j < 8; j++) {
				struct spell *sp = &eo->spells[j];
				int ref = ref_read(f);
				struct spell_skeleton *_sp = SPELL_SKELETON(ref);
				/* warn("entity! flags %d ref %d sp %p\n", eo->flags, ref); */
				sp->_sp = _sp;
				sp->val = SPELL_DMG(eo, _sp);
				sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
			}
			/* warn("entity! flags %d ref %d\n", eo->flags, ref_read(f)); */
			o->owner = o;
			if (eo->flags & EF_PLAYER) {
				player_put(o);
				eo->fds = 0;
			}
			birth(o);
			/* warn("ENTITY\n"); */
		}
		break;
	case TYPE_GARBAGE:
		return;
	}
}

OBJ *
objects_read(FILE * f)
{
	int i;
	dbref grow;
	const char *special;
	char c;

	art_hd = hash_init();

	int hash_i = 0;
	for (; hash_i < sizeof(core_art) / sizeof(struct core_art); hash_i++)
		SHASH_PUT(art_hd, core_art[hash_i].name, &core_art[hash_i]);

	/* Parse the header */
	grow = ref_read(f);
	objects_grow( grow );

	c = getc(f);			/* get next char */
	for (i = 0;; i++) {
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
				return NULL;
			} else {
				free((void *) special);
				special = STRING_READ(f);
				if (special)
					free((void *) special);
				for (i = 0; i < db_top; i++) {
					OBJ *o = object_get(i);
					if (o->type == TYPE_GARBAGE) {
						o->next = recyclable;
						recyclable = o;
					}
				}
				return object_get(db_top);
			}
			break;
		default:
			warn("default '%c'\n", c);
			return NULL;
			/* break; */
		}
		c = getc(f);
	}							/* for */
}								/* db_read */

OBJ *
object_copy(OBJ *player, OBJ *old)
{
	OBJ *nu = object_new();
	nu->name = strdup(old->name);
	nu->contents = nu->next = nu->location = NULL;
	nu->owner = old->owner;
        return nu;
}

static inline void
object_update(OBJ *what, unsigned long long dt) {
	if (what->type == TYPE_ENTITY)
		entity_update(what, dt);
}

void
objects_update(unsigned long long dt)
{
	dbref i;
	for (i = db_top; i-- > 0;)
		object_update(object_get(i), dt);
}

static inline int
object_llc(OBJ *source, OBJ *dest)
{
	unsigned int level = 0;
	unsigned int place = 0;
	OBJ *pstack[MAX_PARENT_DEPTH+2];

	if (source == dest) {
		return 1;
	}

	if (!dest)
		return 0;

	pstack[0] = source;
	pstack[1] = dest;

	while (level < MAX_PARENT_DEPTH) {
		dest = dest->location;
		if (!dest || object_ref(dest) == (dbref) 0) {   /* Reached the top of the chain. */
			return 0;
		}
		/* Check to see if we've found this item before.. */
		for (place = 0; place < (level+2); place++) {
			if (pstack[place] == dest) {
				return 1;
			}
		}
		pstack[level+2] = dest;
		level++;
	}
	return 1;
}

int
object_plc(OBJ *source, OBJ *dest)
{   
	unsigned int level = 0;
	unsigned int place = 0;
	OBJ *pstack[MAX_PARENT_DEPTH+2];

	if (object_llc(source, dest)) {
		return 1;
	}

	if (source == dest) {
		return 1;
	}
	pstack[0] = source;
	pstack[1] = dest;

	while (level < MAX_PARENT_DEPTH) {
		dest = object_parent(dest);
		if (!dest)
			return 0;
		if (object_ref(dest) == (dbref) 0) {   /* Reached the top of the chain. */
			return 0;
		}
		/* Check to see if we've found this item before.. */
		for (place = 0; place < (level+2); place++) {
			if (pstack[place] == dest) {
				return 1;
			}
		}
		pstack[level+2] = dest;
		level++;
	}
	return 1;
}

/* remove the first occurence of what in list headed by first */
static inline OBJ *
remove_first(OBJ *first, OBJ *what)
{
	OBJ *prev;

	/* special case if it's the first one */
	if (first == what) {
		return first->next;
	} else {
		/* have to find it */
		FOR_LIST(prev, first) {
			if (prev->next == what) {
				prev->next = what->next;
				return first;
			}
		}
		return first;
	}
}

void
object_move(OBJ *what, OBJ *where)
{
	OBJ *loc;

	/* do NOT move garbage */
	CBUG(!what);
	CBUG(what->type == TYPE_GARBAGE);

	loc = what->location;

        if (loc) {
                mcp_content_out(loc, what);
		loc->contents = remove_first(loc->contents, what);
        }

	/* test for special cases */
	if (!where) {
		what->location = NULL;
		return;
	}

	if (object_plc(what, where)) {
		if (what->type == TYPE_ENTITY) {
			where = what->sp.entity.home;
			ENT *ewhat = &what->sp.entity;
			if ((ewhat->flags & EF_SITTING))
				stand(what);
		}
		else
			where = object_get(GLOBAL_ENVIRONMENT);
	}

	/* now put what in where */
	PUSH(what, where->contents);
	what->location = where;
	mcp_content_in(where, what);
}

struct icon
object_icon(OBJ *what)
{
        static char buf[BUFSIZ];
        struct icon ret = {
                .actions = ACT_LOOK,
                .icon = ANSI_RESET ANSI_BOLD "?",
        };
        dbref aux;
        switch (what->type) {
        case TYPE_ROOM:
                ret.icon = ANSI_FG_YELLOW "-";
                break;
        case TYPE_ENTITY:
		{
			ENT *ewhat = &what->sp.entity;

			ret.actions |= ACT_KILL;
			ret.icon = ANSI_BOLD ANSI_FG_YELLOW "!";
			if (ewhat->flags & EF_SHOP) {
				ret.actions |= ACT_SHOP;
				ret.icon = ANSI_BOLD ANSI_FG_GREEN "$";
			}
		}
                break;
	case TYPE_CONSUMABLE:
		{
			CON *cwhat = &what->sp.consumable;
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
			PLA *pwhat = &what->sp.plant;
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

OBJ *
object_get(register dbref ref)
{
	return ref == NOTHING ? NULL : &db[ref];
}

dbref
object_ref(register OBJ *obj)
{
	if (obj)
		return (dbref) (obj - db);
	else
		return NOTHING;
}

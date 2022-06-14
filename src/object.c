/* $Header$ */

#include "copyright.h"
#include "config.h"

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"

#include "externs.h"
#include "spell.h"

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

/* returns true for numbers of form [ + | - ] <series of digits> */
int
number(const char *s)
{
	if (!s)
		return 0;
	while (isspace(*s))
		s++;
	if (*s == '+' || *s == '-')
		s++;
	if (!*s) 
		return 0;
	for (; *s; s++)
		if (*s < '0' || *s > '9')
			return 0;
	return 1;
}

/* returns true for floats of form  [+|-]<digits>.<digits>[E[+|-]<digits>] */
int
ifloat(const char *s)
{
	const char *hold;

	if (!s)
		return 0;
	while (isspace(*s))
		s++;
	if (*s == '+' || *s == '-')
		s++;
	hold = s;
	while ((*s) && (*s >= '0' && *s <= '9'))
		s++;
	if ((!*s) || (s == hold))
		return 0;
	if (*s != '.')
		return 0;
	s++;
	hold = s;
	while ((*s) && (*s >= '0' && *s <= '9'))
		s++;
	if (hold == s)
		return 0;
	if (!*s)
		return 1;
	if ((*s != 'e') && (*s != 'E'))
		return 0;
	s++;
	if (*s == '+' || *s == '-')
		s++;
	hold = s;
	while ((*s) && (*s >= '0' && *s <= '9'))
		s++;
	if (s == hold)
		return 0;
	if (*s)
		return 0;
	return 1;
}

void
object_free(OBJ *o)
{
	if (o->name)
		free((void *) o->name);

	if (o->description)
		free((void *) o->description);

	if (o->art)
		free((void *) o->art);

	if (o->avatar)
		free((void *) o->avatar);

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
	clear_players();
	recyclable = NULL;
}

dbref
ref_read(FILE * f)
{
	static char buf[BUFFER_LEN];
	int peekch;

	/*
	 * Compiled in with or without timestamps, Sep 1, 1990 by Fuzzy, added to
	 * Muck by Kinomon.  Thanks Kino!
	 */
	peekch = getc(f);
	ungetc(peekch, f);

	if (peekch == NUMBER_TOKEN || peekch == LOOKUP_TOKEN)
		return 0;

	fgets(buf, sizeof(buf), f);
	return (atol(buf));
}

static const char *
string_read(FILE * f)
{
	static char xyzzybuf[BUFFER_LEN];
	char *p;
	char c;

	if (fgets(xyzzybuf, sizeof(xyzzybuf), f) == NULL) {
		xyzzybuf[0] = '\0';
		return xyzzybuf;
	}

	if (strlen(xyzzybuf) == BUFFER_LEN - 1) {
		/* ignore whatever comes after */
		if (xyzzybuf[BUFFER_LEN - 2] != '\n')
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
	warn("db_read_object_foxen %d %s location %d contents %d next %d type %d\n", ref, o->name,
			object_ref(o->location),
			object_ref(o->contents),
			object_ref(o->next),
			o->type);

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
			eo->fd = -1;
			eo->last_observed = NULL;
			eo->flags = ref_read(f);
			eo->lvl = ref_read(f);
			eo->cxp = ref_read(f);
			eo->spend = ref_read(f);
			for (j = 0; j < ATTR_MAX; j++)
				eo->attr[j] = ref_read(f);
			eo->wtso = ref_read(f);
			eo->sat = object_get(ref_read(f));
			for (j = 0; j < 8; j++) {
				struct spell *sp = &eo->spells[j];
				struct spell_skeleton *_sp = SPELL_SKELETON(ref_read(f));
				sp->_sp = _sp;
				sp->val = SPELL_DMG(eo, _sp);
				sp->cost = SPELL_COST(sp->val, _sp->y, _sp->flags & AF_BUF);
			}
			o->owner = o;
			if (eo->flags & EF_PLAYER)
				add_player(o);
			birth(o);
			warn("entity!\n");
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
		case LOOKUP_TOKEN:
			special = STRING_READ(f);
			if (strcmp(special, "**END OF DUMP***")) {
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

static void
object_update(OBJ *what) {
	if (what->type == TYPE_ENTITY)
		entity_update(what);
}

void
objects_update()
{
	
	dbref i;
	for (i = db_top; i-- > 0;)
		object_update(object_get(i));
}

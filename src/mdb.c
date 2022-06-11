/* $Header$ */

#include "copyright.h"
#include "config.h"

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "geography.h"

#include "externs.h"

struct object *db = 0;
dbref db_top = 0;
OBJ *recyclable = NULL;

#ifndef DB_INITIAL_SIZE
#define DB_INITIAL_SIZE 10000
#endif							/* DB_INITIAL_SIZE */

#ifdef DB_DOUBLING

dbref db_size = DB_INITIAL_SIZE;

#endif							/* DB_DOUBLING */

extern char *alloc_string(const char *);
int number(const char *s);
int ifloat(const char *s);
void putproperties(FILE * f, int obj);
void getproperties(FILE * f, OBJ *obj, const char *pdir);


OBJ *
getparent_logic(OBJ *obj)
{
	if (!obj)
		return NULL;
	return obj->location;
}

OBJ *
getparent(OBJ *obj)
{
	OBJ *ptr, *oldptr;

	ptr = getparent_logic(obj);
	do {
		obj = getparent_logic(obj);
	} while (obj != (oldptr = ptr = getparent_logic(ptr)) &&
		 obj != (ptr = getparent_logic(ptr)) &&
		 obj && is_item(obj));
	if (obj && (obj == oldptr || obj == ptr)) {
		obj = object_get(GLOBAL_ENVIRONMENT);
	}
	return obj;
}


#ifdef DB_DOUBLING

static void
db_grow(dbref newtop)
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

#else							/* DB_DOUBLING */

static void
db_grow(dbref newtop)
{
	struct object *newdb;

	if (newtop > db_top) {
		db_top = newtop;
		if (db) {
			if ((newdb = (struct object *)
				 realloc((void *) db, db_top * sizeof(struct object))) == 0) {
				abort();
			}
			db = newdb;
		} else {
			/* make the initial one */
			int startsize = (newtop >= DB_INITIAL_SIZE) ? newtop : DB_INITIAL_SIZE;

			if ((db = (struct object *)
				 malloc(startsize * sizeof(struct object))) == 0) {
				abort();
			}
		}
	}
}

#endif							/* DB_DOUBLING */

void
db_clear_object(OBJ *o)
{
	memset(o, 0, sizeof(struct object));

	o->name = 0;
	o->first_observer = NULL;
	o->contents = o->location = o->next = NULL;
	o->properties = 0;
}

OBJ *
object_new(void)
{
	OBJ *newobj;

	if (recyclable) {
		newobj = recyclable;
		recyclable = newobj->next;
		db_free_object(newobj);
	} else {
		newobj = object_get(db_top);
		db_grow(db_top + 1);
	}

	/* clear it out */
	db_clear_object(newobj);
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

void
putproperties_rec(FILE * f, const char *dir, OBJ *obj)
{
	PropPtr pref;
	PropPtr p, pptr;
	char buf[BUFFER_LEN];
	char name[BUFFER_LEN];

	pref = first_prop_nofetch(obj, dir, &pptr, name, sizeof(name));
	while (pref) {
		p = pref;
		db_putprop(f, dir, p);
		strlcpy(buf, dir, sizeof(buf));
		strlcat(buf, name, sizeof(buf));
		if (PropDir(p)) {
			strlcat(buf, "/", sizeof(buf));
			putproperties_rec(f, buf, obj);
		}
		pref = next_prop(pptr, pref, name, sizeof(name));
	}
}

/*** CHANGED:
was: void putproperties(FILE *f, PropPtr p)
 is: void putproperties(FILE *f, dbref obj)
***/
void
putproperties(FILE * f, dbref obj)
{
	putstring(f, "*Props*");
	db_dump_props(f, obj);
	/* putproperties_rec(f, "/", obj); */
	putstring(f, "*End*");
}


static int
db_write_object(FILE * f, dbref i)
{
	OBJ *o = object_get(i);
	int j;
	putstring(f, o->name);
	putref(f, object_ref(o->location));
	putref(f, object_ref(o->contents));
	putref(f, object_ref(o->next));
	putref(f, o->value);
	putref(f, o->type);	/* write non-internal flags */

	putproperties(f, i);


	switch (object_get(i)->type) {
	case TYPE_ENTITY:
		{
			ENT *ent = &o->sp.entity;
			putref(f, object_ref(ent->home));
			putref(f, ent->flags);
			putref(f, ent->lvl);
			putref(f, ent->cxp);
			putref(f, ent->spend);
			for (j = 0; j < ATTR_MAX; j++)
				putref(f, ATTR(ent, j));
			putref(f, ent->wtso);
		}
		break;

	case TYPE_PLANT:
		{
			PLA *pla = &o->sp.plant;
			putref(f, pla->plid);
			putref(f, pla->size);
		}
		break;

	case TYPE_CONSUMABLE:
		{
			CON *con = &o->sp.consumable;
			putref(f, con->food);
			putref(f, con->drink);
		}
		break;
	case TYPE_EQUIPMENT:
		{
			EQU *equ = &o->sp.equipment;
			putref(f, equ->eqw);
			putref(f, equ->msv);
		}
		break;
	case TYPE_THING:
		putref(f, object_ref(o->owner));
		break;

	case TYPE_ROOM:
		{
			ROO *roo = &o->sp.room;
			putref(f, object_ref(roo->dropto));
			putref(f, roo->flags);
			putref(f, roo->exits);
			putref(f, roo->floor);
			putref(f, object_ref(o->owner));
		}
		break;
	}

	return 0;
}

static inline void
db_write_list(FILE * f)
{
	dbref i;

	for (i = db_top; i-- > 0;) {
		if (fprintf(f, "#%d\n", i) < 0)
			abort();
		db_write_object(f, i);
	}
}


dbref
db_write(FILE * f)
{
	putref(f, db_top);

	db_write_list(f);

	fseek(f, 0L, 2);
	putstring(f, "***END OF DUMP***");

	fflush(f);
	return (db_top);
}

#define getstring(x) alloc_string(getstring_noalloc(x))

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

/*** CHANGED:
was: PropPtr getproperties(FILE *f)
now: void getproperties(FILE *f, dbref obj, const char *pdir)
***/
void
getproperties(FILE * f, OBJ *obj, const char *pdir)
{
	char buf[BUFFER_LEN * 3], *p;
	int datalen;

	/* get rid of first line */
	fgets(buf, sizeof(buf), f);

	if (strcmp(buf, "Props*\n")) {
		/* initialize first line stuff */
		fgets(buf, sizeof(buf), f);
		while (1) {
			/* fgets reads in \n too! */
			if (!strcmp(buf, "***Property list end ***\n") || !strcmp(buf, "*End*\n"))
				break;
			p = index(buf, PROP_DELIMITER);
			*(p++) = '\0';		/* Purrrrrrrrrr... */
			datalen = strlen(p);
			p[datalen - 1] = '\0';

			if ((p - buf) >= BUFFER_LEN)
				buf[BUFFER_LEN - 1] = '\0';
			if (datalen >= BUFFER_LEN)
				p[BUFFER_LEN - 1] = '\0';

			if ((*p == '^') && (number(p + 1))) {
				add_prop_nofetch(obj, buf, NULL, atol(p + 1));
			} else {
				if (*buf) {
					add_prop_nofetch(obj, buf, p, 0);
				}
			}
			fgets(buf, sizeof(buf), f);
		}
	} else {
		db_getprops(f, obj, pdir);
	}
}

void
db_free_object(OBJ *o)
{
	if (o->name)
		free((void *) o->name);

	if (o->description)
		free((void *) o->description);

	if (o->art)
		free((void *) o->art);

	if (o->avatar)
		free((void *) o->avatar);

	if (o->properties) {
		delete_proplist(o->properties);
	}

	if (o->type != TYPE_ROOM) {
		struct observer_node *obs, *aux;
		for (obs = o->first_observer; obs; obs = aux) {
			aux = obs->next;
			free(obs);
		}
	}
}

void
db_obs_add(OBJ *observable, OBJ *observer) {
	struct observer_node *node = (struct observer_node *)
		malloc(sizeof(struct observer_node));
	node->next = observable->first_observer;
	node->who = object_ref(observer);
	observable->first_observer = node;
}

int
db_obs_remove(OBJ *observable, OBJ *observer) {
	struct observer_node *cur, *prev = NULL;

	for (cur = observable->first_observer; cur; ) {
		if (cur->who == object_ref(observer)) {
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
db_free(void)
{
	dbref i;

	if (db) {
		for (i = 0; i < db_top; i++)
			db_free_object(object_get(i));
		free((void *) db);
		db = 0;
		db_top = 0;
	}
	clear_players();
	recyclable = NULL;
}

dbref
getref(FILE * f)
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

static char xyzzybuf[BUFFER_LEN];
const char *
getstring_noalloc(FILE * f)
{
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

/* Reads in Foxen, Foxen[2-8], WhiteFire, Mage or Lachesis DB Formats */
void
db_read_object_foxen(FILE * f, dbref objno)
{
	OBJ *o = object_get(objno);
	int c, prop_flag = 0;
	int j = 0;

	db_clear_object(o);

	o->flags = 0;
	o->name = getstring(f);
	o->location = object_get(getref(f));
	o->contents = object_get(getref(f));
	o->next = object_get(getref(f));
	o->value = getref(f);
	o->type = getref(f);
	warn("db_read_object_foxen %d %s location %d contents %d next %d type %d\n", objno, o->name,
			object_ref(o->location),
			object_ref(o->contents),
			object_ref(o->next),
			o->type);

	c = getc(f);
	if (c == '*') {

		getproperties(f, o, NULL);

		prop_flag++;
	} else {
		/* do our own getref */
		int sign = 0;
		char buf[BUFFER_LEN];
		int i = 0;

		if (c == '-')
			sign = 1;
		else if (c != '+') {
			buf[i] = c;
			i++;
		}
		while ((c = getc(f)) != '\n') {
			buf[i] = c;
			i++;
		}
		buf[i] = '\0';
		j = atol(buf);
		if (sign)
			j = -j;
	}

	switch (o->type) {
	case TYPE_PLANT:
		{
			PLA *po = &o->sp.plant;
			po->plid = prop_flag ? getref(f) : j;
			po->size = getref(f);
		}
		return;
	case TYPE_CONSUMABLE:
		{
			CON *co = &o->sp.consumable;
			co->food = prop_flag ? getref(f) : j;
			co->drink = getref(f);
		}
		return;
	case TYPE_EQUIPMENT:
		{
			EQU *eo = &o->sp.equipment;
			eo->eqw = prop_flag ? getref(f) : j;
			eo->msv = getref(f);
		}
		return;
	case TYPE_THING:
		o->owner = object_get(prop_flag ? getref(f) : j);
		return;
	case TYPE_ROOM:
		{
			ROO *ro = &o->sp.room;
			ro->dropto = object_get(prop_flag ? getref(f) : j);
			ro->flags = getref(f);
			ro->exits = getref(f);
			ro->doors = getref(f);
			ro->floor = getref(f);
			o->owner = object_get(getref(f));
		}
		return;
	case TYPE_ENTITY:
		{
			ENT *eo = &o->sp.entity;
			eo->home = object_get(prop_flag ? getref(f) : j);
			eo->fd = -1;
			eo->last_observed = NULL;
			eo->flags = getref(f);
			eo->lvl = getref(f);
			eo->cxp = getref(f);
			eo->spend = getref(f);
			for (j = 0; j < ATTR_MAX; j++)
				ATTR(eo, j) = getref(f);
			eo->wtso = getref(f);
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

dbref
db_read(FILE * f)
{
	int i;
	dbref grow, thisref;
	const char *special;
	char c;

	/* Parse the header */
	grow = getref(f);
	db_grow( grow );

	c = getc(f);			/* get next char */
	for (i = 0;; i++) {
		switch (c) {
		case NUMBER_TOKEN:
			/* another entry, yawn */
			thisref = getref(f);

			/* make space */
			db_grow(thisref + 1);

			/* read it in */
			db_read_object_foxen(f, thisref);
			break;
		case LOOKUP_TOKEN:
			special = getstring(f);
			if (strcmp(special, "**END OF DUMP***")) {
				free((void *) special);
				return -1;
			} else {
				free((void *) special);
				special = getstring(f);
				if (special)
					free((void *) special);
				for (i = 0; i < db_top; i++) {
					OBJ *o = object_get(i);
					if (o->type == TYPE_GARBAGE) {
						o->next = recyclable;
						recyclable = o;
					}
				}
				return db_top;
			}
			break;
		default:
			return -1;
			/* break; */
		}
		c = getc(f);
	}							/* for */
}								/* db_read */

OBJ *
object_copy(OBJ *player, OBJ *old)
{
	OBJ *nu = object_new();
	nu->name = alloc_string(old->name);
	nu->properties = copy_prop(old);
	nu->contents = nu->next = nu->location = NULL;
        nu->flags = old->flags;
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

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
dbref recyclable = NOTHING;

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
void getproperties(FILE * f, int obj, const char *pdir);


dbref
getparent_logic(dbref obj)
{
        if (obj == NOTHING) return NOTHING;
	obj = getloc(obj);
	return obj;
}

dbref
getparent(dbref obj)
{
        dbref ptr, oldptr;

	ptr = getparent_logic(obj);
	do {
		obj = getparent_logic(obj);
	} while (obj != (oldptr = ptr = getparent_logic(ptr)) &&
		 obj != (ptr = getparent_logic(ptr)) &&
		 obj != NOTHING && Typeof(obj) == TYPE_THING);
	if (obj != NOTHING && (obj == oldptr || obj == ptr)) {
		obj = GLOBAL_ENVIRONMENT;
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
db_clear_object(dbref i)
{
	struct object *o = &db[i];

	memset(o, 0, sizeof(struct object));

	NAME(i) = 0;
	o->location = NOTHING;
	o->contents = NOTHING;
	o->exits = NOTHING;
	o->next = NOTHING;
	o->properties = 0;
	o->first_observer = NULL;
	o->mob = NULL;

	/* flags you must initialize yourself */
	/* type-specific fields you must also initialize */
}

dbref
object_new(void)
{
	dbref newobj;

	if (recyclable != NOTHING) {
		newobj = recyclable;
		recyclable = db[newobj].next;
		db_free_object(newobj);
	} else {
		newobj = db_top;
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
putproperties_rec(FILE * f, const char *dir, dbref obj)
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
	struct object *o = &db[i];
	int j;
	putstring(f, NAME(i));
	putref(f, o->location);
	putref(f, o->contents);
	putref(f, o->next);
	putref(f, (FLAGS(i) & ~DUMP_MASK));	/* write non-internal flags */

	putproperties(f, i);


	switch (Typeof(i)) {
	case TYPE_THING:
		putref(f, THING_HOME(i));
		putref(f, o->exits);
		putref(f, OWNER(i));
		break;

	case TYPE_ROOM:
		putref(f, o->sp.room.dropto);
		putref(f, o->exits);
		putref(f, OWNER(i));
		break;

	case TYPE_EXIT:
		putref(f, o->sp.exit.ndest);
		for (j = 0; j < o->sp.exit.ndest; j++) {
			putref(f, (o->sp.exit.dest)[j]);
		}
		putref(f, OWNER(i));
		break;

	case TYPE_PLAYER:
		putref(f, PLAYER_HOME(i));
		putref(f, o->exits);
		putstring(f, PLAYER_PASSWORD(i));
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
getproperties(FILE * f, dbref obj, const char *pdir)
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
db_free_object(dbref i)
{
	struct object *o;

	o = &db[i];
	if (NAME(i))
		free((void *) NAME(i));

	if (o->properties) {
		delete_proplist(o->properties);
	}

	if (Typeof(i) == TYPE_EXIT && o->sp.exit.dest) {
		free((void *) o->sp.exit.dest);
	} else if (Typeof(i) == TYPE_PLAYER) {
		if (PLAYER_PASSWORD(i)) {
			free((void*)PLAYER_PASSWORD(i));
		}
	}
	if (Typeof(i) == TYPE_THING) {
		FREE_THING_SP(i);
	}
	if (Typeof(i) == TYPE_PLAYER) {
		FREE_PLAYER_SP(i);
	}
	if (Typeof(i) != TYPE_ROOM) {
		struct observer_node *obs, *aux;
		for (obs = o->first_observer; obs; obs = aux) {
			aux = obs->next;
			free(obs);
		}
	}
	if (o->mob)
		free(o->mob);
}

void
db_obs_add(dbref observable, dbref observer) {
	struct object *o;
	struct observer_node *node = (struct observer_node *)
		malloc(sizeof(struct observer_node));
	o = &db[observable];
	node->next = o->first_observer;
	node->who = observer;
	o->first_observer = node;
}

int
db_obs_remove(dbref observable, dbref observer) {
	struct object *o = &db[observable];
	struct observer_node *cur, *prev = NULL;

	for (cur = o->first_observer; cur; ) {
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
db_free(void)
{
	dbref i;

	if (db) {
		for (i = 0; i < db_top; i++)
			db_free_object(i);
		free((void *) db);
		db = 0;
		db_top = 0;
	}
	clear_players();
	recyclable = NOTHING;
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
db_read_object_foxen(FILE * f, struct object *o, dbref objno)
{
	int tmp, c, prop_flag = 0;
	int j = 0;
	const char *password;

	db_clear_object(objno);

	FLAGS(objno) = 0;
	NAME(objno) = getstring(f);
	o->location = getref(f);
	o->contents = getref(f);
	o->next = getref(f);
	tmp = getref(f);			/* flags list */
	tmp &= ~DUMP_MASK;
	FLAGS(objno) |= tmp;

	FLAGS(objno) &= ~SAVED_DELTA;

	c = getc(f);
	if (c == '*') {

		getproperties(f, objno, NULL);

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

	switch (FLAGS(objno) & TYPE_MASK) {
	case TYPE_THING:{
			dbref home;

			ALLOC_THING_SP(objno);
			home = prop_flag ? getref(f) : j;
			THING_SET_HOME(objno, home);
			o->exits = getref(f);
			OWNER(objno) = getref(f);
			break;
		}
	case TYPE_ROOM:
		o->sp.room.dropto = prop_flag ? getref(f) : j;
		o->exits = getref(f);
		OWNER(objno) = getref(f);
		return;
	case TYPE_EXIT:
		o->sp.exit.ndest = prop_flag ? getref(f) : j;
		if (o->sp.exit.ndest > 0)	/* only allocate space for linked exits */
			o->sp.exit.dest = (dbref *) malloc(sizeof(dbref) * (o->sp.exit.ndest));
		for (j = 0; j < o->sp.exit.ndest; j++) {
			(o->sp.exit.dest)[j] = getref(f);
		}
		OWNER(objno) = getref(f);
		return;
	case TYPE_PLAYER:
		ALLOC_PLAYER_SP(objno);
		/* PLAYER_SET_HOME(objno, (prop_flag ? getref(f) : j)); */
		if (prop_flag)
			getref(f);
		o->exits = getref(f);
		password = getstring(f);
		PLAYER_SET_PASSWORD(objno, password);
		PLAYER_SP(objno)->fd = -1;
		PLAYER_SP(objno)->last_observed = NOTHING;
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
	struct object *o;
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
			o = &db[thisref];
			db_read_object_foxen(f, o, thisref);
			if (Typeof(thisref) == TYPE_PLAYER) {
				OWNER(thisref) = thisref;
				add_player(thisref);
			}
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
					if (Typeof(i) == TYPE_GARBAGE) {
						db[i].next = recyclable;
						recyclable = i;
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

dbref
object_copy(dbref player, dbref old)
{
        dbref nu = object_new();
	struct object *newp = &db[nu];
	NAME(nu) = alloc_string(NAME(old));
	if (Typeof(old) == TYPE_THING) {
		ALLOC_THING_SP(nu);
		THING_SET_HOME(nu, player);
		/* SETVALUE(nu, 1); */
	}
	newp->properties = copy_prop(old);
	newp->exits = NOTHING;
	newp->contents = NOTHING;
	newp->next = NOTHING;
	newp->location = NOTHING;
        newp->flags = db[old].flags;
        OWNER(nu) = OWNER(old);
        return nu;
}

static void
object_update(dbref what, long long unsigned tick) {
	if (MOB(what))
		mob_update(what, tick);
}

static void
room0_update(long long unsigned tick)
{
	dbref thing;
	DOLIST(thing, db[0].contents) {
		if (Typeof(thing) == TYPE_PLAYER) {
			if (!God(thing)) {
				struct cmd_dir cd;
				cd.rep = STARTING_POSITION;
				cd.dir = '\0';
				geo_teleport(thing, cd);
			}
		}
	}
}

void
objects_update(long long unsigned tick)
{
	
	dbref i;
	for (i = db_top; i-- > 0;)
		object_update(i, tick);

	room0_update(tick);
}

static inline void
object_init(dbref what) {
	if (GETLID(what) >= 0)
		mob_put(what);
}

void
objects_init()
{
	
	dbref i;
	for (i = db_top; i-- > 0;)
		object_init(i);
}

#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdio.h>
#include <qhash.h>
#include "uapi/object.h"
#include "skeleton.h"

#define NOTHING ((dbref) -1)
#define FOR_ALL(ref) for (ref = db_top; ref-- > 0; )

#define GOD ((dbref) 1)
#define God(obj) (object_ref(obj) == (GOD))

extern long contents_hd, observable_hd;

struct object;

enum object_flags {
	OF_INF = 1,
};

struct observer_node {
        struct object *who;
        struct observer_node *next;
};

struct icon {
	int actions;
	char *icon;
};

extern dbref db_top;

extern struct object *db;

OBJ obj_get(dbref ref);
void obj_set(dbref ref, OBJ *obj);

static inline int
object_item(dbref obj_ref)
{
	OBJ obj = obj_get(obj_ref);
	return obj.type == TYPE_THING
		|| obj.type == TYPE_CONSUMABLE
		|| obj.type == TYPE_EQUIPMENT;
}

dbref object_new(OBJ *obj);
dbref object_add(OBJ *nu, SKEL *o, dbref where, void *arg);
char *object_art(dbref ref);
void object_clear(OBJ *obj);
void object_free(dbref ref);
dbref objects_write(FILE *f);
dbref objects_read(FILE *f);
dbref object_copy(OBJ *nu, dbref old_ref);
void objects_update(double dt);
OBJ *object_parent(OBJ *obj);
int object_plc(OBJ *source, OBJ *dest); /* parent loop check */
void object_move(dbref what_ref, dbref where_ref);
void object_drop(dbref where_ref, struct drop **drop);
struct icon object_icon(dbref thing_ref);
/* TODO observables share DB */
void observer_add(dbref observable_ref, dbref observer_ref);
int observer_remove(dbref observable_ref, dbref observer_ref);
unsigned art_max(char *name);
int objects_init();
void objects_sync();
struct hash_cursor contents_iter(dbref parent);
dbref contents_next(struct hash_cursor *c);
void contents_put(dbref parent, dbref child);

#endif

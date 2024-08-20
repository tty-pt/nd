#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdio.h>
#include <qhash.h>
#include "uapi/object.h"
#include "skeleton.h"

#define NOTHING ((dbref) -1)
#define FOR_ALL(var) for (var = object_get(db_top); var-- > object_get(0); )

#define GOD ((dbref) 1)
#define God(obj) (object_ref(obj) == (GOD))

extern long contents_hd;

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

OBJ * object_get(register dbref ref);
dbref object_ref(register OBJ *obj);

static inline int
object_item(register OBJ *obj)
{
	return obj->type == TYPE_THING
		|| obj->type == TYPE_CONSUMABLE
		|| obj->type == TYPE_EQUIPMENT;
}

OBJ *object_new(void);
OBJ *object_add(SKEL *o, OBJ *where, void *arg);
char *object_art(OBJ *obj);
void object_clear(OBJ *obj);
void object_free(OBJ *obj);
OBJ *objects_write(FILE *f);
OBJ *objects_read(FILE *f);
void objects_free(void);
OBJ *object_copy(OBJ *player, OBJ *obj);
void objects_update(double dt);
OBJ *object_parent(OBJ *obj);
int object_plc(OBJ *source, OBJ *dest); /* parent loop check */
void object_move(OBJ *what, OBJ *where);
void object_drop(OBJ *where, struct drop **drop);
struct icon object_icon(OBJ *thing);
void observer_add(OBJ *observable, OBJ *observer);
int observer_remove(OBJ *observable, OBJ *observer);
unsigned art_max(char *name);
int objects_init();
void objects_sync();
struct hash_cursor contents_iter(dbref parent);
OBJ *contents_next(struct hash_cursor *c);
void contents_put(dbref parent, dbref child);

#endif

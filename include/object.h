#ifndef OBJECT_H
#define OBJECT_H

#include <stddef.h>
#include <stdio.h>
#include "skeleton.h"

#define NOTHING ((dbref) -1)
#define FOR_ALL(var) for (var = object_get(db_top); var-- > object_get(0); )
#define FOR_LIST(var, first) for ((var) = (first); var; (var) = var->next)
#define PUSH(thing, locative) { thing->next = (locative); (locative) = thing; }

#define GOD ((dbref) 1)
#define God(obj) (object_ref(obj) == (GOD))

typedef int dbref;

struct object;

enum type {
	TYPE_ROOM,
	TYPE_THING,
	TYPE_PLANT,
	TYPE_ENTITY,
	TYPE_EQUIPMENT,
	TYPE_CONSUMABLE,
	TYPE_GARBAGE,
	TYPE_SEAT,
	TYPE_MINERAL,
};

enum object_flags {
	OF_INF = 1,
};

struct wts {
	const char *a, *b;
};

struct observer_node {
        struct object *who;
        struct observer_node *next;
};

enum room_flags {
	RF_TEMP = 1,
	RF_HAVEN = 2,
};

enum exit {
	E_NULL = 0,
	E_WEST = 1,
	E_NORTH = 2,
	E_UP = 4,
	E_EAST = 8,
	E_SOUTH = 16,
	E_DOWN = 32,
	E_ALL = 63,
};

typedef struct {
	struct object *dropto;
	unsigned flags;
	unsigned char exits;
	unsigned char doors;
	unsigned char floor;
} ROO;

enum entity_flags {
	EF_PLAYER = 1,
	EF_AGGRO = 2,
	EF_SITTING = 4,
	EF_SHOP = 8,
	EF_WIZARD = 16,
	EF_BUILDER = 32,
};

enum wt {
	WT_PUNCH,
	WT_PECK,
	WT_SLASH,
	WT_BITE,
	WT_STRIKE,
};

enum attribute {
	ATTR_STR,
	ATTR_CON,
	ATTR_DEX,
	ATTR_INT,
	ATTR_WIZ,
	ATTR_CHA,
	ATTR_MAX
};

enum equipment_slot {
	ES_HEAD,
	ES_NECK,
	ES_CHEST,
	ES_BACK,
	ES_RHAND,
	ES_LFINGER,
	ES_RFINGER,
	ES_PANTS,
	ES_MAX
};

struct debuf {
	struct spell_skeleton *_sp;
	unsigned duration;
	short val;
};

struct spell {
	struct spell_skeleton *_sp;
	unsigned cost; 
	unsigned short val;
};

enum spell_affects {
	// these are changed by bufs
	AF_HP,
	AF_MOV,
	AF_MDMG,
	AF_MDEF,
	AF_DODGE,

	// these aren't.
	AF_DMG,
	AF_DEF,

	// these are flags, not types of buf
	AF_NEG = 0x10,
	AF_BUF = 0x20,
};

struct effect {
	short value;
	unsigned char mask;
};

typedef struct entity {
	struct object *home;
	unsigned fds;
	struct object *last_observed, *dialog_target;
        const char *dialog;

	struct debuf debufs[8];
	struct spell spells[8];
	struct effect e[7];
	struct object *target, *sat;
	unsigned char wtso;
	struct wts wts;
	unsigned flags; // TODO merge these two
	unsigned short hp, mp, hunger, thirst;
	unsigned char debuf_mask, combo, select, klock;
	unsigned lvl, spend, cxp;
	unsigned attr[ATTR_MAX];
	unsigned equipment[ES_MAX];
	unsigned char hunger_n, thirst_n;
	char *gpt;
} ENT;

enum equipment_flags {
	EQF_EQUIPPED = 1,
};

enum armor_type {
	ARMOR_LIGHT,
	ARMOR_MEDIUM,
	ARMOR_HEAVY,
};

typedef struct {
	unsigned eqw;
	unsigned msv;
	unsigned rare;
	unsigned flags;
} EQU;

typedef struct {
	unsigned food;
	unsigned drink;
	unsigned quantity;
	unsigned capacity;
} CON;

typedef struct {
	unsigned plid;
	unsigned size;
} PLA;

typedef struct {
	unsigned quantity;
	unsigned capacity;
} SEA;

typedef struct {
	short unsigned unused;
} MIN;

union specific {
	ROO room;
	ENT entity;
	EQU equipment;
	CON consumable;
	PLA plant;
	SEA seat;
	MIN mineral;
};

typedef struct object {
        struct observer_node *first_observer;
	char *name, *description;
	struct object *location, *owner, *contents, *next;

	unsigned art_id;
	unsigned char type;
	unsigned value;
	unsigned flags;

	union specific sp;
	/* int skid; */
} OBJ;

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
void objects_update(unsigned long long dt);
OBJ *object_parent(OBJ *obj);
int object_plc(OBJ *source, OBJ *dest); /* parent loop check */
void object_move(OBJ *what, OBJ *where);
void object_drop(OBJ *where, struct drop **drop);
struct icon object_icon(OBJ *thing);
void observer_add(OBJ *observable, OBJ *observer);
int observer_remove(OBJ *observable, OBJ *observer);
unsigned art_max(char *name);

#endif

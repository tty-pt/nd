/* $Header$ */

#include "copyright.h"

#ifndef __MDB_H
#define __MDB_H

#include "config.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mob.h"

#define BUFFER_LEN 8192

/* defines for possible data access mods. */
#define GETMESG(x,y)   (get_property_class(x, y))
#define SETMESG(x,y,z)    {add_prop_nofetch(x, y, z, 0);}

#define MESGPROP_INF	"_/inf"
#define SETINF(x,y)	set_property_value(x, MESGPROP_INF, y)
#define GETINF(x)	get_property_value(x, MESGPROP_INF)
#define USETINF(x)	remove_property(x, MESGPROP_INF)

#define SETHASH(w, x, y, z)	set_property_hash(w, x, y, z)
#define GETHASH(w, x, y)	get_property_hash(w, x, y)

/* equipment props {{{ */

// what is equipped (player)

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

enum wt { PUNCH, PECK, SLASH, BITE, STRIKE, };
enum at { ARMOR_LIGHT, ARMOR_MEDIUM, ARMOR_HEAVY, };

#define EQT(x)		(x>>6)
#define EQ(i, t)	(i | (t<<6))
#define EQL(x)		(x & 15)

/* }}} */

#define MESGPROP_CUR	"_/cur"

#define MESGPROP_CUR_SPELLS MESGPROP_CUR "/@spells"
#define GETCURSPELLS(x)  GETMESG(x, MESGPROP_CUR_SPELLS)
#define SETCURSPELLS(x, y)  SETMESG(x, MESGPROP_CUR_SPELLS, y)

#define MESGPROP_SPELLS "@/spells"
#define GETSPELLS(x, a)  get_property_mark(x, MESGPROP_SPELLS, a)
#define SETSPELLS(x, a, v)  set_property_mark(x, MESGPROP_SPELLS, a, v)

enum type {
	TYPE_ROOM,
	TYPE_THING,
	TYPE_PLANT,
	TYPE_ENTITY,
	TYPE_EQUIPMENT,
	TYPE_CONSUMABLE,
	TYPE_GARBAGE,
};

#define is_item(x) (OBJECT(x)->type == TYPE_THING || OBJECT(x)->type == TYPE_CONSUMABLE || OBJECT(x)->type == TYPE_EQUIPMENT)

/* enum object_flags { */
/* }; */

enum object_flags {
	OF_SANEBIT = 1,
};

#define GOD ((dbref) 1)

#ifdef GOD_PRIV
#define God(x) ((x) == (GOD))
#endif							/* GOD_PRIV */

/* special dbref's */
#define NOTHING ((dbref) -1)	/* null dbref */
#define AMBIGUOUS ((dbref) -2)	/* multiple possibilities, for matchers */

enum room_flags {
	RF_TEMP = 1,
	RF_HAVEN = 2,
};

typedef struct {
	dbref dropto;
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

enum attribute {
	ATTR_STR,
	ATTR_CON,
	ATTR_DEX,
	ATTR_INT,
	ATTR_WIZ,
	ATTR_MAX
};

typedef struct entity {
	dbref home;
	int fd;
	dbref last_observed;
        dbref dialog_target;
        const char *dialog;

	struct debuf debufs[8];
	struct spell spells[8];
	effect_t e[7];
	dbref target, sat;
	unsigned char wtso;
	struct wts wts;
	unsigned respawn_in, flags; // TODO merge these two
	unsigned short hp, mp, hunger, thirst;
	unsigned char debuf_mask, combo, select, klock;
	unsigned lvl, spend, cxp;
	unsigned attributes[ATTR_MAX];
	unsigned equipment[ES_MAX];
} ENT;

enum equipment_flags {
	EF_EQUIPPED = 1,
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

#define OBJECT(x)		(&db[x])
#define REF(op)			((dbref) ((op - db) / sizeof(OBJ)))
#define ENTITY(x)		(&db[x].sp.entity)
#define ROOM(x)			(&db[x].sp.room)
#define ATTR(x, y)		db[x].sp.entity.attributes[y]
#define EQUIPMENT(x)		(&db[x].sp.equipment)
#define EQUIP(x, y)		ENTITY(x)->equipment[y]
#define CONSUM(x)		(&db[x].sp.consumable)
#define PLANT(x)		(&db[x].sp.plant)

/* union of type-specific fields */

union specific {
	ROO room;
	ENT entity;
	EQU equipment;
	CON consumable;
	PLA plant;
};

/* timestamps record */

struct observer_node {
        dbref who;
        struct observer_node *next;
};

typedef struct object {
        struct observer_node *first_observer;
	const char *name, *description, *art, *avatar;
	dbref location;				/* pointer to container */
	dbref owner;
	dbref contents;
	dbref next;					/* pointer to next in contents/exits chain */
	struct plist *properties;

	unsigned char type;
	unsigned char flags;
	unsigned value;

	union specific sp;
	/* int skid; */
} OBJ;

/* Possible data types that may be stored in a hash table */
union u_hash_data {
	dbref dbval;	/* Player hashing will want this */
	void *pval;	/* Command hashing wants this */
};

/* The actual hash entry for each item */
struct t_hash_entry {
	struct t_hash_entry *next;	/* Pointer for conflict resolution */
	const char *name;			/* The name of the item */
	union u_hash_data dat;		/* Data value for item */
};

typedef union u_hash_data hash_data;
typedef struct t_hash_entry hash_entry;
typedef hash_entry *hash_tab;

#define PLAYER_HASH_SIZE   (1024)	/* Table for player lookups */

extern struct object *db;
extern dbref db_top;

extern char *alloc_string(const char *);
extern dbref object_new(void);		/* return a new object */

extern dbref db_write(FILE * f);	/* write db to file, return # of objects */
extern dbref db_read(FILE * f);	/* read db from file, return # of objects */
extern void db_obs_add(dbref observable, dbref observer);
extern int db_obs_remove(dbref observable, dbref observer);

 /* Warning: destroys existing db contents! */

extern void db_free(void);

time_t get_tick();
void objects_update();
dbref getparent(dbref obj);

#define DOLIST(var, first) \
  for((var) = (first); (var) != NOTHING; (var) = db[var].next)
#define PUSH(thing, locative) \
    {db[thing].next = (locative); (locative) = (thing);}
#define getloc(thing) (db[thing].location)

#endif							/* __MDB_H */

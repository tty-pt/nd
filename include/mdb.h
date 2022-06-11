/* $Header$ */

#include "copyright.h"

#ifndef __MDB_H
#define __MDB_H

#include "config.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

#include "mob.h"
#include "object.h"

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

#define GOD ((dbref) 1)

#ifdef GOD_PRIV
#define God(obj) (object_ref(obj) == (GOD))
#endif							/* GOD_PRIV */

#define ATTR(ent, y)		ent->attr[y]
#define EQUIP(ent, y)		ent->equipment[y]

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

extern dbref db_top;

extern char *alloc_string(const char *);

 /* Warning: destroys existing db contents! */

time_t get_tick();

#define PUSH(thing, locative) \
    {thing->next = (locative); (locative) = thing;}

#endif							/* __MDB_H */

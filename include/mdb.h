/* $Header$ */

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

#define SETHASH(w, x, y, z)	set_property_hash(w, x, y, z)
#define GETHASH(w, x, y)	get_property_hash(w, x, y)

/* equipment props {{{ */

// what is equipped (player)

enum wt { PUNCH, PECK, SLASH, BITE, STRIKE, };

#define EQ(i, t)	(i | (t<<6))

/* }}} */

#define MESGPROP_CUR	"_/cur"

#define MESGPROP_CUR_SPELLS MESGPROP_CUR "/@spells"
#define GETCURSPELLS(x)  GETMESG(x, MESGPROP_CUR_SPELLS)
#define SETCURSPELLS(x, y)  SETMESG(x, MESGPROP_CUR_SPELLS, y)

#define MESGPROP_SPELLS "@/spells"
#define GETSPELLS(x, a)  get_property_mark(x, MESGPROP_SPELLS, a)
#define SETSPELLS(x, a, v)  set_property_mark(x, MESGPROP_SPELLS, a, v)

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

#endif							/* __MDB_H */

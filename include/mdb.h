#ifndef __MDB_H
#define __MDB_H

#include "config.h"

#include <stdio.h>

#include "mob.h"
#include "object.h"

#define BUFFER_LEN 8192

#define EQ(i, t)	(i | (t<<6))

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

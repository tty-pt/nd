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

#define NAME(x)     (db[x].name)
#define FLAGS(x)    (db[x].flags)
#define OWNER(x)    (db[x].owner)

/* defines for possible data access mods. */
#define MESGPROP_DESC		"_/de"
#define MESGPROP_LOCK		"_/lok"
#define MESGPROP_CONLOCK	"_/clk"
#define MESGPROP_VALUE		"@/value"

#define GETMESG(x,y)   (get_property_class(x, y))
#define GETDESC(x)	GETMESG(x, MESGPROP_DESC)

#define SETMESG(x,y,z)    {add_prop_nofetch(x, y, z, 0);}
#define SETDESC(x,y)	SETMESG(x, MESGPROP_DESC, y)

#define GETLOCK(x)    (get_property_lock(x, MESGPROP_LOCK))
#define SETLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_LOCK, &mydat);}
#define CLEARLOCK(x)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = TRUE_BOOLEXP; set_property_nofetch(x, MESGPROP_LOCK, &mydat); }

#define SETCONLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_CONLOCK, &mydat);}

#define GETVALUE(x)	get_property_value(x, MESGPROP_VALUE)
#define SETVALUE(x,y)	add_prop_nofetch(x, MESGPROP_VALUE, NULL, y)

#define MESGPROP_WTS	"_/wts"
#define SETWTS(x,y)	set_property_value(x, MESGPROP_WTS, y)
#define GETWTS(x)	get_property_value(x, MESGPROP_WTS)

#define MESGPROP_SIZE	"_/size"
#define SETSIZE(x,y)	set_property_value(x, MESGPROP_SIZE, y)
#define GETSIZE(x)	get_property_value(x, MESGPROP_SIZE)
#define USETSIZE(x)	remove_property(x, MESGPROP_SIZE)

#define MESGPROP_INF	"_/inf"
#define SETINF(x,y)	set_property_value(x, MESGPROP_INF, y)
#define GETINF(x)	get_property_value(x, MESGPROP_INF)
#define USETINF(x)	remove_property(x, MESGPROP_INF)

#define MESGPROP_FOOD	"_/food"
#define GETFOOD(x)	(get_property_value(x, MESGPROP_FOOD) - 1)
#define SETFOOD(x,y)	set_property_value(x, MESGPROP_FOOD, y + 1)

#define MESGPROP_DRINK	"_/drink"
#define GETDRINK(x)	(get_property_value(x, MESGPROP_DRINK) - 1)
#define SETDRINK(x, y)	set_property_value(x, MESGPROP_DRINK, y + 1)

#define MESGPROP_CONSUM	"_/consu_m"
#define GETCONSUM(x)	get_property_value(x, MESGPROP_CONSUM)
#define SETCONSUM(x, y)	set_property_value(x, MESGPROP_CONSUM, y)

#define MESGPROP_CONSUN	"_/consu_n"
#define GETCONSUN(x)	get_property_value(x, MESGPROP_CONSUN)
#define SETCONSUN(x, y)	set_property_value(x, MESGPROP_CONSUN, y)

#define MESGPROP_PLID	"_/plid"
#define GETPLID(x)	(get_property_value(x, MESGPROP_PLID) - 1)
#define SETPLID(x, y)	set_property_value(x, MESGPROP_PLID, y + 1)

#define MESGPROP_DOOR	"_/door"
#define GETDOOR(x)	get_property_value(x, MESGPROP_DOOR)
#define SETDOOR(x, y)	set_property_value(x, MESGPROP_DOOR, y)

#define MESGPROP_LVL	"_/lvl"
#define GETLVL(x)	1 + get_property_value(x, MESGPROP_LVL)
#define SETLVL(x, y)	set_property_value(x, MESGPROP_LVL, y - 1)

#define SETHASH(w, x, y, z)	set_property_hash(w, x, y, z)
#define GETHASH(w, x, y)	get_property_hash(w, x, y)

/* stat props {{{ */

enum st { NOSTAT, STR, CON, DEX, INT, WIZ };
#define STAT_MAX 5

#define MESGPROP_STAT	"_/st"
#define GETSTAT(x, y)	(1 + GETHASH(x, MESGPROP_STAT, y))
#define SETSTAT(x, y, z) SETHASH(x, MESGPROP_STAT, y, z - 1)

/* }}} */

/* equipment props {{{ */

// what is equipped (player)

enum eq { NOEQ, HEAD, NECK, CHEST, BACK, RHAND, LFINGER, RFINGER, PANTS };
enum wt { PUNCH, PECK, SLASH, BITE, STRIKE, };
enum at { ARMOR_LIGHT, ARMOR_MEDIUM, ARMOR_HEAVY, };
/* enum at { NAKED, CLOTH, LEATHER, }; */

#define EQ_MAX		9
#define EQT(x)		(x>>6)
#define EQ(i, t)	(i | (t<<6))
#define EQL(x)		(x & 15)

#define MESGPROP_EQ	"_/eq"
#define GETEQ(x, y)	((dbref) GETHASH(x, MESGPROP_EQ, y))
#define SETEQ(x, y, z)	SETHASH(x, MESGPROP_EQ, EQL(y), z)

// where to equip / type of equip (item)
#define GETEQW(x)	get_property_value(x, MESGPROP_EQ)
#define SETEQW(x, y)	set_property_value(x, MESGPROP_EQ, y)
#define GETEQT(x)	EQT(GETEQW(x))
#define GETEQL(x)	EQL(GETEQW(x))

// mininum stat value (item)
#define MESGPROP_MSV	"_/msv"
#define GETMSV(x)	get_property_value(x, MESGPROP_MSV)
#define SETMSV(x, y)	set_property_value(x, MESGPROP_MSV, y)

// rarity (item)
#define MESGPROP_RARE	"_/rare"
#define GETRARE(x)	get_property_value(x, MESGPROP_RARE)
#define SETRARE(x, y)	set_property_value(x, MESGPROP_RARE, y)

/* }}} */

#define MESGPROP_CUR	"_/cur"

#define MESGPROP_COMBO	MESGPROP_CUR "/combo"
#define GETCOMBO(x)    get_property_value(x, MESGPROP_COMBO)
#define SETCOMBO(x, y) set_property_value(x, MESGPROP_COMBO, y)

#define MESGPROP_CUR_SPELLS MESGPROP_CUR "/@spells"
#define GETCURSPELLS(x)  GETMESG(x, MESGPROP_CUR_SPELLS)
#define SETCURSPELLS(x, y)  SETMESG(x, MESGPROP_CUR_SPELLS, y)

#define MESGPROP_SPELLS "@/spells"
#define GETSPELLS(x, a)  get_property_mark(x, MESGPROP_SPELLS, a)
#define SETSPELLS(x, a, v)  set_property_mark(x, MESGPROP_SPELLS, a, v)

#define MESGPROP_SHOP	"_/shop"
#define GETSHOP(x)	get_property_value(x, MESGPROP_SHOP)

#define MESGPROP_ART	"@/art"
#define GETART(x)	envpropstr(x, MESGPROP_ART)
#define SETART(x, y)	SETMESG(x, MESGPROP_ART, y)

#define MESGPROP_AVATAR	"_/avatar"
#define GETAVATAR(x)	GETMESG(x, MESGPROP_AVATAR)
#define SETAVATAR(x, y)	SETMESG(x, MESGPROP_AVATAR, y)

#define TYPE_ROOM           0x0
#define TYPE_THING          0x1
#define TYPE_EXIT           0x2
#define TYPE_ENTITY         0x3
#define NOTYPE1				0x5 /* Room for expansion */
#define TYPE_GARBAGE        0x6
#define NOTYPE              0x7	/* no particular type */
#define TYPE_MASK           0x7	/* room for expansion */

#define WIZARD             0x10	/* gets automatic control */
#define LINK_OK            0x20	/* anybody can link to this */
#define DARK               0x40	/* contents of room are not printed */

/* This #define disabled to avoid accidentally triggerring debugging code */
/* #define DEBUG DARK */	/* Used to print debugging information on
				 * on MUF programs */

#define RESERVED          0x100	/* this object goes home when dropped */
#define BUILDER           0x200	/* this player can use construction commands */
#define BOUND BUILDER
#define CHOWN_OK          0x400	/* this object can be @chowned, or
									this player can see color */
#define COLOR CHOWN_OK
#define JUMP_OK           0x800	/* A room which can be jumped from, or
								 * a player who can be jumped to */
#define EXPANSION1		 0x1000 /* Expansion bit */
#define EXPANSION2		 0x2000 /* Expansion bit */
#define KILL_OK	         0x4000	/* Kill_OK bit.  Means you can be killed. */
#define EXPANSION3		 0x8000 /* Expansion bit */
#define HAVEN           0x10000	/* can't kill here */
#define HIDE HAVEN
#define ABODE           0x20000	/* can set home here */
#define ABATE ABODE
#define QUELL           0x80000	/* When set, wiz-perms are turned off */
#define INTERACTIVE    0x200000	/* internal: denotes player is in editor, or
								 * muf READ. */
#define SAVED_DELTA    0x800000	/* internal: object last saved to delta file */
#define READMODE     0x10000000	/* internal: when set, player is in a READ */
#define SANEBIT      0x20000000	/* internal: used to check db sanity */


/* what flags to NOT dump to disk. */
#define DUMP_MASK    (INTERACTIVE | SAVED_DELTA | READMODE | SANEBIT)


typedef long object_flag_type;

#define GOD ((dbref) 1)

#ifdef GOD_PRIV
#define God(x) ((x) == (GOD))
#endif							/* GOD_PRIV */

#define DoNull(s) ((s) ? (s) : "")
#define Typeof(x) (x == HOME ? TYPE_ROOM : (FLAGS(x) & TYPE_MASK))
#define Wizard(x) ((FLAGS(x) & WIZARD) != 0 && (FLAGS(x) & QUELL) == 0)

/* TrueWizard is only appropriate when you care about whether the person
   or thing is, well, truely a wizard. Ie it ignores QUELL. */
#define TrueWizard(x) ((FLAGS(x) & WIZARD) != 0)
#define Dark(x) ((FLAGS(x) & DARK) != 0)

#define Builder(x) ((FLAGS(x) & (WIZARD|BUILDER)) != 0)

#define Linkable(x) ((x) == HOME || \
                     (((Typeof(x) == TYPE_ROOM || Typeof(x) == TYPE_THING) ? \
                      (FLAGS(x) & ABODE) : (FLAGS(x) & LINK_OK)) != 0))


/* Boolean expressions, for locks */
typedef char boolexp_type;

#define BOOLEXP_AND 0
#define BOOLEXP_OR 1
#define BOOLEXP_NOT 2
#define BOOLEXP_CONST 3
#define BOOLEXP_PROP 4

struct boolexp {
	boolexp_type type;
	struct boolexp *sub1;
	struct boolexp *sub2;
	dbref thing;
	struct plist *prop_check;
};

#define TRUE_BOOLEXP ((struct boolexp *) 0)

/* special dbref's */
#define NOTHING ((dbref) -1)	/* null dbref */
#define AMBIGUOUS ((dbref) -2)	/* multiple possibilities, for matchers */
#define HOME ((dbref) -3)		/* virtual room, represents mover's home */

enum entity_flags {
	EF_PLAYER = 1,
	EF_AGGRO = 2,
	EF_SITTING = 4,
};

struct entity {
	dbref home;
	int fd;
	dbref last_observed;
        dbref dialog_target;
        const char *dialog;

	struct debuf debufs[8];
	struct spell spells[8];
	effect_t e[7];
	dbref target, sat;
	struct wts wts;
	unsigned respawn_in, flags; // TODO merge these two
	unsigned short hp, mp, hunger, thirst;
	unsigned char debuf_mask, combo, select, klock;
};

#define ENTITY(x)		(&db[x].sp.entity)
#define ROOM(x)			(&db[x].sp.room)

enum room_flags {
	RF_TEMP = 1,
};

/* union of type-specific fields */

union specific {				/* I've been railroaded! */
	struct {					/* ROOM-specific fields */
		dbref dropto;
		unsigned flags;
		/* unsigned char exits; */
		unsigned char doors;
		dbref exits;
		unsigned char floor;
	} room;
	struct {					/* EXIT-specific fields */
		int ndest;
		dbref *dest;
	} exit;
	struct entity entity;
};

/* timestamps record */

struct observer_node {
        dbref who;
        struct observer_node *next;
};

struct object {
        struct observer_node *first_observer;
	const char *name;
	dbref location;				/* pointer to container */
	dbref owner;
	dbref contents;
	dbref next;					/* pointer to next in contents/exits chain */
	struct plist *properties;

	object_flag_type flags;

	union specific sp;
	/* struct object_skeleton *skel; */
};

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

extern struct boolexp *getboolexp(FILE *);	/* get a boolexp */
extern void putboolexp(FILE *, struct boolexp *);	/* put a boolexp */

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

/*
  Usage guidelines:

  To obtain an object use db[i].

  The programmer is responsible for managing storage for string
  components of entries; db_read will produce malloc'd strings.  The
  alloc_string routine is provided for generating malloc'd strings
  duplicates of other strings.  Note that db_free and db_read will
  attempt to free any non-NULL string that exists in db when they are
  invoked.
*/
#endif							/* __MDB_H */

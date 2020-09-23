/* $Header$ */

#include "copyright.h"

#ifndef __DB_H
#define __DB_H

#include "config.h"
#include "defines.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

#define BUFFER_LEN 8192

extern char match_args[BUFFER_LEN];
extern char match_cmdname[BUFFER_LEN];

typedef int dbref;				/* offset into db */

#define DBFETCH(x)  (db + (x))
#define DBDIRTY(x)  {db[x].flags |= OBJECT_CHANGED;}
#define DBSTORE(x, y, z)    {DBFETCH(x)->y = z; DBDIRTY(x);}

#define NAME(x)     (db[x].name)
#define FLAGS(x)    (db[x].flags)
#define OWNER(x)    (db[x].owner)

/* defines for possible data access mods. */
#define MESGPROP_DESC		"_/de"
#define MESGPROP_IDESC		"_/ide"
#define MESGPROP_SUCC		"_/sc"
#define MESGPROP_OSUCC		"_/osc"
#define MESGPROP_FAIL		"_/fl"
#define MESGPROP_OFAIL		"_/ofl"
#define MESGPROP_DROP		"_/dr"
#define MESGPROP_ODROP		"_/odr"
#define MESGPROP_DOING		"_/do"
#define MESGPROP_OECHO		"_/oecho"
#define MESGPROP_PECHO		"_/pecho"
#define MESGPROP_LOCK		"_/lok"
#define MESGPROP_FLOCK		"@/flk"
#define MESGPROP_CONLOCK	"_/clk"
#define MESGPROP_CHLOCK		"_/chlk"
#define MESGPROP_VALUE		"@/value"
#define MESGPROP_GUEST		"@/isguest"

#define GETMESG(x,y)   (get_property_class(x, y))
#define GETDESC(x)	GETMESG(x, MESGPROP_DESC)
#define GETIDESC(x)	GETMESG(x, MESGPROP_IDESC)
#define GETSUCC(x)	GETMESG(x, MESGPROP_SUCC)
#define GETOSUCC(x)	GETMESG(x, MESGPROP_OSUCC)
#define GETFAIL(x)	GETMESG(x, MESGPROP_FAIL)
#define GETOFAIL(x)	GETMESG(x, MESGPROP_OFAIL)
#define GETDROP(x)	GETMESG(x, MESGPROP_DROP)
#define GETODROP(x)	GETMESG(x, MESGPROP_ODROP)
#define GETDOING(x)	GETMESG(x, MESGPROP_DOING)
#define GETOECHO(x)	GETMESG(x, MESGPROP_OECHO)
#define GETPECHO(x)	GETMESG(x, MESGPROP_PECHO)

#define SETMESG(x,y,z)    {add_property(x, y, z, 0);}
#define SETDESC(x,y)	SETMESG(x, MESGPROP_DESC, y)
#define SETIDESC(x,y)	SETMESG(x, MESGPROP_IDESC, y)
#define SETSUCC(x,y)	SETMESG(x, MESGPROP_SUCC, y)
#define SETFAIL(x,y)	SETMESG(x, MESGPROP_FAIL, y)
#define SETDROP(x,y)	SETMESG(x, MESGPROP_DROP, y)
#define SETOSUCC(x,y)	SETMESG(x, MESGPROP_OSUCC, y)
#define SETOFAIL(x,y)	SETMESG(x, MESGPROP_OFAIL, y)
#define SETODROP(x,y)	SETMESG(x, MESGPROP_ODROP, y)
#define SETDOING(x,y)	SETMESG(x, MESGPROP_DOING, y)
#define SETOECHO(x,y)	SETMESG(x, MESGPROP_OECHO, y)
#define SETPECHO(x,y)	SETMESG(x, MESGPROP_PECHO, y)

#define LOADMESG(x,y,z)    {add_prop_nofetch(x,y,z,0); DBDIRTY(x);}
#define LOADDESC(x,y)	LOADMESG(x, MESGPROP_DESC, y)
#define LOADIDESC(x,y)	LOADMESG(x, MESGPROP_IDESC, y)
#define LOADSUCC(x,y)	LOADMESG(x, MESGPROP_SUCC, y)
#define LOADFAIL(x,y)	LOADMESG(x, MESGPROP_FAIL, y)
#define LOADDROP(x,y)	LOADMESG(x, MESGPROP_DROP, y)
#define LOADOSUCC(x,y)	LOADMESG(x, MESGPROP_OSUCC, y)
#define LOADOFAIL(x,y)	LOADMESG(x, MESGPROP_OFAIL, y)
#define LOADODROP(x,y)	LOADMESG(x, MESGPROP_ODROP, y)

#define GETLOCK(x)    (get_property_lock(x, MESGPROP_LOCK))
#define SETLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property(x, MESGPROP_LOCK, &mydat);}
#define LOADLOCK(x,y) {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_LOCK, &mydat); DBDIRTY(x);}
#define CLEARLOCK(x)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = TRUE_BOOLEXP; set_property(x, MESGPROP_LOCK, &mydat); DBDIRTY(x);}

#define GETFLOCK(x)    (get_property_lock(x, MESGPROP_FLOCK))
#define SETFLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property(x, MESGPROP_FLOCK, &mydat);}
#define LOADFLOCK(x,y) {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_FLOCK, &mydat); DBDIRTY(x);}
#define CLEARFLOCK(x)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = TRUE_BOOLEXP; set_property(x, MESGPROP_FLOCK, &mydat); DBDIRTY(x);}

#define GETCONLOCK(x)    (get_property_lock(x, MESGPROP_CONLOCK))
#define SETCONLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property(x, MESGPROP_CONLOCK, &mydat);}
#define LOADCONLOCK(x,y) {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_CONLOCK, &mydat); DBDIRTY(x);}
#define CLEARCONLOCK(x)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = TRUE_BOOLEXP; set_property(x, MESGPROP_CONLOCK, &mydat); DBDIRTY(x);}

#define GETCHLOCK(x)    (get_property_lock(x, MESGPROP_CHLOCK))
#define SETCHLOCK(x,y)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property(x, MESGPROP_CHLOCK, &mydat);}
#define LOADCHLOCK(x,y) {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = y; set_property_nofetch(x, MESGPROP_CHLOCK, &mydat); DBDIRTY(x);}
#define CLEARCHLOCK(x)  {PData mydat; mydat.flags = PROP_LOKTYP; mydat.data.lok = TRUE_BOOLEXP; set_property(x, MESGPROP_CHLOCK, &mydat); DBDIRTY(x);}

#define GETVALUE(x)	get_property_value(x, MESGPROP_VALUE)
#define SETVALUE(x,y)	add_property(x, MESGPROP_VALUE, NULL, y)
#define LOADVALUE(x,y)	add_prop_nofetch(x, MESGPROP_VALUE, NULL, y)

#define MESGPROP_LID	"_/lid"
#define SETLID(x,y)	set_property_value(x, MESGPROP_LID, y + 1)
#define GETLID(x)	(get_property_value(x, MESGPROP_LID) - 1)

#define MESGPROP_AGGRO	"_/aggro"
#define SETAGGRO(x, y)	set_property_value(x, MESGPROP_AGGRO, y)
#define GETAGGRO(x)	get_property_value(x, MESGPROP_AGGRO)

#define MESGPROP_WTS	"_/wts"
#define SETWTS(x,y)	set_property_value(x, MESGPROP_WTS, y)
#define GETWTS(x)	get_property_value(x, MESGPROP_WTS)

#define MESGPROP_STACK	"_/stack"
#define SETSTACK(x,y)	set_property_value(x, MESGPROP_STACK, y)
#define GETSTACK(x)	get_property_value(x, MESGPROP_STACK)
#define USETSTACK(x)	remove_property(x, MESGPROP_STACK)

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

#define MESGPROP_TREE	"_/tree"
#define GETTREE(x)	get_property_value(x, MESGPROP_TREE)
#define SETTREE(x, y)	set_property_value(x, MESGPROP_TREE, y)

#define MESGPROP_PLID	"_/plid"
#define GETPLID(x)	(get_property_value(x, MESGPROP_PLID) - 1)
#define SETPLID(x, y)	set_property_value(x, MESGPROP_PLID, y + 1)

#define MESGPROP_DOOR	"_/door"
#define GETDOOR(x)	get_property_value(x, MESGPROP_DOOR)
#define SETDOOR(x, y)	set_property_value(x, MESGPROP_DOOR, y)

#define MESGPROP_TMP	"_/tmp"
#define GETTMP(x)	get_property_value(x, MESGPROP_TMP)
#define SETTMP(x, y)	set_property_value(x, MESGPROP_TMP, y)

#define MESGPROP_FLOOR	"_/floor"
#define GETFLOOR(x)	(get_property_value(x, MESGPROP_FLOOR) - 1)
#define SETFLOOR(x, y)	set_property_value(x, MESGPROP_FLOOR, y + 1)

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
#define SETSTAT(x, y, z) SETHASH(x, MESGPROP_STAT, y, z)

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

#define ISGUEST(x)	(get_property(x, MESGPROP_GUEST) != NULL)
#define NOGUEST(_cmd,x) if(ISGUEST(x)) { char tmpstr[BUFFER_LEN]; warn("Guest %s(#%d) failed attempt to %s.\n",NAME(x),x,_cmd); snprintf(tmpstr, sizeof(tmpstr), "Guests are not allowed to %s.\r", _cmd); notify_nolisten(x,tmpstr,1); return; }

#define TYPE_ROOM           0x0
#define TYPE_THING          0x1
#define TYPE_EXIT           0x2
#define TYPE_PLAYER         0x3
#define NOTYPE1				0x5 /* Room for expansion */
#define TYPE_GARBAGE        0x6
#define NOTYPE              0x7	/* no particular type */
#define TYPE_MASK           0x7	/* room for expansion */

#define EXPANSION0		   0x08 /* Not a flag, but one add'l flag for
								 * expansion purposes */

#define WIZARD             0x10	/* gets automatic control */
#define LINK_OK            0x20	/* anybody can link to this */
#define DARK               0x40	/* contents of room are not printed */

/* This #define disabled to avoid accidentally triggerring debugging code */
/* #define DEBUG DARK */	/* Used to print debugging information on
				 * on MUF programs */

#define INTERNAL           0x80	/* internal-use-only flag */
#define STICKY            0x100	/* this object goes home when dropped */
#define SILENT STICKY
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
#define OBJECT_CHANGED 0x400000	/* internal: when an object is dbdirty()ed,
								 * set this */
#define SAVED_DELTA    0x800000	/* internal: object last saved to delta file */
#define READMODE     0x10000000	/* internal: when set, player is in a READ */
#define SANEBIT      0x20000000	/* internal: used to check db sanity */


/* what flags to NOT dump to disk. */
#define DUMP_MASK    (INTERACTIVE | SAVED_DELTA | OBJECT_CHANGED | READMODE | SANEBIT)


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
#define WATER ((dbref) -4)	/* water tile */

struct player_specific {
	dbref home;
	const char *password;
	int *descrs;
	int descr_count;
};

#define THING_SP(x)		(DBFETCH(x)->sp.player.sp)
#define ALLOC_THING_SP(x)       { PLAYER_SP(x) = (struct player_specific *)malloc(sizeof(struct player_specific)); }
#define FREE_THING_SP(x)        { dbref foo = x; free(PLAYER_SP(foo)); PLAYER_SP(foo) = NULL; }

#define THING_HOME(x)		(PLAYER_SP(x)->home)

#define THING_SET_HOME(x,y)	(PLAYER_SP(x)->home = y)

#define PLAYER_SP(x)		(DBFETCH(x)->sp.player.sp)
#define ALLOC_PLAYER_SP(x)      { PLAYER_SP(x) = (struct player_specific *)malloc(sizeof(struct player_specific)); bzero(PLAYER_SP(x),sizeof(struct player_specific));}
#define FREE_PLAYER_SP(x)       { dbref foo = x; free(PLAYER_SP(foo)); PLAYER_SP(foo) = NULL; }

#define PLAYER_HOME(x)		(PLAYER_SP(x)->home)
#define PLAYER_PASSWORD(x)	(PLAYER_SP(x)->password)
#define PLAYER_DESCRS(x)    (PLAYER_SP(x)->descrs)
#define PLAYER_DESCRCOUNT(x)    (PLAYER_SP(x)->descr_count)

#define PLAYER_SET_HOME(x,y)		(PLAYER_SP(x)->home = y)
#define PLAYER_SET_PASSWORD(x,y)	(PLAYER_SP(x)->password = y)
#define PLAYER_SET_DESCRS(x,y)		(PLAYER_SP(x)->descrs = y)
#define PLAYER_SET_DESCRCOUNT(x,y)	(PLAYER_SP(x)->descr_count = y)

/* union of type-specific fields */

union specific {				/* I've been railroaded! */
	struct {					/* ROOM-specific fields */
		dbref dropto;
	} room;
	struct {					/* EXIT-specific fields */
		int ndest;
		dbref *dest;
	} exit;
	struct {					/* PLAYER-specific fields */
		struct player_specific *sp;
	} player;
};


/* timestamps record */


struct object {

	const char *name;
	dbref location;				/* pointer to container */
	dbref owner;
	dbref contents;
	dbref exits;
	dbref next;					/* pointer to next in contents/exits chain */
	struct plist *properties;

	object_flag_type flags;

	/* unsigned int mpi_prof_use; */
	/* struct timeval mpi_proftime; */

	union specific sp;
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
#define COMP_HASH_SIZE     (256)	/* Table for compiler keywords */
#define DEFHASHSIZE        (256)	/* Table for compiler $defines */

extern struct object *db;
extern dbref db_top;

extern char *alloc_string(const char *);
extern struct shared_string *alloc_prog_string(const char *);
extern dbref new_object(void);		/* return a new object */

extern struct boolexp *getboolexp(FILE *);	/* get a boolexp */
extern void putboolexp(FILE *, struct boolexp *);	/* put a boolexp */

extern int db_write_object(FILE *, dbref);	/* write one object to file */

extern dbref db_write(FILE * f);	/* write db to file, return # of objects */

extern dbref db_read(FILE * f);	/* read db from file, return # of objects */

 /* Warning: destroys existing db contents! */

extern void db_free(void);

extern dbref parse_dbref(const char *);	/* parse a dbref */

#define DOLIST(var, first) \
  for((var) = (first); (var) != NOTHING; (var) = DBFETCH(var)->next)
#define PUSH(thing, locative) \
    {DBSTORE((thing), next, (locative)); (locative) = (thing);}
#define getloc(thing) (DBFETCH(thing)->location)

/*
  Usage guidelines:

  To obtain an object pointer use DBFETCH(i).  Pointers returned by DBFETCH
  may become invalid after a call to new_object().

  To update an object, use DBSTORE(i, f, v), where i is the object number,
  f is the field (after ->), and v is the new value.

  If you have updated an object without using DBSTORE, use DBDIRTY(i) before
  leaving the routine that did the update.

  When using PUSH, be sure to call DBDIRTY on the object which contains
  the locative (see PUSH definition above).

  Some fields are now handled in a unique way, since they are always memory
  resident, even in the GDBM_DATABASE disk-based muck.  These are: name,
  flags and owner.  Refer to these by NAME(i), FLAGS(i) and OWNER(i).
  Always call DBDIRTY(i) after updating one of these fields.

  The programmer is responsible for managing storage for string
  components of entries; db_read will produce malloc'd strings.  The
  alloc_string routine is provided for generating malloc'd strings
  duplicates of other strings.  Note that db_free and db_read will
  attempt to free any non-NULL string that exists in db when they are
  invoked.
*/
#endif							/* __DB_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef dbh_version
#define dbh_version
const char *db_h_version = "$RCSfile$ $Revision: 1.49 $";
#endif
#else
extern const char *db_h_version;
#endif


/* $Header$ */

#include "copyright.h"
#include "config.h"

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#include "kill.h"
#include "search.h"
#include "web.h"

/* remove the first occurence of what in list headed by first */
static inline dbref
remove_first(dbref first, dbref what)
{
	dbref prev;

	/* special case if it's the first one */
	if (first == what) {
		return db[first].next;
	} else {
		/* have to find it */
		DOLIST(prev, first) {
			if (db[prev].next == what) {
				db[prev].next = db[what].next;
				return first;
			}
		}
		return first;
	}
}

void
moveto(dbref what, dbref where)
{
	dbref loc;

	/* do NOT move garbage */
	CBUG(what == NOTHING);
	CBUG(Typeof(what) == TYPE_GARBAGE);
	/* if (Typeof(what) == TYPE_GARBAGE) */
	/* 	return; */
	CBUG(where == HOME);

	/* remove what from old loc */
	loc = db[what].location;
        if (loc != NOTHING) {
                web_content_out(loc, what);
		db[loc].contents = remove_first(db[loc].contents, what);
        }

	/* test for special cases */
	switch (where) {
	case NOTHING:
		db[what].location = NOTHING;
		return;					/* NOTHING doesn't have contents */
	}

	if (parent_loop_check(what, where)) {
		switch (Typeof(what)) {
		case TYPE_ENTITY:
			where = ENTITY(what)->home;
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_ROOM:
			where = GLOBAL_ENVIRONMENT;
			break;
		}
	}

        if (Typeof(what) == TYPE_ENTITY) {
                dialog_stop(what);
		if ((ENTITY(what)->flags & EF_SITTING))
			stand(what);
	}

	/* now put what in where */
	PUSH(what, db[where].contents);
	db[what].location = where;
	web_content_in(where, what);
        CBUG(getloc(what) != where);
}

/* What are we doing here?  Quick explanation - we want to prevent
   environment loops from happening.  Any item should always be able
   to 'find' its way to room #0.  Since the loop check is recursive,
   we also put in a max iteration check, to keep people from creating
   huge envchains in order to bring the server down.  We have a loop
   if we:
   a) Try to parent to ourselves.
   b) Parent to nothing (not really a loop, but won't get you to #0).
   c) Parent to our own home (not a valid destination).
   d) Find our source room down the environment chain.
   Note: This system will only work if every step _up_ to this point has
   resulted in a consistent (ie: no loops) environment.
*/

int
location_loop_check(dbref source, dbref dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  dbref pstack[MAX_PARENT_DEPTH+2];

  if (source == dest) {
    return 1;
  }

  if (dest == NOTHING)
	  return 0;

  pstack[0] = source;
  pstack[1] = dest;

  while (level < MAX_PARENT_DEPTH) {
    dest = getloc(dest);
    if (dest == NOTHING) {
      return 0;
    }
    if (dest == HOME) {        /* We should never get this, either. */
      return 1;
    }
    if (dest == (dbref) 0) {   /* Reached the top of the chain. */
      return 0;
    }
    /* Check to see if we've found this item before.. */
    for (place = 0; place < (level+2); place++) {
      if (pstack[place] == dest) {
        return 1;
      }
    }
    pstack[level+2] = dest;
    level++;
  }
  return 1;
}

// TODO why is this here?
int
parent_loop_check(dbref source, dbref dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  dbref pstack[MAX_PARENT_DEPTH+2];

  if (dest == HOME) {
	  switch(Typeof(source)) {
		  case TYPE_ENTITY:
			  dest = ENTITY(source)->home;
			  break;
		  case TYPE_THING:
		  case TYPE_ROOM:
		  case TYPE_PLANT:
		  case TYPE_CONSUMABLE:
		  case TYPE_EQUIPMENT:
			  dest = GLOBAL_ENVIRONMENT;
			  break;
		  default:
			  return 1;
	  }
  }
  if (location_loop_check(source, dest)) {
	  return 1;
  }

  if (source == dest) {
    return 1;
  }
  pstack[0] = source;
  pstack[1] = dest;

  while (level < MAX_PARENT_DEPTH) {
    dest = getparent(dest);
    if (dest == NOTHING) {
      return 0;
    }
    if (dest == HOME) {        /* We should never get this, either. */
      return 1;
    }
    if (dest == (dbref) 0) {   /* Reached the top of the chain. */
      return 0;
    }
    /* Check to see if we've found this item before.. */
    for (place = 0; place < (level+2); place++) {
      if (pstack[place] == dest) {
        return 1;
      }
    }
    pstack[level+2] = dest;
    level++;
  }
  return 1;
}

void
enter_room(dbref player, dbref loc)
{
	dbref old;

	old = db[player].location;
	onotifyf(player, "%s has left.", NAME(player));
	moveto(player, loc);
	geo_clean(player, old);
	onotifyf(player, "%s has arrived.", NAME(player));
	mobs_aggro(player);
	look_around(player);
}

void
do_get(command_t *cmd)
{
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	const char *obj = cmd->argv[2];
	dbref thing, cont;
	int cando;

	if (
			(thing = ematch_near(player, what)) == NOTHING
			&& (thing = ematch_mine(player, what)) == NOTHING
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	cont = thing;

	// is this needed?
	if (getloc(thing) != getloc(player) && !(ENTITY(OWNER(player))->flags & EF_WIZARD)) {
		notify(player, "That is too far away from you.");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (thing == NOTHING)
			return;
		if (Typeof(cont) == TYPE_ENTITY) {
			notify(player, "You can't steal from the living.");
			return;
		}
	}
	if (getloc(thing) == player) {
		notify(player, "You already have that!");
		return;
	}
	if (parent_loop_check(thing, player)) {
		notify(player, "You can't pick yourself up by your bootstraps!");
		return;
	}
	switch (Typeof(thing)) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (obj && *obj) {
			cando = 1;
		} else {
			if (OWNER(thing) != player
					&& (Typeof(thing) == TYPE_ENTITY || Typeof(thing) == TYPE_PLANT))
			{
				notify(player, "You can't pick that up.");
				break;
			}

			cando = can_doit(player, thing, "You can't pick that up.");
		}
		if (cando) {
			moveto(thing, player);
			notify(player, "Taken.");
		}
		break;
	default:
		notify(player, "You can't take that!");
		break;
	}
}

void
do_drop(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *obj = cmd->argv[2];
	dbref loc, cont;
	dbref thing;

	if ((loc = getloc(player)) == NOTHING)
		return;

	if (
			(thing = ematch_mine(player, name)) == NOTHING
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	cont = loc;
	if (
			obj && *obj
			&& (cont = ematch_mine(player, obj)) == NOTHING
			&& (cont = ematch_near(player, obj)) == NOTHING
	   )
	{
		notify(player, "I don't know what you mean.");
		return;
	}
        
	switch (Typeof(thing)) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_ENTITY:
	case TYPE_THING:
		if (Typeof(cont) != TYPE_ROOM && Typeof(cont) != TYPE_ENTITY &&
			!is_item(cont)) {
			notify(player, "You can't put anything in that.");
			break;
		}
		if (parent_loop_check(thing, cont)) {
			notify(player, "You can't put something inside of itself.");
			break;
		}

		int immediate_dropto = (Typeof(cont) == TYPE_ROOM &&
				db[cont].sp.room.dropto != NOTHING

				);

		moveto(thing, immediate_dropto ? db[cont].sp.room.dropto : cont);

		if (is_item(cont)) {
			notify(player, "Put away.");
			return;
		} else if (Typeof(cont) == TYPE_ENTITY) {
			notifyf(cont, "%s hands you %s", NAME(player), NAME(thing));
			notifyf(player, "You hand %s to %s", NAME(thing), NAME(cont));
			return;
		}

		notify(player, "Dropped.");
		onotifyf(player, "%s drops %s.", NAME(player), NAME(thing));
		break;
	default:
		notify(player, "You can't drop that.");
		break;
	}
}

void
do_recycle(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref thing;

	if (
			(thing = ematch_absolute(name)) == NOTHING
			&& (thing = ematch_near(player, name)) == NOTHING
			&& (thing = ematch_mine(player, name)) == NOTHING
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}


#ifdef GOD_PRIV
	if(!God(player) && God(OWNER(thing))) {
		notify(player, "Only God may reclaim God's property.");
		return;
	}
#endif
	if (!controls(player, thing)) {
		notify(player, "You can not do that.");
	} else {
		switch (Typeof(thing)) {
		case TYPE_ROOM:
			if (OWNER(thing) != OWNER(player)) {
				notify(player, "Permission denied. (You don't control the room you want to recycle)");
				return;
			}
			if (thing == PLAYER_START) {
				notify(player, "That is the player start room, and may not be recycled.");
				return;
			}
			if (thing == GLOBAL_ENVIRONMENT) {
				notify(player, "If you want to do that, why don't you just delete the database instead?  Room #0 contains everything, and is needed for database sanity.");
				return;
			}
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			if (OWNER(thing) != OWNER(player)) {
				notify(player, "Permission denied. (You can't recycle a thing you don't control)");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (ENTITY(thing)->flags & EF_PLAYER) {
				notify(player, "You can't recycle a player!");
				return;
			}
			break;
		case TYPE_GARBAGE:
			notify(player, "That's already garbage!");
			return;
			/* NOTREACHED */
			break;
		}
		notifyf(player, "Thank you for recycling %.512s (#%d).", NAME(thing), thing);
		recycle(player, thing);
	}
}

void
recycle(dbref player, dbref thing)
{
	extern dbref recyclable;
	static int depth = 0;
	dbref first;
	dbref rest;
	int looplimit;

	depth++;
	switch (Typeof(thing)) {
	case TYPE_ROOM:
		if (!(ENTITY(OWNER(thing))->flags & EF_WIZARD))
			db[OWNER(thing)].value += ROOM_COST;
		if (ROOM(thing)->flags & RF_TEMP)
			for (first = db[thing].contents; first != NOTHING; first = rest) {
				rest = db[first].next;
				if (Typeof(first) != TYPE_ENTITY || !(ENTITY(first)->flags & EF_PLAYER))
					recycle(player, first);
			}
		anotifyf(thing, "You feel a wrenching sensation...");
		map_delete(thing);
		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(ENTITY(OWNER(thing))->flags & EF_WIZARD))
			db[OWNER(thing)].value += db[thing].value;
		if (ROOM(getloc(thing))->flags & RF_TEMP) {
			for (first = db[thing].contents; first != NOTHING; first = rest) {
				rest = db[first].next;
				recycle(player, first);
			}
		}
		break;
	}

	for (rest = 0; rest < db_top; rest++) {
		switch (Typeof(rest)) {
		case TYPE_ROOM:
			if (db[rest].sp.room.dropto == thing)
				db[rest].sp.room.dropto = NOTHING;
			if (OWNER(rest) == thing)
				OWNER(rest) = GOD;
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			if (OWNER(rest) == thing)
				OWNER(rest) = GOD;
			break;
		case TYPE_ENTITY:
			if (ENTITY(rest)->home == thing)
				ENTITY(rest)->home = PLAYER_START;
			break;
		}
		if (db[rest].contents == thing)
			db[rest].contents = db[thing].next;
		if (db[rest].next == thing)
			db[rest].next = db[thing].next;
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && ((first = db[thing].contents) != NOTHING)) {
		if (Typeof(first) == TYPE_ENTITY && (ENTITY(first)->flags & EF_PLAYER)) {
			enter_room(first, HOME);
			/* If the room is set to drag players back, there'll be no
			 * reasoning with it.  DRAG the player out.
			 */
			if (db[first].location == thing) {
				notifyf(player, "Escaping teleport loop!  Going home.");
				moveto(first, PLAYER_START);
			}
		} else {
			/* moveto(first, HOME); */
                        recycle(player, first);
		}
	}


	moveto(thing, NOTHING);

	depth--;

	db_free_object(thing);
	db_clear_object(thing);
	NAME(thing) = (char*) strdup("<garbage>");
	db[thing].description = (char *) strdup("<recyclable>");
	FLAGS(thing) = TYPE_GARBAGE;

	db[thing].next = recyclable;
	recyclable = thing;
}
static const char *move_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_move_c_version(void) { return move_c_version; }

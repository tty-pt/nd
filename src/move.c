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
		return OBJECT(first)->next;
	} else {
		/* have to find it */
		DOLIST(prev, first) {
			if (OBJECT(prev)->next == what) {
				OBJECT(prev)->next = OBJECT(what)->next;
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
	CBUG(OBJECT(what)->type == TYPE_GARBAGE);

	/* remove what from old loc */
	loc = OBJECT(what)->location;
        if (loc != NOTHING) {
                web_content_out(loc, what);
		OBJECT(loc)->contents = remove_first(OBJECT(loc)->contents, what);
        }

	/* test for special cases */
	switch (where) {
	case NOTHING:
		OBJECT(what)->location = NOTHING;
		return;					/* NOTHING doesn't have contents */
	}

	if (parent_loop_check(what, where)) {
		switch (OBJECT(what)->type) {
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

        if (OBJECT(what)->type == TYPE_ENTITY) {
                dialog_stop(what);
		if ((ENTITY(what)->flags & EF_SITTING))
			stand(what);
	}

	/* now put what in where */
	PUSH(what, OBJECT(where)->contents);
	OBJECT(what)->location = where;
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

	old = OBJECT(player)->location;
	onotifyf(player, "%s has left.", OBJECT(player)->name);
	moveto(player, loc);
	geo_clean(player, old);
	onotifyf(player, "%s has arrived.", OBJECT(player)->name);
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
	if (getloc(thing) != getloc(player) && !(ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD)) {
		notify(player, "That is too far away from you.");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (thing == NOTHING)
			return;
		if (OBJECT(cont)->type == TYPE_ENTITY) {
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
	switch (OBJECT(thing)->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (obj && *obj) {
			cando = 1;
		} else {
			if (OBJECT(thing)->owner != player
					&& (OBJECT(thing)->type == TYPE_ENTITY || OBJECT(thing)->type == TYPE_PLANT))
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
        
	switch (OBJECT(thing)->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_ENTITY:
	case TYPE_THING:
		if (OBJECT(cont)->type != TYPE_ROOM && OBJECT(cont)->type != TYPE_ENTITY &&
			!is_item(cont)) {
			notify(player, "You can't put anything in that.");
			break;
		}
		if (parent_loop_check(thing, cont)) {
			notify(player, "You can't put something inside of itself.");
			break;
		}

		int immediate_dropto = (OBJECT(cont)->type == TYPE_ROOM &&
				ROOM(cont)->dropto != NOTHING

				);

		moveto(thing, immediate_dropto ? ROOM(cont)->dropto : cont);

		if (is_item(cont)) {
			notify(player, "Put away.");
			return;
		} else if (OBJECT(cont)->type == TYPE_ENTITY) {
			notifyf(cont, "%s hands you %s", OBJECT(player)->name, OBJECT(thing)->name);
			notifyf(player, "You hand %s to %s", OBJECT(thing)->name, OBJECT(cont)->name);
			return;
		}

		notify(player, "Dropped.");
		onotifyf(player, "%s drops %s.", OBJECT(player)->name, OBJECT(thing)->name);
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
	if(!God(player) && God(OBJECT(thing)->owner)) {
		notify(player, "Only God may reclaim God's property.");
		return;
	}
#endif
	if (!controls(player, thing)) {
		notify(player, "You can not do that.");
	} else {
		switch (OBJECT(thing)->type) {
		case TYPE_ROOM:
			if (OBJECT(thing)->owner != OBJECT(player)->owner) {
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
			if (OBJECT(thing)->owner != OBJECT(player)->owner) {
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
		notifyf(player, "Thank you for recycling %.512s (#%d).", OBJECT(thing)->name, thing);
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
	switch (OBJECT(thing)->type) {
	case TYPE_ROOM:
		if (!(ENTITY(OBJECT(thing)->owner)->flags & EF_WIZARD))
			OBJECT(OBJECT(thing)->owner)->value += ROOM_COST;
		if (ROOM(thing)->flags & RF_TEMP)
			for (first = OBJECT(thing)->contents; first != NOTHING; first = rest) {
				rest = OBJECT(first)->next;
				if (OBJECT(first)->type != TYPE_ENTITY || !(ENTITY(first)->flags & EF_PLAYER))
					recycle(player, first);
			}
		anotifyf(thing, "You feel a wrenching sensation...");
		map_delete(thing);
		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(ENTITY(OBJECT(thing)->owner)->flags & EF_WIZARD))
			OBJECT(OBJECT(thing)->owner)->value += OBJECT(thing)->value;
		if (ROOM(getloc(thing))->flags & RF_TEMP) {
			for (first = OBJECT(thing)->contents; first != NOTHING; first = rest) {
				rest = OBJECT(first)->next;
				recycle(player, first);
			}
		}
		break;
	}

	for (rest = 0; rest < db_top; rest++) {
		switch (OBJECT(rest)->type) {
		case TYPE_ROOM:
			if (ROOM(rest)->dropto == thing)
				ROOM(rest)->dropto = NOTHING;
			if (OBJECT(rest)->owner == thing)
				OBJECT(rest)->owner = GOD;
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			if (OBJECT(rest)->owner == thing)
				OBJECT(rest)->owner = GOD;
			break;
		case TYPE_ENTITY:
			if (ENTITY(rest)->home == thing)
				ENTITY(rest)->home = PLAYER_START;
			break;
		}
		if (OBJECT(rest)->contents == thing)
			OBJECT(rest)->contents = OBJECT(thing)->next;
		if (OBJECT(rest)->next == thing)
			OBJECT(rest)->next = OBJECT(thing)->next;
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && ((first = OBJECT(thing)->contents) != NOTHING)) {
		if (OBJECT(first)->type == TYPE_ENTITY && (ENTITY(first)->flags & EF_PLAYER)) {
			enter_room(first, ENTITY(first)->home);

			/* If the room is set to drag players back, there'll be no
			 * reasoning with it.  DRAG the player out.
			 */
			if (OBJECT(first)->location == thing) {
				notifyf(player, "Escaping teleport loop!  Going home.");
				moveto(first, PLAYER_START);
			}
		} else {
                        recycle(player, first);
		}
	}


	moveto(thing, NOTHING);

	depth--;

	db_free_object(thing);
	db_clear_object(thing);
	OBJECT(thing)->name = (char*) strdup("<garbage>");
	OBJECT(thing)->description = (char *) strdup("<recyclable>");
	OBJECT(thing)->type = TYPE_GARBAGE;
	OBJECT(thing)->next = recyclable;
	recyclable = thing;
}
static const char *move_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_move_c_version(void) { return move_c_version; }

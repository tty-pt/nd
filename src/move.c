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
static inline OBJ *
remove_first(OBJ *first, OBJ *what)
{
	OBJ *prev;

	/* special case if it's the first one */
	if (first == what) {
		return first->next;
	} else {
		/* have to find it */
		DOLIST(prev, first) {
			if (prev->next == what) {
				prev->next = what->next;
				return first;
			}
		}
		return first;
	}
}

void
moveto(OBJ *what, OBJ *where)
{
	OBJ *loc;

	/* do NOT move garbage */
	CBUG(!what);
	CBUG(what->type == TYPE_GARBAGE);

	loc = what->location;

        if (loc) {
                web_content_out(loc, what);
		loc->contents = remove_first(loc->contents, what);
        }

	/* test for special cases */
	if (!where) {
		what->location = NULL;
		return;
	}

	if (parent_loop_check(what, where)) {
		switch (what->type) {
		case TYPE_ENTITY:
			where = what->sp.entity.home;
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_ROOM:
			where = object_get(GLOBAL_ENVIRONMENT);
			break;
		}
	}

        if (what->type == TYPE_ENTITY) {
		ENT *ewhat = &what->sp.entity;
                dialog_stop(what);
		if ((ewhat->flags & EF_SITTING))
			stand(what);
	}

	/* now put what in where */
	PUSH(what, where->contents);
	what->location = where;
	web_content_in(where, what);
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
location_loop_check(OBJ *source, OBJ *dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  OBJ *pstack[MAX_PARENT_DEPTH+2];

  if (source == dest) {
    return 1;
  }

  if (!dest)
	  return 0;

  pstack[0] = source;
  pstack[1] = dest;

  while (level < MAX_PARENT_DEPTH) {
    dest = dest->location;
    if (!dest || object_ref(dest) == (dbref) 0) {   /* Reached the top of the chain. */
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
parent_loop_check(OBJ *source, OBJ *dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  OBJ *pstack[MAX_PARENT_DEPTH+2];

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
    if (!dest)
      return 0;
    if (object_ref(dest) == (dbref) 0) {   /* Reached the top of the chain. */
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
enter_room(OBJ *player, OBJ *loc)
{
	OBJ *old = player->location;

	onotifyf(player, "%s has left.", player->name);
	moveto(player, loc);
	geo_clean(player, old);
	onotifyf(player, "%s has arrived.", player->name);
	mobs_aggro(player);
	look_around(player);
}

void
do_get(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *thing, *cont;
	int cando;

	if (
			!(thing = ematch_near(player, what))
			&& !(thing = ematch_mine(player, what))
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	cont = thing;

	// is this needed?
	if (thing->location != player->location && !(eplayer->flags & EF_WIZARD)) {
		notify(player, "That is too far away from you.");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (!thing)
			return;
		if (cont->type == TYPE_ENTITY) {
			notify(player, "You can't steal from the living.");
			return;
		}
	}
	if (thing->location == player) {
		notify(player, "You already have that!");
		return;
	}
	if (parent_loop_check(thing, player)) {
		notify(player, "You can't pick yourself up by your bootstraps!");
		return;
	}
	switch (thing->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
	case TYPE_PLANT:
		if (obj && *obj) {
			cando = 1;
		} else {
			if (thing->owner != player
					&& (thing->type == TYPE_ENTITY || thing->type == TYPE_PLANT))
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
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *loc = player->location,
	    *thing, *cont;

	if (!(thing = ematch_mine(player, name))) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	cont = loc;
	if (
			obj && *obj
			&& !(cont = ematch_mine(player, obj))
			&& !(cont = ematch_near(player, obj))
	   )
	{
		notify(player, "I don't know what you mean.");
		return;
	}
        
	switch (thing->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_ENTITY:
	case TYPE_THING:
		if (cont->type != TYPE_ROOM && cont->type != TYPE_ENTITY &&
			!is_item(cont)) {
			notify(player, "You can't put anything in that.");
			break;
		}
		if (parent_loop_check(thing, cont)) {
			notify(player, "You can't put something inside of itself.");
			break;
		}

		int immediate_dropto = (cont->type == TYPE_ROOM && cont->sp.room.dropto);

		moveto(thing, immediate_dropto ? cont->sp.room.dropto : cont);

		if (is_item(cont)) {
			notify(player, "Put away.");
			return;
		} else if (cont->type == TYPE_ENTITY) {
			notifyf(cont, "%s hands you %s", player->name, thing->name);
			notifyf(player, "You hand %s to %s", thing->name, cont->name);
			return;
		}

		notify(player, "Dropped.");
		onotifyf(player, "%s drops %s.", player->name, thing->name);
		break;
	default:
		notify(player, "You can't drop that.");
		break;
	}
}

void
do_recycle(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
	   )
	{
		notify(player, NOMATCH_MESSAGE);
		return;
	}


#ifdef GOD_PRIV
	if(!God(player) && God(thing->owner)) {
		notify(player, "Only God may reclaim God's property.");
		return;
	}
#endif
	if (!controls(player, thing)) {
		notify(player, "You can not do that.");
	} else {
		switch (thing->type) {
		case TYPE_ROOM:
			if (thing->owner != player->owner) {
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
			if (thing->owner != player->owner) {
				notify(player, "Permission denied. (You can't recycle a thing you don't control)");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (thing->sp.entity.flags & EF_PLAYER) {
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
		notifyf(player, "Thank you for recycling %.512s (#%d).", thing->name, thing);
		recycle(player, thing);
	}
}

void
recycle(OBJ *player, OBJ *thing)
{
	extern OBJ *recyclable;
	static int depth = 0;
	OBJ *first, *rest;
	int looplimit;

	OBJ *owner = thing->owner;
	ENT *eowner = &owner->sp.entity;

	depth++;
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			if (!(eowner->flags & EF_WIZARD))
				owner->value += ROOM_COST;
			if (rthing->flags & RF_TEMP)
				for (first = thing->contents; first; first = rest) {
					rest = first->next;

					if (first->type == TYPE_ENTITY) {
						ENT *efirst = &first->sp.entity;
						if (efirst->flags & EF_PLAYER)
							continue;
					}

					recycle(player, first);
				}
			anotifyf(thing, "You feel a wrenching sensation...");
			map_delete(thing);
		}

		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (!(eowner->flags & EF_WIZARD))
			owner->value += thing->value;

		OBJ *thingloc = thing->location;

		if (thingloc->type != TYPE_ROOM)
			break;

		ROO *rthingloc = &thingloc->sp.room;

		if (rthingloc->flags & RF_TEMP) {
			for (first = thing->contents; first; first = rest) {
				rest = first->next;
				recycle(player, first);
			}
		}

		break;
	}

	for (rest = object_get(0); rest < object_get(db_top); rest++) {
		switch (rest->type) {
		case TYPE_ROOM:
			{
				ROO *rrest = &rest->sp.room;
				if (rrest->dropto == thing)
					rrest->dropto = NULL;
				if (rest->owner == thing)
					rest->owner = object_get(GOD);
			}

			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			if (rest->owner == thing)
				rest->owner = object_get(GOD);
			break;
		case TYPE_ENTITY:
			{
				ENT *erest = &rest->sp.entity;
				if (erest->home == thing)
					erest->home = object_get(PLAYER_START);
			}

			break;
		}
		if (rest->contents == thing)
			rest->contents = thing->next;
		if (rest->next == thing)
			rest->next = thing->next;
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && (first = thing->contents)) {
		if (first->type == TYPE_ENTITY) {
			ENT *efirst = &first->sp.entity;
			if (efirst->flags & EF_PLAYER) {
				enter_room(first, efirst->home);
				if (first->location == thing) {
					notifyf(player, "Escaping teleport loop!  Going home.");
					moveto(first, object_get(PLAYER_START));
				}
				continue;
			}
		}

		recycle(player, first);
	}


	moveto(thing, NULL);

	depth--;

	db_free_object(thing);
	db_clear_object(thing);
	thing->name = (char*) strdup("<garbage>");
	thing->description = (char *) strdup("<recyclable>");
	thing->type = TYPE_GARBAGE;
	thing->next = recyclable;
	recyclable = thing;
}
static const char *move_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_move_c_version(void) { return move_c_version; }

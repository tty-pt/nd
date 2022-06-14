#include "io.h"
#include "entity.h"
#include "config.h"

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "mob.h"

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
		FOR_LIST(prev, first) {
			if (prev->next == what) {
				prev->next = what->next;
				return first;
			}
		}
		return first;
	}
}

void
object_move(OBJ *what, OBJ *where)
{
	OBJ *loc;

	/* do NOT move garbage */
	CBUG(!what);
	CBUG(what->type == TYPE_GARBAGE);

	loc = what->location;

        if (loc) {
                mcp_content_out(loc, what);
		loc->contents = remove_first(loc->contents, what);
        }

	/* test for special cases */
	if (!where) {
		what->location = NULL;
		return;
	}

	if (object_plc(what, where)) {
		switch (what->type) {
		case TYPE_ENTITY:
			where = what->sp.entity.home;
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_ROOM:
		case TYPE_SEAT:
			where = object_get(GLOBAL_ENVIRONMENT);
			break;
		}
	}

        if (what->type == TYPE_ENTITY) {
		ENT *ewhat = &what->sp.entity;
		if ((ewhat->flags & EF_SITTING))
			stand(what);
	}

	/* now put what in where */
	PUSH(what, where->contents);
	what->location = where;
	mcp_content_in(where, what);
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

static inline int
object_llc(OBJ *source, OBJ *dest)
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
object_plc(OBJ *source, OBJ *dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  OBJ *pstack[MAX_PARENT_DEPTH+2];

  if (object_llc(source, dest)) {
	  return 1;
  }

  if (source == dest) {
    return 1;
  }
  pstack[0] = source;
  pstack[1] = dest;

  while (level < MAX_PARENT_DEPTH) {
    dest = object_parent(dest);
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
do_get(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *thing, *cont;
	int can;

	if (
			!(thing = ematch_near(player, what))
			&& !(thing = ematch_mine(player, what))
	   )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	cont = thing;

	// is this needed?
	if (thing->location != player->location && !(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "That is too far away from you.");
		return;
	}

	if (obj && *obj) {
		thing = ematch_at(player, cont, obj);
		if (!thing)
			return;
		if (cont->type == TYPE_ENTITY) {
			notify(eplayer, "You can't steal from the living.");
			return;
		}
	}
	if (thing->location == player) {
		notify(eplayer, "You already have that!");
		return;
	}
	if (object_plc(thing, player)) {
		notify(eplayer, "You can't pick yourself up by your bootstraps!");
		return;
	}
	switch (thing->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_ENTITY:
	case TYPE_PLANT:
	case TYPE_SEAT:
		if (obj && *obj) {
			can = 1;
		} else {
			if (thing->owner != player
					&& (thing->type == TYPE_ENTITY || thing->type == TYPE_PLANT || thing->type == TYPE_SEAT))
			{
				notify(eplayer, "You can't pick that up.");
				break;
			}

			can = cando(player, thing, "You can't pick that up.");
		}
		if (can) {
			object_move(thing, player);
			notify(eplayer, "Taken.");
		}
		break;
	default:
		notify(eplayer, "You can't take that!");
		break;
	}
}

void
do_drop(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *obj = cmd->argv[2];
	OBJ *loc = player->location,
	    *thing, *cont;

	if (!(thing = ematch_mine(player, name))) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	cont = loc;
	if (
			obj && *obj
			&& !(cont = ematch_mine(player, obj))
			&& !(cont = ematch_near(player, obj))
	   )
	{
		notify(eplayer, "I don't know what you mean.");
		return;
	}
        
	switch (thing->type) {
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_ENTITY:
	case TYPE_THING:
		if (cont->type != TYPE_ROOM && cont->type != TYPE_ENTITY &&
			!object_item(cont)) {
			notify(eplayer, "You can't put anything in that.");
			break;
		}
		if (object_plc(thing, cont)) {
			notify(eplayer, "You can't put something inside of itself.");
			break;
		}

		int immediate_dropto = (cont->type == TYPE_ROOM && cont->sp.room.dropto);

		object_move(thing, immediate_dropto ? cont->sp.room.dropto : cont);

		if (object_item(cont)) {
			notify(eplayer, "Put away.");
			return;
		} else if (cont->type == TYPE_ENTITY) {
			ENT *econt = &cont->sp.entity;
			notifyf(econt, "%s hands you %s", player->name, thing->name);
			notifyf(eplayer, "You hand %s to %s", thing->name, cont->name);
			return;
		}

		notify(eplayer, "Dropped.");
		onotifyf(player, "%s drops %s.", player->name, thing->name);
		break;
	default:
		notify(eplayer, "You can't drop that.");
		break;
	}
}

void
do_recycle(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
	   )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}


	if(!God(player) && God(thing->owner)) {
		notify(eplayer, "Only God may reclaim God's property.");
		return;
	}

	if (!controls(player, thing)) {
		notify(eplayer, "You can not do that.");
	} else {
		switch (thing->type) {
		case TYPE_ROOM:
			if (thing->owner != player->owner) {
				notify(eplayer, "Permission denied. (You don't control the room you want to recycle)");
				return;
			}
			if (thing == PLAYER_START) {
				notify(eplayer, "That is the player start room, and may not be recycled.");
				return;
			}
			if (thing == GLOBAL_ENVIRONMENT) {
				notify(eplayer, "If you want to do that, why don't you just delete the database instead?  Room #0 contains everything, and is needed for database sanity.");
				return;
			}
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_SEAT:
			if (thing->owner != player->owner) {
				notify(eplayer, "Permission denied. (You can't recycle a thing you don't control)");
				return;
			}
			break;
		case TYPE_ENTITY:
			if (thing->sp.entity.flags & EF_PLAYER) {
				notify(eplayer, "You can't recycle a player!");
				return;
			}
			break;
		case TYPE_GARBAGE:
			notify(eplayer, "That's already garbage!");
			return;
			/* NOTREACHED */
			break;
		}
		notifyf(eplayer, "Thank you for recycling %.512s (#%d).", thing->name, thing);
		recycle(player, thing);
	}
}

static const char *move_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_move_c_version(void) { return move_c_version; }

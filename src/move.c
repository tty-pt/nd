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
	loc = DBFETCH(what)->location;
        if (loc != NOTHING) {
                web_content_out(loc, what);
                DBSTORE(loc, contents, remove_first(DBFETCH(loc)->contents, what));
        }

	/* test for special cases */
	switch (where) {
	case NOTHING:
		DBSTORE(what, location, NOTHING);
		return;					/* NOTHING doesn't have contents */
	}

	if (parent_loop_check(what, where)) {
		switch (Typeof(what)) {
		case TYPE_PLAYER:
			where = PLAYER_HOME(what);
			break;
		case TYPE_THING:
			where = THING_HOME(what);
			if (parent_loop_check(what, where)) {
				where = PLAYER_HOME(OWNER(what));
				if (parent_loop_check(what, where))
					where = (dbref) PLAYER_START;
			}
			break;
		case TYPE_ROOM:
			where = GLOBAL_ENVIRONMENT;
			break;
		}
	}

        if (Typeof(what) == TYPE_PLAYER)
                dialog_stop(what);
        if (GETLID(what) > 0 && GETSAT(what) != NOTHING) {
                stand(what);
        }

	/* now put what in where */
	PUSH(what, DBFETCH(where)->contents);
	DBDIRTY(where);
	DBSTORE(what, location, where);
	web_content_in(where, what);
        CBUG(getloc(what) != where);
}

dbref reverse(dbref);
void
send_contents(dbref loc, dbref dest)
{
	dbref first;
	dbref rest;
	dbref where;

	first = DBFETCH(loc)->contents;
	DBSTORE(loc, contents, NOTHING);

	/* blast locations of everything in list */
	DOLIST(rest, first) {
		DBSTORE(rest, location, NOTHING);
	}

	while (first != NOTHING) {
		rest = DBFETCH(first)->next;
		if (Typeof(first) != TYPE_THING) {
			moveto(first, loc);
		} else {
			where = FLAGS(first) & STICKY ? HOME : dest;
			moveto(first, parent_loop_check(first, where) ? loc : where);
		}
		first = rest;
	}

	DBSTORE(loc, contents, reverse(DBFETCH(loc)->contents));
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

int
parent_loop_check(dbref source, dbref dest)
{   
  unsigned int level = 0;
  unsigned int place = 0;
  dbref pstack[MAX_PARENT_DEPTH+2];

  if (dest == HOME) {
	  switch(Typeof(source)) {
		  case TYPE_PLAYER:
			  dest = PLAYER_HOME(source);
			  break;
		  case TYPE_THING:
			  dest = THING_HOME(source);
			  break;
		  case TYPE_ROOM:
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
    /* if (Typeof(dest) == TYPE_THING) {
         dest = THING_HOME(dest);
       } */
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
enter_room(dbref player, dbref loc, dbref exit)
{
	dbref old;
	char buf[BUFFER_LEN];

	/* get old location */
	old = DBFETCH(player)->location;

	/* CBUG(loc == old); */

	/* go there */
	moveto(player, loc);

	CBUG(old == NOTHING);

	snprintf(buf, sizeof(buf), "%s has left.", NAME(player));
	notify_except(DBFETCH(old)->contents, player, buf, player);

	geo_clean(player, old);

	snprintf(buf, sizeof(buf), "%s has arrived.", NAME(player));
	notify_except(DBFETCH(loc)->contents, player, buf, player);

	mobs_aggro(player);
	/* TODO geo_notify(descr, player); */

	look_around(player);
}

void
send_home(dbref thing, int puppethome)
{
	switch (Typeof(thing)) {
	case TYPE_PLAYER:
		/* send his possessions home first! */
		/* that way he sees them when he arrives */
		send_contents(thing, HOME);
		enter_room(thing, PLAYER_HOME(thing), DBFETCH(thing)->location);
		break;
	case TYPE_THING:
		if (puppethome)
			send_contents(thing, HOME);
		moveto(thing, HOME);	/* home */
		break;
	default:
		/* no effect */
		break;
	}
	return;
}

int
can_move(dbref player, const char *direction, int lev)
{
	struct match_data md;

#if ALLOW_HOME
	if (!strcmp(direction, "home"))
		return 1;
#endif

	/* otherwise match on exits */
	init_match(player, direction, TYPE_EXIT, &md);
	md.match_level = lev;
	match_all_exits(&md);
	return (last_match_result(&md) != NOTHING);
}

void
trigger(dbref player, dbref exit)
{
	dbref dest;

	CBUG(!e_exit_is(exit));
	// quickfix for gexits
	union specific sp
		= DBFETCH(exit)->sp;

	if (sp.exit.ndest && sp.exit.dest[0] != NOTHING)
		dest = sp.exit.dest[0];
	else
		dest = geo_there(getloc(exit), exit_e(exit));

	if (dest > 0) {
		enter_room(player, dest, exit);
	} else {
		dest = geo_room(player, exit);
		enter_room(player, dest, exit);
	}
}

void
go_move(dbref player, const char *direction, int lev)
{
	dbref loc;

	struct match_data md;
	dbref exit;

	/* find the exit */
	init_match_check_keys(player, direction, TYPE_EXIT, &md);
	md.match_level = lev;
	match_all_exits(&md);
	switch (exit = match_result(&md)) {
	case NOTHING:
		notify(player, "You can't go that way.");
		break;
	case AMBIGUOUS:
		notify(player, "I don't know which way you mean!");
		break;
	default:
		/* we got one */
		/* check to see if we got through */
		loc = DBFETCH(player)->location;

		if (!can_doit(player, exit, "You can't go that way."))
			break;

		trigger(player, exit);
		break;
	}
}

void
do_move(command_t *cmd)
{
	go_move(cmd->player, cmd->argv[1], atoi(cmd->argv[2]));
}

void
do_get(command_t *cmd)
{
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	const char *obj = cmd->argv[2];
	dbref thing, cont;
	int cando;
	struct match_data md;

	init_match_check_keys(player, what, TYPE_THING, &md);
	match_neighbor(&md);
	match_possession(&md);
        match_absolute(&md);

	if ((thing = noisy_match_result(&md)) != NOTHING) {
		cont = thing;

                if (getloc(thing) != getloc(player) && !Wizard(OWNER(player))) {
                        notify(player, "That is too far away from you.");
                        return;
                }

		if (obj && *obj) {
			init_match_check_keys(player, obj, TYPE_THING, &md);
			match_rmatch(cont, &md);
                        match_absolute(&md);
			if ((thing = noisy_match_result(&md)) == NOTHING) {
				return;
			}
			if (Typeof(cont) == TYPE_PLAYER) {
				notify(player, "You can't steal things from players.");
				return;
			}
			if (!test_lock_false_default(player, cont, "_/clk")) {
				notify(player, "You can't open that container.");
				return;
			}
		}
		if (DBFETCH(thing)->location == player) {
			notify(player, "You already have that!");
			return;
		}
		if (Typeof(cont) == TYPE_PLAYER) {
			notify(player, "You can't steal stuff from players.");
			return;
		}
		if (parent_loop_check(thing, player)) {
			notify(player, "You can't pick yourself up by your bootstraps!");
			return;
		}
		switch (Typeof(thing)) {
		case TYPE_THING:
			if (obj && *obj) {
				cando = could_doit(player, thing);
				if (!cando)
					notify(player, "You can't get that.");
			} else {
                                if (OWNER(thing) != player
                                    && (GETLID(thing) >= 0 || GETPLID(thing) >= 0))
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
}

void
do_drop(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *obj = cmd->argv[2];
	dbref loc, cont;
	dbref thing;
	char buf[BUFFER_LEN];
	struct match_data md;

	if ((loc = getloc(player)) == NOTHING)
		return;

	init_match(player, name, NOTYPE, &md);
	match_possession(&md);
        match_absolute(&md);
	if ((thing = noisy_match_result(&md)) == NOTHING || thing == AMBIGUOUS)
		return;

        if (getloc(thing) != player) {
                notify(player, "You can't drop something you don't have.");
                return;
        }

	cont = loc;
	if (obj && *obj) {
		init_match(player, obj, NOTYPE, &md);
		match_possession(&md);
		match_neighbor(&md);
		/* if (Wizard(OWNER(player))) */
			match_absolute(&md);	/* the wizard has long fingers */
		if ((cont = noisy_match_result(&md)) == NOTHING || thing == AMBIGUOUS) {
			return;
		}
	}
        
	switch (Typeof(thing)) {
	case TYPE_THING:

		if (Typeof(cont) != TYPE_ROOM && Typeof(cont) != TYPE_PLAYER &&
			Typeof(cont) != TYPE_THING) {
			notify(player, "You can't put anything in that.");
			break;
		}
		if (Typeof(cont) != TYPE_ROOM &&
			!test_lock_false_default(player, cont, "_/clk")) {
			notify(player, "You don't have permission to put something in that.");
			break;
		}
		if (parent_loop_check(thing, cont)) {
			notify(player, "You can't put something inside of itself.");
			break;
		}
		if (Typeof(cont) == TYPE_ROOM && (FLAGS(thing) & STICKY) &&
			Typeof(thing) == TYPE_THING) {
			send_home(thing, 0);
		} else {
			int immediate_dropto = (Typeof(cont) == TYPE_ROOM &&
									DBFETCH(cont)->sp.room.dropto != NOTHING

									&& !(FLAGS(cont) & STICKY));

			moveto(thing, immediate_dropto ? DBFETCH(cont)->sp.room.dropto : cont);
		}
		if (Typeof(cont) == TYPE_THING) {
			notify(player, "Put away.");
			return;
		} else if (Typeof(cont) == TYPE_PLAYER) {
			notifyf(cont, "%s hands you %s", NAME(player), NAME(thing));
			notifyf(player, "You hand %s to %s", NAME(thing), NAME(cont));
			return;
		}

		if (GETDROP(thing))
			notify(player, GETDROP(thing));
		else
			notify(player, "Dropped.");

		if (GETDROP(loc))
			notify(player, GETDROP(loc));

		snprintf(buf, sizeof(buf), "%s drops %s.", NAME(player), NAME(thing));
		notify_except(DBFETCH(loc)->contents, player, buf, player);

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
	char buf[BUFFER_LEN];
	struct match_data md;

	NOGUEST("@recycle",player);

	init_match(player, name, TYPE_THING, &md);
	match_all_exits(&md);
	match_neighbor(&md);
	match_possession(&md);
	match_registered(&md);
	match_here(&md);
	match_absolute(&md);
	if ((thing = noisy_match_result(&md)) != NOTHING) {
#ifdef GOD_PRIV
	if(!God(player) && God(OWNER(thing))) {
		notify(player, "Only God may reclaim God's property.");
		return;
	}
#endif
		if (!controls(player, thing)) {
			if(Wizard(OWNER(player)) && (Typeof(thing) == TYPE_GARBAGE))
				notify(player, "That's already garbage!");
			else
				notify(player, "Permission denied. (You don't control what you want to recycle)");
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
			case TYPE_THING:
				if (OWNER(thing) != OWNER(player)) {
					notify(player, "Permission denied. (You can't recycle a thing you don't control)");
					return;
				}
				/* player may be a zombie or puppet */
				if (thing == player) {
					snprintf(buf, sizeof(buf),
							"%.512s's owner commands it to kill itself.  It blinks a few times in shock, and says, \"But.. but.. WHY?\"  It suddenly clutches it's heart, grimacing with pain..  Staggers a few steps before falling to it's knees, then plops down on it's face.  *thud*  It kicks its legs a few times, with weakening force, as it suffers a seizure.  It's color slowly starts changing to purple, before it explodes with a fatal *POOF*!",
							NAME(thing));
					notify_except(DBFETCH(getloc(thing))->contents, thing, buf, player);
					notify(OWNER(player), buf);
					notify(OWNER(player), "Now don't you feel guilty?");
				}
				break;
			case TYPE_EXIT:
				if (OWNER(thing) != OWNER(player)) {
					notify(player, "Permission denied. (You may not recycle an exit you don't own)");
					return;
				}
				if (!unset_source(player, DBFETCH(player)->location, thing)) {
					notify(player, "You can't do that to an exit in another room.");
					return;
				}
				break;
			case TYPE_PLAYER:
				notify(player, "You can't recycle a player!");
				return;
				/* NOTREACHED */
				break;
			case TYPE_GARBAGE:
				notify(player, "That's already garbage!");
				return;
				/* NOTREACHED */
				break;
			}
			snprintf(buf, sizeof(buf), "Thank you for recycling %.512s (#%d).", NAME(thing), thing);
			recycle(player, thing);
			notify(player, buf);
		}
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
		if (!Wizard(OWNER(thing)))
			SETVALUE(OWNER(thing), GETVALUE(OWNER(thing)) + ROOM_COST);
		DBDIRTY(OWNER(thing));
		for (first = DBFETCH(thing)->exits; first != NOTHING; first = rest) {
			rest = DBFETCH(first)->next;
			if (DBFETCH(first)->location == NOTHING
			    || DBFETCH(first)->location == thing) {
				if (e_exit_is(first))
					gexit_snull(player, first);
				recycle(player, first);
			}
		}
		if (GETTMP(thing))
			for (first = DBFETCH(thing)->contents; first != NOTHING; first = rest) {
				rest = DBFETCH(first)->next;
				if (Typeof(first) != TYPE_PLAYER)
					recycle(player, first);
			}
		notify_except(DBFETCH(thing)->contents, NOTHING,
					  "You feel a wrenching sensation...", player);
		map_delete(thing);
		break;
	case TYPE_THING:
		if (!Wizard(OWNER(thing)))
			SETVALUE(OWNER(thing), GETVALUE(OWNER(thing)) + GETVALUE(thing));
		DBDIRTY(OWNER(thing));
		for (first = DBFETCH(thing)->exits; first != NOTHING; first = rest) {
			rest = DBFETCH(first)->next;
			if (DBFETCH(first)->location == NOTHING || DBFETCH(first)->location == thing)
				recycle(player, first);
		}
		if (GETTMP(getloc(thing))) {
			SETTMP(thing, 1);
			for (first = DBFETCH(thing)->contents; first != NOTHING; first = rest) {
				rest = DBFETCH(first)->next;
				recycle(player, first);
			}
		}
		break;
	case TYPE_EXIT:
		if (!Wizard(OWNER(thing)))
			SETVALUE(OWNER(thing), GETVALUE(OWNER(thing)) + EXIT_COST);
		if (!Wizard(OWNER(thing)))
			if (DBFETCH(thing)->sp.exit.ndest != 0)
				SETVALUE(OWNER(thing), GETVALUE(OWNER(thing)) + LINK_COST);
		DBDIRTY(OWNER(thing));
		break;
	}

	for (rest = 0; rest < db_top; rest++) {
		switch (Typeof(rest)) {
		case TYPE_ROOM:
			if (DBFETCH(rest)->sp.room.dropto == thing) {
				DBFETCH(rest)->sp.room.dropto = NOTHING;
				DBDIRTY(rest);
			}
			if (DBFETCH(rest)->exits == thing) {
				DBFETCH(rest)->exits = DBFETCH(thing)->next;
				DBDIRTY(rest);
			}
			if (OWNER(rest) == thing) {
				OWNER(rest) = GOD;
				DBDIRTY(rest);
			}
			break;
		case TYPE_THING:
			if (THING_HOME(rest) == thing) {
			  dbref loc;

			  if (PLAYER_HOME(OWNER(rest)) == thing)
			    PLAYER_SET_HOME(OWNER(rest), PLAYER_START);
			  loc = PLAYER_HOME(OWNER(rest));
			  if (parent_loop_check(rest, loc)) {
			    loc = OWNER(rest);
			    if (parent_loop_check(rest, loc)) {
			      loc = (dbref) 0;
			    }
			  }
			  THING_SET_HOME(rest, loc);
			  DBDIRTY(rest);
			}
			if (DBFETCH(rest)->exits == thing) {
				DBFETCH(rest)->exits = DBFETCH(thing)->next;
				DBDIRTY(rest);
			}
			if (OWNER(rest) == thing) {
				OWNER(rest) = GOD;
				DBDIRTY(rest);
			}
			break;
		case TYPE_EXIT:
			{
				int i, j;

				for (i = j = 0; i < DBFETCH(rest)->sp.exit.ndest; i++) {
					if ((DBFETCH(rest)->sp.exit.dest)[i] != thing)
						(DBFETCH(rest)->sp.exit.dest)[j++] = (DBFETCH(rest)->sp.exit.dest)[i];
				}
				if (j < DBFETCH(rest)->sp.exit.ndest) {
					SETVALUE(OWNER(rest), GETVALUE(OWNER(rest)) + LINK_COST);
					DBDIRTY(OWNER(rest));
					DBFETCH(rest)->sp.exit.ndest = j;
					DBDIRTY(rest);
				}
			}
			if (OWNER(rest) == thing) {
				OWNER(rest) = GOD;
				DBDIRTY(rest);
			}
			break;
		case TYPE_PLAYER:
			if (PLAYER_HOME(rest) == thing) {
				PLAYER_SET_HOME(rest, PLAYER_START);
				DBDIRTY(rest);
			}
			if (DBFETCH(rest)->exits == thing) {
				DBFETCH(rest)->exits = DBFETCH(thing)->next;
				DBDIRTY(rest);
			}
			break;
		}
		/*
		 *if (DBFETCH(rest)->location == thing)
		 *    DBSTORE(rest, location, NOTHING);
		 */
		if (DBFETCH(rest)->contents == thing)
			DBSTORE(rest, contents, DBFETCH(thing)->next);
		if (DBFETCH(rest)->next == thing)
			DBSTORE(rest, next, DBFETCH(thing)->next);
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && ((first = DBFETCH(thing)->contents) != NOTHING)) {
		if (Typeof(first) == TYPE_PLAYER) {
			enter_room(first, HOME, DBFETCH(thing)->location);
			/* If the room is set to drag players back, there'll be no
			 * reasoning with it.  DRAG the player out.
			 */
			if (DBFETCH(first)->location == thing) {
				notifyf(player, "Escaping teleport loop!  Going home.");
				moveto(first, HOME);
			}
		} else {
			moveto(first, HOME);
		}
	}


	moveto(thing, NOTHING);

	depth--;

	db_free_object(thing);
	db_clear_object(thing);
	NAME(thing) = (char*) strdup("<garbage>");
	SETDESC(thing, "<recyclable>");
	FLAGS(thing) = TYPE_GARBAGE;

	DBFETCH(thing)->next = recyclable;
	recyclable = thing;
	DBDIRTY(thing);
}
static const char *move_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_move_c_version(void) { return move_c_version; }

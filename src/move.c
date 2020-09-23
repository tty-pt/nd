/* $Header$ */

#include "copyright.h"
#include "config.h"

#include "db.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "fb.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#include "kill.h"
#include "search.h"

void
moveto(dbref what, dbref where)
{
	dbref loc;

	/* do NOT move garbage */
	if (what != NOTHING && Typeof(what) == TYPE_GARBAGE) {
		return;
	}

	/* remove what from old loc */
	if ((loc = DBFETCH(what)->location) != NOTHING) {
		DBSTORE(loc, contents, remove_first(DBFETCH(loc)->contents, what));
	}
	/* test for special cases */
	switch (where) {
	case NOTHING:
		DBSTORE(what, location, NOTHING);
		return;					/* NOTHING doesn't have contents */
	case HOME:
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
		break;
        default:
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
	}

	/* now put what in where */
	PUSH(what, DBFETCH(where)->contents);
	DBDIRTY(where);
	DBSTORE(what, location, where);
}

dbref reverse(dbref);
void
send_contents(command_t *cmd, dbref dest)
{
	dbref loc = cmd->player;
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

void
maybe_dropto(command_t *cmd, dbref dropto)
{
	dbref loc = cmd->player;
	dbref thing;

	if (loc == dropto)
		return;					/* bizarre special case */

	/* check for players */
	DOLIST(thing, DBFETCH(loc)->contents) {
		if (Typeof(thing) == TYPE_PLAYER)
			return;
	}

	/* no players, send everything to the dropto */
	send_contents(cmd, dropto);
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

static int donelook = 0;
void
enter_room(command_t *cmd, dbref loc, dbref exit)
{
	dbref player = cmd->player;
	dbref old;
	dbref dropto;
	char buf[BUFFER_LEN];

	/* check for room == HOME */
	if (loc == HOME)
		loc = PLAYER_HOME(player);	/* home */

	/* get old location */
	old = DBFETCH(player)->location;

	if (parent_loop_check(player, loc)) {
	  switch (Typeof(player)) {
	  case TYPE_PLAYER:
	    loc = PLAYER_HOME(player);
	    break;
	  case TYPE_THING:
	    loc = THING_HOME(player);
	    if (parent_loop_check(player, loc)) {
	      loc = PLAYER_HOME(OWNER(player));
	      if (parent_loop_check(player, loc))
		loc = (dbref) PLAYER_START;
	    }
	    break;
	  case TYPE_ROOM:
	    loc = GLOBAL_ENVIRONMENT;
	    break;
	  }
	}

	/* check for self-loop */
	/* self-loops don't do move or other player notification */
	/* but you still get autolook and penny check */
	if (loc != old) {
		/* go there */
		moveto(player, loc);

		if (old != NOTHING) {
			propqueue(cmd, old, exit, player, NOTHING, "_depart", "Depart", 1, 1);
			envpropqueue(cmd, old, exit, old, NOTHING, "_depart", "Depart", 1, 1);

			propqueue(cmd, old, exit, player, NOTHING, "_odepart", "Odepart", 1, 0);
			envpropqueue(cmd, old, exit, old, NOTHING, "_odepart", "Odepart", 1, 0);

			/* notify others unless DARK */
			if (exit == NOTHING || (!Dark(old) && !Dark(player) &&
				Typeof(player) != TYPE_THING
				&& (Typeof(exit) != TYPE_EXIT || !Dark(exit)))) {
#if !defined(QUIET_MOVES)
				snprintf(buf, sizeof(buf), "%s has left.", NAME(player));
				notify_except(DBFETCH(old)->contents, player, buf, player);
#endif
			}
		}

		/* if old location has STICKY dropto, send stuff through it */
		if (old != NOTHING && Typeof(old) == TYPE_ROOM
			&& (dropto = DBFETCH(old)->sp.room.dropto) != NOTHING && (FLAGS(old) & STICKY)) {
			command_t cmd_dt = command_new_null(cmd->fd, old);
			maybe_dropto(&cmd_dt, dropto);
		}

		geo_clean(cmd, old);

		/* tell other folks in new location if not DARK */
		if (loc != NOTHING && !Dark(loc) && !Dark(player) &&
			 Typeof(player) != TYPE_THING
			&& (exit == NOTHING || Typeof(exit) != TYPE_EXIT || !Dark(exit))) {
#if !defined(QUIET_MOVES)
			snprintf(buf, sizeof(buf), "%s has arrived.", NAME(player));
			notify_except(DBFETCH(loc)->contents, player, buf, player);
#endif
		}

		mobs_aggro(cmd);
		/* TODO geo_notify(descr, player); */
	}



	/* autolook */
	if (Typeof(player) != TYPE_THING) {
		if (donelook < 8) {
			donelook++;
			if (can_move(cmd, AUTOLOOK_CMD, 1)) {
				go_move(cmd, AUTOLOOK_CMD, 1);
			} else {
				do_look_around(cmd);
			}
			donelook--;
		} else {
			notify(player, "Look aborted because of look action loop.");
		}
	}

#if PENNY_RATE != 0
	/* check for pennies */
	if (!controls(player, loc)
	    && GETVALUE(OWNER(player)) <= MAX_PENNIES 
	    && RANDOM() % PENNY_RATE == 0) {
		notify_fmt(player, "You found one %s!", PENNY);
		SETVALUE(OWNER(player), GETVALUE(OWNER(player)) + 1);
		DBDIRTY(OWNER(player));
	}
#endif

	if (loc != old) {
		envpropqueue(cmd, loc, exit, player, NOTHING, "_arrive", "Arrive", 1, 1);
		envpropqueue(cmd, loc, exit, player, NOTHING, "_oarrive", "Oarrive", 1, 0);
	}
}

void
send_home(command_t *cmd, int puppethome)
{
	dbref thing = cmd->player;
	switch (Typeof(thing)) {
	case TYPE_PLAYER:
		/* send his possessions home first! */
		/* that way he sees them when he arrives */
		send_contents(cmd, HOME);
		enter_room(cmd, PLAYER_HOME(thing), DBFETCH(thing)->location);
		break;
	case TYPE_THING:
		if (puppethome)
			send_contents(cmd, HOME);
		moveto(thing, HOME);	/* home */
		break;
	default:
		/* no effect */
		break;
	}
	return;
}

int
can_move(command_t *cmd, const char *direction, int lev)
{
	struct match_data md;

#if ALLOW_HOME
	if (!strcmp(direction, "home"))
		return 1;
#endif

	/* otherwise match on exits */
	init_match(cmd, direction, TYPE_EXIT, &md);
	md.match_level = lev;
	match_all_exits(&md);
	return (last_match_result(&md) != NOTHING);
}

/*
 * trigger()
 *
 * This procedure triggers a series of actions, or meta-actions
 * which are contained in the 'dest' field of the exit.
 * Locks other than the first one are over-ridden.
 *
 * `player' is the player who triggered the exit
 * `exit' is the exit triggered
 * `pflag' is a flag which indicates whether player and room exits
 * are to be used (non-zero) or ignored (zero).  Note that
 * player/room destinations triggered via a meta-link are
 * ignored.
 *
 */

void
trigger(command_t *cmd, dbref exit, int pflag)
{
	dbref player = cmd->player;
	int i;
	dbref dest;
	int sobjact;				/* sticky object action flag, sends home

								   * source obj */
	int succ;

	sobjact = 0;
	succ = 0;

	// quickfix for gexits
	if (e_exit_is(exit)) {
		union specific sp
			= DBFETCH(exit)->sp;
		if (sp.exit.ndest && sp.exit.dest[0] != NOTHING)
			dest = sp.exit.dest[0];
		else
			dest = geo_there(getloc(exit), exit_e(exit));

		if (dest > 0) {
			enter_room(cmd, dest, exit);
			succ = 1;
		} else {
			dest = geo_room(cmd, exit);
			enter_room(cmd, dest, exit);
			succ = 1;
		}

	} else for (i = 0; i < DBFETCH(exit)->sp.exit.ndest; i++) {
		dest = (DBFETCH(exit)->sp.exit.dest)[i];
		if (dest == HOME) {
			dest = PLAYER_HOME(player);

			/* fix #1112946 temporarily -- premchai21 */
			if (Typeof(dest) == TYPE_THING) {
				notify(player, "That would be an undefined operation.");
				continue;
			}
		}

		switch (Typeof(dest)) {
		case TYPE_ROOM:
			if (pflag) {
				if (parent_loop_check(player, dest)) {
					notify(player, "That would cause a paradox.");
					break;
				}
				if (GETDROP(exit))
					exec_or_notify_prop(cmd, exit, MESGPROP_DROP, "(@Drop)");
				if (GETODROP(exit) && !Dark(player)) {
					parse_oprop(cmd, dest, exit, MESGPROP_ODROP,
								   NAME(player), "(@Odrop)");
				}
				enter_room(cmd, dest, exit);
				succ = 1;
			}
			break;
		case TYPE_THING:
			if (Typeof(DBFETCH(exit)->location) == TYPE_THING) {
				if (parent_loop_check(dest, getloc(getloc(exit)))) {
					notify(player, "That would cause a paradox.");
					break;
				}
				moveto(dest, DBFETCH(DBFETCH(exit)->location)->location);
				if (!(FLAGS(exit) & STICKY)) {
					/* send home source object */
					sobjact = 1;
				}
			} else {
				if (parent_loop_check(dest, getloc(exit))) {
					notify(player, "That would cause a paradox.");
					break;
				}
				moveto(dest, DBFETCH(exit)->location);
			}
			if (GETSUCC(exit))
				succ = 1;
			break;
		case TYPE_EXIT:		/* It's a meta-link(tm)! */
			trigger(cmd, (DBFETCH(exit)->sp.exit.dest)[i], 0);
			if (GETSUCC(exit))
				succ = 1;
			break;
		case TYPE_PLAYER:
			if (pflag && DBFETCH(dest)->location != NOTHING) {
				if (parent_loop_check(player, dest)) {
					notify(player, "That would cause a paradox.");
					break;
				}
				succ = 1;
				if (FLAGS(dest) & JUMP_OK) {
					if (GETDROP(exit)) {
						exec_or_notify_prop(cmd, exit, MESGPROP_DROP, "(@Drop)");
					}
					if (GETODROP(exit) && !Dark(player)) {
						parse_oprop(cmd, getloc(dest), exit,
									   MESGPROP_ODROP, NAME(player), "(@Odrop)");
					}
					enter_room(cmd, DBFETCH(dest)->location, exit);
				} else {
					notify(player, "That player does not wish to be disturbed.");
				}
			}
			break;
		}
	}
	if (sobjact) {
		command_t cmd_sh = command_new_null(cmd->fd, DBFETCH(exit)->location);
		send_home(&cmd_sh, 0);
	}
	if (!succ && pflag)
		notify(player, "Done.");
}

void
go_move(command_t *cmd, const char *direction, int lev)
{
	dbref player = cmd->player;
	dbref loc;

#if ALLOW_HOME
	if (!strcmp(direction, "home")) {
		char buf[BUFFER_LEN];

		/* send him home */
		/* but steal all his possessions */
		if ((loc = DBFETCH(player)->location) != NOTHING) {
			/* tell everybody else */
			snprintf(buf, sizeof(buf), "%s goes home.", NAME(player));
			notify_except(DBFETCH(loc)->contents, player, buf, player);
		}
		/* give the player the messages */
		send_home(cmd, 1);
	} else
#endif
	{
		struct match_data md;
		dbref exit;

		/* find the exit */
		init_match_check_keys(cmd, direction, TYPE_EXIT, &md);
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

			if (!can_doit(cmd, exit, "You can't go that way."))
				break;

			trigger(cmd, exit, 1);
			break;
		}
	}
}

void
do_move(command_t *cmd)
{
	go_move(cmd, cmd->argv[1], atoi(cmd->argv[2]));
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

	init_match_check_keys(cmd, what, TYPE_THING, &md);
	match_neighbor(&md);
	match_possession(&md);
	if (Wizard(OWNER(player)))
		match_absolute(&md);	/* the wizard has long fingers */

	if ((thing = noisy_match_result(&md)) != NOTHING) {
		cont = thing;
		if (obj && *obj) {
			init_match_check_keys(cmd, obj, TYPE_THING, &md);
			match_rmatch(cont, &md);
			if (Wizard(OWNER(player)))
				match_absolute(&md);	/* the wizard has long fingers */
			if ((thing = noisy_match_result(&md)) == NOTHING) {
				return;
			}
			if (Typeof(cont) == TYPE_PLAYER) {
				notify(player, "You can't steal things from players.");
				return;
			}
			if (!test_lock_false_default(cmd, cont, "_/clk")) {
				notify(player, "You can't open that container.");
				return;
			}
		}
		if (Typeof(player) != TYPE_PLAYER) {
			if (Typeof(DBFETCH(thing)->location) != TYPE_ROOM) {
				if (OWNER(player) != OWNER(thing)) {
					notify(player, "Zombies aren't allowed to be thieves!");
					return;
				}
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
				cando = could_doit(cmd, thing);
				if (!cando)
					notify(player, "You can't get that.");
			} else {
				cando = can_doit(cmd, thing, "You can't pick that up.");
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

	init_match(cmd, name, NOTYPE, &md);
	match_possession(&md);
	if ((thing = noisy_match_result(&md)) == NOTHING || thing == AMBIGUOUS)
		return;

	cont = loc;
	if (obj && *obj) {
		init_match(cmd, obj, NOTYPE, &md);
		match_possession(&md);
		match_neighbor(&md);
		if (Wizard(OWNER(player)))
			match_absolute(&md);	/* the wizard has long fingers */
		if ((cont = noisy_match_result(&md)) == NOTHING || thing == AMBIGUOUS) {
			return;
		}
	}
	switch (Typeof(thing)) {
	case TYPE_THING:
		if (DBFETCH(thing)->location != player) {
			/* Shouldn't ever happen. */
			notify(player, "You can't drop that.");
			break;
		}
		if (Typeof(cont) != TYPE_ROOM && Typeof(cont) != TYPE_PLAYER &&
			Typeof(cont) != TYPE_THING) {
			notify(player, "You can't put anything in that.");
			break;
		}
		if (Typeof(cont) != TYPE_ROOM &&
			!test_lock_false_default(cmd, cont, "_/clk")) {
			notify(player, "You don't have permission to put something in that.");
			break;
		}
		if (parent_loop_check(thing, cont)) {
			notify(player, "You can't put something inside of itself.");
			break;
		}
		if (Typeof(cont) == TYPE_ROOM && (FLAGS(thing) & STICKY) &&
			Typeof(thing) == TYPE_THING) {
			command_t cmd_sh = command_new_null(cmd->fd, thing);
			send_home(&cmd_sh, 0);
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
			notify_fmt(cont, "%s hands you %s", NAME(player), NAME(thing));
			notify_fmt(player, "You hand %s to %s", NAME(thing), NAME(cont));
			return;
		}

		if (GETDROP(thing))
			exec_or_notify_prop(cmd, thing, MESGPROP_DROP, "(@Drop)");
		else
			notify(player, "Dropped.");

		if (GETDROP(loc))
			exec_or_notify_prop(cmd, loc, MESGPROP_DROP, "(@Drop)");

		if (GETODROP(thing)) {
			parse_oprop(cmd, loc, thing, MESGPROP_ODROP,
						   NAME(player), "(@Odrop)");
		} else {
			snprintf(buf, sizeof(buf), "%s drops %s.", NAME(player), NAME(thing));
			notify_except(DBFETCH(loc)->contents, player, buf, player);
		}

		if (GETODROP(loc)) {
			parse_oprop(cmd, loc, loc, MESGPROP_ODROP, NAME(thing), "(@Odrop)");
		}
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

	init_match(cmd, name, TYPE_THING, &md);
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
			recycle(cmd, thing);
			notify(player, buf);
		}
	}
}

void
recycle(command_t *cmd, dbref thing)
{
	dbref player = cmd->player;
	extern dbref recyclable;
	static int depth = 0;
	dbref first;
	dbref rest;
	int looplimit;

	depth++;
	if (force_level) {
		if(thing == force_prog) {
			warn("SANITYCHECK: Was about to recycle FORCEing object #%d!", thing);
			notify(player, "ERROR: Cannot recycle an object FORCEing you!");
			return;
		}
	}
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
					gexit_snull(cmd, first);
				recycle(cmd, first);
			}
		}
		if (GETTMP(thing))
			for (first = DBFETCH(thing)->contents; first != NOTHING; first = rest) {
				rest = DBFETCH(first)->next;
				if (Typeof(first) != TYPE_PLAYER)
					recycle(cmd, first);
			}
		notify_except(DBFETCH(thing)->contents, NOTHING,
					  "You feel a wrenching sensation...", player);
		map_delete(thing);
		break;
	case TYPE_THING:
		if (GETLID(thing) >= 0) {
			mobi_t *liv = MOBI(thing);
			liv->who = -1;
		}
		if (!Wizard(OWNER(thing)))
			SETVALUE(OWNER(thing), GETVALUE(OWNER(thing)) + GETVALUE(thing));
		DBDIRTY(OWNER(thing));
		for (first = DBFETCH(thing)->exits; first != NOTHING; first = rest) {
			rest = DBFETCH(first)->next;
			if (DBFETCH(first)->location == NOTHING || DBFETCH(first)->location == thing)
				recycle(cmd, first);
		}
		if (GETTMP(getloc(thing))) {
			SETTMP(thing, 1);
			for (first = DBFETCH(thing)->contents; first != NOTHING; first = rest) {
				rest = DBFETCH(first)->next;
				recycle(cmd, first);
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
			command_t cmd_er = command_new_null(cmd->fd, first);
			enter_room(&cmd_er, HOME, DBFETCH(thing)->location);
			/* If the room is set to drag players back, there'll be no
			 * reasoning with it.  DRAG the player out.
			 */
			if (DBFETCH(first)->location == thing) {
				notify_fmt(player, "Escaping teleport loop!  Going home.");
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

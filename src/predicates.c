/* $Header$ */

#include "copyright.h"
#include "config.h"

/* Predicates for testing various conditions */

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "props.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "externs.h"
#include "geography.h"
#include "mob.h"

int
OkObj(dbref obj)
{
	return(!(obj < 0 || obj >= db_top || Typeof(obj) == TYPE_GARBAGE));
}


int
can_link_to(dbref who, object_flag_type what_type, dbref where)
{
		/* Can always link to HOME */
	if (where == HOME)
		return 1;
		/* Can't link to an invalid dbref */
	if (where < 0 || where >= db_top)
		return 0;
	switch (what_type) {
	case TYPE_EXIT:
		/* If the target is LINK_OK, then any exit may be linked
		 * there.  Otherwise, only someone who controls the
		 * target may link there. */
		return (controls(who, where) || (FLAGS(where) & LINK_OK));
		/* NOTREACHED */
		break;
	case TYPE_PLAYER:
		/* Players may only be linked to rooms, that are either
		 * controlled by the player or set either L or A. */
		return (Typeof(where) == TYPE_ROOM && (controls(who, where)
											   || Linkable(where)));
		/* NOTREACHED */
		break;
	case TYPE_ROOM:
		/* Rooms may be linked to rooms or things (this sets their
		 * dropto location).  Target must be controlled, or be L or A. */
		return ((Typeof(where) == TYPE_ROOM || Typeof(where) == TYPE_THING)
				&& (controls(who, where) || Linkable(where)));
		/* NOTREACHED */
		break;
	case TYPE_THING:
		/* Things may be linked to rooms, players, or other things (this
		 * sets the thing's home).  Target must be controlled, or be L or A. */
		return (
				(Typeof(where) == TYPE_ROOM || Typeof(where) == TYPE_PLAYER ||
				 Typeof(where) == TYPE_THING) && 
				 (controls(who, where) || Linkable(where)));
		/* NOTREACHED */
		break;
	case NOTYPE:
		/* Why is this here? -winged */
		return (controls(who, where) || (FLAGS(where) & LINK_OK) ||
				(Typeof(where) != TYPE_THING && (FLAGS(where) & ABODE)));
		/* NOTREACHED */
		break;
	default:
		/* Programs can't be linked anywhere */
		return 0;
		/* NOTREACHED */
		break;
	}
	/* NOTREACHED */
	return 0;
}

/* This checks to see if what can be linked to something else by who. */
int
can_link(dbref who, dbref what)
{
	/* Anyone can link an exit that is currently unlinked. */
	return (controls(who, what) || ((Typeof(what) == TYPE_EXIT)
									&& DBFETCH(what)->sp.exit.ndest == 0));
}

/*
 * Revision 1.2 -- SECURE_TELEPORT
 * you can only jump with an action from rooms that you own
 * or that are jump_ok, and you cannot jump to players that are !jump_ok.
 */

/*
 * could_doit: Checks to see if player could actually do what is proposing
 * to be done: if thing is an exit, this checks to see if the exit will
 * perform a move that is allowed. Then, it checks the @lock on the thing,
 * whether it's an exit or not.
 */
int
could_doit(dbref player, dbref thing)
{
	dbref source, dest, owner;

	if (Typeof(thing) == TYPE_EXIT) {
		/* If exit is unlinked, can't do it.
		 * Unless its a geo exit */
		if (DBFETCH(thing)->sp.exit.ndest == 0) {
			if (e_exit_is(thing))
				goto geo;
			else
				return 0;
		}

		owner = OWNER(thing);
		source = DBFETCH(player)->location;
		dest = *(DBFETCH(thing)->sp.exit.dest);

		if (dest < 0 && e_exit_is(thing))
			goto geo;

		else if (Typeof(dest) == TYPE_PLAYER) {
			/* Check for additional restrictions related to player dests */
			dbref destplayer = dest;

			dest = DBFETCH(dest)->location;
			/* If the dest player isn't JUMP_OK, or if the dest player's loc
			 * is set BOUND, can't do it. */
			if (!(FLAGS(destplayer) & JUMP_OK) || (FLAGS(dest) & BUILDER)) {
				return 0;
			}
		}

		/* for actions */
		if ((DBFETCH(thing)->location != NOTHING) &&
			(Typeof(DBFETCH(thing)->location) != TYPE_ROOM)) {

			/* If this is an exit on a Thing or a Player... */

			/* If the destination is a room or player, and the current
			 * location is set BOUND (note: if the player is in a vehicle
			 * set BUILDER this will also return failure) */
			if ((Typeof(dest) == TYPE_ROOM || Typeof(dest) == TYPE_PLAYER) &&
				(FLAGS(source) & BUILDER)) return 0;
		}
	}

	goto out;
geo:
	if (!e_exit_can(player, thing))
		return 0;
out:
		/* Check the @lock on the thing, as a final test. */
	return (eval_boolexp(player, GETLOCK(thing), thing));
}


int
test_lock(dbref player, dbref thing, const char *lockprop)
{
	struct boolexp *lokptr;

	lokptr = get_property_lock(thing, lockprop);
	return (eval_boolexp(player, lokptr, thing));
}


int
test_lock_false_default(dbref player, dbref thing, const char *lockprop)
{
	struct boolexp *lok;

	lok = get_property_lock(thing, lockprop);

	if (lok == TRUE_BOOLEXP)
		return 0;
	return (eval_boolexp(player, lok, thing));
}

int
can_doit(dbref player, dbref thing, const char *default_fail_msg)
{
	char const *dwts = "door";
	char dir = '\0';
	int door = 0;

	if (thing == NOTHING) {
		notify(player, default_fail_msg);
		return 0;
	}

	if (MOB(player)->klock) {
		notify(player, "You can not do that right now.");
		return 0;
	}

	if (Typeof(thing) == TYPE_EXIT) {
		dir = NAME(thing)[0];

		if (GETDOOR(thing)) {
			if (dir == 'u' || dir == 'd') {
				dwts = "hatch";
				door = 2;
			} else
				door = 1;
		}

	}

	if (!could_doit(player, thing)) {
		/* can't do it */
		if (GETFAIL(thing)) {
			notify(player, GETFAIL(thing));
		} else if (door) {
			notifyf(player, "That %s is locked.", dwts);
		} else if (default_fail_msg) {
			notify(player, default_fail_msg);
		}
		return 0;
	} else {
		do_stand_silent(player);

		if (door)
			notifyf(player, "You open the %s.", dwts);

		/* can do it */
		if (GETSUCC(thing)) {
			notify(player, GETSUCC(thing));
		} else if (Typeof(thing) == TYPE_EXIT && e_exit_is(thing))
			notifyf(player, "You go %s.", e_name(exit_e(thing)));
		if (door)
			notifyf(player, "You close the %s.", dwts);
		return 1;
	}
}

int
can_see(dbref player, dbref thing, int can_see_loc)
{
	if (player == thing || Typeof(thing) == TYPE_EXIT ||
			Typeof(thing) == TYPE_ROOM)
		return 0;

	if (can_see_loc) {
		return (!Dark(thing) ||
			(controls(player, thing) && !(FLAGS(player) & STICKY)));
	} else {
		/* can't see loc */
		return (controls(player, thing) && !(FLAGS(player) & STICKY));
	}
}

int
controls(dbref who, dbref what)
{
	/* No one controls invalid objects */
	if (what < 0 || what >= db_top)
		return 0;

	/* No one controls garbage */
	if (Typeof(what) == TYPE_GARBAGE)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	if (Typeof(who) != TYPE_PLAYER)
		who = OWNER(who);

	/* Wizard controls everything */
	if (Wizard(who)) {
#ifdef GOD_PRIV
		if(God(OWNER(what)) && !God(who))
			/* Only God controls God's objects */
			return 0;
		else
#endif
		return 1;
	}

	/* exits are also controlled by the owners of the source and destination */
	/* ACTUALLY, THEY AREN'T.  IT OPENS A BAD MPI SECURITY HOLE. */
	/* any MPI on an exit's @succ or @fail would be run in the context
	 * of the owner, which would allow the owner of the src or dest to
	 * write malicious code for the owner of the exit to run.  Allowing them
	 * control would allow them to modify _ properties, thus enabling the
	 * security hole. -winged */
	/*
	 * if (Typeof(what) == TYPE_EXIT) {
	 *    int     i = DBFETCH(what)->sp.exit.ndest;
	 *
	 *    while (i > 0) {
	 *        if (who == OWNER(DBFETCH(what)->sp.exit.dest[--i]))
	 *            return 1;
	 *    }
	 *    if (who == OWNER(DBFETCH(what)->location))
	 *        return 1;
	 * }
	 */

	/* owners control their own stuff */
	return (who == OWNER(what));
}

int
restricted(dbref player, dbref thing, object_flag_type flag)
{
	switch (flag) {
	case ABODE:
		return 0;
		/* NOTREACHED */
		break;
	case DARK:
		/* Dark can be set on a Program or Room by anyone. */
		return !Wizard(OWNER(player))
			&& (/* Setting a player dark requires a wizard. */
			    Typeof(thing) == TYPE_PLAYER

			    /* If exit darking is restricted, it requires a wizard. */
			    || (!EXIT_DARKING && Typeof(thing) == TYPE_EXIT)

			    /* If thing darking is restricted, it requires a wizard. */
			    || (!THING_DARKING && Typeof(thing) == TYPE_THING));
	case QUELL:
#ifdef GOD_PRIV
		/* Only God (or God's stuff) can quell or unquell another wizard. */
		return (God(OWNER(player)) || (TrueWizard(thing) && (thing != player) &&
					Typeof(thing) == TYPE_PLAYER));
#else
		/* You cannot quell or unquell another wizard. */
		return (TrueWizard(thing) && (thing != player) && (Typeof(thing) == TYPE_PLAYER));
#endif
		/* NOTREACHED */
		break;
	case BUILDER:
		/* Would someone tell me why setting a program SMUCKER|MUCKER doesn't
		 * go through here? -winged */
		/* Setting a program Bound causes any function called therein to be
		 * put in preempt mode, regardless of the mode it had before.
		 * Since this is just a convenience for atomic-functionwriters,
		 * why is it limited to only a Wizard? -winged */
		/* Setting a player Builder is limited to a Wizard. */
		return (!Wizard(OWNER(player)));
		/* NOTREACHED */
		break;
	case WIZARD:
			/* To do anything with a Wizard flag requires a Wizard. */
		if (Wizard(OWNER(player))) {
#ifdef GOD_PRIV
			/* ...but only God can make a player a Wizard, or re-mort
			 * one. */
			return ((Typeof(thing) == TYPE_PLAYER) && !God(player));
#else							/* !GOD_PRIV */
			/* We don't want someone setting themselves !W, to prevent
			 * a case where there are no wizards at all */
			return ((Typeof(thing) == TYPE_PLAYER && thing == OWNER(player)));
#endif							/* GOD_PRIV */
		} else
			return 1;
		/* NOTREACHED */
		break;
	default:
			/* No other flags are restricted. */
		return 0;
		/* NOTREACHED */
		break;
	}
	/* NOTREACHED */
}

/* Removes 'cost' value from 'who', and returns 1 if the act has been
 * paid for, else returns 0. */
int
payfor(dbref who, int cost)
{
	who = OWNER(who);
		/* Wizards don't have to pay for anything. */
	if (Wizard(who)) {
		return 1;
	} else if (GETVALUE(who) >= cost) {
		SETVALUE(who, GETVALUE(who) - cost);
		DBDIRTY(who);
		return 1;
	} else {
		return 0;
	}
}

int
word_start(const char *str, const char let)
{
	int chk;

	for (chk = 1; *str; str++) {
		if (chk && (*str == let))
			return 1;
		chk = (*str == ' ');
	}
	return 0;
}

int
ok_ascii_any(const char *name)
{
	const unsigned char *scan;
	for( scan=(const unsigned char*) name; *scan; ++scan ) {
		if( *scan>127 )
			return 0;
	}
	return 1;
}

int
ok_name(const char *name)
{
	return (name
			&& *name
			&& *name != LOOKUP_TOKEN
			&& *name != REGISTERED_TOKEN
			&& *name != NUMBER_TOKEN
			&& !strchr(name, ARG_DELIMITER)
			&& !strchr(name, AND_TOKEN)
			&& !strchr(name, OR_TOKEN)
			&& !strchr(name, '\r')
			&& !strchr(name, ESCAPE_CHAR)
			&& !word_start(name, NOT_TOKEN)
			&& strcmp(name, "me")
			&& strcmp(name, "home")
			&& strcmp(name, "here")
			&& (
				!*RESERVED_NAMES ||
				!equalstr((char*)RESERVED_NAMES, (char*)name)
			));
}

int
ok_player_name(const char *name)
{
	const char *scan;

	if (!ok_name(name) || strlen(name) > PLAYER_NAME_LIMIT)
		return 0;
	

	for (scan = name; *scan; scan++) {
		if (!(isprint(*scan)
			 && !isspace(*scan))
			 && *scan != '('
			 && *scan != ')'
			 && *scan != '\''
			 && *scan != ',') {	
		    /* was isgraph(*scan) */
			return 0;
		}
	}

	/* Check the name isn't reserved */
	if (*RESERVED_PLAYER_NAMES && equalstr((char*)RESERVED_PLAYER_NAMES, (char*)name))
		return 0;

	/* lookup name to avoid conflicts */
	return (lookup_player(name) == NOTHING);
}

int
ok_password(const char *password)
{
	const char *scan;

	/* Password cannot be blank */
	if (*password == '\0')
		return 0;

	/* Password also cannot contain any nonprintable or space-type
	 * characters */
	for (scan = password; *scan; scan++) {
		if (!(isprint(*scan) && !isspace(*scan))) {
			return 0;
		}
	}

	/* Anything else is fair game */
	return 1;
}

static const char *predicates_c_version = "$RCSfile$ $Revision: 1.20 $";
const char *get_predicates_c_version(void) { return predicates_c_version; }

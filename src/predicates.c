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

/*
 * Revision 1.2 -- SECURE_TELEPORT
 * you can only jump with an action from rooms that you own
 * or that are jump_ok, and you cannot jump to players that are !jump_ok.
 */

int
can_doit(dbref player, dbref thing, const char *default_fail_msg)
{
	if (thing == NOTHING) {
		notify(player, default_fail_msg);
		return 0;
	}

	if (Typeof(player) != TYPE_ENTITY) {
		notify(player, "You are not an entity.");
		return 0;
	}

	if (ENTITY(player)->klock) {
		notify(player, "You can not do that right now.");
		return 0;
	}

	do_stand_silent(player);

	return 1;
}

int
can_see(dbref player, dbref thing, int can_see_loc)
{
	if (player == thing || Typeof(thing) == TYPE_ROOM)
		return 0;

	if (can_see_loc) {
		return (!Dark(thing) ||
			controls(player, thing));
	} else {
		/* can't see loc */
		return controls(player, thing);
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
	if (Typeof(who) != TYPE_ENTITY)
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

	/* owners control their own stuff */
	return (who == OWNER(what));
}

int
restricted(dbref player, dbref thing, object_flag_type flag)
{
	switch (flag) {
	case DARK:
		/* Dark can be set on a Program or Room by anyone. */
		return !Wizard(OWNER(player))
			&& (/* Setting a player dark requires a wizard. */
			    Typeof(thing) == TYPE_ENTITY

			    /* If thing darking is restricted, it requires a wizard. */
			    || (!THING_DARKING && is_item(thing)));
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
			return ((Typeof(thing) == TYPE_ENTITY) && !God(player));
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

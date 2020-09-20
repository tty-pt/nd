/* $Header$ */

#include "copyright.h"
#include "config.h"

/* Commands that create new objects */

#include <string.h>
#include <ctype.h>
#include "db.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include "match.h"
#include "fbstrings.h"

/* parse_linkable_dest()
 *
 * A utility for open and link which checks whether a given destination
 * string is valid.  It returns a parsed dbref on success, and NOTHING
 * on failure.
 */

static dbref
parse_linkable_dest(command_t *cmd, dbref exit, const char *dest_name)
{
	dbref player = cmd->player;
	dbref dobj;					/* destination room/player/thing/link */
	static char buf[BUFFER_LEN];
	struct match_data md;

	init_match(cmd, dest_name, NOTYPE, &md);
	match_absolute(&md);
	match_everything(&md);
	match_home(&md);

	if ((dobj = match_result(&md)) == NOTHING || dobj == AMBIGUOUS) {
		snprintf(buf, sizeof(buf), "I couldn't find '%s'.", dest_name);
		notify(player, buf);
		return NOTHING;

	}

#if !TELEPORT_TO_PLAYER
	if (Typeof(dobj) == TYPE_PLAYER) {
		snprintf(buf, sizeof(buf), "You can't link to players.  Destination %s ignored.",
				unparse_object(player, dobj));
		notify(player, buf);
		return NOTHING;
	}
#endif

	if (!can_link(player, exit)) {
		notify(player, "You can't link that.");
		return NOTHING;
	}
	if (!can_link_to(player, Typeof(exit), dobj)) {
		snprintf(buf, sizeof(buf), "You can't link to %s.", unparse_object(player, dobj));
		notify(player, buf);
		return NOTHING;
	} else {
		return dobj;
	}
}

/* exit_loop_check()
 *
 * Recursive check for loops in destinations of exits.  Checks to see
 * if any circular references are present in the destination chain.
 * Returns 1 if circular reference found, 0 if not.
 */
int
exit_loop_check(dbref source, dbref dest)
{

	int i;

	if (source == dest)
		return 1;				/* That's an easy one! */
	if (Typeof(dest) != TYPE_EXIT)
		return 0;
	for (i = 0; i < DBFETCH(dest)->sp.exit.ndest; i++) {
		if ((DBFETCH(dest)->sp.exit.dest)[i] == source) {
			return 1;			/* Found a loop! */
		}
		if (Typeof((DBFETCH(dest)->sp.exit.dest)[i]) == TYPE_EXIT) {
			if (exit_loop_check(source, (DBFETCH(dest)->sp.exit.dest)[i])) {
				return 1;		/* Found one recursively */
			}
		}
	}
	return 0;					/* No loops found */
}

/* use this to create an exit */
void
do_open(command_t *cmd)
{
	dbref player = cmd->player;
	const char *direction = cmd->argv[1];
	const char *linkto = cmd->argv[2];
	dbref loc, exit;
	dbref good_dest[MAX_LINKS];
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *rname, *qname;
	int i, ndest;

	NOGUEST("@open",player);

	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	strlcpy(buf2, linkto, sizeof(buf2));
	for (rname = buf2; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		rname++;
	*qname = '\0';

	while (((qname--) > buf2) && (isspace(*qname)))
		*qname = '\0';
	qname = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	if ((loc = getloc(player)) == NOTHING)
		return;
	if (!*direction) {
		notify(player, "You must specify a direction or action name to open.");
		return;
	} else if (!ok_name(direction)) {
		notify(player, "That's a strange name for an exit!");
		return;
	}
	if (!controls(player, loc)) {
		notify(player, "Permission denied. (you don't control the location)");
		return;
	} else if (!payfor(player, EXIT_COST)) {
		notify_fmt(player, "Sorry, you don't have enough %s to open an exit.", PENNIES);
		return;
	} else {
		/* create the exit */
		exit = new_object();

		/* initialize everything */
		NAME(exit) = alloc_string(direction);
		DBFETCH(exit)->location = loc;
		OWNER(exit) = OWNER(player);
		FLAGS(exit) = TYPE_EXIT;
		DBFETCH(exit)->sp.exit.ndest = 0;
		DBFETCH(exit)->sp.exit.dest = NULL;

		/* link it in */
		PUSH(exit, DBFETCH(loc)->exits);
		DBDIRTY(loc);

		/* and we're done */
		snprintf(buf, sizeof(buf), "Exit opened with number %d.", exit);
		notify(player, buf);

		/* check second arg to see if we should do a link */
		if (*qname != '\0') {
			notify(player, "Trying to link...");
			if (!payfor(player, LINK_COST)) {
				notify_fmt(player, "You don't have enough %s to link.", PENNIES);
			} else {
				ndest = link_exit(cmd, exit, (char *) qname, good_dest);
				DBFETCH(exit)->sp.exit.ndest = ndest;
				DBFETCH(exit)->sp.exit.dest = (dbref *) malloc(sizeof(dbref) * ndest);
				for (i = 0; i < ndest; i++) {
					(DBFETCH(exit)->sp.exit.dest)[i] = good_dest[i];
				}
				DBDIRTY(exit);
			}
		}
	}

	if (*rname) {
		PData mydat;

		snprintf(buf, sizeof(buf), "Registered as $%s", rname);
		notify(player, buf);
		snprintf(buf, sizeof(buf), "_reg/%s", rname);
		mydat.flags = PROP_REFTYP;
		mydat.data.ref = exit;
		set_property(player, buf, &mydat);
	}
}

int
_link_exit(command_t *cmd, dbref exit, char *dest_name, dbref * dest_list, int dryrun)
{
	dbref player = cmd->player;
	char *p, *q;
	int prdest;
	dbref dest;
	int ndest, error;
	char buf[BUFFER_LEN], qbuf[BUFFER_LEN];

	prdest = 0;
	ndest = 0;
	error = 0;

	while (*dest_name) {
		while (isspace(*dest_name))
			dest_name++;		/* skip white space */
		p = dest_name;
		while (*dest_name && (*dest_name != EXIT_DELIMITER))
			dest_name++;
		q = (char *) strncpy(qbuf, p, BUFFER_LEN);	/* copy word */
		q[(dest_name - p)] = '\0';	/* terminate it */
		if (*dest_name)
			for (dest_name++; *dest_name && isspace(*dest_name); dest_name++) ;

		if ((dest = parse_linkable_dest(cmd, exit, q)) == NOTHING)
			continue;

		switch (Typeof(dest)) {
		case TYPE_PLAYER:
		case TYPE_ROOM:
			if (prdest) {
				snprintf(buf, sizeof(buf),
						"Only one player or room destination allowed. Destination %s ignored.",
						unparse_object(player, dest));
				notify(player, buf);

				if(dryrun)
					error = 1;

				continue;
			}
			dest_list[ndest++] = dest;
			prdest = 1;
			break;
		case TYPE_THING:
			dest_list[ndest++] = dest;
			break;
		case TYPE_EXIT:
			if (exit_loop_check(exit, dest)) {
				snprintf(buf, sizeof(buf),
						"Destination %s would create a loop, ignored.",
						unparse_object(player, dest));
				notify(player, buf);
				
				if(dryrun)
					error = 1;

				continue;
			}
			dest_list[ndest++] = dest;
			break;
		default:
			notify(player, "Internal error: weird object type.");
			warn("PANIC: weird object: Typeof(%d) = %ld", dest, Typeof(dest));

			if(dryrun)
				error = 1;
				
			break;
		}
		if(!dryrun) {
			if (dest == HOME) {
				notify(player, "Linked to HOME.");
			} else {
				snprintf(buf, sizeof(buf), "Linked to %s.", unparse_object(player, dest));
				notify(player, buf);
			}
		}
		
		if (ndest >= MAX_LINKS) {
			notify(player, "Too many destinations, rest ignored.");

			if(dryrun)
				error = 1;

			break;
		}
	}
	
	if(dryrun && error)
		return 0;
		
	return ndest;
}

/*
 * link_exit()
 *
 * This routine connects an exit to a bunch of destinations.
 *
 * 'player' contains the player's name.
 * 'exit' is the the exit whose destinations are to be linked.
 * 'dest_name' is a character string containing the list of exits.
 *
 * 'dest_list' is an array of dbref's where the valid destinations are
 * stored.
 *
 */

int
link_exit(command_t *cmd, dbref exit, char *dest_name, dbref * dest_list)
{
	return _link_exit(cmd, exit, dest_name, dest_list, 0);
}

/*
 * link_exit_dry()
 *
 * like link_exit(), but only checks whether the link would be ok or not.
 * error messages are still output.
 */
int
link_exit_dry(command_t *cmd, dbref exit, char *dest_name, dbref * dest_list)
{
	return _link_exit(cmd, exit, dest_name, dest_list, 1);
}

/* do_link
 *
 * Use this to link to a room that you own.  It also sets home for
 * objects and things, and drop-to's for rooms.
 * It seizes ownership of an unlinked exit, and costs 1 penny
 * plus a penny transferred to the exit owner if they aren't you
 *
 * All destinations must either be owned by you, or be LINK_OK.
 */
void
do_link(command_t *cmd)
{
	dbref player = cmd->player;
	const char *thing_name = cmd->argv[1];
	const char *dest_name = cmd->argv[2];
	dbref thing;
	dbref dest;
	dbref good_dest[MAX_LINKS];
	struct match_data md;

	int ndest, i;

	NOGUEST("@link",player);

	init_match(cmd, thing_name, TYPE_EXIT, &md);
	match_all_exits(&md);
	match_neighbor(&md);
	match_possession(&md);
	match_me(&md);
	match_here(&md);
	match_absolute(&md);
	match_registered(&md);
	if (Wizard(OWNER(player))) {
		match_player(&md);
	}
	if ((thing = noisy_match_result(&md)) == NOTHING)
		return;

	switch (Typeof(thing)) {
	case TYPE_EXIT:
		/* we're ok, check the usual stuff */
		if (DBFETCH(thing)->sp.exit.ndest != 0) {
			if (controls(player, thing)) {
				notify(player, "That exit is already linked.");
				return;
			} else {
				notify(player, "Permission denied. (you don't control the exit to relink)");
				return;
			}
		}
		/* handle costs */
		if (OWNER(thing) == OWNER(player)) {
			if (!payfor(player, LINK_COST)) {
				notify_fmt(player, "It costs %d %s to link this exit.",
						   LINK_COST, (LINK_COST == 1) ? PENNY : PENNIES);
				return;
			}
		} else {
			if (!payfor(player, LINK_COST + EXIT_COST)) {
				notify_fmt(player, "It costs %d %s to link this exit.",
						   (LINK_COST + EXIT_COST),
						   (LINK_COST + EXIT_COST == 1) ? PENNY : PENNIES);
				return;
			} else if (!Builder(player)) {
				notify(player, "Only authorized builders may seize exits.");
				return;
			} else {
				/* pay the owner for his loss */
				dbref owner = OWNER(thing);

				SETVALUE(owner, GETVALUE(owner) + EXIT_COST);
				DBDIRTY(owner);
			}
		}

		/* link has been validated and paid for; do it */
		OWNER(thing) = OWNER(player);
		ndest = link_exit(cmd, thing, (char *) dest_name, good_dest);
		if (ndest == 0) {
			notify(player, "No destinations linked.");
			SETVALUE(player, GETVALUE(player) + LINK_COST);
			DBDIRTY(player);
			break;
		}
		DBFETCH(thing)->sp.exit.ndest = ndest;
		if (DBFETCH(thing)->sp.exit.dest) {
		    free(DBFETCH(thing)->sp.exit.dest);
		}
		DBFETCH(thing)->sp.exit.dest = (dbref *) malloc(sizeof(dbref) * ndest);
		for (i = 0; i < ndest; i++) {
			(DBFETCH(thing)->sp.exit.dest)[i] = good_dest[i];
		}
		break;
	case TYPE_THING:
	case TYPE_PLAYER:
		init_match(cmd, dest_name, TYPE_ROOM, &md);
		match_neighbor(&md);
		match_absolute(&md);
		match_registered(&md);
		match_me(&md);
		match_here(&md);
		if (Typeof(thing) == TYPE_THING)
			match_possession(&md);
		if ((dest = noisy_match_result(&md)) == NOTHING)
			return;
		if (!controls(player, thing)
			|| !can_link_to(player, Typeof(thing), dest)) {
			notify(player, "Permission denied. (you don't control the thing, or you can't link to dest)");
			return;
		}
		if (parent_loop_check(thing, dest)) {
			notify(player, "That would cause a parent paradox.");
			return;
		}
		/* do the link */
		if (Typeof(thing) == TYPE_THING) {
			THING_SET_HOME(thing, dest);
		} else {
			PLAYER_SET_HOME(thing, dest);
		}
		notify(player, "Home set.");
		break;
	case TYPE_ROOM:			/* room dropto's */
		init_match(cmd, dest_name, TYPE_ROOM, &md);
		match_neighbor(&md);
		match_possession(&md);
		match_registered(&md);
		match_absolute(&md);
		match_home(&md);
		if ((dest = noisy_match_result(&md)) == NOTHING)
			break;
		if (!controls(player, thing) || !can_link_to(player, Typeof(thing), dest)
			|| (thing == dest)) {
			notify(player, "Permission denied. (you don't control the room, or can't link to the dropto)");
		} else {
			DBFETCH(thing)->sp.room.dropto = dest;	/* dropto */
			notify(player, "Dropto set.");
		}
		break;
	default:
		notify(player, "Internal error: weird object type.");
		warn("PANIC: weird object: Typeof(%d) = %ld", thing, Typeof(thing));
		break;
	}
	DBDIRTY(thing);
	return;
}

/*
 * do_dig
 *
 * Use this to create a room.
 */
void
do_dig(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *pname = cmd->argv[2];
	dbref room;
	dbref parent;
	dbref newparent;
	char buf[BUFFER_LEN];
	char rbuf[BUFFER_LEN];
	char qbuf[BUFFER_LEN];
	char *rname;
	char *qname;
	struct match_data md;

	NOGUEST("@dig",player);

	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	if (*name == '\0') {
		notify(player, "You must specify a name for the room.");
		return;
	}
	if(!OK_ASCII_OTHER(name)) {
		notify(player, "Room names are limited to 7-bit ASCII.");
		return;
	}
	if (!ok_name(name)) {
		notify(player, "That's a silly name for a room!");
		return;
	}
	if (!payfor(player, ROOM_COST)) {
		notify_fmt(player, "Sorry, you don't have enough %s to dig a room.", PENNIES);
		return;
	}
	room = new_object();

	/* Initialize everything */
	newparent = DBFETCH(DBFETCH(player)->location)->location;
	while ((newparent != NOTHING) && !(FLAGS(newparent) & ABODE))
		newparent = DBFETCH(newparent)->location;
	if (newparent == NOTHING)
		newparent = GLOBAL_ENVIRONMENT;

	NAME(room) = alloc_string(name);
	DBFETCH(room)->location = newparent;
	OWNER(room) = OWNER(player);
	DBFETCH(room)->exits = NOTHING;
	DBFETCH(room)->sp.room.dropto = NOTHING;
	FLAGS(room) = TYPE_ROOM | (FLAGS(player) & JUMP_OK);
	PUSH(room, DBFETCH(newparent)->contents);
	DBDIRTY(room);
	DBDIRTY(newparent);

	snprintf(buf, sizeof(buf), "%s created with room number %d.", name, room);
	notify(player, buf);

	strlcpy(buf, pname, sizeof(buf));
	for (rname = buf; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		*(rname++) = '\0';
	while ((qname > buf) && (isspace(*qname)))
		*(qname--) = '\0';
	qname = buf;
	for (; *rname && isspace(*rname); rname++) ;
	strlcpy(rbuf, rname, sizeof(rbuf));
	rname = rbuf;
	strlcpy(qbuf, qname, sizeof(qbuf));
	qname = qbuf;

	if (*qname) {
		notify(player, "Trying to set parent...");
		init_match(cmd, qname, TYPE_ROOM, &md);
		match_absolute(&md);
		match_registered(&md);
		match_here(&md);
		if ((parent = noisy_match_result(&md)) == NOTHING || parent == AMBIGUOUS) {
			notify(player, "Parent set to default.");
		} else {
			if (!can_link_to(player, Typeof(room), parent) || room == parent) {
				notify(player, "Permission denied.  Parent set to default.");
			} else {
				moveto(room, parent);
				snprintf(buf, sizeof(buf), "Parent set to %s.", unparse_object(player, parent));
				notify(player, buf);
			}
		}
	}

	if (*rname) {
		PData mydat;

		snprintf(buf, sizeof(buf), "_reg/%s", rname);
		mydat.flags = PROP_REFTYP;
		mydat.data.ref = room;
		set_property(player, buf, &mydat);
		snprintf(buf, sizeof(buf), "Room registered as $%s", rname);
		notify(player, buf);
	}
}

/*
 * copy a single property, identified by its name, from one object to
 * another. helper routine for copy_props (below).
 */
void
copy_one_prop(dbref player, dbref source, dbref destination, char *propname)
{
	PropPtr currprop;
	PData newprop;

	/* read property from old object */
	currprop = get_property(source, propname);

	if(currprop) {

		/* flags can be copied. */		
		newprop.flags = currprop->flags;

		/* data, however, must be cloned in case it's a string or a
		   lock. */
		switch(PropType(currprop)) {
			case PROP_STRTYP:
				newprop.data.str = alloc_string((currprop->data).str);
				break;
			case PROP_INTTYP:
			case PROP_FLTTYP:
			case PROP_REFTYP:
				newprop.data = currprop->data;
				break;
			case PROP_LOKTYP:
				newprop.data.lok = copy_bool((currprop->data).lok);
			case PROP_DIRTYP:
				break;
		}

		/* now hook the new property into the destination object. */
		set_property(destination, propname, &newprop);
	}
	
	return;
}

/*
 * copy a property (sub)tree from one object to another one. this is a
 * helper routine used by do_clone, based loosely on listprops_wildcard from
 * look.c.
 */
void
copy_props(dbref player, dbref source, dbref destination, const char *dir)
{
	char propname[BUFFER_LEN];
	char buf[BUFFER_LEN];
	PropPtr propadr, pptr;

	/* loop through all properties in the current propdir */
	propadr = first_prop(source, (char *) dir, &pptr, propname, sizeof(propname));
	while (propadr) {
	
		/* generate name for current property */
		snprintf(buf, sizeof(buf), "%s%c%s", dir, PROPDIR_DELIMITER, propname);

		/* notify player */
#if VERBOSE_CLONE
		if(Wizard(OWNER(player))) {
			char buf2[BUFFER_LEN];
			snprintf(buf2, sizeof(buf2), "copying property %s", buf);
			notify(player, buf2);
		}
#endif

		/* copy this property */
		copy_one_prop(player, source, destination, buf);
		
		/* recursively copy this property directory */
		copy_props(player, source, destination, buf);
		
		/* find next property in current dir */
		propadr = next_prop(pptr, propadr, propname, sizeof(propname));

	}
	
	/* chaos and disorder - our work here is done. */
	return;
}

/*
 * do_clone
 *
 * Use this to clone an object.
 */
void
do_clone(command_t *cmd)
{
	dbref player = cmd->player;
	char *name = cmd->argv[1];
	static char buf[BUFFER_LEN];
	dbref  thing, clonedthing;
	int    cost;
	struct match_data md;

	/* Perform sanity checks */

	NOGUEST("@clone",player);

	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	
	if (*name == '\0') {
		notify(player, "Clone what?");
		return;
	} 

	/* All OK so far, so try to find the thing that should be cloned. We
	   do not allow rooms, exits, etc. to be cloned for now. */

	init_match(cmd, name, TYPE_THING, &md);
	match_possession(&md);
	match_neighbor(&md);
	match_registered(&md);
	match_absolute(&md);
	
	if ((thing = noisy_match_result(&md)) == NOTHING)
		return;

	if (thing == AMBIGUOUS) {
		notify(player, "I don't know which one you mean!");
		return;
	}

	/* Further sanity checks */

	/* things only. */
	if(Typeof(thing) != TYPE_THING) {
		notify(player, "That is not a cloneable object.");
		return;
	}		
	
	/* check the name again, just in case reserved name patterns have
	   changed since the original object was created. */
	if (!ok_name(NAME(thing))) {
		notify(player, "You cannot clone something with such a weird name!");
		return;
	}

	/* no stealing stuff. */
	if(!controls(player, thing)) {
		notify(player, "Permission denied. (you can't clone this)");
		return;
	}

	/* there ain't no such lunch as a free thing. */
	cost = OBJECT_GETCOST(GETVALUE(thing));
	if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	
	if (!payfor(player, cost)) {
		notify_fmt(player, "Sorry, you don't have enough %s.", PENNIES);
		return;
	} else {
#if VERBOSE_CLONE
		snprintf(buf, sizeof(buf), "Now cloning %s...", unparse_object(player, thing));
		notify(player, buf);
#endif
		/* create the object */
		clonedthing = new_object();

		/* initialize everything */
		NAME(clonedthing) = alloc_string(NAME(thing));
		ALLOC_THING_SP(clonedthing);
		DBFETCH(clonedthing)->location = player;
		OWNER(clonedthing) = OWNER(player);
		SETVALUE(clonedthing, GETVALUE(thing));
/* FIXME: should we clone attached actions? */
		DBFETCH(clonedthing)->exits = NOTHING;
		FLAGS(clonedthing) = FLAGS(thing);

		/* copy all properties */
		copy_props(player, thing, clonedthing, "");

		/* endow the object */
		if (GETVALUE(thing) > MAX_OBJECT_ENDOWMENT) {
			SETVALUE(thing, MAX_OBJECT_ENDOWMENT);
		}
		
		/* Home, sweet home */
		THING_SET_HOME(clonedthing, THING_HOME(thing));

		/* link it in */
		PUSH(clonedthing, DBFETCH(player)->contents);
		DBDIRTY(player);

		/* and we're done */
		snprintf(buf, sizeof(buf), "%s created with number %d.", NAME(thing), clonedthing);
		notify(player, buf);
		DBDIRTY(clonedthing);
	}
	
}

/*
 * do_create
 *
 * Use this to create an object.
 */
void
do_create(command_t *cmd)
{
	dbref player = cmd->player;
	char *name = cmd->argv[1];
	char *acost = cmd->argv[2];
	dbref loc;
	dbref thing;
	int cost;

	static char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *rname, *qname;

	NOGUEST("@create",player);

	strlcpy(buf2, acost, sizeof(buf2));
	for (rname = buf2; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		*(rname++) = '\0';
	while ((qname > buf2) && (isspace(*qname)))
		*(qname--) = '\0';
	qname = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	cost = atoi(qname);
	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	if (*name == '\0') {
		notify(player, "Create what?");
		return;
	} else if(!OK_ASCII_THING(name)) {
		notify(player, "Thing names are limited to 7-bit ASCII.");
		return;
	} else if (!ok_name(name)) {
		notify(player, "That's a silly name for a thing!");
		return;
	} else if (cost < 0) {
		notify(player, "You can't create an object for less than nothing!");
		return;
	} else if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	if (!payfor(player, cost)) {
		notify_fmt(player, "Sorry, you don't have enough %s.", PENNIES);
		return;
	} else {
		/* create the object */
		thing = new_object();

		/* initialize everything */
		NAME(thing) = alloc_string(name);
		ALLOC_THING_SP(thing);
		DBFETCH(thing)->location = player;
		OWNER(thing) = OWNER(player);
		SETVALUE(thing, OBJECT_ENDOWMENT(cost));
		DBFETCH(thing)->exits = NOTHING;
		FLAGS(thing) = TYPE_THING;

		/* endow the object */
		if (GETVALUE(thing) > MAX_OBJECT_ENDOWMENT) {
			SETVALUE(thing, MAX_OBJECT_ENDOWMENT);
		}
		if ((loc = DBFETCH(player)->location) != NOTHING && controls(player, loc)) {
			THING_SET_HOME(thing, loc);	/* home */
		} else {
			THING_SET_HOME(thing, player);	/* home */
			/* set thing's home to player instead */
		}

		/* link it in */
		PUSH(thing, DBFETCH(player)->contents);
		DBDIRTY(player);

		/* and we're done */
		snprintf(buf, sizeof(buf), "%s created with number %d.", name, thing);
		notify(player, buf);
		DBDIRTY(thing);
	}
	if (*rname) {
		PData mydat;

		snprintf(buf, sizeof(buf), "Registered as $%s", rname);
		notify(player, buf);
		snprintf(buf, sizeof(buf), "_reg/%s", rname);
		mydat.flags = PROP_REFTYP;
		mydat.data.ref = thing;
		set_property(player, buf, &mydat);
	}
}

/*
 * parse_source()
 *
 * This is a utility used by do_action and do_attach.  It parses
 * the source string into a dbref, and checks to see that it
 * exists.
 *
 * The return value is the dbref of the source, or NOTHING if an
 * error occurs.
 *
 */
dbref
parse_source(command_t *cmd, const char *source_name)
{
	dbref player = cmd->player;
	dbref source;
	struct match_data md;

	init_match(cmd, source_name, NOTYPE, &md);
	/* source type can be any */
	match_neighbor(&md);
	match_me(&md);
	match_here(&md);
	match_possession(&md);
	match_registered(&md);
	match_absolute(&md);
	source = noisy_match_result(&md);

	if (source == NOTHING)
		return NOTHING;

	/* You can only attach actions to things you control */
	if (!controls(player, source)) {
		notify(player, "Permission denied. (you don't control the attachment point)");
		return NOTHING;
	}
	if (Typeof(source) == TYPE_EXIT) {
		notify(player, "You can't attach an action to an action.");
		return NOTHING;
	}
	return source;
}

/*
 * set_source()
 *
 * This routine sets the source of an action to the specified source.
 * It is called by do_action and do_attach.
 *
 */
void
set_source(dbref player, dbref action, dbref source)
{
	switch (Typeof(source)) {
	case TYPE_ROOM:
	case TYPE_THING:
	case TYPE_PLAYER:
		PUSH(action, DBFETCH(source)->exits);
		break;
	default:
		notify(player, "Internal error: weird object type.");
		warn("PANIC: tried to source %d to %d: type: %ld",
				   action, source, Typeof(source));
		return;
		break;
	}
	DBDIRTY(source);
	DBSTORE(action, location, source);
	return;
}

int
unset_source(dbref player, dbref loc, dbref action)
{

	dbref oldsrc;

	if ((oldsrc = DBFETCH(action)->location) == NOTHING) {
		/* old-style, sourceless exit */
		if (!member(action, DBFETCH(loc)->exits)) {
			return 0;
		}
		DBSTORE(DBFETCH(player)->location, exits,
				remove_first(DBFETCH(DBFETCH(player)->location)->exits, action));
	} else {
		switch (Typeof(oldsrc)) {
		case TYPE_PLAYER:
		case TYPE_ROOM:
		case TYPE_THING:
			DBSTORE(oldsrc, exits, remove_first(DBFETCH(oldsrc)->exits, action));
			break;
		default:
			warn("PANIC: source of action #%d was type: %ld.", action, Typeof(oldsrc));
			return 0;
			/* NOTREACHED */
			break;
		}
	}
	return 1;
}

/*
 * do_action()
 *
 * This routine attaches a new existing action to a source object,
 * where possible.
 * The action will not do anything until it is LINKed.
 *
 */
void
do_action(command_t *cmd)
{
	dbref player = cmd->player;
	char *action_name = cmd->argv[1],
		*source_name = cmd->argv[2];;
	dbref action, source;
	static char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *rname, *qname;

	NOGUEST("@action",player);

	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	strlcpy(buf2, source_name, sizeof(buf2));
	for (rname = buf2; (*rname && (*rname != '=')); rname++) ;
	qname = rname;
	if (*rname)
		*(rname++) = '\0';
	while ((qname > buf2) && (isspace(*qname)))
		*(qname--) = '\0';
	qname = buf2;
	for (; *rname && isspace(*rname); rname++) ;

	if (!*action_name || !*qname) {
		notify(player, "You must specify an action name and a source object.");
		return;
	} else if(!OK_ASCII_OTHER(action_name)) {
		notify(player, "Action names are limited to 7-bit ASCII.");
		return;
	} else if (!ok_name(action_name)) {
		notify(player, "That's a strange name for an action!");
		return;
	}
	if (((source = parse_source(cmd, qname)) == NOTHING))
		return;
        if (!payfor(player, EXIT_COST)) {
                notify_fmt(player, "Sorry, you don't have enough %s to make an action.", PENNIES);
                return;
        }

	action = new_object();

	NAME(action) = alloc_string(action_name);
	DBFETCH(action)->location = NOTHING;
	OWNER(action) = OWNER(player);
	DBFETCH(action)->sp.exit.ndest = 0;
	DBFETCH(action)->sp.exit.dest = NULL;
	FLAGS(action) = TYPE_EXIT;

	set_source(player, action, source);
	snprintf(buf, sizeof(buf), "Action created with number %d and attached.", action);
	notify(player, buf);
	DBDIRTY(action);

	if (*rname) {
		PData mydat;

		snprintf(buf, sizeof(buf), "Registered as $%s", rname);
		notify(player, buf);
		snprintf(buf, sizeof(buf), "_reg/%s", rname);
		mydat.flags = PROP_REFTYP;
		mydat.data.ref = action;
		set_property(player, buf, &mydat);
	}
}

/*
 * do_attach()
 *
 * This routine attaches a previously existing action to a source object.
 * The action will not do anything unless it is LINKed.
 *
 */
void
do_attach(command_t *cmd) {
	dbref player = cmd->player;
	const char *action_name = cmd->argv[1];
	const char *source_name = cmd->argv[2];
	dbref action, source;
	dbref loc;				/* player's current location */
	struct match_data md;

	NOGUEST("@attach",player);

	if ((loc = DBFETCH(player)->location) == NOTHING)
		return;

	if (!Builder(player)) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	if (!*action_name || !*source_name) {
		notify(player, "You must specify an action name and a source object.");
		return;
	}
	init_match(cmd, action_name, TYPE_EXIT, &md);
	match_all_exits(&md);
	match_registered(&md);
	match_absolute(&md);

	if ((action = noisy_match_result(&md)) == NOTHING)
		return;

	if (Typeof(action) != TYPE_EXIT) {
		notify(player, "That's not an action!");
		return;
	} else if (!controls(player, action)) {
		notify(player, "Permission denied. (you don't control the action you're trying to reattach)");
		return;
	}
	if ((source = parse_source(cmd, source_name)) == NOTHING)
		return;

	if (!unset_source(player, loc, action)) {
		return;
	}
	set_source(player, action, source);
	notify(player, "Action re-attached.");
}
static const char *create_c_version = "$RCSfile$ $Revision: 1.28 $";
const char *get_create_c_version(void) { return create_c_version; }

/* $Header$ */

#include "copyright.h"
#include "config.h"

/* Commands that create new objects */

#include <string.h>
#include <ctype.h>
#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include "match.h"

/*
 * copy a single property, identified by its name, from one object to
 * another. helper routine for copy_props (below).
 */
void
copy_one_prop(dbref player, dbref source, dbref destination, char *propname, int ignore)
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
			case PROP_DIRTYP:
				break;
		}

		/* now hook the new property into the destination object. */
		set_property_nofetch(destination, propname + ignore, &newprop);
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

		/* copy this property */
		copy_one_prop(player, source, destination, buf, 0);
		
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
 * TODO improve/remove this. use skeletons to copy objects?
 */
void
do_clone(command_t *cmd)
{
	dbref player = cmd->player;
	char *name = cmd->argv[1];
	static char buf[BUFFER_LEN];
	dbref  thing, clonedthing;
	int    cost;

	/* Perform sanity checks */

	if (!(ENTITY(player)->flags & (EF_WIZARD | EF_BUILDER))) {
		notify(player, "That command is restricted to authorized builders.");
		return;
	}
	
	if (*name == '\0') {
		notify(player, "Clone what?");
		return;
	} 

	/* All OK so far, so try to find the thing that should be cloned. We
	   do not allow rooms, exits, etc. to be cloned for now. */

	if (
			(thing = ematch_absolute(name)) == NOTHING
			&& (thing = ematch_mine(player, name)) == NOTHING
			&& (thing = ematch_near(player, name)) == NOTHING
	   )
	{
		notify(player, "I don't know what you mean.");
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
	cost = OBJECT_GETCOST(db[thing].value);
	if (cost < OBJECT_COST) {
		cost = OBJECT_COST;
	}
	
	if (!payfor(player, cost)) {
		notifyf(player, "Sorry, you don't have enough %s.", PENNIES);
		return;
	} else {
		/* create the object */
		clonedthing = object_new();

		/* initialize everything */
		NAME(clonedthing) = alloc_string(NAME(thing));
		db[clonedthing].location = player;
		OWNER(clonedthing) = OWNER(player);
		db[clonedthing].value = db[thing].value;
		/* FIXME: should we clone attached actions? */
		switch (Typeof(thing)) {
			case TYPE_ROOM:
				ROOM(clonedthing)->exits = ROOM(clonedthing)->doors = 0;
				break;
			case TYPE_ENTITY:
				/* Home, sweet home */
				ENTITY(clonedthing)->home = ENTITY(thing)->home;

		}
		FLAGS(clonedthing) = FLAGS(thing);

		/* copy all properties */
		copy_props(player, thing, clonedthing, "");

		/* endow the object */
		if (db[thing].value > MAX_OBJECT_ENDOWMENT)
			db[thing].value = MAX_OBJECT_ENDOWMENT;
		
		/* link it in */
		PUSH(clonedthing, db[player].contents);

		/* and we're done */
		snprintf(buf, sizeof(buf), "%s created with number %d.", NAME(thing), clonedthing);
		notify(player, buf);
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
	dbref thing;
	int cost;

	static char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *rname, *qname;

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
	if (!(ENTITY(player)->flags & (EF_WIZARD | EF_BUILDER))) {
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
		notifyf(player, "Sorry, you don't have enough %s.", PENNIES);
		return;
	} else {
		/* create the object */
		thing = object_new();

		/* initialize everything */
		NAME(thing) = alloc_string(name);
		db[thing].location = player;
		OWNER(thing) = OWNER(player);
		db[thing].value = OBJECT_ENDOWMENT(cost);
		FLAGS(thing) = TYPE_THING;

		/* endow the object */
		if (db[thing].value > MAX_OBJECT_ENDOWMENT)
			db[thing].value = MAX_OBJECT_ENDOWMENT;

		/* link it in */
		PUSH(thing, db[player].contents);

		/* and we're done */
		snprintf(buf, sizeof(buf), "%s created with number %d.", name, thing);
		notify(player, buf);
	}
	if (*rname) {
		PData mydat;

		snprintf(buf, sizeof(buf), "Registered as $%s", rname);
		notify(player, buf);
		snprintf(buf, sizeof(buf), "_reg/%s", rname);
		mydat.flags = PROP_REFTYP;
		mydat.data.ref = thing;
		set_property_nofetch(player, buf, &mydat);
	}
}

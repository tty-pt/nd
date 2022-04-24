
/* $Header$ */


#include "copyright.h"
#include "config.h"

/* commands which set parameters */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "match.h"
#include "interface.h"
#include "externs.h"
#include "geography.h"


static dbref
match_controlled(dbref player, const char *name)
{
	dbref match = ematch_all(player, name);

	if (match == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return NOTHING;
	}

	else if (!controls(player, match)) {
		notify(player, "Permission denied. (You don't control what was matched)");
		return NOTHING;
	}

	return match;
}

void
do_name(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	char *newname = cmd->argv[2];
	dbref thing;
	char *password;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		/* check for bad name */
		if (*newname == '\0') {
			notify(player, "Give it what new name?");
			return;
		}
		/* check for renaming a player */
		if (Typeof(thing) == TYPE_PLAYER) {
			/* split off password */
			for (password = newname; *password && !isspace(*password); password++) ;
			/* eat whitespace */
			if (*password) {
				*password++ = '\0';	/* terminate name */
				while (*password && isspace(*password))
					password++;
			}
			/* check for null password */
			if (!*password) {
				notify(player, "You must specify a password to change a player name.");
				notify(player, "E.g.: name player = newname password");
				return;
			} else if (!check_password(thing, password)) {
				notify(player, "Incorrect password.");
				return;
			} else if (strcmp(newname, NAME(thing))
					   && !ok_player_name(newname)) {
				notify(player, "You can't give a player that name.");
				return;
			}
			/* everything ok, notify */
			warn("NAME CHANGE: %s(#%d) to %s", NAME(thing), thing, newname);
			delete_player(thing);
			if (NAME(thing)) {
				free((void *) NAME(thing));
			}
			NAME(thing) = alloc_string(newname);
			add_player(thing);
			notify(player, "Name set.");
			return;
		} else {
			if ((Typeof(thing) == TYPE_THING && !OK_ASCII_THING(newname)) ||
			    (Typeof(thing) != TYPE_THING && !OK_ASCII_OTHER(newname)) ) {
				notify(player, "Invalid 8-bit name.");
				return;
			}
			if (!ok_name(newname)) {
				notify(player, "That is not a reasonable name.");
				return;
			}
		}

		/* everything ok, change the name */
		if (NAME(thing)) {
			free((void *) NAME(thing));
		}
		NAME(thing) = alloc_string(newname);
		notify(player, "Name set.");
		DBDIRTY(thing);
	}
}

void
do_describe(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *description = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETDESC(thing, description);
		if(description && *description) {
			notify(player, "Description set.");
		} else {
			notify(player, "Description cleared.");
		}
	}
}

void
do_idescribe(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *description = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETIDESC(thing, description);
		if(description && *description) {
			notify(player, "Description set.");
		} else {
			notify(player, "Description cleared.");
		}
	}
}

void
do_doing(command_t *cmd)
{
	dbref player = cmd->player;
	dbref thing;
	const char *name = cmd->argv[1];
	const char *mesg = cmd->argv[2];

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETDOING(thing, mesg);
		if(mesg && *mesg) {
			notify(player, "Doing set.");
		} else {
			notify(player, "Doing cleared.");
		}
	}
}

void
do_fail(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETFAIL(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_success(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETSUCC(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

/* sets the drop message for player */
void
do_drop_message(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETDROP(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_osuccess(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETOSUCC(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_ofail(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETOFAIL(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_odrop(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETODROP(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_oecho(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETOECHO(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

void
do_pecho(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *message = cmd->argv[2];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		SETPECHO(thing, message);
		if(message && *message) {
			notify(player, "Message set.");
		} else {
			notify(player, "Message cleared.");
		}
	}
}

/* sets a lock on an object to the lockstring passed to it.
   If the lockstring is null, then it unlocks the object.
   this returns a 1 or a 0 to represent success. */
int
setlockstr(dbref player, dbref thing, const char *keyname)
{
	struct boolexp *key;

	if (*keyname != '\0') {
		key = parse_boolexp(player, keyname, 0);
		if (key == TRUE_BOOLEXP) {
			return 0;
		} else {
			/* everything ok, do it */
			SETLOCK(thing, key);
			return 1;
		}
	} else {
		CLEARLOCK(thing);
		return 1;
	}
}

void
do_conlock(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *keyname = cmd->argv[2];
	dbref thing = ematch_all(player, name);
	struct boolexp *key;
	PData mydat;

	if (thing == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		notify(player, "You can't set the container-lock on that!");
		return;
	}

	if (!*keyname) {
		mydat.flags = PROP_LOKTYP;
		mydat.data.lok = TRUE_BOOLEXP;
		set_property(thing, "_/clk", &mydat);
		notify(player, "Container lock cleared.");
	} else {
		key = parse_boolexp(player, keyname, 0);
		if (key == TRUE_BOOLEXP) {
			notify(player, "I don't understand that key.");
		} else {
			/* everything ok, do it */
			mydat.flags = PROP_LOKTYP;
			mydat.data.lok = key;
			set_property(thing, "_/clk", &mydat);
			notify(player, "Container lock set.");
		}
	}
}

void
do_chlock(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *keyname = cmd->argv[2];
	dbref thing = ematch_all(player, name);
	struct boolexp *key;
	PData mydat;

	if (thing == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player, thing)) {
		notify(player, "You can't set the chown-lock on that!");
		return;
	}

	if (!*keyname) {
		mydat.flags = PROP_LOKTYP;
		mydat.data.lok = TRUE_BOOLEXP;
		set_property(thing, "_/chlk", &mydat);
		notify(player, "Chown lock cleared.");
		return;
	}

	key = parse_boolexp(player, keyname, 0);
	if (key == TRUE_BOOLEXP) {
		notify(player, "I don't understand that key.");
	} else {
		/* everything ok, do it */
		mydat.flags = PROP_LOKTYP;
		mydat.data.lok = key;
		set_property(thing, "_/chlk", &mydat);
		notify(player, "Chown lock set.");
	}
}

void
do_lock(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *keyname = cmd->argv[2];
	dbref thing = ematch_all(player, name);
	struct boolexp *key;

	if (thing == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (Typeof(thing) == TYPE_EXIT
			&& e_exit_is(thing)
			&& geo_claim(player, getloc(thing)))
		return;

	if (!controls(player, thing)) {
		notify(player, "You can't lock that!");
		return;
	}

	if(keyname && *keyname) {
		key = parse_boolexp(player, keyname, 0);
		if (key == TRUE_BOOLEXP) {
			notify(player, "I don't understand that key.");
		} else {
			/* everything ok, do it */
			if (Typeof(thing) == TYPE_EXIT
			    && e_exit_is(thing)
			    && geo_claim(player, getloc(thing)))
				return;

			SETLOCK(thing, key);
			notify(player, "Locked.");
		}
	} else
		do_unlock(cmd);

}

void
do_unlock(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref thing;

	if ((thing = match_controlled(player, name)) != NOTHING) {
		CLEARLOCK(thing);
		notify(player, "Unlocked.");
	}
}

int
controls_link(dbref who, dbref what)
{
	switch (Typeof(what)) {
	case TYPE_EXIT:
		{
			int i = DBFETCH(what)->sp.exit.ndest;

			while (i > 0) {
				if (controls(who, DBFETCH(what)->sp.exit.dest[--i]))
					return 1;
			}
			if (who == OWNER(DBFETCH(what)->location))
				return 1;
			return 0;
		}

	case TYPE_ROOM:
		{
			if (controls(who, DBFETCH(what)->sp.room.dropto))
				return 1;
			return 0;
		}

	case TYPE_PLAYER:
		{
			if (controls(who, PLAYER_HOME(what)))
				return 1;
			return 0;
		}

	case TYPE_THING:
		{
			if (controls(who, THING_HOME(what)))
				return 1;
			return 0;
		}

	default:
		return 0;
	}
}

void
do_chown(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *newowner = cmd->argv[2];
	dbref thing;
	dbref owner;

	if (!*name) {
		notify(player, "You must specify what you want to take ownership of.");
		return;
	}

	thing = ematch_all(player, name);
	if (thing == NOTHING) {
		notify(player, NOMATCH_MESSAGE);
		return;
	}

	if (*newowner && strcmp(newowner, "me")) {
		if ((owner = lookup_player(newowner)) == NOTHING) {
			notify(player, "I couldn't find that player.");
			return;
		}
	} else {
		owner = OWNER(player);
	}
	if (!Wizard(OWNER(player)) && OWNER(player) != owner) {
		notify(player, "Only wizards can transfer ownership to others.");
		return;
	}
#ifdef GOD_PRIV
	if (Wizard(OWNER(player)) && !God(player) && God(owner)) {
		notify(player, "God doesn't need an offering or sacrifice.");
		return;
	}
#endif /* GOD_PRIV */
	if (!Wizard(OWNER(player))) {
		if (Typeof(thing) != TYPE_EXIT ||
			(DBFETCH(thing)->sp.exit.ndest && !controls_link(player, thing))) {
			if (!(FLAGS(thing) & CHOWN_OK) ||
				!test_lock(player, thing, "_/chlk")) {
				notify(player, "You can't take possession of that.");
				return;
			}
		}
	}

	switch (Typeof(thing)) {
	case TYPE_ROOM:
		if (!Wizard(OWNER(player)) && DBFETCH(player)->location != thing) {
			notify(player, "You can only chown \"here\".");
			return;
		}
		OWNER(thing) = OWNER(owner);
		break;
	case TYPE_THING:
		if (!Wizard(OWNER(player)) && DBFETCH(thing)->location != player) {
			notify(player, "You aren't carrying that.");
			return;
		}
		OWNER(thing) = OWNER(owner);
		break;
	case TYPE_PLAYER:
		notify(player, "Players always own themselves.");
		return;
	case TYPE_EXIT:
		OWNER(thing) = OWNER(owner);
		break;
	case TYPE_GARBAGE:
		notify(player, "No one wants to own garbage.");
		return;
	}
	if (owner == player)
		notify(player, "Owner changed to you.");
	else {
		char buf[BUFFER_LEN];

		snprintf(buf, sizeof(buf), "Owner changed to %s.", unparse_object(player, owner));
		notify(player, buf);
	}
	DBDIRTY(thing);
}


/* Note: Gender code taken out.  All gender references are now to be handled
   by property lists...
   Setting of flags and property code done here.  Note that the PROP_DELIMITER
   identifies when you're setting a property.
   A @set <thing>= :clear
   will clear all properties.
   A @set <thing>= type:
   will remove that property.
   A @set <thing>= propname:string
   will add that string property or replace it.
   A @set <thing>= propname:^value
   will add that integer property or replace it.
 */

void
do_set(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *flag = cmd->argv[2];
	dbref thing;
	const char *p;
	object_flag_type f;

	/* find thing */
	if ((thing = match_controlled(player, name)) == NOTHING)
		return;
#ifdef GOD_PRIV
	/* Only God can set anything on any of his stuff */
	if(!God(player) && God(OWNER(thing))) {
		notify(player,"Only God may touch God's property.");
		return;
	}
#endif

	/* move p past NOT_TOKEN if present */
	for (p = flag; *p && (*p == NOT_TOKEN || isspace(*p)); p++) ;

	/* Now we check to see if it's a property reference */
	/* if this gets changed, please also modify boolexp.c */
	if (strchr(flag, PROP_DELIMITER)) {
		/* copy the string so we can muck with it */
		char *type = alloc_string(flag);	/* type */
		char *pname = (char *) strchr(type, PROP_DELIMITER);	/* propname */
		char *x;				/* to preserve string location so we can free it */
		char *temp;
		int ival = 0;

		x = type;
		while (isspace(*type) && (*type != PROP_DELIMITER))
			type++;
		if (*type == PROP_DELIMITER) {
			/* clear all properties */
			for (type++; isspace(*type); type++) ;
			if (strcmp(type, "clear")) {
				notify(player, "Use '@set <obj>=:clear' to clear all props on an object");
				free((void *)x);
				return;
			}
			remove_property_list(thing, Wizard(OWNER(player)));
			notify(player, "All user-owned properties removed.");
			free((void *) x);
			return;
		}
		/* get rid of trailing spaces and slashes */
		for (temp = pname - 1; temp >= type && isspace(*temp); temp--) ;
		while (temp >= type && *temp == '/')
			temp--;
		*(++temp) = '\0';

		pname++;				/* move to next character */
		/* while (isspace(*pname) && *pname) pname++; */
		if (*pname == '^' && number(pname + 1))
			ival = atoi(++pname);

		if (Prop_System(type) || (!Wizard(OWNER(player)) && (Prop_SeeOnly(type) || Prop_Hidden(type)))) {
			notify(player, "Permission denied. (The property is hidden from you.)");
			free((void *)x);
			return;
		}

		if (!(*pname)) {
			remove_property(thing, type);
			notify(player, "Property removed.");
		} else {
			if (ival) {
				add_property(thing, type, NULL, ival);
			} else {
				add_property(thing, type, pname, 0);
			}
			notify(player, "Property set.");
		}
		free((void *) x);
		return;
	}
	/* identify flag */
	if (*p == '\0') {
		notify(player, "You must specify a flag to set.");
		return;
	} else if (string_prefix("WIZARD", p)) {
		f = WIZARD;
	} else if (string_prefix("LINK_OK", p)) {
		f = LINK_OK;

	} else if (string_prefix("KILL_OK", p)) {
		f = KILL_OK;
	} else if ((string_prefix("DARK", p)) || (string_prefix("DEBUG", p))) {
		f = DARK;
	} else if ((string_prefix("STICKY", p)) || (string_prefix("SETUID", p)) ||
			   (string_prefix("SILENT", p))) {
		f = STICKY;
	} else if (string_prefix("QUELL", p)) {
		f = QUELL;
	} else if (string_prefix("BUILDER", p) || string_prefix("BOUND", p)) {
		f = BUILDER;
	} else if (string_prefix("CHOWN_OK", p) || string_prefix("COLOR", p)) {
		f = CHOWN_OK;
	} else if (string_prefix("JUMP_OK", p)) {
		f = JUMP_OK;
	} else if (string_prefix("HAVEN", p) || string_prefix("HARDUID", p)) {
		f = HAVEN;
	} else if ((string_prefix("ABODE", p)) ||
			   (string_prefix("AUTOSTART", p)) || (string_prefix("ABATE", p))) {
		f = ABODE;
	} else {
		notify(player, "I don't recognize that flag.");
		return;
	}

	/* check for restricted flag */
	if (restricted(player, thing, f)) {
		notify(player, "Permission denied. (restricted flag)");
		return;
	}
	/* check for stupid wizard */
	if (f == WIZARD && *flag == NOT_TOKEN && thing == player) {
		notify(player, "You cannot make yourself mortal.");
		return;
	}
	/* else everything is ok, do the set */
	if (*flag == NOT_TOKEN) {
		/* reset the flag */
		FLAGS(thing) &= ~f;
		DBDIRTY(thing);
		notify(player, "Flag reset.");
	} else {
		/* set the flag */
		FLAGS(thing) |= f;
		DBDIRTY(thing);
		notify(player, "Flag set.");
	}
}

void
do_propset(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *prop = cmd->argv[2];
	dbref thing, ref;
	char *p, *q;
	char buf[BUFFER_LEN];
	char *type, *pname, *value;
	struct boolexp *lok;
	PData mydat;

	/* find thing */
	if ((thing = match_controlled(player, name)) == NOTHING)
		return;

	while (isspace(*prop))
		prop++;
	strlcpy(buf, prop, sizeof(buf));

	type = p = buf;
	while (*p && *p != PROP_DELIMITER)
		p++;
	if (*p)
		*p++ = '\0';

	if (*type) {
		q = type + strlen(type) - 1;
		while (q >= type && isspace(*q))
			*(q--) = '\0';
	}

	pname = p;
	while (*p && *p != PROP_DELIMITER)
		p++;
	if (*p)
		*p++ = '\0';
	value = p;

	while (*pname == PROPDIR_DELIMITER || isspace(*pname))
		pname++;
	if (*pname) {
		q = pname + strlen(pname) - 1;
		while (q >= pname && isspace(*q))
			*(q--) = '\0';
	}

	if (!*pname) {
		notify(player, "I don't know which property you want to set!");
		return;
	}

	if (Prop_System(pname) || (!Wizard(OWNER(player)) && (Prop_SeeOnly(pname) || Prop_Hidden(pname)))) {
		notify(player, "Permission denied. (can't set a property that's restricted against you)");
		return;
	}

	if (!*type || string_prefix("string", type)) {
		add_property(thing, pname, value, 0);
	} else if (string_prefix("integer", type)) {
		if (!number(value)) {
			notify(player, "That's not an integer!");
			return;
		}
		add_property(thing, pname, NULL, atoi(value));
	} else if (string_prefix("float", type)) {
		if (!ifloat(value)) {
			notify(player, "That's not a floating point number!");
			return;
		}
		mydat.flags = PROP_FLTTYP;
		mydat.data.fval = strtod(value, NULL);
		set_property(thing, pname, &mydat);
	} else if (string_prefix("dbref", type)) {
		ref = ematch_all(player, value);
		if (ref == NOTHING) {
			notify(player, NOMATCH_MESSAGE);
			return;
		}

		mydat.flags = PROP_REFTYP;
		mydat.data.ref = ref;
		set_property(thing, pname, &mydat);
	} else if (string_prefix("lock", type)) {
		lok = parse_boolexp(player, value, 0);
		if (lok == TRUE_BOOLEXP) {
			notify(player, "I don't understand that lock.");
			return;
		}
		mydat.flags = PROP_LOKTYP;
		mydat.data.lok = lok;
		set_property(thing, pname, &mydat);
	} else if (string_prefix("erase", type)) {
		if (*value) {
			notify(player, "Don't give a value when erasing a property.");
			return;
		}
		remove_property(thing, pname);
		notify(player, "Property erased.");
		return;
	} else {
		notify(player, "I don't know what type of property you want to set!");
		notify(player, "Valid types are string, integer, float, dbref, lock, and erase.");
		return;
	}
	notify(player, "Property set.");
}

void
set_flags_from_tunestr(dbref obj, const char* tunestr)
{
	const char *p = tunestr;
	object_flag_type f = 0;

	for (;;) {
		char pcc = toupper(*p);
		if (pcc == '\0' || pcc == '\n' || pcc == '\r') {
			break;
		} else if (pcc == 'A') {
			f = ABODE;
		} else if (pcc == 'B') {
			f = BUILDER;
		} else if (pcc == 'C') {
			f = CHOWN_OK;
		} else if (pcc == 'D') {
			f = DARK;
		} else if (pcc == 'H') {
			f = HAVEN;
		} else if (pcc == 'J') {
			f = JUMP_OK;
		} else if (pcc == 'K') {
			f = KILL_OK;
		} else if (pcc == 'L') {
			f = LINK_OK;
		} else if (pcc == 'Q') {
			f = QUELL;
		} else if (pcc == 'S') {
			f = STICKY;
		} else if (pcc == 'W') {
			/* f = WIZARD;     This is very bad to auto-set. */
		}
		FLAGS(obj) |= f;
		p++;
	}
	DBDIRTY(obj);
}


static const char *set_c_version = "$RCSfile$ $Revision: 1.31 $";
const char *get_set_c_version(void) { return set_c_version; }

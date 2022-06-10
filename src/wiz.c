/* $Header$ */

#include "copyright.h"
#include "config.h"

/* Wizard-only commands */

#include <stdio.h>

#include <sys/resource.h>

#include <stdlib.h>

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "geography.h"
#include "kill.h"

void
do_teleport(command_t *cmd) {
	OBJ *player = OBJECT(cmd->player);
	const char *arg1 = cmd->argv[1];
	const char *arg2 = cmd->argv[2];
	OBJ *victim;
	OBJ *destination;
	const char *to;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim = player;
		to = arg1;
	} else if (!(victim = ematch_all(player, arg1))) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

#ifdef GOD_PRIV
	if(!God(REF(player)) && God(victim->owner)) {
		notify(REF(player), "God has already set that where He wants it to be.");
		return;
	}
#endif

	destination = ematch_all(player, to);

	if (!destination) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

	switch (victim->type) {
	case TYPE_ENTITY:
		if (!controls(REF(player), REF(victim)) ||
			!controls(REF(player), REF(destination)) ||
			!controls(REF(player), victim->location) ||
			(is_item(REF(destination)) && !controls(REF(player), destination->location))) {
			notify(REF(player), "Permission denied. (must control victim, dest, victim's loc, and dest's loc)");
			break;
		}
		if (destination->type != TYPE_ROOM) {
			notify(REF(player), "Bad destination.");
			break;
		}
		if (parent_loop_check(REF(victim), REF(destination))) {
			notify(REF(player), "Objects can't contain themselves.");
			break;
		}
		notify(REF(victim), "You feel a wrenching sensation...");
		enter_room(REF(victim), REF(destination));
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (parent_loop_check(REF(victim), REF(destination))) {
			notify(REF(player), "You can't make a container contain itself!");
			break;
		}
		if (destination->type != TYPE_ROOM
			&& destination->type != TYPE_ENTITY
			&& !is_item(REF(destination))) {
			notify(REF(player), "Bad destination.");
			break;
		}
		if (!(controls(REF(player), REF(destination)) &&
			  (controls(REF(player), REF(victim)) || controls(REF(player), victim->location)))) {
			notify(REF(player), "Permission denied. (must control dest and be able to link to it, or control dest's loc)");
			break;
		}
		/* check for non-sticky dropto */
		if (destination->type == TYPE_ROOM) {
			ROO *rdestination = &destination->sp.room;
			if (rdestination->dropto != NOTHING)
				destination = OBJECT(rdestination->dropto);
		}
		moveto(REF(victim), REF(destination));
		notify(REF(player), "Teleported.");
		break;
	case TYPE_GARBAGE:
		notify(REF(player), "That object is in a place where magic cannot reach it.");
		break;
	case TYPE_ROOM:
	default:
		notify(REF(player), "You can't teleport that.");
		break;
	}
}

int
blessprops_wildcard(dbref player, dbref thing, const char *dir, const char *wild, int blessp)
{
	char propname[BUFFER_LEN];
	char wld[BUFFER_LEN];
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *ptr, *wldcrd = wld;
	PropPtr propadr, pptr;
	int i, cnt = 0;
	int recurse = 0;

#ifdef GOD_PRIV
	if(!God(player) && God(OBJECT(thing)->owner)) {
		notify(player,"Only God may touch what is God's.");
		return 0;
	}
#endif

	strlcpy(wld, wild, sizeof(wld));
	i = strlen(wld);
	if (i && wld[i - 1] == PROPDIR_DELIMITER)
		strlcat(wld, "*", sizeof(wld));
	for (wldcrd = wld; *wldcrd == PROPDIR_DELIMITER; wldcrd++) ;
	if (!strcmp(wldcrd, "**"))
		recurse = 1;

	for (ptr = wldcrd; *ptr && *ptr != PROPDIR_DELIMITER; ptr++) ;
	if (*ptr)
		*ptr++ = '\0';

	propadr = first_prop(thing, (char *) dir, &pptr, propname, sizeof(propname));
	while (propadr) {
		if (equalstr(wldcrd, propname)) {
			snprintf(buf, sizeof(buf), "%s%c%s", dir, PROPDIR_DELIMITER, propname);
			if (!Prop_System(buf) && ((!Prop_Hidden(buf) && !(PropFlags(propadr) & PROP_SYSPERMS))
				|| (ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD))) {
				if (!*ptr || recurse) {
					cnt++;
					if (blessp) {
						set_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Blessed %s", buf);
					} else {
						clear_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Unblessed %s", buf);
					}
					notify(player, buf2);
				}
				if (recurse)
					ptr = "**";
				cnt += blessprops_wildcard(player, thing, buf, ptr, blessp);
			}
		}
		propadr = next_prop(pptr, propadr, propname, sizeof(propname));
	}
	return cnt;
}


void
do_unbless(command_t *cmd) {
	OBJ *player = OBJECT(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	OBJ *victim;
	char buf[BUFFER_LEN];
	int cnt;

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(REF(player), "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(REF(player), "Usage is @unbless object=propname.");
		return;
	}

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(REF(player), "Permission denied. (You're not a wizard)");
		return;
	}

	/* get victim */
	victim = ematch_all(player, what);
	if (!victim) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

	cnt = blessprops_wildcard(REF(player), REF(victim), "", propname, 0);
	snprintf(buf, sizeof(buf), "%d propert%s unblessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(REF(player), buf);
}


void
do_bless(command_t *cmd) {
	OBJ *player = OBJECT(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	OBJ *victim;
	char buf[BUFFER_LEN];
	int cnt;

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(REF(player), "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(REF(player), "Usage is @bless object=propname.");
		return;
	}

	/* get victim */
	victim = ematch_all(player, what);
	if (!victim) {
		notify(REF(player), NOMATCH_MESSAGE);
		return;
	}

#ifdef GOD_PRIV
	if(!God(REF(player)) && God(victim->owner)) {
		notify(REF(player), "Only God may touch God's stuff.");
		return;
	}
#endif

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(REF(player), "Permission denied. (you're not a wizard)");
		return;
	}

	cnt = blessprops_wildcard(REF(player), REF(victim), "", propname, 1);
	snprintf(buf, sizeof(buf), "%d propert%s blessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(REF(player), buf);
}

void
do_boot(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref victim;
	char buf[BUFFER_LEN];

	CBUG(OBJECT(player)->type != TYPE_ENTITY);
	if (!(ENTITY(player)->flags & EF_WIZARD)) {
		notify(player, "Only a Wizard player can boot someone off.");
		return;
	}
	if ((victim = lookup_player(name)) == NOTHING) {
		notify(player, "That player does not exist.");
		return;
	}
	if (OBJECT(victim)->type != TYPE_ENTITY) {
		notify(player, "You can only boot entities!");
	}
#ifdef GOD_PRIV
	else if (God(victim)) {
		notify(player, "You can't boot God!");
	}
#endif							/* GOD_PRIV */

	else {
		notify(victim, "You have been booted off the game.");
		if (boot_off(victim)) {
			warn("BOOTED: %s(%d) by %s(%d)", OBJECT(victim)->name,
					   victim, OBJECT(player)->name, player);
			if (player != victim) {
				snprintf(buf, sizeof(buf), "You booted %s off!", OBJECT(victim)->name);
				notify(player, buf);
			}
		} else {
			snprintf(buf, sizeof(buf), "%s is not connected.", OBJECT(victim)->name);
			notify(player, buf);
		}
	}
}

static inline dbref
reverse(dbref list)
{
	dbref newlist;
	dbref rest;

	newlist = NOTHING;
	while (list != NOTHING) {
		rest = OBJECT(list)->next;
		PUSH(list, newlist);
		list = rest;
	}
	return newlist;
}

static inline void
send_contents(dbref loc, dbref dest)
{
	dbref first;
	dbref rest;

	first = OBJECT(loc)->contents;
	OBJECT(loc)->contents = NOTHING;

	/* blast locations of everything in list */
	DOLIST(rest, first)
		OBJECT(rest)->location = NOTHING;

	while (first != NOTHING) {
		rest = OBJECT(first)->next;
		if (!is_item(first)) {
			moveto(first, loc);
		} else {
			moveto(first, parent_loop_check(first, dest) ? loc : dest);
		}
		first = rest;
	}

	OBJECT(loc)->contents = reverse(OBJECT(loc)->contents);
}

void
do_toad(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *recip = cmd->argv[2];
	dbref victim;
	dbref recipient;
	dbref stuff;
	char buf[BUFFER_LEN];

	CBUG(OBJECT(player)->type != TYPE_ENTITY);

	if (!(ENTITY(player)->flags & EF_WIZARD)) {
		notify(player, "Only a Wizard player can turn an entity into a toad.");
		return;
	}
	if ((victim = lookup_player(name)) == NOTHING) {
		notify(player, "That player does not exist.");
		return;
	}
#ifdef GOD_PRIV
	if (God(victim)) {
		notify(player, "You cannot @toad God.");
		if(!God(player)) {
			warn("TOAD ATTEMPT: %s(#%d) tried to toad God.",OBJECT(player)->name,player);
		}
		return;
	}
#endif
	if(player == victim) {
		/* If GOD_PRIV isn't defined, this could happen: we don't want the
		 * last wizard to be toaded, in any case, so only someone else can
		 * do it. */
		notify(player, "You cannot toad yourself.  Get someone else to do it for you.");
		return;
	}
	if (!*recip) {
		/* FIXME: Make me a tunable parameter! */
		recipient = GOD;
	} else {
		if ((recipient = lookup_player(recip)) == NOTHING || recipient == victim) {
			notify(player, "That recipient does not exist.");
			return;
		}
	}

	if (OBJECT(victim)->type != TYPE_ENTITY) {
		notify(player, "You can only turn entities into toads!");
#ifdef GOD_PRIV
	} else if (!(God(player)) && (ENTITY(victim)->flags & EF_WIZARD)) {
#else
	} else if (Wizard(victim)) {
#endif
		notify(player, "You can't turn a Wizard into a toad.");
	} else {
		/* we're ok */
		/* do it */
		send_contents(victim, ENTITY(victim)->home);
		for (stuff = 0; stuff < db_top; stuff++) {
			if (OBJECT(stuff)->owner == victim) {
				switch (OBJECT(stuff)->type) {
				case TYPE_ROOM:
				case TYPE_CONSUMABLE:
				case TYPE_EQUIPMENT:
				case TYPE_THING:
					OBJECT(stuff)->owner = recipient;
					break;
				}
			}
		}

		/* notify people */
		notify(victim, "You have been turned into a toad.");
		snprintf(buf, sizeof(buf), "You turned %s into a toad!", OBJECT(victim)->name);
		notify(player, buf);
		warn("TOADED: %s(%d) by %s(%d)", OBJECT(victim)->name, victim, OBJECT(player)->name, player);
		/* reset name */
		delete_player(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", OBJECT(victim)->name);
		free((void *) OBJECT(victim)->name);
		OBJECT(victim)->name = alloc_string(buf);

		boot_player_off(victim); /* Disconnect the toad */

		OBJECT(victim)->type = TYPE_THING;
		OBJECT(victim)->owner = player;	/* you get it */
		OBJECT(victim)->value = 1; /* don't let him keep his immense wealth */
	}
}

void
do_usage(command_t *cmd) {
	dbref player = cmd->player;
	int pid, psize;

#ifdef HAVE_GETRUSAGE
	struct rusage usage;
#endif

	if (!(ENTITY(OBJECT(player)->owner)->flags & EF_WIZARD)) {
		notify(player, "Permission denied. (@usage is wizard-only)");
		return;
	}
#ifndef NO_USAGE_COMMAND
	pid = getpid();
#ifdef HAVE_GETRUSAGE
	psize = getpagesize();
	getrusage(RUSAGE_SELF, &usage);
#endif

	notifyf(player, "Compiled on: %s", UNAME_VALUE);
	notifyf(player, "Process ID: %d", pid);
	notifyf(player, "Max descriptors/process: %ld", sysconf(_SC_OPEN_MAX));

#ifdef HAVE_GETRUSAGE
	notifyf(player, "Performed %d input servicings.", usage.ru_inblock);
	notifyf(player, "Performed %d output servicings.", usage.ru_oublock);
	notifyf(player, "Sent %d messages over a socket.", usage.ru_msgsnd);
	notifyf(player, "Received %d messages over a socket.", usage.ru_msgrcv);
	notifyf(player, "Received %d signals.", usage.ru_nsignals);
	notifyf(player, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	notifyf(player, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	notifyf(player, "Swapped out of main memory %d times.", usage.ru_nswap);
	notifyf(player, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	notifyf(player, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	notifyf(player, "User time used: %d sec.", usage.ru_utime.tv_sec);
	notifyf(player, "System time used: %d sec.", usage.ru_stime.tv_sec);
	notifyf(player, "Pagesize for this machine: %d", psize);
	notifyf(player, "Maximum resident memory: %ldk",
			   (long) (usage.ru_maxrss * (psize / 1024)));
	notifyf(player, "Integral resident memory: %ldk",
			   (long) (usage.ru_idrss * (psize / 1024)));
#endif							/* HAVE_GETRUSAGE */

#else							/* NO_USAGE_COMMAND */

	notify(player, "Sorry, this server was compiled with NO_USAGE_COMMAND.");

#endif							/* NO_USAGE_COMMAND */
}

static const char *wiz_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_wiz_c_version(void) { return wiz_c_version; }

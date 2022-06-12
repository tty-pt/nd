/* $Header$ */

#include "io.h"
#include "entity.h"
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
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
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
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

#ifdef GOD_PRIV
	if(!God(player) && God(victim->owner)) {
		notify(eplayer, "God has already set that where He wants it to be.");
		return;
	}
#endif

	destination = ematch_all(player, to);

	if (!destination) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	switch (victim->type) {
	case TYPE_ENTITY:
		{
			ENT *evictim = &victim->sp.entity;
			if (!controls(player, victim) ||
					!controls(player, destination) ||
					!controls(player, victim->location) ||
					(object_item(destination) && !controls(player, destination->location))) {
				notify(eplayer, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)");
				break;
			}
			if (destination->type != TYPE_ROOM) {
				notify(eplayer, "Bad destination.");
				break;
			}
			if (object_plc(victim, destination)) {
				notify(eplayer, "Objects can't contain themselves.");
				break;
			}
			notify(evictim, "You feel a wrenching sensation...");
			enter(victim, destination);
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (object_plc(victim, destination)) {
			notify(eplayer, "You can't make a container contain itself!");
			break;
		}
		if (destination->type != TYPE_ROOM
			&& destination->type != TYPE_ENTITY
			&& !object_item(destination)) {
			notify(eplayer, "Bad destination.");
			break;
		}
		if (!(controls(player, destination) &&
			  (controls(player, victim) || controls(player, victim->location)))) {
			notify(eplayer, "Permission denied. (must control dest and be able to link to it, or control dest's loc)");
			break;
		}
		/* check for non-sticky dropto */
		if (destination->type == TYPE_ROOM) {
			ROO *rdestination = &destination->sp.room;
			if (rdestination->dropto)
				destination = rdestination->dropto;
		}
		object_move(victim, destination);
		notify(eplayer, "Teleported.");
		break;
	case TYPE_GARBAGE:
		notify(eplayer, "That object is in a place where magic cannot reach it.");
		break;
	case TYPE_ROOM:
	default:
		notify(eplayer, "You can't teleport that.");
		break;
	}
}

int
blessprops_wildcard(OBJ *player, OBJ *thing, const char *dir, const char *wild, int blessp)
{
	ENT *eplayer = &player->sp.entity;
	char propname[BUFFER_LEN];
	char wld[BUFFER_LEN];
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *ptr, *wldcrd = wld;
	PropPtr propadr, pptr;
	int i, cnt = 0;
	int recurse = 0;

#ifdef GOD_PRIV
	if(!God(player) && God(thing->owner)) {
		notify(eplayer, "Only God may touch what is God's.");
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
				|| (eplayer->flags & EF_WIZARD))) {
				if (!*ptr || recurse) {
					cnt++;
					if (blessp) {
						set_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Blessed %s", buf);
					} else {
						clear_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Unblessed %s", buf);
					}
					notify(eplayer, buf2);
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
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	OBJ *victim;
	char buf[BUFFER_LEN];
	int cnt;

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(eplayer, "Usage is @unbless object=propname.");
		return;
	}

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Permission denied. (You're not a wizard)");
		return;
	}

	/* get victim */
	victim = ematch_all(player, what);
	if (!victim) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	cnt = blessprops_wildcard(player, victim, "", propname, 0);
	snprintf(buf, sizeof(buf), "%d propert%s unblessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(eplayer, buf);
}


void
do_bless(command_t *cmd) {
	OBJ *player = object_get(cmd->player);
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	OBJ *victim;
	char buf[BUFFER_LEN];
	int cnt;

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(eplayer, "Usage is @bless object=propname.");
		return;
	}

	/* get victim */
	victim = ematch_all(player, what);
	if (!victim) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

#ifdef GOD_PRIV
	if(!God(player) && God(victim->owner)) {
		notify(eplayer, "Only God may touch God's stuff.");
		return;
	}
#endif

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Permission denied. (you're not a wizard)");
		return;
	}

	cnt = blessprops_wildcard(player, victim, "", propname, 1);
	snprintf(buf, sizeof(buf), "%d propert%s blessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(eplayer, buf);
}

void
do_boot(command_t *cmd) {
	OBJ *player = object_get(cmd->player);
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *victim;
	char buf[BUFFER_LEN];

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only a Wizard player can boot someone off.");
		return;
	}

	victim = lookup_player(name);

	if (!victim) {
		notify(eplayer, "That player does not exist.");
		return;
	}

	if (victim->type != TYPE_ENTITY) {
		notify(eplayer, "You can only boot entities!");
	}

	ENT *evictim = &victim->sp.entity;

#ifdef GOD_PRIV
	if (God(victim)) {
		notify(eplayer, "You can't boot God!");
	}
#endif							/* GOD_PRIV */

	else {
		notify(evictim, "You have been booted off the game.");
		if (boot_off(victim)) {
			warn("BOOTED: %s(%d) by %s(%d)", victim->name,
					   object_ref(victim), player->name, object_ref(player));
			if (player != victim) {
				snprintf(buf, sizeof(buf), "You booted %s off!", victim->name);
				notify(eplayer, buf);
			}
		} else {
			snprintf(buf, sizeof(buf), "%s is not connected.", victim->name);
			notify(eplayer, buf);
		}
	}
}

static inline OBJ *
reverse(OBJ *list)
{
	OBJ *newlist = NULL;
	OBJ *rest;

	while (list) {
		rest = list->next;
		PUSH(list, newlist);
		list = rest;
	}

	return newlist;
}

static inline void
send_contents(OBJ *loc, OBJ *dest)
{
	OBJ *first, *rest;

	first = loc->contents;
	loc->contents = NULL;

	/* blast locations of everything in list */
	FOR_LIST(rest, first)
		rest->location = NULL;

	while (first) {
		rest = first->next;
		if (!object_item(first)) {
			object_move(first, loc);
		} else {
			object_move(first, object_plc(first, dest) ? loc : dest);
		}
		first = rest;
	}

	loc->contents = reverse(loc->contents);
}

void
do_toad(command_t *cmd) {
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	const char *recip = cmd->argv[2];
	OBJ *victim;
	OBJ *recipient;
	dbref stuff;
	char buf[BUFFER_LEN];

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Only a Wizard player can turn an entity into a toad.");
		return;
	}

	victim = lookup_player(name);

	if (!victim) {
		notify(eplayer, "That player does not exist.");
		return;
	}

#ifdef GOD_PRIV
	if (God(victim)) {
		notify(eplayer, "You cannot @toad God.");
		if(!God(player)) {
			warn("TOAD ATTEMPT: %s(#%d) tried to toad God.", player->name, object_ref(player));
		}
		return;
	}
#endif
	if(player == victim) {
		/* If GOD_PRIV isn't defined, this could happen: we don't want the
		 * last wizard to be toaded, in any case, so only someone else can
		 * do it. */
		notify(eplayer, "You cannot toad yourself.  Get someone else to do it for you.");
		return;
	}
	if (!*recip) {
		/* FIXME: Make me a tunable parameter! */
		recipient = object_get(GOD);
	} else {
		recipient = lookup_player(recip);
		if (!recipient || recipient == victim) {
			notify(eplayer, "That recipient does not exist.");
			return;
		}
	}

	if (victim->type != TYPE_ENTITY)
		notify(eplayer, "You can only turn entities into toads!");

	ENT *evictim = &victim->sp.entity;

#ifdef GOD_PRIV
	if (!(God(player)) && (evictim->flags & EF_WIZARD)) {
#else
	if (Wizard(victim)) {
#endif
		notify(eplayer, "You can't turn a Wizard into a toad.");
	} else {
		/* we're ok */
		/* do it */
		send_contents(victim, evictim->home);
		for (stuff = 0; stuff < db_top; stuff++) {
			OBJ *ostuff = object_get(stuff);
			if (ostuff->owner == victim) {
				switch (ostuff->type) {
				case TYPE_ROOM:
				case TYPE_CONSUMABLE:
				case TYPE_EQUIPMENT:
				case TYPE_THING:
					ostuff->owner = recipient;
					break;
				}
			}
		}

		/* notify people */
		notify(evictim, "You have been turned into a toad.");
		snprintf(buf, sizeof(buf), "You turned %s into a toad!", victim->name);
		notify(eplayer, buf);
		warn("TOADED: %s(%d) by %s(%d)", victim->name, object_ref(victim), player->name, object_ref(player));
		/* reset name */
		delete_player(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", victim->name);
		free((void *) victim->name);
		victim->name = alloc_string(buf);

		boot_player_off(victim); /* Disconnect the toad */

		victim->type = TYPE_THING;
		victim->owner = player;	/* you get it */
		victim->value = 1; /* don't let him keep his immense wealth */
	}
}

void
do_usage(command_t *cmd) {
	OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
	int pid, psize;

#ifdef HAVE_GETRUSAGE
	struct rusage usage;
#endif

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "Permission denied. (@usage is wizard-only)");
		return;
	}
#ifndef NO_USAGE_COMMAND
	pid = getpid();
#ifdef HAVE_GETRUSAGE
	psize = getpagesize();
	getrusage(RUSAGE_SELF, &usage);
#endif

	notifyf(eplayer, "Compiled on: %s", UNAME_VALUE);
	notifyf(eplayer, "Process ID: %d", pid);
	notifyf(eplayer, "Max descriptors/process: %ld", sysconf(_SC_OPEN_MAX));

#ifdef HAVE_GETRUSAGE
	notifyf(eplayer, "Performed %d input servicings.", usage.ru_inblock);
	notifyf(eplayer, "Performed %d output servicings.", usage.ru_oublock);
	notifyf(eplayer, "Sent %d messages over a socket.", usage.ru_msgsnd);
	notifyf(eplayer, "Received %d messages over a socket.", usage.ru_msgrcv);
	notifyf(eplayer, "Received %d signals.", usage.ru_nsignals);
	notifyf(eplayer, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	notifyf(eplayer, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	notifyf(eplayer, "Swapped out of main memory %d times.", usage.ru_nswap);
	notifyf(eplayer, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	notifyf(eplayer, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	notifyf(eplayer, "User time used: %d sec.", usage.ru_utime.tv_sec);
	notifyf(eplayer, "System time used: %d sec.", usage.ru_stime.tv_sec);
	notifyf(eplayer, "Pagesize for this machine: %d", psize);
	notifyf(eplayer, "Maximum resident memory: %ldk",
			   (long) (usage.ru_maxrss * (psize / 1024)));
	notifyf(eplayer, "Integral resident memory: %ldk",
			   (long) (usage.ru_idrss * (psize / 1024)));
#endif							/* HAVE_GETRUSAGE */

#else							/* NO_USAGE_COMMAND */

	notify(eplayer, "Sorry, this server was compiled with NO_USAGE_COMMAND.");

#endif							/* NO_USAGE_COMMAND */
}

static const char *wiz_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_wiz_c_version(void) { return wiz_c_version; }

/* $Header$ */

#include <ndc.h>
#include "io.h"
#include "entity.h"
#include "config.h"

/* Wizard-only commands */

#include <stdio.h>

#include <sys/resource.h>

#include <stdlib.h>

#include "params.h"
#include "defaults.h"
#include "match.h"
#include "player.h"
#include "debug.h"

void
do_teleport(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	char *arg1 = argv[1];
	char *arg2 = argv[2];
	OBJ *victim;
	OBJ *destination;
	char *to;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim = player;
		to = arg1;
	} else if (!(victim = ematch_all(player, arg1))) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

	if(!God(player) && victim->owner == GOD) {
		nd_writef(player, "God has already set that where He wants it to be.\n");
		return;
	}

	destination = ematch_all(player, to);

	if (!destination) {
		nd_writef(player, NOMATCH_MESSAGE);
		return;
	}

	switch (victim->type) {
	case TYPE_ENTITY:
		{
			if (!controls(player, victim) ||
					!controls(player, destination) ||
					!controls(player, object_get(victim->location)) ||
					(object_item(destination) && !controls(player, object_get(destination->location)))) {
				nd_writef(player, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)\n");
				break;
			}
			if (destination->type != TYPE_ROOM) {
				nd_writef(player, "Bad destination.\n");
				break;
			}
			if (object_plc(victim, destination)) {
				nd_writef(player, "Objects can't contain themselves.\n");
				break;
			}
			nd_writef(victim, "You feel a wrenching sensation...\n");
			enter(victim, destination, E_NULL);
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (object_plc(victim, destination)) {
			nd_writef(player, "You can't make a container contain itself!\n");
			break;
		}
		if (destination->type != TYPE_ROOM
			&& destination->type != TYPE_ENTITY
			&& !object_item(destination)) {
			nd_writef(player, "Bad destination.\n");
			break;
		}
		if (!(controls(player, destination) &&
			  (controls(player, victim) || controls(player, object_get(victim->location))))) {
			nd_writef(player, "Permission denied. (must control dest and be able to link to it, or control dest's loc)\n");
			break;
		}
		object_move(victim, destination);
		nd_writef(player, "Teleported.\n");
		break;
	case TYPE_GARBAGE:
		nd_writef(player, "That object is in a place where magic cannot reach it.\n");
		break;
	case TYPE_ROOM:
	default:
		nd_writef(player, "You can't teleport that.\n");
		break;
	}
}

void
do_boot(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	OBJ *victim;

	if (!(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "Only a Wizard player can boot someone off.\n");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		nd_writef(player, "That player does not exist.\n");
		return;
	}

	if (victim->type != TYPE_ENTITY)
		nd_writef(player, "You can only boot entities!\n");

	if (God(victim))
		nd_writef(player, "You can't boot God!\n");

	else {
		nd_writef(victim, "You have been booted off the game.\n");
		nd_close(victim);
		nd_writef(player, "You booted %s off!", victim->name);
	}
}

void
do_toad(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	char *name = argv[1];
	char *recip = argv[2];
	OBJ *victim;
	OBJ *recipient;
	dbref stuff;
	char buf[BUFFER_LEN];

	CBUG(player->type != TYPE_ENTITY);

	if (!(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "Only a Wizard player can turn an entity into a toad.\n");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		nd_writef(player, "That player does not exist.\n");
		return;
	}

	if (God(victim)) {
		nd_writef(player, "You cannot @toad God.\n");
		if(!God(player)) {
			warn("TOAD ATTEMPT: %s(#%d) tried to toad God.", player->name, object_ref(player));
		}
		return;
	}

	if (!*recip) {
		/* FIXME: Make me a tunable parameter! */
		recipient = object_get(GOD);
	} else {
		recipient = player_get(recip);
		if (!recipient || recipient == victim) {
			nd_writef(player, "That recipient does not exist.\n");
			return;
		}
	}

	if (victim->type != TYPE_ENTITY)
		nd_writef(player, "You can only turn entities into toads!\n");

	ENT *evictim = &victim->sp.entity;

	if (!(God(player)) && (evictim->flags & EF_WIZARD)) {
		nd_writef(player, "You can't turn a Wizard into a toad.\n");
	} else {
		/* we're ok */
		/* do it */
		OBJ *rest;
		FOR_LIST(rest, victim)
			recycle(object_get(GOD), rest);
		for (stuff = 0; stuff < db_top; stuff++) {
			OBJ *ostuff = object_get(stuff);
			if (ostuff->owner == object_ref(victim)) {
				switch (ostuff->type) {
				case TYPE_ROOM:
				case TYPE_CONSUMABLE:
				case TYPE_EQUIPMENT:
				case TYPE_THING:
					ostuff->owner = object_ref(recipient);
					break;
				}
			}
		}

		/* ndc_writef people */
		nd_writef(victim, "You have been turned into a toad.\n");
		nd_writef(player, "You turned %s into a toad!", victim->name);
		warn("TOADED: %s(%d) by %s(%d)", victim->name, object_ref(victim), player->name, object_ref(player));
		/* reset name */
		player_delete(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", victim->name);
		free((void *) victim->name);
		victim->name = strdup(buf);
		nd_close(victim);
		victim->type = TYPE_THING;
		victim->owner = object_ref(player);	/* you get it */
		victim->value = 1; /* don't let him keep his immense wealth */
	}
}

void
do_usage(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	struct rusage usage;

	if (!(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "Permission denied. (@usage is wizard-only)\n");
		return;
	}
	getrusage(RUSAGE_SELF, &usage);

	nd_writef(player, "Performed %d input servicings.", usage.ru_inblock);
	nd_writef(player, "Performed %d output servicings.", usage.ru_oublock);
	nd_writef(player, "Sent %d messages over a socket.", usage.ru_msgsnd);
	nd_writef(player, "Received %d messages over a socket.", usage.ru_msgrcv);
	nd_writef(player, "Received %d signals.", usage.ru_nsignals);
	nd_writef(player, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	nd_writef(player, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	nd_writef(player, "Swapped out of main memory %d times.", usage.ru_nswap);
	nd_writef(player, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	nd_writef(player, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	nd_writef(player, "User time used: %d sec.", usage.ru_utime.tv_sec);
	nd_writef(player, "System time used: %d sec.", usage.ru_stime.tv_sec);
}

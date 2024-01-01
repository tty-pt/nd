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
		ndc_writef(fd, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

	if(!God(player) && God(victim->owner)) {
		ndc_writef(fd, "God has already set that where He wants it to be.\n");
		return;
	}

	destination = ematch_all(player, to);

	if (!destination) {
		ndc_writef(fd, NOMATCH_MESSAGE);
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
				ndc_writef(fd, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)\n");
				break;
			}
			if (destination->type != TYPE_ROOM) {
				ndc_writef(fd, "Bad destination.\n");
				break;
			}
			if (object_plc(victim, destination)) {
				ndc_writef(fd, "Objects can't contain themselves.\n");
				break;
			}
			ndc_writef(evictim->fd, "You feel a wrenching sensation...\n");
			enter(victim, destination, E_NULL);
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (object_plc(victim, destination)) {
			ndc_writef(fd, "You can't make a container contain itself!\n");
			break;
		}
		if (destination->type != TYPE_ROOM
			&& destination->type != TYPE_ENTITY
			&& !object_item(destination)) {
			ndc_writef(fd, "Bad destination.\n");
			break;
		}
		if (!(controls(player, destination) &&
			  (controls(player, victim) || controls(player, victim->location)))) {
			ndc_writef(fd, "Permission denied. (must control dest and be able to link to it, or control dest's loc)\n");
			break;
		}
		/* check for non-sticky dropto */
		if (destination->type == TYPE_ROOM) {
			ROO *rdestination = &destination->sp.room;
			if (rdestination->dropto)
				destination = rdestination->dropto;
		}
		object_move(victim, destination);
		ndc_writef(fd, "Teleported.\n");
		break;
	case TYPE_GARBAGE:
		ndc_writef(fd, "That object is in a place where magic cannot reach it.\n");
		break;
	case TYPE_ROOM:
	default:
		ndc_writef(fd, "You can't teleport that.\n");
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
		ndc_writef(fd, "Only a Wizard player can boot someone off.\n");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		ndc_writef(fd, "That player does not exist.\n");
		return;
	}

	if (victim->type != TYPE_ENTITY) {
		ndc_writef(fd, "You can only boot entities!\n");
	}

	ENT *evictim = &victim->sp.entity;

	if (God(victim)) {
		ndc_writef(fd, "You can't boot God!\n");
	}

	else {
		ndc_writef(evictim->fd, "You have been booted off the game.\n");
		ndc_close(evictim->fd);
		ndc_writef(fd, "You booted %s off!", victim->name);
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
		ndc_writef(fd, "Only a Wizard player can turn an entity into a toad.\n");
		return;
	}

	victim = player_get(name);

	if (!victim) {
		ndc_writef(fd, "That player does not exist.\n");
		return;
	}

	if (God(victim)) {
		ndc_writef(fd, "You cannot @toad God.\n");
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
			ndc_writef(fd, "That recipient does not exist.\n");
			return;
		}
	}

	if (victim->type != TYPE_ENTITY)
		ndc_writef(fd, "You can only turn entities into toads!\n");

	ENT *evictim = &victim->sp.entity;

	if (!(God(player)) && (evictim->flags & EF_WIZARD)) {
		ndc_writef(fd, "You can't turn a Wizard into a toad.\n");
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

		/* ndc_writef people */
		ndc_writef(evictim->fd, "You have been turned into a toad.\n");
		ndc_writef(fd, "You turned %s into a toad!", victim->name);
		warn("TOADED: %s(%d) by %s(%d)", victim->name, object_ref(victim), player->name, object_ref(player));
		/* reset name */
		player_delete(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", victim->name);
		free((void *) victim->name);
		victim->name = strdup(buf);

		ndc_close(evictim->fd);

		victim->type = TYPE_THING;
		victim->owner = player;	/* you get it */
		victim->value = 1; /* don't let him keep his immense wealth */
	}
}

void
do_usage(int fd, int argc, char *argv[]) {
	OBJ *player = FD_PLAYER(fd);
	ENT *eplayer = &player->sp.entity;
	struct rusage usage;

	if (!(eplayer->flags & EF_WIZARD)) {
		ndc_writef(fd, "Permission denied. (@usage is wizard-only)\n");
		return;
	}
	getrusage(RUSAGE_SELF, &usage);

	ndc_writef(fd, "Performed %d input servicings.", usage.ru_inblock);
	ndc_writef(fd, "Performed %d output servicings.", usage.ru_oublock);
	ndc_writef(fd, "Sent %d messages over a socket.", usage.ru_msgsnd);
	ndc_writef(fd, "Received %d messages over a socket.", usage.ru_msgrcv);
	ndc_writef(fd, "Received %d signals.", usage.ru_nsignals);
	ndc_writef(fd, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	ndc_writef(fd, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	ndc_writef(fd, "Swapped out of main memory %d times.", usage.ru_nswap);
	ndc_writef(fd, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	ndc_writef(fd, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	ndc_writef(fd, "User time used: %d sec.", usage.ru_utime.tv_sec);
	ndc_writef(fd, "System time used: %d sec.", usage.ru_stime.tv_sec);
}

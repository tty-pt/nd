#include "uapi/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "config.h"
#include "uapi/entity.h"
#include "uapi/match.h"
#include "player.h"

void
do_teleport(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), victim_ref, destination_ref;
	char *arg1 = argv[1];
	char *arg2 = argv[2];
	char *to;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim_ref = player_ref;
		to = arg1;
	} else if ((victim_ref = ematch_all(player_ref, arg1)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	} else
		to = arg2;

	OBJ victim;
	lhash_get(obj_hd, &victim, victim_ref);

	destination_ref = ematch_all(player_ref, to);

	if (destination_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ destination;
	lhash_get(obj_hd, &destination, destination_ref);

	if (victim_ref == destination_ref || destination.location == victim_ref) {
		nd_writef(player_ref, "You move a thing inside itself!\n");
		return;
	}

	switch (victim.type) {
	case TYPE_ENTITY:
		{
			if (!controls(player_ref, victim_ref) ||
					!controls(player_ref, destination_ref) ||
					!controls(player_ref, victim.location) ||
					(object_item(destination_ref) && !controls(player_ref, destination.location))) {
				nd_writef(player_ref, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)\n");
				break;
			}
			if (destination.type != TYPE_ROOM) {
				nd_writef(player_ref, "Bad destination.\n");
				break;
			}
			nd_writef(victim_ref, "You feel a wrenching sensation...\n");
			enter(victim_ref, destination_ref, E_NULL);
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (destination.type != TYPE_ROOM
			&& destination.type != TYPE_ENTITY
			&& !object_item(destination_ref)) {
			nd_writef(player_ref, "Bad destination.\n");
			break;
		}
		if (!(controls(player_ref, destination_ref) &&
			  (controls(player_ref, victim_ref) || controls(player_ref, victim.location)))) {
			nd_writef(player_ref, "Permission denied. (must control dest and be able to link to it, or control dest's loc)\n");
			break;
		}
		object_move(victim_ref, destination_ref);
		nd_writef(player_ref, "Teleported.\n");
		break;
	case TYPE_ROOM:
	default:
		nd_writef(player_ref, "You can't teleport that.\n");
		break;
	}
}

void
do_boot(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd), victim_ref;
	char *name = argv[1];

	if (!(ent_get(player_ref).flags & EF_WIZARD)) {
		nd_writef(player_ref, "Only a Wizard player can boot someone off.\n");
		return;
	}

	victim_ref = player_get(name);

	if (victim_ref == NOTHING) {
		nd_writef(player_ref, "That player does not exist.\n");
		return;
	}

	if (victim_ref == ROOT)
		nd_writef(player_ref, "You can't boot root!\n");

	OBJ victim;
	lhash_get(obj_hd, &victim, victim_ref);

	if (victim.type != TYPE_ENTITY)
		nd_writef(player_ref, "You can only boot entities!\n");

	else {
		nd_writef(victim_ref, "You have been booted off the game.\n");
		nd_close(victim_ref);
		nd_writef(player_ref, "You booted %s off!", victim.name);
	}
}

void
do_toad(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	char *recip = argv[2];
	unsigned recipient_ref;
	char buf[BUFFER_LEN];

	if (!(ent_get(player_ref).flags & EF_WIZARD)) {
		nd_writef(player_ref, "Only a Wizard player can turn an entity into a toad.\n");
		return;
	}

	unsigned victim_ref = player_get(name);

	if (victim_ref == NOTHING) {
		nd_writef(player_ref, "That player does not exist.\n");
		return;
	}

	if (victim_ref == ROOT) {
		nd_writef(player_ref, "You cannot toad root.\n");
		return;
	}

	if (!*recip)
		recipient_ref = ROOT;
	else {
		recipient_ref = player_get(recip);
		if (recipient_ref == NOTHING || recipient_ref == victim_ref) {
			nd_writef(player_ref, "That recipient does not exist.\n");
			return;
		}
	}

	OBJ victim;
	lhash_get(obj_hd, &victim, victim_ref);

	if (victim.type != TYPE_ENTITY)
		nd_writef(player_ref, "You can only turn entities into toads!\n");

	if (player_ref != ROOT && (ent_get(victim_ref).flags & EF_WIZARD))
		nd_writef(player_ref, "You can't turn a Wizard into a toad.\n");
	else {
		unsigned tmp_ref;
		struct hash_cursor c = fhash_iter(contents_hd, victim_ref);
		while (ahash_next(&tmp_ref, &c))
			object_move(tmp_ref, NOTHING);
		struct hash_cursor c2 = lhash_iter(obj_hd);
		OBJ tmp;
		while (lhash_next(&tmp_ref, &tmp, &c2)) {
			if (tmp.owner == victim_ref) {
				switch (tmp.type) {
				case TYPE_ROOM:
				case TYPE_CONSUMABLE:
				case TYPE_EQUIPMENT:
				case TYPE_THING:
					tmp.owner = recipient_ref;
					lhash_put(obj_hd, tmp_ref, &tmp);
				}
			}
		}

		nd_writef(victim_ref, "You have been turned into a toad.\n");
		nd_writef(player_ref, "You turned %s into a toad!", victim.name);
		player_delete(victim.name);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", victim.name);
		strcpy(victim.name, buf);
		nd_close(victim_ref);
		victim.type = TYPE_THING;
		victim.owner = player_ref;
		victim.value = 1;
		lhash_put(obj_hd, victim_ref, &victim);
	}
}

void
do_usage(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused))) {
	unsigned player_ref = fd_player(fd);
	struct rusage usage;

	if (!(ent_get(player_ref).flags & EF_WIZARD)) {
		nd_writef(player_ref, "Permission denied. (@usage is wizard-only)\n");
		return;
	}
	getrusage(RUSAGE_SELF, &usage);

	nd_writef(player_ref, "Performed %d input servicings.", usage.ru_inblock);
	nd_writef(player_ref, "Performed %d output servicings.", usage.ru_oublock);
	nd_writef(player_ref, "Sent %d messages over a socket.", usage.ru_msgsnd);
	nd_writef(player_ref, "Received %d messages over a socket.", usage.ru_msgrcv);
	nd_writef(player_ref, "Received %d signals.", usage.ru_nsignals);
	nd_writef(player_ref, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	nd_writef(player_ref, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	nd_writef(player_ref, "Swapped out of main memory %d times.", usage.ru_nswap);
	nd_writef(player_ref, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	nd_writef(player_ref, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	nd_writef(player_ref, "User time used: %d sec.", usage.ru_utime.tv_sec);
	nd_writef(player_ref, "System time used: %d sec.", usage.ru_stime.tv_sec);
}

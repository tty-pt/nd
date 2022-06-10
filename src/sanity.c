/* $Header$ */


#undef POINTERS

#include "copyright.h"
#include "config.h"

#include <stdio.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>

#include "mdb.h"
#include "defaults.h"
#include "externs.h"

#include "params.h"
#include "props.h"
#include "search.h"

#define unparse(x) ((char*)unparse_object(GOD, (x)))

int sanity_violated = 0;

void
SanPrint(dbref player, const char *format, ...)
{
	va_list args;
	char buf[16384];

	va_start(args, format);

	vsnprintf(buf, sizeof(buf), format, args);
	if (player == NOTHING) {
		fprintf(stdout, "%s\n", buf);
		fflush(stdout);
	} else if (player == AMBIGUOUS) {
		fprintf(stderr, "%s\n", buf);
	} else {
		notify(player, buf);
	}

	va_end(args);
}

void
violate(dbref player, dbref i, const char *s)
{
	SanPrint(player, "Object \"%s\" %s!", unparse(i), s);
	sanity_violated = 1;
}


static int
valid_ref(dbref obj)
{
	if (obj == NOTHING) {
		return 1;
	}
	if (obj < 0) {
		return 0;
	}
	if (obj >= db_top) {
		return 0;
	}
	return 1;
}


static int
valid_obj(dbref obj)
{
	if (obj == NOTHING) {
		return 0;
	}
	if (!valid_ref(obj)) {
		return 0;
	}
	switch (OBJECT(obj)->type) {
	case TYPE_ROOM:
	case TYPE_ENTITY:
	case TYPE_CONSUMABLE:
	case TYPE_PLANT:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		return 1;
		break;
	default:
		return 0;
	}
}


static void
check_next_chain(dbref player, dbref obj)
{
	dbref i;
	dbref orig;

	orig = obj;
	while (obj != NOTHING && valid_ref(obj)) {
		for (i = orig; i != NOTHING; i = OBJECT(i)->next) {
			if (i == OBJECT(obj)->next) {
				violate(player, obj,
						"has a 'next' field that forms an illegal loop in an object chain");
				return;
			}
			if (i == obj) {
				break;
			}
		}
		obj = OBJECT(obj)->next;
	}
	if (!valid_ref(obj)) {
		violate(player, obj, "has an invalid object in its 'next' chain");
	}
}


extern dbref recyclable;

static void
find_orphan_objects(dbref player)
{
	int i;

	SanPrint(player, "Searching for orphan objects...");

	for (i = 0; i < db_top; i++) {
		OBJECT(i)->flags &= ~OF_SANEBIT;
	}

	if (recyclable != NOTHING) {
		OBJECT(recyclable)->flags |= OF_SANEBIT;
	}
	OBJECT(GLOBAL_ENVIRONMENT)->flags |= OF_SANEBIT;

	for (i = 0; i < db_top; i++) {
		switch (OBJECT(i)->type) {
		case TYPE_ROOM:
			OBJECT(i)->flags |= OF_SANEBIT;
		}
		if (OBJECT(i)->contents != NOTHING) {
			if (OBJECT(OBJECT(i)->contents)->flags & OF_SANEBIT) {
				violate(player, OBJECT(i)->contents,
						"is referred to by more than one object's Next or Contents field");
			} else {
				OBJECT(OBJECT(i)->contents)->flags |= OF_SANEBIT;
			}
		}
		if (OBJECT(i)->next != NOTHING) {
			if (OBJECT(OBJECT(i)->next)->flags & OF_SANEBIT) {
				violate(player, OBJECT(i)->next,
						"is referred to by more than one object's Next or Contents field");
			} else {
				OBJECT(OBJECT(i)->next)->flags |= OF_SANEBIT;
			}
		}
	}

	for (i = 0; i < db_top; i++) {
		if (!(OBJECT(i)->flags & OF_SANEBIT)) {
			violate(player, i,
					"appears to be an orphan object, that is not referred to by any other object");
		}
	}

	for (i = 0; i < db_top; i++) {
		OBJECT(i)->flags &= ~OF_SANEBIT;
	}
}


void
check_room(dbref player, dbref obj)
{
	dbref i;

	i = ROOM(obj)->dropto;

	if (!valid_ref(i)) {
		violate(player, obj, "has its dropto set to an invalid object");
	} else if (i >= 0 && !is_item(i) && OBJECT(i)->type != TYPE_ROOM) {
		violate(player, obj, "has its dropto set to a non-room, non-item object");
	}
}

void
check_entity(dbref player, dbref obj)
{
	dbref i;

	i = ENTITY(obj)->home;

	if (!valid_obj(i)) {
		violate(player, obj, "has its home set to an invalid object");
	} else if (i >= 0 && OBJECT(i)->type != TYPE_ROOM) {
		violate(player, obj, "has its home set to a non-room object");
	}
}


void
check_garbage(dbref player, dbref obj)
{
	if (OBJECT(obj)->next != NOTHING && OBJECT(OBJECT(obj)->next)->type != TYPE_GARBAGE) {
		violate(player, obj,
				"has a non-garbage object as the 'next' object in the garbage chain");
	}
}


void
check_contents_list(dbref player, dbref obj)
{
	dbref i;
	int limit;

	if (OBJECT(obj)->type != TYPE_GARBAGE) {
		for (i = OBJECT(obj)->contents, limit = db_top;
			 valid_obj(i) &&
			 --limit && OBJECT(i)->location == obj; i = OBJECT(i)->next) ;
		if (i != NOTHING) {
			if (!limit) {
				check_next_chain(player, OBJECT(obj)->contents);
				violate(player, obj,
						"is the containing object, and has the loop in its contents chain");
			} else {
				if (!valid_obj(i)) {
					violate(player, obj, "has an invalid object in its contents list");
				} else {
					if (OBJECT(i)->location != obj) {
						violate(player, obj,
								"has an object in its contents lists that thinks it is located elsewhere");
					}
				}
			}
		}
	} else {
		if (OBJECT(obj)->contents != NOTHING) {
			if (OBJECT(obj)->type == TYPE_GARBAGE) {
				violate(player, obj, "is a garbage object whose contents aren't #-1");
			} else {
				violate(player, obj, "is a program whose contents aren't #-1");
			}
		}
	}
}

void
check_object(dbref player, dbref obj)
{
	/*
	 * Do we have a name?
	 */
	if (!OBJECT(obj)->name)
		violate(player, obj, "doesn't have a name");

	/*
	 * Check the ownership
	 */
	if (OBJECT(obj)->type != TYPE_GARBAGE) {
		if (!valid_obj(OBJECT(obj)->owner)) {
			violate(player, obj, "has an invalid object as its owner.");
		} else if (OBJECT(OBJECT(obj)->owner)->type != TYPE_ENTITY) {
			violate(player, obj, "has a non-player object as its owner.");
		}

		/* 
		 * check location 
		 */
		if (!valid_obj(OBJECT(obj)->location) &&
			!((obj == GLOBAL_ENVIRONMENT || OBJECT(obj)->type == TYPE_ROOM) && OBJECT(obj)->location == NOTHING)) {
			violate(player, obj, "has an invalid object as it's location");
		}
	}

	if (OBJECT(obj)->location != NOTHING && OBJECT(OBJECT(obj)->location)->type == TYPE_GARBAGE)
		violate(player, obj, "thinks it is located in a non-container object");

	if ((OBJECT(obj)->type == TYPE_GARBAGE) && (OBJECT(obj)->location != NOTHING))
		violate(player, obj, "is a garbage object with a location that isn't #-1");

	check_contents_list(player, obj);

	switch (OBJECT(obj)->type) {
	case TYPE_ROOM:
		check_room(player, obj);
		break;
	case TYPE_PLANT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		break;
	case TYPE_ENTITY:
		check_entity(player, obj);
		break;
	case TYPE_GARBAGE:
		check_garbage(player, obj);
		break;
	default:
		violate(player, obj, "has an unknown object type, and its flags may also be corrupt");
		break;
	}
}


void
sanity(dbref player)
{
	const int increp = 10000;
	int i;
	int j;

#ifdef GOD_PRIV
	if (player > NOTHING && !God(player)) {
#else
	if (player > NOTHING && !Wizard(player)) {
#endif

		notify(player, "Permission Denied.");
		return;
	}

	sanity_violated = 0;

	for (i = 0; i < db_top; i++) {
		if (!(i % increp)) {
			j = i + increp - 1;
			j = (j >= db_top) ? (db_top - 1) : j;
			SanPrint(player, "Checking objects %d to %d...", i, j);
		}
		check_object(player, i);
	}

	find_orphan_objects(player);

	SanPrint(player, "Done.");
}

#define SanFixed(ref, fixed) san_fixed_log((fixed), 1, (ref), -1)
#define SanFixed2(ref, ref2, fixed) san_fixed_log((fixed), 1, (ref), (ref2))
#define SanFixedRef(ref, fixed) san_fixed_log((fixed), 0, (ref), -1)
void
san_fixed_log(char *format, int unparse, dbref ref1, dbref ref2)
{
	char buf1[4096];
	char buf2[4096];

	if (unparse) {
		if (ref1 >= 0) {
			strlcpy(buf1, unparse(ref1), sizeof(buf1));
		}
		if (ref2 >= 0) {
			strlcpy(buf2, unparse(ref2), sizeof(buf2));
		}
		warn(format, buf1, buf2);
	} else {
		warn(format, ref1, ref2);
	}
}

void
cut_all_chains(dbref obj)
{
	if (OBJECT(obj)->contents != NOTHING) {
		SanFixed(obj, "Cleared contents of %s");
		OBJECT(obj)->contents = NOTHING;
	}
}

void
cut_bad_recyclable(void)
{
	dbref loop, prev;

	loop = recyclable;
	prev = NOTHING;
	while (loop != NOTHING) {
		if (!valid_ref(loop) || OBJECT(loop)->type != TYPE_GARBAGE || OBJECT(loop)->flags & OF_SANEBIT) {
			SanFixed(loop, "Recyclable object %s is not TYPE_GARBAGE");
			if (prev != NOTHING)
				OBJECT(prev)->next = NOTHING;
			else {
				recyclable = NOTHING;
			}
			return;
		}
		OBJECT(loop)->flags |= OF_SANEBIT;
		prev = loop;
		loop = OBJECT(loop)->next;
	}
}

void
cut_bad_contents(dbref obj)
{
	dbref loop, prev;

	loop = OBJECT(obj)->contents;
	prev = NOTHING;
	while (loop != NOTHING) {
		if (!valid_obj(loop) || OBJECT(loop)->flags & OF_SANEBIT ||
			OBJECT(loop)->location != obj || loop == obj) {
			if (!valid_obj(loop)) {
				SanFixed(obj, "Contents chain for %s cut at invalid dbref");
			} else if (loop == obj) {
				SanFixed(obj, "Contents chain for %s cut at self-reference");
			} else if (OBJECT(loop)->location != obj) {
				SanFixed2(obj, loop, "Contents chain for %s cut at misplaced object %s");
			} else if (OBJECT(loop)->flags & OF_SANEBIT) {
				SanFixed2(obj, loop, "Contents chain for %s cut at already chained object %s");
			} else {
				SanFixed2(obj, loop, "Contents chain for %s cut at %s");
			}
			if (prev != NOTHING)
				OBJECT(prev)->next = NOTHING;
			else
				OBJECT(obj)->contents = NOTHING;
			return;
		}
		OBJECT(loop)->flags |= OF_SANEBIT;
		prev = loop;
		loop = OBJECT(loop)->next;
	}
}

void
hacksaw_bad_chains(void)
{
	dbref loop;

	cut_bad_recyclable();
	for (loop = 0; loop < db_top; loop++) {
		if (OBJECT(loop)->type != TYPE_ROOM && !is_item(loop) &&
			OBJECT(loop)->type != TYPE_ENTITY) {
			cut_all_chains(loop);
		} else {
			cut_bad_contents(loop);
		}
	}
}

char *
rand_password(void)
{
	int loop;
	char pwdchars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char password[17];
	int charslen = strlen(pwdchars);

	for (loop = 0; loop < 16; loop++) {
		password[loop] = pwdchars[(RANDOM() >> 8) % charslen];
	}
	password[16] = 0;
	return alloc_string(password);
}

void
create_lostandfound(dbref * player, dbref * room)
{
	char player_name[PLAYER_NAME_LIMIT + 2] = "lost+found";
	int temp = 0;

	*room = object_new();
	OBJECT(*room)->name = alloc_string("lost+found");
	OBJECT(*room)->location = GLOBAL_ENVIRONMENT;
	ROOM(*room)->dropto = NOTHING;
	OBJECT(*room)->type = TYPE_ROOM;
	OBJECT(*room)->flags = OF_SANEBIT;
	PUSH(*room, OBJECT(GLOBAL_ENVIRONMENT)->contents);
	SanFixed(*room, "Using %s to resolve unknown location");

	while (lookup_player(player_name) != NOTHING && strlen(player_name) < PLAYER_NAME_LIMIT) {
		snprintf(player_name, sizeof(player_name), "lost+found%d", ++temp);
	}
	if (strlen(player_name) >= PLAYER_NAME_LIMIT) {
		warn("WARNING: Unable to get lost+found player, using %s",
		     unparse(GOD));
		*player = GOD;
	} else {
		const char *rpass;
		*player = object_new();
		OBJECT(*player)->name = alloc_string(player_name);
		OBJECT(*player)->location = *room;
		OBJECT(*player)->type = TYPE_ENTITY;
		OBJECT(*player)->flags = OF_SANEBIT;
		OBJECT(*player)->owner = *player;
		ENTITY(*player)->home = *room;
		OBJECT(*player)->value = START_PENNIES;
		rpass = rand_password();
		PUSH(*player, OBJECT(*room)->contents);
		add_player(*player);
		warn("Using %s (with password %s) to resolve "
				 "unknown owner", unparse(*player), rpass);
	}
	OBJECT(*room)->owner = *player;
}

void
fix_room(dbref obj)
{
	dbref i;

	i = ROOM(obj)->dropto;

	if (!valid_ref(i)) {
		SanFixed(obj, "Removing invalid drop-to from %s");
		ROOM(obj)->dropto = NOTHING;
	} else if (i >= 0 && !is_item(i) && OBJECT(i)->type != TYPE_ROOM) {
		SanFixed2(obj, i, "Removing drop-to on %s to %s");
		ROOM(obj)->dropto = NOTHING;
	}
}

void
fix_entity(dbref obj)
{
	dbref i;

	i = ENTITY(obj)->home;

	if (!valid_obj(i) || OBJECT(i)->type != TYPE_ROOM) {
		SanFixed2(obj, PLAYER_START, "Setting the home on %s to %s");
		ENTITY(obj)->home = PLAYER_START;
	}
}

void
fix_garbage(dbref obj)
{
	return;
}

void
find_misplaced_objects(void)
{
	dbref loop, player = NOTHING, room;

	for (loop = 0; loop < db_top; loop++) {
		if (OBJECT(loop)->type != TYPE_ROOM &&
			!is_item(loop) &&
			OBJECT(loop)->type != TYPE_ENTITY &&
			OBJECT(loop)->type != TYPE_GARBAGE) {
			SanFixedRef(loop, "Object #%d is of unknown type");
			sanity_violated = 1;
			continue;
		}
		if (!OBJECT(loop)->name || !(*OBJECT(loop)->name)) {
			switch (OBJECT(loop)->type) {
			case TYPE_GARBAGE:
				OBJECT(loop)->name = "<garbage>";
				break;
			case TYPE_ENTITY:
				{
					char name[PLAYER_NAME_LIMIT + 1] = "Unnamed";
					int temp = 0;

					while (lookup_player(name) != NOTHING && strlen(name) < PLAYER_NAME_LIMIT) {
						snprintf(name, sizeof(name), "Unnamed%d", ++temp);
					}
					OBJECT(loop)->name = alloc_string(name);
					add_player(loop);
				}
				break;
			default:
				OBJECT(loop)->name = alloc_string("Unnamed");
				}
			SanFixed(loop, "Gave a name to %s");
		}
		if (OBJECT(loop)->type != TYPE_GARBAGE) {
			if (!valid_obj(OBJECT(loop)->owner) || OBJECT(OBJECT(loop)->owner)->type != TYPE_ENTITY) {
				if (player == NOTHING) {
					create_lostandfound(&player, &room);
				}
				SanFixed2(loop, player, "Set owner of %s to %s");
				OBJECT(loop)->owner = player;
			}
			if (loop != GLOBAL_ENVIRONMENT && (!valid_obj(OBJECT(loop)->location) ||
											   OBJECT(OBJECT(loop)->location)->type == TYPE_GARBAGE ||
											   (OBJECT(loop)->type == TYPE_ENTITY &&
												OBJECT(OBJECT(loop)->location)->type == TYPE_ENTITY))) {
				if (OBJECT(loop)->type == TYPE_ENTITY) {
					if (valid_obj(OBJECT(loop)->location) && OBJECT(OBJECT(loop)->location)->type == TYPE_ENTITY) {
						dbref loop1;

						loop1 = OBJECT(loop)->location;
						if (OBJECT(loop1)->contents == loop)
							OBJECT(loop1)->contents = OBJECT(loop)->next;
						else
							for (loop1 = OBJECT(loop1)->contents;
								 loop1 != NOTHING; loop1 = OBJECT(loop1)->next) {
								if (OBJECT(loop1)->next == loop) {
									OBJECT(loop1)->next = OBJECT(loop)->next;
									break;
								}
							}
					}
					OBJECT(loop)->location = PLAYER_START;
				} else {
					if (player == NOTHING) {
						create_lostandfound(&player, &room);
					}
					OBJECT(loop)->location = room;
				}
				PUSH(loop, OBJECT(OBJECT(loop)->location)->contents);
				OBJECT(loop)->flags |= OF_SANEBIT;
				SanFixed2(loop, OBJECT(loop)->location, "Set location of %s to %s");
			}
		} else {
			if (OBJECT(loop)->owner != NOTHING) {
				SanFixedRef(loop, "Set owner of recycled object #%d to NOTHING");
				OBJECT(loop)->owner = NOTHING;
			}
			if (OBJECT(loop)->location != NOTHING) {
				SanFixedRef(loop, "Set location of recycled object #%d to NOTHING");
				OBJECT(loop)->location = NOTHING;
			}
		}
		switch (OBJECT(loop)->type) {
		case TYPE_ROOM:
			fix_room(loop);
			break;
		case TYPE_PLANT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
			break;
		case TYPE_ENTITY:
			fix_entity(loop);
			break;
		case TYPE_GARBAGE:
			fix_garbage(loop);
			break;
		}
	}
}

void
adopt_orphans(void)
{
	dbref loop;

	for (loop = 0; loop < db_top; loop++) {
		if (!(OBJECT(loop)->flags & OF_SANEBIT)) {
			switch (OBJECT(loop)->type) {
			case TYPE_ROOM:
			case TYPE_PLANT:
			case TYPE_CONSUMABLE:
			case TYPE_EQUIPMENT:
			case TYPE_THING:
			case TYPE_ENTITY:
				OBJECT(loop)->next = OBJECT(OBJECT(loop)->location)->contents;
				OBJECT(OBJECT(loop)->location)->contents = loop;
				SanFixed2(loop, OBJECT(loop)->location, "Orphaned object %s added to contents of %s");
				break;
			case TYPE_GARBAGE:
				OBJECT(loop)->next = recyclable;
				recyclable = loop;
				SanFixedRef(loop, "Litter object %d moved to recycle bin");
				break;
			default:
				sanity_violated = 1;
				break;
			}
		}
	}
}

void
clean_global_environment(void)
{
	if (OBJECT(GLOBAL_ENVIRONMENT)->next != NOTHING) {
		SanFixed(GLOBAL_ENVIRONMENT, "Removed the global environment %s from a chain");
		OBJECT(GLOBAL_ENVIRONMENT)->next = NOTHING;
	}
	if (OBJECT(GLOBAL_ENVIRONMENT)->location != NOTHING) {
		SanFixed2(GLOBAL_ENVIRONMENT, OBJECT(GLOBAL_ENVIRONMENT)->location,
				  "Removed the global environment %s from %s");
		OBJECT(GLOBAL_ENVIRONMENT)->location = NOTHING;
	}
}

void
sanfix(dbref player)
{
	dbref loop;

#ifdef GOD_PRIV
	if (player > NOTHING && !God(player)) {
#else
	if (player > NOTHING && !Wizard(player)) {
#endif

		notify(player, "Yeah right!  With a psyche like yours, you think "
			   "theres any hope of getting your sanity fixed?");
		return;
	}

	sanity_violated = 0;

	for (loop = 0; loop < db_top; loop++) {
		OBJECT(loop)->flags &= ~OF_SANEBIT;
	}
	OBJECT(GLOBAL_ENVIRONMENT)->flags |= OF_SANEBIT;

	hacksaw_bad_chains();
	find_misplaced_objects();
	adopt_orphans();
	clean_global_environment();

	for (loop = 0; loop < db_top; loop++) {
		OBJECT(loop)->flags &= ~OF_SANEBIT;
	}

	if (player > NOTHING) {
		if (!sanity_violated) {
			notify(player, "Database repair complete, please re-run"
			       " @sanity.  For details of repairs, check logs/sanfixed.");
		} else {
			notify(player, "Database repair complete, however the "
			       "database is still corrupt.  Please re-run @sanity.");
		}
	} else {
		fprintf(stderr, "Database repair complete, ");
		if (!sanity_violated)
			fprintf(stderr, "please re-run sanity check.\n");
		else
			fprintf(stderr, "however the database is still corrupt.\n"
					"Please re-run sanity check for details and fix it by hand.\n");
		fprintf(stderr, "For details of repairs made, check logs/sanfixed.\n");
	}
	if (sanity_violated)
		warn("WARNING: The database is still corrupted, please repair by hand");
}



char cbuf[1000];
char buf2[1000];

void
sanechange(dbref player, const char *command)
{
	dbref d, v;
	char field[50];
	char which[1000];
	char value[1000];
	int *ip;
	int results;

	if (player > NOTHING) {

#ifdef GOD_PRIV
		if (!God(player)) {
			notify(player, "Only GOD may alter the basic structure of the universe!");
#else
		if (!Wizard(player)) {
			notify(player, "Only Wizards may alter the basic structure of the universe!");
#endif
			return;
		}
		results = sscanf(command, "%s %s %s", which, field, value);
		sscanf(which, "#%d", &d);
		sscanf(value, "#%d", &v);
	} else {
		results = sscanf(command, "%s %s %s", which, field, value);
		sscanf(which, "%d", &d);
		sscanf(value, "%d", &v);
	}

	if (results != 3) {
		d = v = 0;
		strlcpy(field, "help", sizeof(field));
	}

	*buf2 = 0;

	if (!valid_ref(d) || d < 0) {
		SanPrint(player, "## %d is an invalid dbref.", d);
		return;
	}

	if (!strcmp(field, "next")) {
		strlcpy(buf2, unparse(OBJECT(d)->next), sizeof(buf2));
		OBJECT(d)->next = v;
		SanPrint(player, "## Setting #%d's next field to %s", d, unparse(v));

	} else if (!strcmp(field, "contents")) {
		strlcpy(buf2, unparse(OBJECT(d)->contents), sizeof(buf2));
		OBJECT(d)->contents = v;
		SanPrint(player, "## Setting #%d's Contents list start to %s", d, unparse(v));

	} else if (!strcmp(field, "location")) {
		strlcpy(buf2, unparse(OBJECT(d)->location), sizeof(buf2));
		OBJECT(d)->location = v;
		SanPrint(player, "## Setting #%d's location to %s", d, unparse(v));

	} else if (!strcmp(field, "owner")) {
		strlcpy(buf2, unparse(OBJECT(d)->owner), sizeof(buf2));
		OBJECT(d)->owner = v;
		SanPrint(player, "## Setting #%d's owner to %s", d, unparse(v));

	} else if (!strcmp(field, "home")) {
		switch (OBJECT(d)->type) {
		case TYPE_ENTITY:
			ip = &(ENTITY(d)->home);
			break;

		default:
			printf("%s has no home to set.\n", unparse(d));
			return;
		}

		strlcpy(buf2, unparse(*ip), sizeof(buf2));
		*ip = v;
		printf("Setting home to: %s\n", unparse(v));

	} else {
		if (player > NOTHING) {
			notify(player, "@sanchange <dbref> <field> <object>");
		} else {
			SanPrint(player, "change command help:");
			SanPrint(player, "c <dbref> <field> <object>");
		}
		SanPrint(player, "Fields are:     contents    Start of Contents list.");
		SanPrint(player, "                next        Next object in list.");
		SanPrint(player, "                location    Object's Location.");
		SanPrint(player, "                home        Object's Home.");
		SanPrint(player, "                owner       Object's Owner.");
		return;
	}

	if (*buf2) {
		SanPrint(player, "## Old value was %s", buf2);
	}
}

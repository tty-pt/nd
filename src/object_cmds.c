#include "uapi/io.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ndc.h>

#include "params.h"
#include "player.h"
#include "uapi/entity.h"
#include "uapi/match.h"

int ok_name(const char *name);

void
do_clone(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	char *name = argv[1];

	if (!(ent_get(player_ref).flags & (EF_WIZARD | EF_BUILDER)) || !*name || !ok_name(name)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}
	
	if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ thing;
	lhash_get(obj_hd, &thing, thing_ref);

	if(!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);
	OBJ clone;
	unsigned clone_ref = object_new(&clone);

	strcpy(clone.name, thing.name);
	clone.location = player_ref;
	clone.owner = player.owner;
	clone.value = thing.value;

	switch (thing.type) {
		case TYPE_ROOM:
			{
				ROO *rclone = &clone.sp.room;
				rclone->exits = rclone->doors = 0;
			}
			break;
		case TYPE_ENTITY:
			{
				ENT eclone = ent_get(clone_ref);
				eclone.home = ent_get(thing_ref).home;
				ent_set(clone_ref, &eclone);
			}
			break;
	}
	clone.type = thing.type;

	lhash_put(obj_hd, clone_ref, &clone);
	object_move(clone_ref, player_ref);
}

void
do_create(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	unsigned pflags = ent_get(player_ref).flags;
	char *name = argv[1];
	int cost = 30;

	if (!(pflags & (EF_WIZARD | EF_BUILDER)) || *name == '\0' || !ok_name(name)) {
		nd_writef(player_ref, "You can't do that.\n");
		return;
	}

	OBJ player;
	lhash_get(obj_hd, &player, player_ref);

	OBJ thing;
	thing_ref = object_new(&thing);

	strcpy(thing.name, name);
	thing.location = player_ref;
	thing.owner = player.owner;
	thing.value = cost;
	thing.type = TYPE_THING;

	lhash_put(obj_hd, thing_ref, &thing);
	object_move(thing_ref, player_ref);
}
void
do_name(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	char *newname = argv[2];
	unsigned thing_ref = ematch_all(player_ref, name);

	if (thing_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player_ref, thing_ref) || !*newname || !ok_name(newname)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	OBJ thing;
	lhash_get(obj_hd, &thing, thing_ref);
	strcpy(thing.name, newname);
	lhash_put(obj_hd, thing_ref, &thing);
}

void
do_chown(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), owner_ref, thing_ref;
	int wizard = ent_get(player_ref).flags & EF_WIZARD;
	char *name = argv[1];
	char *newowner = argv[2];

	if (!*name || (thing_ref = ematch_all(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ player, thing;
	lhash_get(obj_hd, &player, player_ref);

	owner_ref = *newowner && strcmp(newowner, "me") ? player_get(newowner) : player.owner;
	if (owner_ref == NOTHING)
		goto error;

	lhash_get(obj_hd, &thing, thing_ref);

	if (thing.type == TYPE_ENTITY ||
			(!wizard && ((thing.type == TYPE_ROOM && player.location != thing_ref)
				     || (thing.type != TYPE_ROOM && thing.location != player_ref ))))
		goto error;

	thing.owner = owner_ref;
	lhash_put(obj_hd, thing_ref, &thing);
	return;

error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

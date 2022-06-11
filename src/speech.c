/* $Header$ */


#include "copyright.h"
#include "config.h"

#include "mdb.h"
#include "interface.h"
#include "match.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "externs.h"
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

/* Commands which involve speaking */

void
do_say(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *message = cmd->argv[1];
	char buf[BUFFER_LEN];

	notifyf(player, "You say, \"%s\"", message);
	onotifyf(player, buf, "%s says, \"%s\"", player->name, message);
}

void
do_pose(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *message = cmd->argv[1];

	anotifyf(player->location, "%s %s", player->name, message);
}

void
do_wall(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *message = cmd->argv[1];
	dbref i;
	char buf[BUFFER_LEN];

	CBUG(player->type != TYPE_ENTITY);

	ENT *eplayer = &player->sp.entity;

	if (!(eplayer->flags & EF_WIZARD)) {
		notify(player, "But what do you want to do with the wall?");
		return;
	}

	warn("WALL from %s(%d): %s", player->name, object_ref(player), message);
	snprintf(buf, sizeof(buf), "%s shouts, \"%s\"", player->name, message);
	for (i = 0; i < db_top; i++) {
		OBJ *oi = object_get(i);
		if (oi->type == TYPE_ENTITY)
			notify(oi, buf);
	}
}

static int
blank(const char *s)
{
	while (*s && isspace(*s))
		s++;

	return !(*s);
}

/* doesn't really belong here, but I couldn't figure out where else */
void
do_page(command_t *cmd)
{
	OBJ *player = object_get(cmd->player);
	const char *arg1 = cmd->argv[1];
	const char *arg2 = cmd->argv[2];
	char buf[BUFFER_LEN];
	OBJ *target;

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
		return;
	}

	target = lookup_player(arg1);

	if (!target) {
		notify(player, "I don't recognize that name.");
		return;
	}

	if (blank(arg2))
		snprintf(buf, sizeof(buf), "You sense that %s is looking for you in %s.",
				player->name, player->location->name);
	else
		snprintf(buf, sizeof(buf), "%s pages from %s: \"%s\"", player->name,
				player->location->name, arg2);
	if (notify(target, buf))
		notify(player, "Your message has been sent.");
	else {
		snprintf(buf, sizeof(buf), "%s is not connected.", target->name);
		notify(player, buf);
	}
}

void
notify_wts(OBJ *who, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s%s.", a, buf);
	onotifyf(who, "%s %s%s.", who->name, b, buf);
	va_end(args);
}

void
notify_wts_to(OBJ *who, OBJ *tar, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s %s%s.", a, tar->name, buf);
	onotifyf(who, "%s %s %s%s.", who->name, b, tar->name, buf);
	va_end(args);
}

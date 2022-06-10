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
	dbref player = cmd->player;
	const char *message = cmd->argv[1];
	char buf[BUFFER_LEN];

	notifyf(player, "You say, \"%s\"", message);
	onotifyf(player, buf, "%s says, \"%s\"", OBJECT(player)->name, message);
}

void
do_pose(command_t *cmd)
{
	dbref player = cmd->player;
	const char *message = cmd->argv[1];

	anotifyf(getloc(player), "%s %s", OBJECT(player)->name, message);
}

void
do_wall(command_t *cmd)
{
	dbref player = cmd->player;
	const char *message = cmd->argv[1];
	dbref i;
	char buf[BUFFER_LEN];

	CBUG(OBJECT(player)->type != TYPE_ENTITY);

	if (!(ENTITY(player)->flags & EF_WIZARD)) {
		notify(player, "But what do you want to do with the wall?");
		return;
	}

	warn("WALL from %s(%d): %s", OBJECT(player)->name, player, message);
	snprintf(buf, sizeof(buf), "%s shouts, \"%s\"", OBJECT(player)->name, message);
	for (i = 0; i < db_top; i++)
		if (OBJECT(i)->type == TYPE_ENTITY)
			notify(i, buf);
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
	dbref player = cmd->player;
	const char *arg1 = cmd->argv[1];
	const char *arg2 = cmd->argv[2];
	char buf[BUFFER_LEN];
	dbref target;

	if (!payfor(player, LOOKUP_COST)) {
		notifyf(player, "You don't have enough %s.", PENNIES);
		return;
	}
	if ((target = lookup_player(arg1)) == NOTHING) {
		notify(player, "I don't recognize that name.");
		return;
	}
	if (ROOM(target)->flags & RF_HAVEN) {
		notify(player, "That player does not wish to be disturbed.");
		return;
	}
	if (blank(arg2))
		snprintf(buf, sizeof(buf), "You sense that %s is looking for you in %s.",
				OBJECT(player)->name, OBJECT(OBJECT(player)->location)->name);
	else
		snprintf(buf, sizeof(buf), "%s pages from %s: \"%s\"", OBJECT(player)->name,
				OBJECT(OBJECT(player)->location)->name, arg2);
	if (notify(target, buf))
		notify(player, "Your message has been sent.");
	else {
		snprintf(buf, sizeof(buf), "%s is not connected.", OBJECT(target)->name);
		notify(player, buf);
	}
}

void
notify_wts(dbref who, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s%s.", a, buf);
	onotifyf(who, "%s %s%s.", OBJECT(who)->name, b, buf);
	va_end(args);
}

void
notify_wts_to(dbref who, dbref tar, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s %s%s.", a, OBJECT(tar)->name, buf);
	onotifyf(who, "%s %s %s%s.", OBJECT(who)->name, b, OBJECT(tar)->name, buf);
	va_end(args);
}

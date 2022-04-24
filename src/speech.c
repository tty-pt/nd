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
	dbref loc;
	char buf[BUFFER_LEN];

	if ((loc = getloc(player)) == NOTHING)
		return;

	/* notify everybody */
	snprintf(buf, sizeof(buf), "You say, \"%s\"", message);
	notify(player, buf);
	snprintf(buf, sizeof(buf), "%s says, \"%s\"", NAME(player), message);
	notify_except(db[loc].contents, player, buf, player);
}

void
do_pose(command_t *cmd)
{
	dbref player = cmd->player;
	const char *message = cmd->argv[1];
	dbref loc;
	char buf[BUFFER_LEN];

	if ((loc = getloc(player)) == NOTHING)
		return;

	/* notify everybody */
	snprintf(buf, sizeof(buf), "%s %s", NAME(player), message);
	notify_except(db[loc].contents, NOTHING, buf, player);
}

void
do_wall(command_t *cmd)
{
	dbref player = cmd->player;
	const char *message = cmd->argv[1];
	dbref i;
	char buf[BUFFER_LEN];

	if (Wizard(player) && Typeof(player) == TYPE_PLAYER) {
		warn("WALL from %s(%d): %s", NAME(player), player, message);
		snprintf(buf, sizeof(buf), "%s shouts, \"%s\"", NAME(player), message);
		for (i = 0; i < db_top; i++) {
			if (Typeof(i) == TYPE_PLAYER) {
				notify(i, buf);
			}
		}
	} else {
		notify(player, "But what do you want to do with the wall?");
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
	if (FLAGS(target) & HAVEN) {
		notify(player, "That player does not wish to be disturbed.");
		return;
	}
	if (blank(arg2))
		snprintf(buf, sizeof(buf), "You sense that %s is looking for you in %s.",
				NAME(player), NAME(db[player].location));
	else
		snprintf(buf, sizeof(buf), "%s pages from %s: \"%s\"", NAME(player),
				NAME(db[player].location), arg2);
	if (notify(target, buf))
		notify(player, "Your message has been sent.");
	else {
		snprintf(buf, sizeof(buf), "%s is not connected.", NAME(target));
		notify(player, buf);
	}
}

void
notify_except(dbref first, dbref exception, const char *msg, dbref who)
{
	DOLIST(first, first) {
		if (Typeof(first) == TYPE_PLAYER && first != exception)
			notify(first, msg);
	}
}

void
notify_except_fmt(dbref first, dbref exception, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notify_except(first, exception, buf, exception);
	va_end(args);
}

void
notify_wts(dbref who, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s%s.", a, buf);
	ONOTIFYF(who, "%s %s%s.", NAME(who), b, buf);
	va_end(args);
}

void
notify_wts_to(dbref who, dbref tar, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	notifyf(who, "You %s %s%s.", a, NAME(tar), buf);
	ONOTIFYF(who, "%s %s %s%s.", NAME(who), b, NAME(tar), buf);
	va_end(args);
}

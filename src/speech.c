#include "io.h"

#include "entity.h"
#include "config.h"

#include "match.h"
#include "params.h"
#include "defaults.h"
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include "player.h"
#include "debug.h"

/* Commands which involve speaking */
char *
argscat(int argc, char *argv[]) {
	static char message[BUFFER_LEN];
	int rem = sizeof(message);
	memset(message, '\0', sizeof(message));
	
	for (int i = 1; i < argc; i++) {
		int ret = snprintf(message + sizeof(message) - rem, rem, " %s", argv[i]);
		if (ret <= 0)
			return message;
		rem -= ret;
	}

	return message;
}

void
do_say(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	char *message = argscat(argc, argv);
	nd_writef(player_ref, "You say:%s.\n", message);
	nd_owritef(player_ref, "%s says:%s\n", obj_get(player_ref).name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	char *message = argscat(argc, argv);

	nd_writef(player_ref, "You %s\n", message);
	nd_owritef(player_ref, "%s%s\n", obj_get(player_ref).name, message);
}

void
do_wall(int fd, int argc, char *argv[])
{
	dbref player_ref = FD_PLAYER(fd);
	dbref oi_ref;
	OBJ player = obj_get(player_ref);
	CBUG(player.type != TYPE_ENTITY);
	char buf[BUFFER_LEN];
	char *message = argscat(argc, argv);

	if (!(ent_get(player_ref).flags & EF_WIZARD)) {
		nd_writef(player_ref, "But what do you want to do with the wall?\n");
		return;
	}

	snprintf(buf, sizeof(buf), "%s shouts: %s", player.name, message);
	FOR_ALL(oi_ref) if (obj_get(oi_ref).type == TYPE_ENTITY)
		nd_writef(oi_ref, buf);
}

void
notify_wts(dbref who_ref, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	nd_writef(who_ref, "You %s%s.\n", a, buf);
	nd_owritef(who_ref, "%s %s%s.\n", obj_get(who_ref).name, b, buf);
}

void
notify_wts_to(dbref who_ref, dbref tar_ref, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	OBJ who = obj_get(who_ref),
	    tar = obj_get(tar_ref);
	nd_writef(who_ref, "You %s %s%s.\n", a, tar.name, buf);
	nd_owritef(who_ref, "%s %s %s%s.\n", who.name, b, tar.name, buf);
}

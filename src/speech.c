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
	OBJ *player = FD_PLAYER(fd);
	char *message = argscat(argc, argv);
	nd_writef(player, "You say:%s.\n", message);
	nd_owritef(player, "%s says:%s\n", player->name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char *message = argscat(argc, argv);
	/* ENT *eplayer = &player->sp.entity; */

	nd_owritef(player, "%s%s\n", player->name, message);
	/* if (eplayer->gpt) { */
	/* 	char contents[BUFFER_LEN]; */
	/* 	sprintf(contents, "Describe %s %s.\n\n", player->name, message); */
	/* 	entity_gpt(player, 1, contents); */
	/* } else */
	/* 	anotifyf(player->location, "%s %s", player->name, message); */
}

void
do_wall(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd), *oi;
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;
	char buf[BUFFER_LEN];
	char *message = argscat(argc, argv);

	if (!(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "But what do you want to do with the wall?\n");
		return;
	}

	warn("WALL from %s(%d): %s", player->name, object_ref(player), message);
	snprintf(buf, sizeof(buf), "%s shouts: %s", player->name, message);
	FOR_ALL(oi) if (oi->type == TYPE_ENTITY)
		nd_writef(oi, buf);
}

void
notify_wts(OBJ *who, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	nd_writef(who, "You %s%s.\n", a, buf);
	nd_owritef(who, "%s %s%s.\n", who->name, b, buf);
}

void
notify_wts_to(OBJ *who, OBJ *tar, char const *a, char const *b, char *format, ...)
{
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	nd_writef(who, "You %s %s%s.\n", a, tar->name, buf);
	nd_owritef(who, "%s %s %s%s.\n", who->name, b, tar->name, buf);
}

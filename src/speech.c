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

void
do_say(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	char message[BUFSIZ];
	*message = '\0';

	for (int i = 1; i < argc; i++) {
		strcat(message, argv[i]);
		if (i + 1 < argc)
			strcat(message, " ");
	}


	nd_writef(player, "You say, \"%s\".\n", message);
	nd_rwritef(player->location, player, "%s says, \"%s\"\n", player->name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	/* ENT *eplayer = &player->sp.entity; */
	char message[BUFSIZ];
	*message = '\0';

	for (int i = 1; i < argc; i++) {
		strcat(message, argv[i]);
		if (i + 1 < argc)
			strcat(message, " ");
	}

	nd_rwritef(player->location, player, "%s %s\n", player->name, message);
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
	char message[BUFSIZ];
	*message = '\0';

	for (int i = 1; i < argc; i++) {
		strcat(message, argv[i]);
		if (i + 1 < argc)
			strcat(message, " ");
	}

	if (!(eplayer->flags & EF_WIZARD)) {
		nd_writef(player, "But what do you want to do with the wall?\n");
		return;
	}

	warn("WALL from %s(%d): %s", player->name, object_ref(player), message);
	snprintf(buf, sizeof(buf), "%s shouts, \"%s\"", player->name, message);
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
	nd_rwritef(who->location, who, "%s %s%s.\n", who->name, b, buf);
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
	nd_rwritef(who->location, who, "%s %s %s%s.\n", who->name, b, tar->name, buf);
}

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
	const char *message = argv[1];

	ndc_writef(fd, "You say, \"%s\".", message);
	nd_rwritef(player->location, player, "%s says, \"%s\"", player->name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	OBJ *player = FD_PLAYER(fd);
	/* ENT *eplayer = &player->sp.entity; */
	const char *message = argv[1];

	nd_rwritef(player->location, player, "%s %s", player->name, message);
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
	const char *message = argv[1];
	char buf[BUFFER_LEN];


	if (!(eplayer->flags & EF_WIZARD)) {
		ndc_writef(fd, "But what do you want to do with the wall?\n");
		return;
	}

	warn("WALL from %s(%d): %s", player->name, object_ref(player), message);
	snprintf(buf, sizeof(buf), "%s shouts, \"%s\"", player->name, message);
	FOR_ALL(oi) {
		if (oi->type == TYPE_ENTITY)
			ndc_writef(oi->sp.entity.fd, buf);
	}
}

void
notify_wts(OBJ *who, char const *a, char const *b, char *format, ...)
{
	ENT *ewho = &who->sp.entity;
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	ndc_writef(ewho->fd, "You %s%s.\n", a, buf);
	nd_rwritef(who->location, who, "%s %s%s.", who->name, b, buf);
	va_end(args);
}

void
notify_wts_to(OBJ *who, OBJ *tar, char const *a, char const *b, char *format, ...)
{
	ENT *ewho = &who->sp.entity;
	va_list args;
	char buf[BUFFER_LEN];
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	ndc_writef(ewho->fd, "You %s %s%s.\n", a, tar->name, buf);
	nd_rwritef(who->location, who, "%s %s %s%s.", who->name, b, tar->name, buf);
	va_end(args);
}

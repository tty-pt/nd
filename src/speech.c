#include "uapi/io.h"

#include <stdio.h>

#include "config.h"
#include "player.h"
#include "uapi/entity.h"
#include "uapi/match.h"

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
	unsigned player_ref = fd_player(fd);
	char *message = argscat(argc, argv);
	OBJ player;

	nd_writef(player_ref, "You say:%s.\n", message);
	lhash_get(obj_hd, &player, player_ref);
	nd_owritef(player_ref, "%s says:%s\n", player.name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *message = argscat(argc, argv);
	OBJ player;

	nd_writef(player_ref, "You %s\n", message);
	lhash_get(obj_hd, &player, player_ref);
	nd_owritef(player_ref, "%s%s\n", player.name, message);
}

void
do_wall(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	unsigned oi_ref;
	OBJ player;
	char buf[BUFFER_LEN];
	char *message = argscat(argc, argv);

	if (!(ent_get(player_ref).flags & EF_WIZARD)) {
		nd_writef(player_ref, "But what do you want to do with the wall?\n");
		return;
	}

	lhash_get(obj_hd, &player, player_ref);
	snprintf(buf, sizeof(buf), "%s shouts: %s", player.name, message);
	struct hash_cursor c = lhash_iter(obj_hd);
	OBJ oi;
	while (lhash_next(&oi_ref, &oi, &c))
		nd_writef(oi_ref, buf);
}

void
dnotify_wts(unsigned who_ref, char const *a, char const *b, char *format, va_list args)
{
	char buf[BUFFER_LEN];
	OBJ who;
	vsnprintf(buf, sizeof(buf), format, args);
	nd_writef(who_ref, "You %s%s.\n", a, buf);
	lhash_get(obj_hd, &who, who_ref);
	nd_owritef(who_ref, "%s %s%s.\n", who.name, b, buf);
}

void
dnotify_wts_to(unsigned who_ref, unsigned tar_ref, char const *a, char const *b, char *format, va_list args)
{
	char buf[BUFFER_LEN];
	vsnprintf(buf, sizeof(buf), format, args);
	OBJ who, tar;
	lhash_get(obj_hd, &who, who_ref);
	lhash_get(obj_hd, &tar, tar_ref);
	nd_writef(who_ref, "You %s %s%s.\n", a, tar.name, buf);
	nd_owritef(who_ref, "%s %s %s%s.\n", who.name, b, tar.name, buf);
}

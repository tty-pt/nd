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
	qmap_get(obj_hd, &player, &player_ref);
	nd_owritef(player_ref, "%s says:%s\n", player.name, message);
}

void
do_pose(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *message = argscat(argc, argv);
	OBJ player;

	nd_writef(player_ref, "You %s\n", message);
	qmap_get(obj_hd, &player, &player_ref);
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
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	qmap_get(obj_hd, &player, &player_ref);
	snprintf(buf, sizeof(buf), "%s shouts: %s", player.name, message);
	unsigned c = qmap_iter(obj_hd, NULL);
	OBJ oi;
	while (qmap_next(&oi_ref, &oi, c))
		nd_writef(oi_ref, buf);
}

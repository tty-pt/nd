#include "uapi/io.h"

#include <stdlib.h>
#include <qdb.h>

#include "uapi/entity.h"
#include "uapi/match.h"

void
do_select(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	const char *n_s = argv[1];
	unsigned n = strtoul(n_s, NULL, 0);
	ENT eplayer = ent_get(player_ref);
	eplayer.select = n;
	ent_set(player_ref, &eplayer);
}

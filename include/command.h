#ifndef COMMAND_H
#define COMMAND_H

#include "object.h"

typedef struct {
	OBJ *player;
	int fd, argc;
	char *argv[8];
} command_t; /* FIXME commandi_t */

typedef void core_command_cb_t(command_t *);

enum command_flags {
	CF_NOAUTH = 1,
	/* CF_EOL = 2, */
        CF_NOARGS = 4,
};

typedef struct {
	char *name;
	/* size_t nargs; */
	core_command_cb_t *cb;
	int flags;
} core_command_t;

extern core_command_t cmds[];

#endif

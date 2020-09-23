#ifndef COMMAND_H
#define COMMAND_H

typedef int ref_t;

typedef struct {
	ref_t player;
	int fd, argc;
	char *argv[8];
} command_t; /* FIXME commandi_t */

typedef void core_command_cb_t(command_t *);

typedef struct {
	char *name;
	/* size_t nargs; */
	core_command_cb_t *cb;
} core_command_t;

extern core_command_t cmds[];

command_t command_null(command_t *cmd);
command_t command_new_null(int descr, ref_t player);
void command_debug(command_t *cmd, char *label);
core_command_t * command_match(command_t *cmd);

#endif

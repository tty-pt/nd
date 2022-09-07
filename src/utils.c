#include "utils.h"
#include <string.h>
#include <ctype.h>
/* #include <unistd.h> */
/* #include <stdlib.h> */
#include "params.h"
/* #include "debug.h" */
/* #include "config.h" */

struct popen2 {
	int in, out, pid;
};

int
ok_name(const char *name)
{
	return (name
			&& *name
			&& *name != NUMBER_TOKEN
			&& !strchr(name, ARG_DELIMITER)
			&& !strchr(name, '\r')
			&& !strchr(name, ESCAPE_CHAR)
			&& strcmp(name, "me")
			&& strcmp(name, "home")
			&& strcmp(name, "here"));
}

int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
		string++, prefix++;
	return *prefix == '\0';
}

/* static int */
/* popen2(struct popen2 *child, const char *cmdline) */
/* { */
/* 	pid_t p; */
/* 	int pipe_stdin[2], pipe_stdout[2]; */

/* 	if (pipe(pipe_stdin) || pipe(pipe_stdout)) */
/* 		return -1; */

/* 	p = fork(); */
/* 	if (p < 0) */
/* 		return p; */

/* 	if(p == 0) { /1* child *1/ */
/* 		close(pipe_stdin[1]); */
/* 		dup2(pipe_stdin[0], 0); */
/* 		close(pipe_stdout[0]); */
/* 		dup2(pipe_stdout[1], 1); */
/* 		execl("/bin/sh", "sh", "-c", cmdline, NULL); */
/* 		perror("execl"); exit(99); */
/* 	} */

/* 	child->pid = p; */
/* 	child->in = pipe_stdin[1]; */
/* 	child->out = pipe_stdout[0]; */
/* 	close(pipe_stdin[0]); */
/* 	close(pipe_stdout[1]); */
/* 	return 0; */ 
/* } */

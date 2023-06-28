#include "utils.h"
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "params.h"
/* #include "debug.h" */
/* #include "config.h" */

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

int
popen2(struct popen2 *child, const char *cmdline)
{
	pid_t p;
	int pipe_stdin[2], pipe_stdout[2];

	if (pipe(pipe_stdin) || pipe(pipe_stdout))
		return -1;

	p = fork();
	if (p < 0)
		return p;

	if(p == 0) { /* child */
		close(pipe_stdin[1]);
		dup2(pipe_stdin[0], 0);
		close(pipe_stdout[0]);
		dup2(pipe_stdout[1], 1);
		execl("/bin/sh", "sh", "-c", cmdline, NULL);
		perror("execl"); exit(99);
	}

	child->pid = p;
	child->in = pipe_stdin[1];
	child->out = pipe_stdout[0];
	close(pipe_stdin[0]);
	close(pipe_stdout[1]);
	return 0;
}

#if 0
char *
gpt(char *prompt)
{
	static char buf[BUFFER_LEN];
	struct popen2 child;

	CBUG(popen2(&child, "/gpt3.sh"));
	write(child.in, prompt, strlen(prompt));
	close(child.in);
	memset(buf, 0, sizeof(buf));
	read(child.out, buf, sizeof(buf));
	close(child.out);
	kill(child.pid, 0);
	return buf;
}
#endif

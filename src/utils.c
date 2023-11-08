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

#include "utils.h"
#include <string.h>
#include <ctype.h>
#include "params.h"

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

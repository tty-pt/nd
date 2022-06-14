/* $Header$ */


#include "copyright.h"
#include "config.h"
#include "mdb.h"
#include "params.h"
#include "interface.h"

/* String utilities */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "externs.h"

int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
		string++, prefix++;
	return *prefix == '\0';
}

/* $Header$ */

#include "copyright.h"
#include "entity.h"
#include "config.h"

/* Predicates for testing various conditions */

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "externs.h"
#include "mob.h"

/* Removes 'cost' value from 'who', and returns 1 if the act has been
 * paid for, else returns 0. */
int
payfor(OBJ *who, int cost)
{
	who = who->owner;

	ENT *ewho = &who->sp.entity;

	if (ewho->flags & EF_WIZARD)
		return 1;

	if (who->value >= cost) {
		who->value -= cost;
		return 1;
	} else {
		return 0;
	}
}

int
ok_ascii_any(const char *name)
{
	const unsigned char *scan;
	for( scan=(const unsigned char*) name; *scan; ++scan ) {
		if( *scan>127 )
			return 0;
	}
	return 1;
}

int
ok_name(const char *name)
{
	return (name
			&& *name
			&& *name != LOOKUP_TOKEN
			&& *name != NUMBER_TOKEN
			&& !strchr(name, ARG_DELIMITER)
			&& !strchr(name, '\r')
			&& !strchr(name, ESCAPE_CHAR)
			&& strcmp(name, "me")
			&& strcmp(name, "home")
			&& strcmp(name, "here"));
}

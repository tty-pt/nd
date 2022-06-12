/* $Header$ */

#include "copyright.h"
#include "entity.h"
#include "config.h"

/* Predicates for testing various conditions */

#include <ctype.h>
#include <string.h>

#include "mdb.h"
#include "props.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "externs.h"
#include "geography.h"
#include "mob.h"

/*
 * Revision 1.2 -- SECURE_TELEPORT
 * you can only jump with an action from rooms that you own
 * or that are jump_ok, and you cannot jump to players that are !jump_ok.
 */

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
word_start(const char *str, const char let)
{
	int chk;

	for (chk = 1; *str; str++) {
		if (chk && (*str == let))
			return 1;
		chk = (*str == ' ');
	}
	return 0;
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
			&& *name != REGISTERED_TOKEN
			&& *name != NUMBER_TOKEN
			&& !strchr(name, ARG_DELIMITER)
			&& !strchr(name, AND_TOKEN)
			&& !strchr(name, OR_TOKEN)
			&& !strchr(name, '\r')
			&& !strchr(name, ESCAPE_CHAR)
			&& !word_start(name, NOT_TOKEN)
			&& strcmp(name, "me")
			&& strcmp(name, "home")
			&& strcmp(name, "here")
			&& (
				!*RESERVED_NAMES ||
				!equalstr((char*)RESERVED_NAMES, (char*)name)
			));
}

static const char *predicates_c_version = "$RCSfile$ $Revision: 1.20 $";
const char *get_predicates_c_version(void) { return predicates_c_version; }

#ifndef _CONFIG_H
#define _CONFIG_H

#define STD_DB "std.db"
#define GEO_DB "geo.db"

#define CONFIG_SECURE
#define TINYPORT 4201			/* Port that players connect to */

#define HUMAN_BEING 1

#define NO_INFO_MSG "That file does not exist.  Type 'info' to get a list of the info files available."

/************************************************************************/
/************************************************************************/
/*    FOR INTERNAL USE ONLY.  DON'T CHANGE ANYTHING PAST THIS POINT.    */
/************************************************************************/
/************************************************************************/

/*
 * Include all the good standard headers here.
 */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define SRANDOM(seed)	srandom((seed))
#define RANDOM()	random()

#include <sys/time.h>
#include <time.h>

#endif /* _CONFIG_H */

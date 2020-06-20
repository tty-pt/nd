/* do_parse_mesg()  Parses a message string, expanding {prop}s, {list}s, etc.
 * Written 4/93 - 5/93 by Foxen
 *
 * Args:
 *   int descr             Descriptor that triggered this.
 *   dbref player          Player triggering this.
 *   dbref what            Object that mesg is on.
 *   dbref perms           Object permissions are checked from.
 *   const char *inbuf     A pointer to the input raw mesg string.
 *   char *abuf            Argument string for {&how}.
 *   char *outbuf          Buffer to return results in.
 *   int mesgtyp           1 for personal messages, 0 for omessages.
 *
 *   extern char *match_args     Arg string for {&args}
 *   extern char *match_cmdname  Arg string for {&cmd}
 *
 * Returns a pointer to output string.  Usually in outbuf.
 */


#include "config.h"
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "params.h"
#include "db.h"
#include "defaults.h"

#include "mpi.h"
#include "externs.h"
#include "props.h"
#include "match.h"
#include "interp.h"
#include "interface.h"
#include "msgparse.h"

/******** MPI Variable handling ********/

/***** MPI function handling *****/

/*** End of MFUNs. ***/

#ifndef MSGHASHSIZE
#define MSGHASHSIZE 256
#endif

char *
do_parse_mesg(command_t *cmd, dbref what, const char *inbuf, const char *abuf, char *outbuf, int outbuflen, int mesgtyp)
{
	strlcpy(outbuf, inbuf, outbuflen);
	return outbuf;
}

char *
do_parse_prop(command_t *cmd, dbref what, const char *propname, const char *abuf, char *outbuf, int outbuflen, int mesgtyp)
{
	const char* propval = get_property_class(what, propname);
	if (!propval)
		return NULL;
	return do_parse_mesg(cmd, what, propval, abuf, outbuf, outbuflen, mesgtyp);
}

static const char *msgparse_c_version = "$RCSfile$ $Revision: 1.30 $";
const char *get_msgparse_c_version(void) { return msgparse_c_version; }

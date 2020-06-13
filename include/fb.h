#ifndef _FB_H
#define _FB_H

/* We need struct inst before anything */
#include "db.h"
#include "array.h"
#include "autoconf.h"
#include "config.h"
#include "copyright.h"
#ifdef MALLOC_PROFILING
# include "crt_malloc.h"
#endif
#include "dbsearch.h"
#include "defaults.h"
#include "externs.h"
#include "inst.h"
#include "interface.h"
#include "interp.h"
#include "match.h"
#include "mcp.h"
#include "mcppkg.h"
#include "mfun.h"
#include "mfunlist.h"
#include "mpi.h"
#include "msgparse.h"
#include "mufevent.h"
#include "p_array.h"
#include "p_db.h"
#include "p_error.h"
#include "p_stack.h"
#include "p_strings.h"
#include "p_regex.h"
#include "params.h"
#include "patchlevel.h"
#include "props.h"
#include "smatch.h"
#include "fbstrings.h"
#include "speech.h"
#include "version.h"

#endif /* _FB_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef fbh_version
#define fbh_version
const char *fb_h_version = "$RCSfile$ $Revision: 1.8 $";
#endif
#else
extern const char *fb_h_version;
#endif


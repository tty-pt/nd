#ifndef _CONFIG_H
#define _CONFIG_H

#define STD_DB "std.db"
#define STD_DB_OK "std.db.ok"
#define GEO_DB "geo.db"

#ifdef __OpenBSD__
#define CONFIG_SECURE
#endif
#define TINYPORT 4201			/* Port that players connect to */

#define HUMAN_BEING 1
#define BUFFER_LEN 8192
#define BIGBUFSIZ 32768
#define SUPERBIGSIZ 70000 * 8192

#endif

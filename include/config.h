#ifndef _CONFIG_H
#define _CONFIG_H

#define STD_DB "/var/nd/std.db"
#define STD_DB_OK "/var/nd/std.db.ok"
#define GEO_DB "/var/nd/geo.db"

#define TINYPORT 4201
#define HUMAN_BEING 1
#define BUFFER_LEN 8192
#define BIGBUFSIZ 32768
#define SUPERBIGSIZ 80000 * 8192

#include <ndc.h>
extern struct ndc_config nd_config;
extern int euid;

#endif

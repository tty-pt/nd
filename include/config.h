#ifndef _CONFIG_H
#define _CONFIG_H

#define BUFFER_LEN 8192
#define STD_DB "/var/nd/std.db"
#define STD_DB_OK "/var/nd/std.db.ok"

#include <ndc.h>
extern struct ndc_config nd_config;
extern int euid;

#endif

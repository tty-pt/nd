#ifndef UAPI_ST_H
#define UAPI_ST_H

#include <sys/types.h>
#include "./object.h"

#define DIM 2

typedef int16_t coord_t;

typedef coord_t point_t[DIM];

typedef coord_t point4D_t[4];

typedef point4D_t pos_t;

typedef uint64_t morton_t;

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

typedef int st_teleport_t(unsigned player_ref, struct cmd_dir cd);
st_teleport_t st_teleport;

typedef void st_run_t(unsigned player_ref, char *symbol);
st_run_t st_run;

#endif


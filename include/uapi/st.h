#ifndef UAPI_ST_H
#define UAPI_ST_H

#include <sys/types.h>
#include "./object.h"

#define DIM 2
#define NOISE_MAX ((uint32_t) -1)

typedef int16_t coord_t;
typedef uint16_t ucoord_t;

typedef coord_t point_t[DIM];

typedef coord_t point4D_t[4];

typedef point4D_t pos_t;

typedef uint64_t morton_t;

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

struct bio {
	coord_t tmp;
	ucoord_t rn;
	uint32_t ty;
	unsigned bio_idx;
	unsigned raw[16];
};

typedef struct view_tile {
	unsigned bio_idx;
	unsigned room;
	ucoord_t flags, exits, doors;
	unsigned raw[16];
} view_tile_t;

typedef int st_teleport_t(unsigned player_ref, struct cmd_dir cd);
st_teleport_t st_teleport;

typedef void st_run_t(unsigned player_ref, char *symbol);
st_run_t st_run;

typedef struct bio noise_point_t(pos_t p);
noise_point_t noise_point;

struct spawn_arg {
	unsigned where_ref;
	void *arg;
	coord_t *pos;
};

#endif


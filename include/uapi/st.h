#ifndef UAPI_ST_H
#define UAPI_ST_H

#include <sys/types.h>
#include "./object.h"

// TODO max plant_n should be 1
#define DIM 2
#define PLANT_N(pln, i) ((pln >> (i * 2)) & 3)

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

struct plant_data {
	unsigned id[3];
	unsigned char n, max;
};

struct bio {
	coord_t tmp;
	ucoord_t rn;
	uint32_t ty;
	struct plant_data pd;
	unsigned bio_idx;
};

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


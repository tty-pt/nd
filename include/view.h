#ifndef VIEW_H
#define VIEW_H

#include "geometry.h"
#include "biome.h"
#include "plant.h"

#define ROOM_MAX 32

enum vtf {
	VTF_SHOP = 1,
	VTF_ENTITY = 2,
	VTF_POND = 4,
	VTF_MAX = 4
};

typedef struct {
	const char *pre;
	char emp;
} vtf_t;

typedef struct view_tile {
	/* ref_t players[ROOM_MAX]; /1* dbref actually *1/ */
	enum biome bio_idx;
	dbref room;
	ucoord_t flags, exits, doors;
	struct plant_data pd;
} view_tile_t;

typedef view_tile_t view_t[VIEW_M];

void view(OBJ *player);
#endif

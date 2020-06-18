#ifndef VIEW_H
#define VIEW_H

#include "geometry.h"
#include "biome.h"
#include "plant.h"

#define ROOM_MAX 32

enum vtf {
	VTF_SHOP = 1,
	VTF_NPC = 2,
	VTF_POND = 4,
	VTF_PLAYER = 8,
	VTF_MAX = 4,
};

typedef struct {
	const char *pre;
	char emp;
} vtf_t;

typedef struct view_tile {
	ref_t players[ROOM_MAX]; /* dbref actually */
	enum biome bio_idx;
	ref_t room;
	ucoord_t flags, exits, doors;
	struct plant_data pd;
} view_tile_t;

typedef view_tile_t view_t[VIEW_M];

#endif

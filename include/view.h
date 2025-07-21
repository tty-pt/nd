#ifndef VIEW_H
#define VIEW_H

#include "biome.h"
#include "st.h"

#define ROOM_MAX 32

typedef struct view_tile {
	enum biome bio_idx;
	unsigned room;
	ucoord_t flags, exits, doors;
	struct plant_data pd;
} view_tile_t;

typedef view_tile_t view_t[VIEW_M];

void view(unsigned player_ref);

#endif

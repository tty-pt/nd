#ifndef VIEW_H
#define VIEW_H

#include "biome.h"
#include "st.h"

#define ROOM_MAX 32

typedef view_tile_t view_t[VIEW_M];

void view(unsigned player_ref);

#endif

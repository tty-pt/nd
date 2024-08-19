#ifndef MAP_H
#define MAP_H

#include "object.h"
#include "spacetime.h"

int map_init();
void map_search(dbref *mat,
		pos_t pos, unsigned radius);
int map_close(void);
int map_sync(void);
int map_has(dbref thing);
morton_t map_mwhere(dbref thing);
void map_where(pos_t p, dbref thing);
int map_delete(dbref what);
OBJ *map_get(pos_t at);
void map_put(pos_t p, dbref thing, int flags);

#endif

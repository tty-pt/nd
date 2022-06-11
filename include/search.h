#ifndef MAP_H
#define MAP_H
#include "mdb.h"
#include "geometry.h"
/* #include "noise.h" */
int map_init();
void map_search(dbref *mat,
		pos_t pos, unsigned radius);
int map_close(void);
void map_where(pos_t p, OBJ *thing);
int map_delete(OBJ *what);
OBJ *map_get(pos_t at);
void map_put(pos_t p, OBJ *thing, int flags);
#endif

#ifndef MAP_H
#define MAP_H
#include "db.h"
#include "geometry.h"
/* #include "noise.h" */
int map_init();
void map_search(dbref *mat,
		pos_t pos, unsigned radius);
int map_close(void);
void map_where(pos_t p, dbref thing);
int map_delete(dbref what);
dbref map_clean(int descr, dbref player, dbref here);
dbref map_get(pos_t at);
void map_put(pos_t p, dbref thing, int flags);
#endif

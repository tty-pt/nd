#ifndef GEO_H
#define GEO_H

#include "db.h"
#include "geometry.h"

// adds 2^DAYTICK_Y to day tick until it reaches DAYSIZE
#define DAY_Y		16
#define DAYTICK_Y	6
#define DAYTICK		(1<<DAYTICK_Y)
#define NIGHT_IS	(day_tick > (1 << (DAY_Y - 1)))
#define GEXPAND(c)	geo_map[(int) c]

extern unsigned short day_tick;
extern const char *geo_map[]; // map of characters for commands, not geo_view

/* TODO isomorphic (wasm) */

enum gexit_flags {
        GEF_WEST = 1,
        GEF_SOUTH = 2,
        GEF_NORTH = 4,
        GEF_EAST = 8,
        GEF_DOWN = 16,
        GEF_UP = 32,
};

void geo_update(void);
dbref geo_room(int descr, dbref player, dbref exit);
int geo_claim(int descr, dbref player, dbref room);
int geo_v(int descr, dbref player, const char *cmd);

dbref geo_there(dbref where, enum exit e);
int gexit_is(dbref exit);
void gexit_dest_set(dbref exit, dbref dest);
int gexit_can(dbref player, dbref exit);
void gexit_snull(int descr, dbref player, dbref exit);
dbref gexit_where(int descr, dbref player, dbref loc, enum exit e);
int gexits(int descr, dbref player, dbref where);

void geo_notify(int descr, dbref player);
dbref geo_clean(int descr, dbref player, dbref here);

static inline enum exit
exit_e(dbref exit) {
	const char dir = NAME(exit)[0];
	return dir_e(dir);
}

#endif

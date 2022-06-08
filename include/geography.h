#ifndef GEO_H
#define GEO_H

#include "mdb.h"
#include "geometry.h"

// adds 2^DAYTICK_Y to day tick until it reaches DAYSIZE
#define DAY_Y		16
#define DAYTICK_Y	6
#define DAYTICK		(1<<DAYTICK_Y)
#define NIGHT_IS	(day_tick > (1 << (DAY_Y - 1)))
#define GEXPAND(c)	geo_map[(int) c]

struct cmd_dir {
	char dir;
	enum exit e;
	morton_t rep;
};

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
dbref geo_room(dbref player, enum exit e);
int geo_claim(dbref player, dbref room);
int geo_v(dbref player, const char *);

dbref geo_there(dbref where, enum exit e);
void gexit_snull(dbref player, dbref exit);
int gexits(dbref player, dbref where);

/* void geo_notify(int descr, dbref player); */
dbref geo_clean(dbref player, dbref here);

int geo_teleport(dbref player, struct cmd_dir cd);

#endif

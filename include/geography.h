#ifndef GEO_H
#define GEO_H

#include "db.h"

// adds 2^DAYTICK_Y to day tick until it reaches DAYSIZE
#define DAY_Y		16
#define DAYTICK_Y	6
#define DAYTICK		(1<<DAYTICK_Y)
#define DAYSIZE		((unsigned short) -1)
#define NIGHT_IS	(day_tick > (1 << (DAY_Y - 1)))

extern unsigned short day_tick;

const char * geo_expand(char c);
int geo_is(dbref exit);

dbref untmp_clean(int descr, dbref player, dbref here);
dbref geo_enter_room(dbref *exit_there, int descr, dbref player, dbref exit, int v, int permanent);
void do_map(int descr, dbref player);
int geo_init(void);
void geo_update(void);
int geo_close(void);
int geo_has(dbref what);
int geo_delete(dbref what);
int room_claim(dbref player, dbref room);

#endif

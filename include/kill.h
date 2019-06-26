#ifndef KILL_H
#define KILL_H

#include "db.h"
#include "item.h"
#include "params.h"

struct living *living_get(dbref who);
int living_put(dbref who);

void do_living_init(void);
void livings_update(void);
void do_living_save(void);

void dodge_recompute(struct living *liv);

#endif

#ifndef OBJECT_H
#define OBJECT_H

#include "uapi/object.h"
#include "skeleton.h"

extern dbref db_top;

dbref object_add(OBJ *nu, unsigned skel_id, dbref where, void *arg);
void object_drop(dbref where_ref, unsigned skel_id);
void observer_add(dbref observable_ref, dbref observer_ref);
int observer_remove(dbref observable_ref, dbref observer_ref);
int objects_init();
void objects_update(double dt);
void objects_sync();

#endif

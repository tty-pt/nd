#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "nddb.h"

void hash_init(DB **db);
void hash_put(DB *db, const char *key, void *data);
void *hash_get(DB *db, const char *key);
void hash_delete(DB *db, const char *key);

#endif

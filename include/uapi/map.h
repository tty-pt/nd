#ifndef UAPI_MAP_H
#define UAPI_MAP_H

#include "uapi/st.h"

int map_has(unsigned thing);
morton_t map_mwhere(unsigned thing);
void map_where(pos_t p, unsigned thing);
int map_delete(unsigned what);
unsigned map_get(pos_t at);

#endif

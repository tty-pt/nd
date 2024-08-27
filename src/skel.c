#include "skel.h"

LHASH_DEF(skel, SKEL);
LHASH_DEF(drop, DROP);
LHASH_ASSOC_DEF(drop, skel, drop, DROP);

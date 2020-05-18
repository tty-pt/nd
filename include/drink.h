#ifndef DRINK_H
#define DRINK_H

#define DRINK(id) (&drink_map[id])
#define DRINK_SOURCE(ref) (DRINK(GETDRINK(ref)))

enum drink_affects {
	DA_NONE,
	DA_HP,
	DA_MP,
};

enum drink {
	DRINK_WATER,
	DRINK_MILK,
	DRINK_AYUHASCA,
};

typedef struct {
	char *name;
	enum drink_affects da;
} drink_t;

extern drink_t drink_map[];

#endif

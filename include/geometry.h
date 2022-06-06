#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "command.h"

/* it is possible to add other coord_t (like char) but a corresponding
 * hash function must be used */

#define Y_COORD 0
#define X_COORD 1

#define COORD_MIN SHRT_MIN
#define COORD_MAX SHRT_MAX
#define UCOORD_MAX USHRT_MAX

#define DIM 2
#define WDIM X_COORD
#define POOP register int I; for (I = 0; I < DIM; I++)
#define POOP3D register int I; for (I = 0; I < 3; I++)
#define POOP4D register int I; for (I = 0; I < 4; I++)
#define POINT3D_ADD(r, a, b) { POOP3D r[I] = a[I] + b[I]; }

#define DIM2 (DIM * 2)
#define RECT_SIZE (2 * sizeof(point_t))

#define OBITS(code) (code >> 48)

#define VIEW_AROUND 3
#define VIEW_SIZE ((VIEW_AROUND<<1) + 1)
#define VIEW_M VIEW_SIZE * VIEW_SIZE
#define VIEW_BDI (VIEW_SIZE * (VIEW_SIZE - 1))
#define MORTON_READ(pos) (* (morton_t *) pos)

typedef int16_t coord_t;
typedef uint16_t ucoord_t;

typedef coord_t point_t[DIM];
typedef ucoord_t upoint_t[DIM];

typedef coord_t point3D_t[3];
typedef ucoord_t upoint3D_t[3];
typedef coord_t point4D_t[4];
typedef ucoord_t upoint4D_t[4];

typedef point4D_t pos_t;
typedef upoint4D_t upos_t;

typedef uint64_t morton_t; // 4d morton, btw
typedef int64_t smorton_t;

struct rect {
	point_t s;
	upoint_t l;
};

/* struct rect3D { */
/* 	point3D_t s; */
/* 	upoint3D_t l; */
/* }; */

struct rect4D {
	point4D_t s;
	upoint4D_t l;
};

enum exit {
	E_NULL = 0,
	E_WEST = 1,
	E_NORTH = 2,
	E_UP = 4,
	E_EAST = 8,
	E_SOUTH = 16,
	E_DOWN = 32,
	E_ALL = 63,
};

typedef struct {
	const char name[16];
	const char fname[16];
	const char other[16];
	enum exit simm;
	coord_t dim, dis;
} exit_t;

enum object_skeleton_type {
        S_TYPE_OTHER,
        S_TYPE_FOOD,
        S_TYPE_DRINK,
        S_TYPE_EQUIPMENT,
        S_TYPE_ENTITY,
	S_TYPE_PLANT,
	S_TYPE_BIOME,
};

enum element {
	ELM_PHYSICAL,
	ELM_FIRE,
	ELM_ICE,
	ELM_AIR,
	ELM_EARTH,
	ELM_SPIRIT,
	ELM_VAMP,
	ELM_DARK,
};

struct drop {
	struct object_skeleton *i;
	unsigned char y, yield, yield_v;
};

struct entity_skeleton {
	struct drop *drop[32];
	unsigned char y, stat, lvl, lvl_v, wt, flags;
	enum element type;
	unsigned biomes;
};

struct plant_skeleton {
	char const *pre, small, big, *post;
	coord_t tmp_min, tmp_max;
	ucoord_t rn_min, rn_max;
	struct drop *drop[32];
	unsigned y;
};

struct object_skeleton {
	char const *name;
	char const *art;
	char const *description;
        char const *avatar;

        enum object_skeleton_type type;

        union {
                unsigned short food;
                unsigned short drink;
                struct {
                        unsigned short eqw, msv;
                } equipment;
                struct entity_skeleton entity;
		struct plant_skeleton plant;
                struct {
                        const char *bg;
                } biome;
        } sp;
};

extern enum exit e_map[];
extern exit_t exit_map[];

morton_t pos_morton(pos_t);
void morton_pos(pos_t p, morton_t code);

dbref e_exit_where(dbref player, dbref loc, enum exit e);
int e_exit_can(dbref player, dbref exit);
int e_ground(dbref room, enum exit e);
void e_exit_dest_set(dbref exit, dbref dest);

dbref
object_add(struct object_skeleton o, dbref where);

void
object_drop(dbref where, struct drop **drop);

dbref e_exit_here(dbref player, enum exit e);
void pos_move(pos_t d, pos_t o, enum exit e);
enum exit dir_e(const char dir);
const char e_dir(enum exit e);
enum exit e_simm(enum exit e);
const char * e_name(enum exit e);
const char * e_fname(enum exit e);
const char * e_other(enum exit e);
morton_t point_rel_idx(point_t p, point_t s, smorton_t w);
enum exit exit_e(dbref exit);
int e_exit_is(dbref exit);
dbref e_exit_dest(dbref exit);


#endif

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <limits.h>
#include <stdint.h>
#ifndef CLIENT
#include <stddef.h>
#include <string.h>
#include "db.h"
#endif

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

typedef int ref_t;

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

struct obj {
	char const *name;
	char const *art;
	char const *description;
};

extern enum exit e_map[];
extern exit_t exit_map[];

static inline void
pos_move(pos_t d, pos_t o, enum exit e) {
	exit_t *ex = &exit_map[e];
	POOP4D d[I] = o[I];
	d[ex->dim] += ex->dis;
}

static inline enum exit
dir_e(const char dir) {
	return e_map[(int) dir];
}

static inline const char
e_dir(enum exit e) {
	return exit_map[e].name[0];
}

static inline enum exit
e_simm(enum exit e) {
	return exit_map[e].simm;
}

static inline const char *
e_name(enum exit e) {
	return exit_map[e].name;
}

static inline const char *
e_fname(enum exit e) {
	return exit_map[e].fname;
}

static inline const char *
e_other(enum exit e) {
	return exit_map[e].other;
}

#ifndef CLIENT

morton_t pos_morton(pos_t);
void morton_pos(pos_t p, morton_t code);

static inline morton_t
point_rel_idx(point_t p, point_t s, smorton_t w)
{
	smorton_t s0 = s[Y_COORD],
		s1 = s[X_COORD];
	if (s0 > p[Y_COORD])
		s0 -= UCOORD_MAX;
	if (s1 > p[X_COORD])
		s1 -= UCOORD_MAX;
	return (p[Y_COORD] - s0) * w + (p[X_COORD] - s1);
}

static inline enum exit
exit_e(ref_t exit) {
	const char dir = NAME(exit)[0];
	return dir_e(dir);
}

static inline int
e_exit_is(ref_t exit)
{ 
	char const *fname = e_fname(exit_e(exit));
	return *fname && !strncmp(NAME(exit), fname, 4);
}

static inline ref_t
e_exit_dest(dbref exit)
{
	if (!DBFETCH(exit)->sp.exit.ndest)
		return NOTHING;

	else
		return DBFETCH(exit)->sp.exit.dest[0];
}

ref_t e_exit_where(int descr, dbref player, ref_t loc, enum exit e);
int e_exit_can(ref_t player, ref_t exit);
int e_ground(ref_t room, enum exit e);
void e_exit_dest_set(dbref exit, dbref dest);

static inline ref_t
e_exit_here(int descr, ref_t player, enum exit e)
{
	return e_exit_where(descr, player, getloc(player), e);
}

ref_t
obj_add(struct obj o, ref_t where);

ref_t
obj_stack_add(struct obj o, ref_t where,
		unsigned char n);
ref_t
contents_find(int descr, ref_t player, ref_t where,
		const char *name);
#endif
#endif

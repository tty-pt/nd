#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "db.h"

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
#define POINT3D_ADD(r, a, b) { POOP3D r[I] = a[I] + b[I]; }

#define DIM2 (DIM * 2)
#define RECT_SIZE (2 * sizeof(point_t))

#define OBITS(code) (code >> 48)

typedef int16_t coord_t;
typedef uint16_t ucoord_t;

typedef coord_t point_t[DIM];
typedef ucoord_t upoint_t[DIM];

typedef coord_t point3D_t[3];
typedef ucoord_t upoint3D_t[3];
typedef uint64_t morton_t; // 3d morton, btw
typedef int64_t smorton_t;

struct rect {
	point_t s;
	upoint_t l;
};

struct rect3D {
	point3D_t s;
	upoint3D_t l;
};

morton_t morton3D_encode(point3D_t p, ucoord_t obits);
void morton3D_decode(point3D_t p, morton_t code);
int rects_intersection(struct rect *r, struct rect *a, struct rect *b);

static inline int
rects_intersect(struct rect *a, struct rect *b)
{
	struct rect r;
	return rects_intersection(&r, a, b);
}

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

#endif

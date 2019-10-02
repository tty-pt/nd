#include "geometry.h"
#include "debug.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* https://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN */

static __inline__ ucoord_t
unsign(coord_t n)
{
	ucoord_t r = ((smorton_t) n + COORD_MAX);

	if (r == UCOORD_MAX)
		return 1;

	return r;
}

morton_t
morton3D_encode(point3D_t p, ucoord_t obits)
{
	upoint3D_t up;
	morton_t res = ((morton_t) obits) << 48;
	int i;

	POOP3D up[I] = unsign(p[I]);

	for (i = 0; i < 16; i ++) {
		POOP3D res |= ((morton_t) ((up[I] >> i) & 1)) << (I + (3 * i));
	}

	debug("encoded point %d %d %d -> x%llx", p[0], p[1], p[2], res);
	return res;
}

static inline coord_t
sign(ucoord_t n)
{
	return (smorton_t) n - COORD_MAX;
}

void
morton3D_decode(point3D_t p, morton_t code)
{
	morton_t up[3] = { 0, 0, 0 };
	int i;

	for (i = 0; i < 16; i ++) {
		POOP3D up[I] |= ((code >> (I + 3 * i)) & 1) << i;
	}

	POOP3D p[I] = sign(up[I]);
	debug("decoded point x%llx -> %d %d %d", code, p[0], p[1], p[2]);
}

static __inline__ ucoord_t
gtl(coord_t s, coord_t ae, coord_t be)
{
	int c1 = s <= ae;
	int c2 = s <= be;

	if (c1 && c2)
		return MIN(ae, be) - s;

	else {
		coord_t e = c1 != c2 ? MAX(ae, be) : MIN(ae, be);
		return e - s;
	}
}

static __inline__ coord_t
gts(coord_t as, coord_t bs, coord_t ae, coord_t be)
{
	int c1 = as > ae;
	int c2 = bs > be;

	if (c1 != c2) {
		if (c1 && bs <= 0)
			return bs;
		else if (as <= 0)
			return as;
	}

	return MAX(as, bs);
}

static int
gtsl(coord_t *s, coord_t *l, coord_t as, ucoord_t al, coord_t bs, ucoord_t bl)
{
	coord_t ae = as + al;
	coord_t be = bs + bl;

	*s = gts(as, bs, ae, be);
	*l = gtl(*s, ae, be);

	return *l > bl || *l > al;
}

int
rects_intersection(struct rect *r, struct rect *a, struct rect *b)
{
	POOP if (gtsl(&r->s[I], &r->l[I],
		      a->s[I], a->l[I],
		      b->s[I], b->l[I]))

		return 0;

	return 1;
}

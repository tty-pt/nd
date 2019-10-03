#include "geometry.h"
#include "debug.h"

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
morton_encode(point3D_t p, ucoord_t obits)
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
morton_decode(point3D_t p, morton_t code)
{
	morton_t up[3] = { 0, 0, 0 };
	int i;

	for (i = 0; i < 16; i ++) {
		POOP3D up[I] |= ((code >> (I + 3 * i)) & 1) << i;
	}

	POOP3D p[I] = sign(up[I]);
	debug("decoded point x%llx -> %d %d %d", code, p[0], p[1], p[2]);
}

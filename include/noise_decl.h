/* A noise algorithm that wraps well around data type limits.
 *
 * LICENCE: CC BY-NC-SA 4.0
 * For commercial use, contact me at q@qnixsoft.com
 *
 * DEFINITIONS:
 *
 * a point has DIM coordinates
 *
 * a matrix has 2^y (l) edge length.
 * It is an array of noise_t where we want to store the result.
 *
 * A noise feature quad has 2^x (d) edge length
 * and 2^DIM vertices with random values.
 *
 * Other values are the result of a "fade" between those.
 * And they are added to the resulting matrix (for multiple octaves);
 *
 * v is an array that stores a (noise feature) quad's vertices.
 * It has a peculiar order FIXME
 *
 * recursive implementations are here for reference
 * TODO further optimizations
 * TODO does this work in 3D+?
 * TODO improve documentation
 * TODO speed tests
 * TODO think before changing stuff
 * */

#ifndef NOISE_H
#define NOISE_H

#include <stdint.h>
#include <stddef.h>

#define NOISE_MAX ((noise_t) -1)

typedef uint32_t noise_t;
typedef int32_t snoise_t;

typedef struct { unsigned x, w; } octave_t;

void
noise_oct(noise_t *m, int16_t *s, size_t oct_n,
	  octave_t *oct, unsigned seed, unsigned cy, uint8_t dim);

#endif

#ifdef NOISE_IMPLEMENTATION

#include <string.h>
#include "xxhash.h"

#define HASH XXH32

/* gets a mask for use with get_s */
#define COORDMASK_LSHIFT(x) (uint16_t) ((((uint16_t) -1) << x))

/* gets a (deterministic) random value for point p */
static inline noise_t
noise_r(int16_t *p, unsigned seed, unsigned w, uint8_t dim) {
	register noise_t v = HASH(p, sizeof(int16_t) * dim, seed);
		return ((long long unsigned) v) >> w;
}

/* generate values at quad vertices. */
static inline void
noise_get_v(noise_t *v, int16_t *s, uint16_t x, unsigned w, unsigned seed, uint8_t dim) {
	const int16_t d = 1 << x;
	int16_t va[dim * (1 << dim)];
	int i, j;

	for (i = 0; i < (1 << dim); i++) {
		for (j = 0; j < dim; j++)
			va[i * dim + j] = s[j] + ((i & (1 << (dim - j - 1))) ? d : 0);

		v[i] = noise_r(va + dim * i, seed, w, dim);
	}
}

static inline void
calc_steps(snoise_t *st,
	   noise_t *v,
	   uint16_t z,
	   uint16_t vl,
	   unsigned y)
{
	int n;
	for (n = 0; n < vl; n ++)
		// FIXME FIND PARENS
		st[n] = ((long) v[n + vl] - v[n]) >> z << y;
}

static inline void
step(noise_t *v, snoise_t *st, uint16_t vl, uint16_t mul)
{
	int n;
	for (n = 0; n < vl; n++)
		v[n] = (long) v[n] + ((long) st[n] * mul);
}

/* Given a set of vertices,
 * fade between them and set target values acoordingly
 */
static inline void
noise_quad(noise_t *c, noise_t *vc, unsigned z, unsigned w, unsigned cy, uint8_t dim) {
	uint16_t ndim = dim - 1;
	uint16_t tvl = 1 << ndim; /* length of input values */
	snoise_t st[(tvl << 1) - 1], *stc = st;
	noise_t *ce_p[dim], *vt;
	size_t cd = 1 << cy * ndim; /* (2^y)^ndim */
	goto start;
	do {
		do {
			/* PUSH */
			cd >>= cy; ndim--; vc = vt; stc += tvl; tvl >>= 1;

start:			ce_p[ndim] = c + (cd << z);
			vt = vc + (tvl<<1);

			calc_steps(stc, vc, z, tvl, 0);
			memcpy(vt, vc, tvl * sizeof(noise_t));
		} while (ndim);

		for (; c < ce_p[0]; c += cd, vt[0] += *stc)
		*c += vt[0];

		do {
			/* POP */
			++ndim;
			if (ndim >= dim)
				return;
			c -= cd << z; cd <<= cy; vt = vc;
			tvl <<= 1; stc -= tvl; vc -= (tvl<<1);

			step(vt, stc, tvl, 1);
			c += cd;
		} while (c >= ce_p[ndim]);
	} while (1);
}

// FIXME there is a memory leak on the non recursive version on linux
#ifndef __OpenBSD__
static inline void
_noise_mr(noise_t *c, noise_t *v, unsigned x, int16_t *qs, uint16_t ndim, unsigned w, unsigned seed, unsigned cy, uint8_t dim) {
	int i = dim - 1 - ndim;
	uint16_t ced = (1 << (cy * (ndim + 1))), cd;
	noise_t *ce = c + ced;

	ced >>= cy;
	cd = ced << x;

	for (; c < ce; qs[i] += (1<<x), c += cd)
		if (ndim == 0) {
			noise_get_v(v, qs, x, w, seed, dim);
			noise_quad(c, v, x, w, cy, dim);
		} else
			_noise_mr(c, v, x, qs, ndim - 1, w, seed, cy, dim);

	qs[i] -= 1 << cy; // reset
}

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, int16_t *qs, unsigned w, unsigned seed, unsigned cy, uint8_t dim)
{
	_noise_mr(c, v, x, qs, dim - 1, w, seed, cy, dim);
}

#else

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, int16_t *qs, unsigned w, unsigned seed, unsigned cy, uint8_t dim) {
	noise_t *ce_p[dim];
	noise_t ced = 1<<(cy * dim);
	int16_t *qsc = qs; /* quad coordinate */
	uint16_t ndim = dim - 1;

	goto start;

	do {
		do { /* PUSH */
			qsc++; ndim--;
start:			ce_p[ndim] = c + ced;
			ced >>= cy;
		} while (ndim);

		do {
			noise_get_v(v, qs, x, w, seed, dim);
			noise_quad(c, v, x, w, cy, dim);
			*qsc += 1 << x;
			c += ced << x;
		} while (c < ce_p[ndim]);

		do { /* POP */
			*qsc -= 1 << cy;
			ced <<= cy;
			c += (ced << x) - ced; /* reset and inc */
			qsc--; ndim++;
			*qsc += 1 << x;
		} while (c >= ce_p[ndim]);
	} while (ndim < dim);
}

#endif

/* fixes v (vertex values)
 * when noise quad starts before matrix quad aka x > y aka d > l.
 * assumes ndim to be at least 1
 * vl = 1 << ndim
 */
static inline void
__fix_v(noise_t *v, snoise_t *st, int16_t *ms, int16_t *qs, unsigned x, uint16_t vl, unsigned cy) {
	register noise_t *vn = v + vl;
	register uint16_t dd = (*ms - *qs) >> cy;

	// TODO make this more efficient
	calc_steps(st, v, x, vl, cy);

	if (dd) /* ms > qs */
		step(v, st, vl, dd);

	memcpy(vn, v, sizeof(noise_t) * vl); // COPY to opposite values
	step(vn, st, vl, 1); // STEP by side length of M
}

// yes this does work fine a bit of a hack but ok i guess
static inline void
noise_fix_v(noise_t *v, int16_t *qs, int16_t *ms, unsigned x, unsigned cy, uint8_t dim) {
	uint16_t vl = 1 << dim;
	snoise_t st[vl - 1], *stc = st;
	int ndim = dim - 1;
	int first = 1;
	vl >>= 1;

	for (;;) {
		__fix_v(v, stc, ms, qs, x, vl, cy);

		if (ndim) {
			ndim--; qs++; ms++; vl >>= 1; stc += vl; /* PUSH */
		} else if (first) {
			v += vl << 1;
			first = 0;
		} else break;
	}
}

static inline void
noise_get_s(int16_t *s, int16_t *p, uint16_t mask, uint8_t dim) {
	int i;
	for (i = 0; i < dim; i++)
		s[i] = ((int16_t) p[i]) & mask;
}

/* Fills in matrix "mat" with deterministic noise.
 * "ms" is the matrix's min point in the world;
 * "x" 2^x = side length of noise quads;
 * "y" 2^y = side length of matrix "mat".
 * */
static inline void
noise_m(noise_t *mat, int16_t *ms, unsigned x,
	unsigned w, unsigned seed, unsigned cy, uint8_t dim)
{
	noise_t v[(1 << (dim + 1)) - 1];

	if (cy > x)
		_noise_m(mat, v, x, ms, w, seed, cy, dim);
	else {
		int16_t qs[dim];

		noise_get_s(qs, ms, COORDMASK_LSHIFT(x), dim);
		noise_get_v(v, qs, x, w, seed, dim);
		if (x > cy)
			noise_fix_v(v, qs, ms, x, cy, dim);

		noise_quad(mat, v, cy, w, cy, dim);
	}
}

void
noise_oct(noise_t *m, int16_t *s, size_t oct_n,
	  octave_t *oct, unsigned seed, unsigned cy, uint8_t dim)
{
	unsigned cm = 1 << dim * cy;
	octave_t *oe;
	memset(m, 0, sizeof(noise_t) * cm);
	for (oe = oct + oct_n; oct < oe; oct++)
		noise_m(m, s, oct->x, oct->w, seed, cy, dim);
}

#endif

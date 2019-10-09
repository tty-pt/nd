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

#include "item.h"
/* #include "morton.h" */
#include <string.h>
#include "xxhash.h"
#include "params.h"
#include "debug.h"

#define XXH XXH32

#define CHUNK_Y 5
#define CHUNK_SIZE (1 << CHUNK_Y)
#define CHUNK_M (CHUNK_SIZE * CHUNK_SIZE)

/* gets a mask for use with get_s */
#define COORDMASK_LSHIFT(x) (ucoord_t) ((((ucoord_t) -1) << x))
#define NOISE_MAX ((noise_t) -1)

#define PLANTS_SEED 5

#define NOISE_OCT(to, x, seed) \
	noise_oct(to, s, sizeof(x) / sizeof(octave_t), x, seed)

typedef struct { unsigned x, w; } octave_t;

struct tilemap {
	struct rect r;
	struct bio *bio;
};

static size_t v_total = (1<<(DIM+1)) - 1;
static struct bio chunks_bio_raw[CHUNK_M * 4],
		  chunks_bio[CHUNK_M * 4],
		  *bio;
static struct rect chunks_r;
static coord_t chunks_obits = -1;
static const noise_t noise_fourth = (NOISE_MAX >> 2);
static noise_t n_he[CHUNK_M],
	       n_cl[CHUNK_M],
	       n_tm[CHUNK_M];

/* REGARDING DRAWABLE THINGS THAT DEPEND ON NOISE {{{ */

/* TODO calculate water needs */
struct plant plants[] = {{
	// taiga
	{ "pinus silvestris", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'x', 'X', ANSI_RESET_BOLD,
	-65, 70, 50, 200, 1,
	{ "fruit", "", "" }, 1,
}, {	// temperate rainforest
	{ "pseudotsuga menziesii", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 't', 'T', ANSI_RESET_BOLD,
	32, 100, 180, 350, 1,
	{ "fruit", "", "" }, 1,
}, {	// woodland / grassland / shrubland
	{ "betula pendula", "", "" }, ANSI_FG_YELLOW, 'x', 'X', "",
	-4, 86, 400, 700, 1,
	{ "fruit", "", "" }, 1,
}, {	// woodland / grassland?
	{ "betula pubescens", "", "" }, ANSI_FG_WHITE, 'x', 'X', "",
	0, 96, 500, 900, 1,
	{ "fruit", "", "" }, 1,
}, {	// temperate forest
	{ "abies alba", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'x', 'X', ANSI_RESET_BOLD,
	-40, 86, 100, 200, 1,
	{ "fruit", "", "" }, 1,
}, {	// desert
	{ "arthrocereus rondonianus", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 'i', 'I', "",
	110, 130, 10, 150, 1,
	{ "fruit", "", "" }, 1,
}, {	// savannah
	{ "acacia senegal", "", "" }, ANSI_BOLD ANSI_FG_GREEN, 't', 'T', "",
	40, 120, 20, 120, 1,
	{ "fruit", "", "" }, 1,
}, {
	{ "daucus carota", "", "" }, ANSI_FG_WHITE, 'x', 'X', "",
	38, 96, 100, 200, 1,
	{ "carrot", "", "" }, 1,
}, {
	{ "solanum lycopersicum", "", "" }, ANSI_FG_RED, 'x', 'X', "", 
	50, 98, 100, 200, 5,
	{ "tomato", "", "" }, 1,
}};

static const unsigned char plid_max = sizeof(plants) / sizeof(struct plant);

/* WORLD GEN STATIC VARS {{{ */

static coord_t tmp_max = 170,
	       tmp_min = -41;

static ucoord_t rn_max = 1000,
	       rn_min = 50;

static const size_t tmp_vary = BIOME_VOLCANIC / 3;

/* }}} */

/* WORLD GEN PRIMITIVES {{{ */
static inline noise_t
water_level(ucoord_t obits)
{
	// returns (0-3) * (NOISE_MAX / 4)
	return (((obits >> 2) + 2) & 3) * noise_fourth;
}

static inline unsigned
rain(ucoord_t obits, noise_t w, noise_t he, noise_t cl)
{
	smorton_t x;
	x = w - he;
	x += cl / 50;
	// change 300 to affect how
	// terrain height decreases rain
	x /= UCOORD_MAX * 290;
	x -= 100;
	return x;
}

static inline noise_t
sun_dist(ucoord_t obits)
{
	// returns (1-4)
	return ((obits + 1) & 3);
}

static inline coord_t
temp(ucoord_t obits, noise_t he, noise_t tm, coord_t pos_y) // fahrenheit * 10
{
	smorton_t x = 0;
	x = he - water_level(obits) + (tm - NOISE_MAX) / 3;
	// change 200 to affect how
	// terrain height decreases temperature
	x /= 4 * UCOORD_MAX * 260;
	x += (tmp_max<<1) - tmp_min // min temp of hot planet
		- (tmp_max - tmp_min) * sun_dist(obits);
	x += (pos_y * tmp_min) / 6300;
	return tmp_min + x;
}

/* }}} */
/* }}} */

/* gets a (deterministic) random value for point p */

static inline noise_t
r(point_t p, unsigned seed, unsigned w)
{
	/* morton_t m = morton2D_point(p); */
	/* return XXH(&m, sizeof(morton_t), seed); */
	register noise_t v = XXH(p, sizeof(point_t), seed);
	return ((long long unsigned) v) >> w;
}

/* generate values at quad vertices. */

static inline void
get_v(noise_t v[1 << DIM], point_t s, ucoord_t x, unsigned w, unsigned seed)
{
	const coord_t d = 1 << x;
	const point_t va[] = {
		{ s[0], s[1] },
		{ s[0], s[1] + d },
		{ s[0] + d, s[1] },
		{ s[0] + d, s[1] + d }
	};
	int i;

	for (i = 0; i < (1 << DIM); i++)
		v[i] = r((coord_t *) va[i], seed, w);
}

static inline void
calc_steps(snoise_t *st,
	   noise_t *v,
	   ucoord_t z,
	   ucoord_t vl,
	   unsigned y)
{
	int n;
	for (n = 0; n < vl; n ++)
		// FIXME FIND PARENS
		st[n] = ((long) v[n + vl] - v[n]) >> z << y;
}

static inline void
step(noise_t *v, snoise_t *st, ucoord_t vl, ucoord_t mul)
{
	int n;
	for (n = 0; n < vl; n++)
		v[n] = (long) v[n] + ((long) st[n] * mul);
}

/* Given a set of vertices,
 * fade between them and set target values acoordingly
 */

static inline void
noise_quad(noise_t *c, noise_t *vc, unsigned z, unsigned w)
{
	ucoord_t ndim = DIM - 1;
	ucoord_t tvl = 1<<ndim; // length of input values
	snoise_t st[(tvl<<1) - 1], *stc = st;
	noise_t *ce_p[DIM], *vt;
	size_t cd = 1 << CHUNK_Y * ndim; /* (2^y)^ndim */

	goto start;

	do {
		do {
			// PUSH
			cd >>= CHUNK_Y; ndim--; vc = vt; stc += tvl; tvl >>= 1;

start:			ce_p[ndim] = c + (cd<<z);
			vt = vc + (tvl<<1);

			calc_steps(stc, vc, z, tvl, 0);
			memcpy(vt, vc, tvl * sizeof(noise_t));
		} while (ndim);

		for (; c < ce_p[0]; c+=cd, vt[0]+=*stc)
			*c += vt[0];

		do {
			// POP
			++ndim;
			if (ndim >= DIM)
				return;
			c -= cd << z; cd <<= CHUNK_Y; vt = vc;
			tvl <<= 1; stc -= tvl; vc -= (tvl<<1);

			step(vt, stc, tvl, 1);
			c += cd;
		} while (c >= ce_p[ndim]);
	} while (1);
}

#if 0

static inline void
_noise_mr(noise_t *c, noise_t *v, unsigned x, point_t qs, ucoord_t ndim, unsigned w, unsigned seed) {
	int i = DIM - 1 - ndim;
	ucoord_t ced = (1<<(CHUNK_Y*(ndim+1))), cd;
	noise_t *ce = c + ced;

	ced >>= CHUNK_Y;
	cd = ced << x;

	for (; c < ce; qs[i] += (1<<x), c += cd)
		if (ndim == 0) {
			get_v(v, qs, x, w, seed);
			noise_quad(c, v, x, w);
		} else
			_noise_mr(c, v, x, qs, ndim - 1, w, seed);

	qs[i] -= 1 << CHUNK_Y; // reset
}

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, point_t qs, unsigned w, unsigned seed)
{
	_noise_mr(c, v, x, qs, DIM - 1, w, seed);
}

#else

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, point_t qs, unsigned w, unsigned seed) {
	noise_t *ce_p[DIM];
	noise_t ced = 1<<(CHUNK_Y * DIM);
	coord_t *qsc = qs; // quad coordinate
	ucoord_t ndim = DIM - 1;

	goto start;

	do {
		do { // PUSH
			qsc++; ndim--;
start:			ce_p[ndim] = c + ced;
			ced >>= CHUNK_Y;
		} while (ndim);

		do {
			get_v(v, qs, x, w, seed);
			noise_quad(c, v, x, w);
			*qsc += 1<<x;
			c += ced<<x;
		} while (c < ce_p[ndim]);

		do { // POP
			*qsc -= 1 << CHUNK_Y;
			ced <<= CHUNK_Y;
			c += (ced<<x) - ced; // reset and inc
			qsc--; ndim++;
			*qsc += 1<<x;
		} while (c >= ce_p[ndim]);
	} while (ndim < DIM);
}

#endif

/* fixes v (vertex values)
 * when noise quad starts before matrix quad aka x > y aka d > l.
 * assumes ndim to be at least 1
 * vl = 1 << ndim
 */

static inline void
__fix_v(noise_t *v, snoise_t *st, coord_t *ms, coord_t *qs, unsigned x, ucoord_t vl) {
	register noise_t *vn = v + vl;
	register ucoord_t dd = (*ms - *qs) >> CHUNK_Y;

	// TODO make this more efficient
	calc_steps(st, v, x, vl, CHUNK_Y);

	if (dd) /* ms > qs */
		step(v, st, vl, dd);

	memcpy(vn, v, sizeof(noise_t) * vl); // COPY to opposite values
	step(vn, st, vl, 1); // STEP by side length of M
}

// yes this does work fine a bit of a hack but ok i guess
static inline void
fix_v(noise_t *v,
       coord_t *qs,
       coord_t *ms,
       unsigned x)
{
	ucoord_t vl = 1 << DIM;
	snoise_t st[vl - 1], *stc = st;
	int ndim = DIM - 1;
	int first = 1;
	vl >>= 1;

	for (;;) {
		__fix_v(v, stc, ms, qs, x, vl);

		if (ndim) {
			ndim--; qs++; ms++; vl>>=1; stc+=vl; // PUSH
		} else if (first) {
			v += vl<<1;
			first = 0;
		} else break;
	}
}

static inline void
get_s(point_t s, point_t p, ucoord_t mask) {
	POOP s[I] = ((coord_t) p[I]) & mask;
}

/* Fills in matrix "mat" with deterministic noise.
 * "ms" is the matrix's min point in the world;
 * "x" 2^x = side length of noise quads;
 * "y" 2^y = side length of matrix "mat".
 * */

static inline void
noise_m(noise_t *mat, point_t ms, unsigned x,
	unsigned w, unsigned seed)
{
	noise_t v[v_total];

	if (CHUNK_Y > x)
		_noise_m(mat, v, x, ms, w, seed);
	else {
		point_t qs;

		get_s(qs, ms, COORDMASK_LSHIFT(x));
		get_v(v, qs, x, w, seed);
		if (x > CHUNK_Y)
			fix_v(v, qs, ms, x);

		noise_quad(mat, v, CHUNK_Y, w); }
}

static inline void
noise_oct(noise_t *m, point_t s, size_t oct_n, octave_t *oct, unsigned seed)
{
	octave_t *oe;

	memset(m, 0, sizeof(noise_t) * CHUNK_M);

	for (oe = oct + oct_n; oct < oe; oct++)
		noise_m(m, s, oct->x, oct->w, seed);
}

static inline unsigned
bio_idx(ucoord_t rn, coord_t tmp)
{
	if (tmp > tmp_max)
		return 1 + 3 * tmp_vary;
	else if (tmp < tmp_min)
		return 1;

	if (rn > rn_max)
		rn = rn_max;
	else if (rn < rn_min)
		rn = rn_min;

	ucoord_t rn_bit = 3 * (rn - rn_min) / (rn_max - rn_min);
	coord_t tmp_bit = 3 * tmp_vary * (tmp - tmp_min) / (tmp_max - tmp_min);
	return 1 + rn_bit + tmp_bit;
}

static inline int
noise_plant(unsigned char *plid, unsigned char *pln, struct bio *b, noise_t v, unsigned char n)
{
	struct plant *pl = &plants[n];

	CBUG(n >= plid_max);

	if (((v >> 6) ^ (v >> 3) ^ v) & 1)
		return 0;

	if (v & 0x18
	    && b->tmp < pl->tmp_max && b->tmp > pl->tmp_min
	    && b->rn < pl->rn_max && b->rn > pl->rn_min) {
		*plid = n;
		v = (v >> 1) & TREE_N_MASK;
		if (v == 3)
			v = 0;
		*pln = v;
		return 1;
	}

	return 0;
}

static inline unsigned char
noise_plants(unsigned char *plid, unsigned char *pln, struct bio *b, unsigned n, unsigned o)
{
	noise_t v = b->ty;
	register int i;
	unsigned char *plidc = plid;

	memset(plid, 0, n);
	memset(pln, 0, n);

	for (i = o;
	     i < plid_max && plidc < plid + n;
	     i++, v >>= 8) {
		if (!v)
			v = ~b->ty >> 4;

		if (noise_plant(plidc, pln, b, v, i)) {
			plidc++;
			pln++;
		}
	}

	return i;
}

static inline void
shuffle(unsigned char plid[3], unsigned char pln[3], noise_t v)
{
	register unsigned char aux;

	if (v & 1) {
		aux = plid[0];
		plid[0] = plid[1];
		plid[1] = aux;

		aux = pln[0];
		pln[0] = pln[1];
		pln[1] = aux;
	}

	if (v & 2) {
		aux = plid[1];
		plid[1] = plid[2];
		plid[2] = aux;

		aux = pln[1];
		pln[1] = pln[2];
		pln[2] = aux;
	}
}

unsigned char
noise_rplants(unsigned char plid[EXTRA_TREE], unsigned char pln[EXTRA_TREE],
		  struct bio *b)
{
	return noise_plants(plid, pln, b, EXTRA_TREE, b->mplid);
}

static inline void
noise_full(size_t i, point_t s, ucoord_t obits)
{
	struct bio *bio = &chunks_bio_raw[i];
	static octave_t
		x1[] = {{ 7, 2 }, { 6, 2 }, { 5, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }},
		x2[] = {{ 5, 1 }, { 3, 3 }, { 1, 2 }, { 2, 3 }},
		x3[] = {{ 5, 1 }, { 8, 1 }};

	int j;

	NOISE_OCT(n_he, x1, 55 + obits);
	NOISE_OCT(n_cl, x2, 1 + obits);
	NOISE_OCT(n_tm, x3, 53 + obits);

	for (j = 0; j < CHUNK_M; j++) {
		/* x_pos s[1] + (j % (1 << CHUNK_Y)); */
		struct bio *r = &bio[j];
		noise_t _cl = n_cl[j], _tm = n_tm[j];
		register noise_t _he = n_he[j], w = water_level(obits);
		r->rn = rain(obits, w, _he, _cl);
		r->tmp = temp(obits, _he, _tm, s[Y_COORD] + (j >> CHUNK_Y));
		r->ty = XXH(&_tm, sizeof(noise_t), PLANTS_SEED);
		r->bio_idx = _he < w ? 0 : bio_idx(r->rn, r->tmp);
		r->mplid = noise_plants(r->plid, r->pln, r, 3, 0);
		shuffle(r->plid, r->pln, ~(r->ty >> 8));
	}
}

static inline morton_t
view_idx(point_t pos)
{
	CBUG(chunks_r.s[Y_COORD] > pos[Y_COORD]);
	return (pos[Y_COORD] - chunks_r.s[Y_COORD]) * chunks_r.l[X_COORD]
		+ pos[X_COORD] - chunks_r.s[X_COORD];
}

/* im pretty sures squares must be involved */

static inline void
view_print(struct bio *to, point_t pos)
{
	point_t vprs = {
		pos[0] - VIEW_AROUND,
		pos[1] - VIEW_AROUND,
	};
	register struct bio
		*bo = bio + view_idx(vprs),
		*bd = to;
	register int y, x;
	ucoord_t ol;

	ol = chunks_r.l[WDIM];
	CBUG(ol < CHUNK_SIZE);

	for (y = 0; y < VIEW_SIZE; y++, bo += ol - VIEW_SIZE)
		for (x = 0; x < VIEW_SIZE; x++, bd++, bo++)
			*bd = *bo;
}

static inline void __attribute__((hot))
spread(unsigned ny)
{
	static const size_t bio_line_size = sizeof(struct bio) << CHUNK_Y;
	static const unsigned nx = 2;
	unsigned w;
	int p;

	for (w = 0; w < ny * nx * CHUNK_M; w += nx * CHUNK_M)
		for (p = 0; p <= nx * CHUNK_M; p += nx << CHUNK_Y) {
			memcpy(&chunks_bio[w + p],
			       &chunks_bio_raw[w + (p >> 1)],
			       bio_line_size);

			memcpy(&chunks_bio[w + p + CHUNK_SIZE],
			       &chunks_bio_raw[w + (p >> 1) + CHUNK_M],
			       bio_line_size);
		}
}

static inline void
noise_chunks(point_t pos, ucoord_t obits)
{
	struct rect r;
	size_t n[DIM], i;
	point_t s, vprs = {
		pos[0] - VIEW_AROUND,
		pos[1] - VIEW_AROUND
	};

	get_s(r.s, vprs, COORDMASK_LSHIFT(CHUNK_Y));

	POOP {
		n[I] = ((VIEW_SIZE + vprs[I] - r.s[I]) >> CHUNK_Y) + 1;
		r.l[I] = n[I] << CHUNK_Y;
	}

	if (obits == chunks_obits
            && r.s[0] >= chunks_r.s[0]
	    && r.s[1] >= chunks_r.s[1]
	    && r.s[0] + r.l[0] <= chunks_r.s[0] + chunks_r.l[0]
	    && r.s[1] + r.l[1] <= chunks_r.s[1] + chunks_r.l[1])
		return;

	i = 0;

	for (s[Y_COORD] = r.s[Y_COORD];
	     s[Y_COORD] < r.s[Y_COORD] + r.l[Y_COORD];
	     s[Y_COORD] += CHUNK_SIZE)

		for (s[X_COORD] = r.s[X_COORD];
		     s[X_COORD] < r.s[X_COORD] + r.l[X_COORD];
		     s[X_COORD] += CHUNK_SIZE)

		{
			noise_full(i, s, obits);
			i += CHUNK_M;
		}

	bio = chunks_bio_raw;

	if (n[X_COORD] > 1) {
		CBUG(n[X_COORD] != 2);
		spread(n[Y_COORD]);
		bio = chunks_bio;
	}

	memcpy(&chunks_r, &r, sizeof(r));
}

struct bio empty[VIEW_M] = {
	[0 ... VIEW_M - 1] = { .bio_idx = BIOME_VOLCANIC },
};

void
noise_view(struct bio to[VIEW_M], point_t pos, ucoord_t obits)
{
	if (pos[2] == 0) {
		noise_chunks(pos, obits);
		view_print(to, pos);
	} else
		memcpy(to, &empty, sizeof(empty));
}

struct bio *
noise_point(morton_t p)
{
	point3D_t pos;
	morton_decode(pos, p);
	noise_chunks(pos, OBITS(p));
	return &bio[view_idx(pos)];
}

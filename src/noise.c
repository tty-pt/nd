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

#include "noise.h"
/* #include "morton.h" */
#include <string.h>
#include "xxhash.h"
#include "params.h"

#define XXH XXH32

#define CHUNK_Y 5
#define CHUNK_SIZE (1 << CHUNK_Y)
#define CHUNK_M (CHUNK_SIZE * CHUNK_SIZE)

/* gets a mask for use with get_s */
#define COORDMASK_LSHIFT(x) (ucoord_t) ((((ucoord_t) -1) << x))
#define NOISE_MAX ((noise_t) -1)

typedef struct { unsigned x, w; } octave_t;

struct tilemap {
	struct rect r;
	struct bio *bio;
};

static size_t v_total = (1<<(DIM+1)) - 1;
static ucoord_t chunk_mask = COORDMASK_LSHIFT(CHUNK_Y);

/* WORLD GEN STATIC VARS {{{ */
static const noise_t noise_fourth = (NOISE_MAX >> 2);
static const noise_t noise_eighth = (NOISE_MAX >> 3);

static coord_t tmp_max = 170,
	       tmp_min = -41;

static ucoord_t rn_max = 1000,
	       rn_min = 50;

/* BIOMES {{{ */
struct biome const biomes[] = {
	{	// Water
		{ "", "", "" },
		ANSI_BOLD ANSI_FG_WHITE "~~~" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_WHITE "~",
		ANSI_BG_BLUE,
	},

	// ----

	{	// Cold dry desert
		{ "Pile of frozen rocks", "", "" },
		ANSI_BOLD ANSI_FG_WHITE "oOo" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_WHITE "o",
		ANSI_BG_BLUE,
	},

	{	// Cold stone
		{ "Block of Ice", "", "" },
		ANSI_BOLD ANSI_FG_GREEN "oOo" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_GREEN "o",
		ANSI_BG_CYAN,
	},

	{	// Cold snowy rock
		{ "Bear", "", "" },
		ANSI_BOLD ANSI_FG_YELLOW " 8 " ANSI_RESET_BOLD,
		" ",
		ANSI_BG_WHITE,
	},

	// ----

	{	// Tundra
		{ "Frozen Pine Tree", "", "" },
		ANSI_BOLD ANSI_FG_WHITE "xXx" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_WHITE "x",
		ANSI_BG_CYAN,
	},

	{	// Taiga
		{ "Pine Tree", "", "" },
		ANSI_BOLD ANSI_FG_GREEN "xXx" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_GREEN "x",
		ANSI_BG_GREEN,
	},

	{	// Temperate rain forest
		{ "Tall Tree", "", "" },
		ANSI_BOLD ANSI_FG_GREEN "tTt" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_GREEN "t",
		ANSI_BG_GREEN,
	},

	// ----

	{	// Woodland / Grassland / Shrubland
		{ "Shrub", "", "" },
		ANSI_FG_YELLOW "xXx",
		ANSI_FG_YELLOW "x",
		ANSI_BG_GREEN,
	},

	{
		{ "Tree", "", "" },
		ANSI_FG_YELLOW "xXx",
		ANSI_FG_YELLOW "x",
		ANSI_BG_GREEN,
	},

	{	// Temperate forest
		{ "Tree", "", "" },
		ANSI_BOLD ANSI_FG_GREEN "xXx" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_GREEN "x",
		ANSI_BG_GREEN,
	},

	// ----

	{	// Desert
		{ "Cactus", "", "" },
		ANSI_FG_GREEN "ili",
		ANSI_FG_GREEN "i",
		ANSI_BG_YELLOW,
	},

	{	// Savannah
		{ "Acacia Tree", "", "" },
		ANSI_FG_GREEN "tTt",
		ANSI_FG_GREEN "t",
		ANSI_BG_YELLOW,
	},

	{	// Temperate Seasonal forest
		{ "Red Tree", "", "" },
		ANSI_FG_RED "xXx",
		ANSI_FG_RED "x",
		ANSI_RESET,
	},

	// ---

	{	// Volcanic
		{ "Puddle of lava", "", "" },
		ANSI_BOLD ANSI_FG_RED "oOo" ANSI_RESET_BOLD,
		ANSI_BOLD ANSI_FG_RED "o",
		ANSI_RESET,
	},

	/* {	// Hot springs */
	/* 	"Hot spring", */
	/* 	ANSI_BOLD ANSI_FG_WHITE " ~ ", */
		/* ANSI_BG_CYAN, */
	/* 	" ", */
	/* }, */
};
/* }}} */

static const size_t tmp_vary = (sizeof(biomes) / sizeof(struct biome) - 1) / 3;

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
temp(ucoord_t obits, noise_t he, noise_t tm, coord_t y) // fahrenheit * 10
{
	smorton_t x = 0;
	x = he - water_level(obits) + (tm - NOISE_MAX) / 3;
	// change 200 to affect how
	// terrain height decreases temperature
	x /= 4 * UCOORD_MAX * 260;
	x += (tmp_max<<1) - tmp_min // min temp of hot planet
		- (tmp_max - tmp_min) * sun_dist(obits);
	x += ((y > 0 ? y : -y) * tmp_min) / 6300;
	return tmp_min + x;
}

static inline unsigned
_tree_count(ucoord_t rn, coord_t tmp, noise_t tr)
{
	if (tr > noise_fourth
	    + (2 * tmp / 3 - 2 * rn) * noise_eighth
	    && tr & 1)

		return (rn^tmp) & 7;
	else
		return 0;
}

/* }}} */

/* gets a (deterministic) random value for point p */

static noise_t
r(point_t p, unsigned seed, unsigned w)
{
	/* morton_t m = morton2D_point(p); */
	/* return XXH(&m, sizeof(morton_t), seed); */
	register noise_t v = XXH(p, sizeof(point_t), seed);
	return ((long long unsigned) v) >> w;
}

/* generate deterministic random value
 * for each vertex of the quad of side length 2^x
 * that starts in point s,
 * and store in v in a particular order
 * (active bits of the current index)
 * */

static void
get_v(noise_t v[1 << DIM], point_t s, ucoord_t x, unsigned w, unsigned seed)
{
	int i, j, k;
	point_t vp;

	// (0 <= i < 2^DIM)
	for (i = 0; i < (1 << DIM); i++) {

		// for each coordinate
		for (j = 0, k = 1 << (DIM - 1);
		     j < DIM;
		     j++, k >>= 1)

			vp[j] = s[j] + (!!(i & k) << x);

		v[i] = r(vp, seed, w);
	}
}

/* The following call:
 * get_v(v, s, x), DIM = 2
 * outputs
 * v[0] = r(s)
 * v[1] = r(s + {d, 0})
 * v[2] = r(s + {0, d})
 * v[3] = r(s + {d, d})
 *
 * DIM = 3:
 * v[0] = r(s)
 * v[1] = r(s + {d, 0, 0})
 * ...
 * v[7] = r(s + {d, d, d})
 */

static inline snoise_t
calc_step(noise_t v1, noise_t v0, ucoord_t z, unsigned y)
{
	// FIXME FIND PARENS
	return ((long) v1 - v0) >> z << y;
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
		st[n] = calc_step(v[n + vl], v[n], z, y);
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
noise_quad(noise_t *c, noise_t *vc, unsigned z, unsigned y, unsigned w)
{
	ucoord_t ndim = DIM - 1;
	ucoord_t tvl = 1<<ndim; // length of input values
	snoise_t st[(tvl<<1) - 1], *stc = st;
	noise_t *ce_p[DIM], *vt;
	size_t cd = 1 << y * ndim;	/* (2^y)^ndim */

	goto start;

	do {
		do {
			// PUSH
			cd >>= y; ndim--; vc = vt; stc += tvl; tvl >>= 1;

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
			c -= cd<<z; cd <<= y; vt = vc;
			tvl <<= 1; stc -= tvl; vc -= (tvl<<1);

			step(vt, stc, tvl, 1);
			c += cd;
		} while (c >= ce_p[ndim]);
	} while (1);
}

#if 1

static inline void
_noise_mr(noise_t *c, noise_t *v, unsigned x, unsigned y, point_t qs, ucoord_t ndim, unsigned w, unsigned seed) {
	int i = DIM - 1 - ndim;
	ucoord_t ced = (1<<(y*(ndim+1))), cd;
	noise_t *ce = c + ced;

	ced>>=y;
	cd = ced<<x;

	for (; c < ce; qs[i] += (1<<x), c += cd)
		if (ndim == 0) {
			get_v(v, qs, x, w, seed);
			noise_quad(c, v, x, y, w);
		} else
			_noise_mr(c, v, x, y, qs, ndim - 1, w, seed);

	qs[i] -= 1<<y; // reset
}

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, unsigned y, point_t qs, unsigned w, unsigned seed)
{
	_noise_mr(c, v, x, y, qs, DIM - 1, w, seed);
}

#else

static inline void
_noise_m(noise_t *c, noise_t *v, unsigned x, unsigned y, point_t qs, unsigned w, unsigned seed) {
	noise_t *ce_p[DIM];
	noise_t ced = 1<<(y*DIM);
	coord_t *qsc = qs;		// quad coordinate
	ucoord_t ndim = DIM - 1;

	goto start;

	do {
		do {
			qsc++; ndim--; // PUSH
start:			ce_p[ndim] = c + ced;
			ced >>= y; // PUSH
		} while (ndim);

		do {
			get_v(v, qs, x, w, seed);
			noise_quad(c, v, x, y, w);
			*qsc += 1<<x;
			c += ced<<x;
		} while (c < ce_p[ndim]);

		do {
			*qsc -= 1<<y;
			ced <<= y; // POP
			c += (ced<<x) - ced; // reset and inc c
			qsc--; ndim++; // POP
			*qsc += 1<<x; // inc quad coord
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
__fix_v(noise_t *v, snoise_t *st, coord_t *ms, coord_t *qs, unsigned x, unsigned y, ucoord_t vl) {
	register noise_t *vn = v + vl;
	register ucoord_t dd = (*ms - *qs)>>y;

	// TODO make this more efficient
	calc_steps(st, v, x, vl, y);

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
       unsigned x,
       unsigned y)
{
	ucoord_t vl = 1 << DIM;
	snoise_t st[vl - 1], *stc = st;
	int ndim = DIM - 1;
	int first = 1;
	vl >>= 1;

	for (;;) {
		__fix_v(v, stc, ms, qs, x, y, vl);

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
	unsigned y, unsigned w, unsigned seed)
{
	noise_t v[v_total];

	/* DEBUG("M x=%u y=%u ms={%d,%d} c={%p:%p}\n", x, y, ms[0], ms[1], mat, mat+(1<<y*DIM)); */
	if (y > x)
		_noise_m(mat, v, x, y, ms, w, seed);
	else {
		point_t qs;

		get_s(qs, ms, COORDMASK_LSHIFT(x));
		get_v(v, qs, x, w, seed);
		if (x > y)
			fix_v(v, qs, ms, x, y);

		noise_quad(mat, v, y, y, w);
	}
}

static inline void
noise_oct(noise_t *m, point_t s, size_t oct_n, octave_t *oct, unsigned y, unsigned seed)
{
	octave_t *oe;

	memset(m, 0, sizeof(noise_t) << DIM * y);

	for (oe = oct + oct_n; oct < oe; oct++)
		noise_m(m, s, oct->x, y, oct->w, seed);
}

static unsigned
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

static inline struct bio
bio_init(ucoord_t obits, noise_t he, noise_t cl, noise_t tr, noise_t tm, coord_t y)
{
	struct bio r;
	noise_t w = water_level(obits);
	r.he = he;
	r.cl = cl;
	r.cl = cl;
	r.tm = tm;
	r.rn = rain(obits, w, he, cl);
	r.tmp = temp(obits, he, tm, y);
	if (he < w)
		r.tree_n = r.bio_idx = 0;
	else {
		r.tree_n = _tree_count(r.rn, r.tmp, tr);
		r.bio_idx = bio_idx(r.rn, r.tmp);
	}
	return r;
}

#define NOISE_OCT(to, x, y, seed) \
	noise_oct(to, s, sizeof(x) / sizeof(octave_t), x, y, seed)

static void noise_full(struct bio *bio, size_t y, point_t s, ucoord_t obits)
{
	static octave_t
		x1[] = {{ 7, 2 }, { 6, 2 }, { 5, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }},
		x2[] = {{ 5, 1 }, { 3, 3 }, { 1, 2 }, { 2, 3 }},
		x3[] = {{ 1, 1 }, { 2, 2 }},
		x4[] = {{ 5, 1 }, { 8, 1 }};

	unsigned const m = (1 << y) * (1 << y);

	noise_t he[m];
	noise_t cl[m];
	noise_t tr[m];
	noise_t tm[m];

	int i;

	NOISE_OCT(he, x1, y, 55 + obits);
	NOISE_OCT(cl, x2, y, 51 + obits);
	NOISE_OCT(tr, x3, y, 52 + obits);
	NOISE_OCT(tm, x4, y, 53 + obits);

	for (i = 0; i < m; i++)
		bio[i] = bio_init(obits, he[i], cl[i], tr[i], tm[i], s[0] + i / CHUNK_SIZE);
}

static morton_t
tilemap_idx(struct rect s, struct rect b)
{
	morton_t r = s.s[1] - b.s[1];
	if (b.s[0] > s.s[0])
		return (b.s[0] - s.s[0]) * s.l[1] + r;
	else
		return (s.s[0] - b.s[0]) * b.l[1] + r;
}

static void
tilemap_print(struct tilemap d, struct tilemap o)
{
	register ucoord_t dl = d.r.l[WDIM];
	register ucoord_t ol = o.r.l[WDIM];
	register ucoord_t wd1, wd2;
	int y, x;
	struct bio *bo = o.bio,
		   *bd = d.bio;
	int ny, nx;

	// FIXME relying on unsigned arithmetic
	// only supports squares?
	if (dl > ol) {
		wd2 = 0;
		wd1 = dl - ol;
		bd += tilemap_idx(o.r, d.r);
		ny = ol;
		nx = ol;
	} else {
		wd1 = 0;
		wd2 = ol - dl;
		if (o.r.l[0] > d.r.l[0])
			bo += tilemap_idx(d.r, o.r);
		else
			bd += tilemap_idx(d.r, o.r);
		ny = dl;
		nx = dl;
	}

	for (y = 0; y < ny; y++, bd += wd1, bo += wd2)
		for (x = 0; x < nx; x++, bd++, bo++)
			*bd = *bo;
}

static struct rect
tilemap_rect_contain_n(struct tilemap *tm, size_t n)
{
	int i = 0;
	struct rect r, aux;
	memcpy(&r, &tm[i].r, sizeof(struct rect));
	do {
		i++;
		if (i >= n)
			break;
		memcpy(&aux, &tm[i].r, sizeof(aux));
		r = rect_contain2(r, aux);
	} while (1);
	return r;
}

static inline struct tilemap
tilemap_cat(struct bio *to, struct tilemap *tm, size_t n)
{
	struct tilemap c;
	int i;

	c.r = tilemap_rect_contain_n(tm, n);
	c.bio = to;

	for (i = 0; i < n; i++)
		tilemap_print(c, tm[i]);

	return c;
}

struct bio
noise_point(morton_t p)
{
	struct bio res;
	point3D_t s;
	ucoord_t obits = OBITS(p);
	morton3D_decode(s, p);
	noise_full(&res, 0, s, obits);
	return res;
}

static inline void
noise_chunk(struct tilemap *res, point_t s, ucoord_t obits)
{
	POOP {
		res->r.s[I] = s[I];
		res->r.l[I] = CHUNK_SIZE;
	}

	noise_full(res->bio, CHUNK_Y, s, obits);
}

static inline struct tilemap
_noise_chunks(struct bio *to, struct rect cr, size_t n, ucoord_t obits)
{
	struct bio bio[CHUNK_M * n];
	struct tilemap chunks[n];
	point_t s;
	int i;

	for (s[0] = cr.s[0], i = 0;
	     s[0] < cr.s[0] + cr.l[0];
	     s[0] += CHUNK_SIZE)

		for (s[1] = cr.s[1];
		     s[1] < cr.s[1] + cr.l[1];
		     s[1] += CHUNK_SIZE, i++)
		{
			chunks[i] = (struct tilemap) {
				{ { s[0], s[1], }, { CHUNK_SIZE, CHUNK_SIZE }, },
					&bio[i * CHUNK_M],
			};

			noise_chunk(&chunks[i], s, obits);
		}

	return tilemap_cat(to, chunks, n);
}

static inline struct tilemap
noise_chunks(struct bio *to, point_t vprs, ucoord_t obits)
{
	struct rect cr;
	size_t nn = 1;
	get_s(cr.s, vprs, chunk_mask);
	POOP {
		size_t n = ((VIEW_SIZE + vprs[I] - cr.s[I]) >> CHUNK_Y) + 1;
		nn *= n;
		cr.l[I] = n * CHUNK_SIZE;
	}
	return _noise_chunks(to, cr, nn, obits);
}

void
noise_view(struct bio to[VIEW_M], point_t vprs, ucoord_t obits)
{
	struct bio chunks[CHUNK_M * 4];
	struct tilemap inters = {
		{ { vprs[0], vprs[1], }, { VIEW_SIZE, VIEW_SIZE } },
		to,
	};
	struct tilemap chunks_tm = noise_chunks(chunks, vprs, obits);
	tilemap_print(inters, chunks_tm);
}

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
#define NOISE_IMPLEMENTATION
#include "nd/noise.h"
#include <string.h>
#include "xxhash.h"
#include "debug.h"

#define HASH XXH32

#define CHUNK_Y 5
#define CHUNK_SIZE (1 << CHUNK_Y)
#define CHUNK_M (CHUNK_SIZE * CHUNK_SIZE)

#define PLANTS_SEED 5

#define NOISE_OCT(to, x, seed) \
	noise_oct(to, s, sizeof(x) / sizeof(octave_t), x, seed, CHUNK_Y, 2)

struct tilemap {
	struct rect r;
	struct bio *bio;
};

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

/* WORLD GEN STATIC VARS {{{ */

static coord_t tmp_max = 170,
	       tmp_min = -41;

#define RAIN_DIV (NOISE_MAX >> 9)
static noise_t rn_max = NOISE_MAX / RAIN_DIV;

/* }}} */

/* WORLD GEN PRIMITIVES {{{ */
static inline noise_t
water_level(ucoord_t obits)
{
	// returns (0-3) * (NOISE_MAX / 4)
	return (((obits >> 2) + 2) & 3) * noise_fourth;
}

static inline unsigned
rain(ucoord_t obits, noise_t w, noise_t he, noise_t cl, noise_t tmp)
{
	noise_t x = cl / RAIN_DIV;
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
	snoise_t x = 0;
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

static inline unsigned
bio_idx(ucoord_t rn, coord_t tmp)
{
	if (tmp < tmp_min)
		return BIOME_PERMANENT_ICE;
	else if (tmp >= tmp_max)
		return BIOME_VOLCANIC;

	ucoord_t rn_bit = (4 * rn / rn_max) & 3;
	coord_t tmp_bit = (4 * (tmp - tmp_min) / (tmp_max - tmp_min)) & 3;
	unsigned result = 2 + rn_bit + (tmp_bit << 2);
	return result;
}

static inline void
noise_full(size_t i, point_t s, ucoord_t obits)
{
	struct bio *bio = &chunks_bio_raw[i];
	static octave_t
		x1[] = {{ 7, 2 }, { 6, 2 }, { 5, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }},
		/* x2[] = {{ 8, 1 }, { 7, 2 }, { 6, 2 }, { 5, 3 }, { 3, 3 }}, */
		x4[] = {{ 5, 1 }, { 2, 1 }},
		x3[] = {{ 5, 1 }, { 8, 1 }};

	int j;

	NOISE_OCT(n_he, x1, 55 + obits);
	NOISE_OCT(n_cl, x4, 1 + obits);
	NOISE_OCT(n_tm, x3, 53 + obits);

	/* fprintf(stderr, "noise max: %u, rain max: %u\n", NOISE_MAX, rn_max); */

	for (j = 0; j < CHUNK_M; j++) {
		/* x_pos s[1] + (j % (1 << CHUNK_Y)); */
		struct bio *r = &bio[j];
		noise_t _cl = n_cl[j], _tm = n_tm[j];
		register noise_t _he = n_he[j], w = water_level(obits);
		r->tmp = temp(obits, _he, _tm, s[Y_COORD] + (j >> CHUNK_Y));
		r->rn = rain(obits, w, _he, _cl, r->tmp);
		r->bio_idx = _he < w ? 0 : bio_idx(r->rn, r->tmp);
		r->pd.max = 0;
		if (_he > w) {
			r->ty = HASH(&_tm, sizeof(noise_t), PLANTS_SEED);
			plants_noise(&r->pd, r->ty, r->tmp, r->rn, 3);
			plants_shuffle(&r->pd, ~(r->ty >> 8));
		} else {
			memset(r->pd.id, 0, 3);
			r->pd.n = 0;
		}
	}
}

static inline noise_t
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
noise_chunks(point_t pos)
{
	struct rect r;
	size_t n[DIM], i;
	point_t s, vprs = {
		pos[0] - VIEW_AROUND,
		pos[1] - VIEW_AROUND
	};

	noise_get_s(r.s, vprs, COORDMASK_LSHIFT(CHUNK_Y), 2);

	POOP {
		n[I] = ((VIEW_SIZE + vprs[I] - r.s[I]) >> CHUNK_Y) + 1;
		r.l[I] = n[I] << CHUNK_Y;
	}

	if (pos[3] == chunks_obits
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
			noise_full(i, s, pos[3]);
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
noise_view(struct bio to[VIEW_M], pos_t pos)
{
	if (pos[2] == 0) {
		noise_chunks(pos);
		view_print(to, pos);
	} else
		memcpy(to, &empty, sizeof(empty));
}

struct bio *
noise_point(pos_t p)
{
	noise_chunks(p);
	return &bio[view_idx(p)];
}

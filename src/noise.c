#include "noise.h"
#define NOISE_IMPLEMENTATION
#include "noise_decl.h"
#include <string.h>
#include "xxhash.h"

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
static const uint32_t noise_fourth = (NOISE_MAX >> 2);
static uint32_t n_he[CHUNK_M],
	       n_cl[CHUNK_M],
	       n_tm[CHUNK_M];

extern unsigned plant_hd;

/* REGARDING DRAWABLE THINGS THAT DEPEND ON NOISE {{{ */

/* WORLD GEN STATIC VARS {{{ */

static coord_t tmp_max = 170,
	       tmp_min = -41;

#define RAIN_DIV (NOISE_MAX >> 9)
static uint32_t rn_max = NOISE_MAX / RAIN_DIV;

/* }}} */

/* WORLD GEN PRIMITIVES {{{ */
static inline uint32_t
water_level(ucoord_t obits)
{
	// returns (0-3) * (NOISE_MAX / 4)
	return (((obits >> 2) + 2) & 3) * noise_fourth;
}

static inline unsigned
rain(
		ucoord_t obits __attribute__((unused)),
		uint32_t w __attribute__((unused)),
		uint32_t he __attribute__((unused)),
		uint32_t cl,
		uint32_t tmp __attribute__((unused)) )
{
	uint32_t x = cl / RAIN_DIV;
	return x;
}

static inline uint32_t
sun_dist(ucoord_t obits)
{
	// returns (1-4)
	return ((obits + 1) & 3);
}

static inline coord_t
temp(ucoord_t obits, uint32_t he, uint32_t tm, coord_t pos_y) // fahrenheit * 10
{
	int32_t x = 0;
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

/**
 * gives you an index from 0 to 15 that maps in 2D:
 *
 * 	  cold
 *      0  1  2  3
 *      4  5  6  7
 * dry  8  9 10 11 wet
 *     12 13 14 15
 * 	   hot
 */
unsigned
_bio_idx(
	coord_t tmp_floor,
	coord_t tmp_ceil,
	ucoord_t rn_floor,
	ucoord_t rn_ceil,
	coord_t tmp,
	ucoord_t rn)
{
	ucoord_t rn_bit = (4 * (rn - rn_floor) / (rn_ceil - rn_floor)) & 3;
	coord_t tmp_bit = (4 * (tmp - tmp_floor) / (tmp_ceil - tmp_floor)) & 3;
	return rn_bit + (tmp_bit << 2);
}

static inline unsigned
bio_idx(ucoord_t rn, coord_t tmp)
{
	if (tmp < tmp_min)
		return BIOME_PERMANENT_ICE;
	else if (tmp >= tmp_max)
		return BIOME_VOLCANIC;
	return 2 + _bio_idx(tmp_min, tmp_max, 0, rn_max, tmp, rn);
}

static inline unsigned char
plant_noise(unsigned *plid, coord_t tmp, ucoord_t rn, uint32_t v, unsigned plant_ref)
{
	SKEL skel;
	qdb_get(skel_hd, &skel, &plant_ref);
	SPLA *pl = &skel.sp.plant;

        if (v >= (NOISE_MAX >> pl->y))
                return 0;
	/* if (((v >> 6) ^ (v >> 3) ^ v) & 1) */
	/* 	return 0; */

	if (tmp < pl->tmp_max && tmp > pl->tmp_min
	    && rn < pl->rn_max && rn > pl->rn_min) {
		*plid = plant_ref;
		v = (v >> 1) & PLANT_MASK;
		if (v == 3)
			v = 0;
                return v;
	}

	return 0;
}

static inline void
plants_noise(struct plant_data *pd, uint32_t ty, coord_t tmp, ucoord_t rn, unsigned n)
{
	uint32_t v = ty;
	register int cpln;
	unsigned *idc = pd->id;
	qdb_cur_t c = qdb_iter(plant_hd, NULL);
	unsigned ign, ref;
	unsigned pdn = 0;

	while (qdb_next(&ign, &ref, &c)) {
		if (idc >= &pd->id[n]) {
			qdb_fin(&c);
			break;
		}

		if (!v)
			v = XXH32((const char *) &ty, sizeof(ty), ref);

		cpln = plant_noise(idc, tmp, rn, v, ref);

		if (cpln) {
			pdn = (pdn << 2) | (cpln & 3);
			idc++;
		}

		v >>= 8;
	}
	pd->max = *idc;
	pd->n = pdn;
}

static void
plants_shuffle(struct plant_data *pd, morton_t v)
{
        unsigned char apln[3] = {
                pd->n & 3,
                (pd->n >> 2) & 3,
                (pd->n >> 4) & 3,
        };
	register unsigned char i;
	unsigned aux, pdn = 0;

        for (i = 1; i <= 3; i++) {
		pdn = pdn << 2;

                if (v & i)
                        continue;

                aux = pd->id[i - 1];
                pd->id[i - 1] = pd->id[i];
                pd->id[i] = aux;

                aux = apln[i - 1];
                apln[i - 1] = apln[i];
                apln[i] = aux;
        }

	pd->n = apln[0] | (apln[1] << 2) | (apln[2] << 4);
}

static inline void
noise_full(size_t i, point_t s, ucoord_t obits)
{
	static octave_t
		x1[] = {{ 7, 2 }, { 6, 2 }, { 5, 2 }, { 4, 3 }, { 3, 3 }, { 2, 3 }},
		/* x2[] = {{ 8, 1 }, { 7, 2 }, { 6, 2 }, { 5, 3 }, { 3, 3 }}, */
		x4[] = {{ 5, 1 }, { 2, 1 }},
		x3[] = {{ 5, 1 }, { 8, 1 }};

	int j;

	NOISE_OCT(n_he, x1, 55 + obits);
	NOISE_OCT(n_cl, x4, 1 + obits);
	NOISE_OCT(n_tm, x3, 53 + obits);

	/* syslog_err(LOG_ERR, "noise max: %u, rain max: %u", NOISE_MAX, rn_max); */

	for (j = 0; j < CHUNK_M; j++) {
		/* x_pos s[1] + (j % (1 << CHUNK_Y)); */
		struct bio r = chunks_bio_raw[i + j];
		uint32_t _cl = n_cl[j], _tm = n_tm[j];
		register uint32_t _he = n_he[j], w = water_level(obits);
		r.tmp = temp(obits, _he, _tm, s[Y_COORD] + (j >> CHUNK_Y));
		r.rn = rain(obits, w, _he, _cl, r.tmp);
		r.bio_idx = _he < w ? 0 : bio_idx(r.rn, r.tmp);
		r.pd.max = 0;
		if (_he > w) {
			r.ty = HASH(&_tm, sizeof(uint32_t), PLANTS_SEED);
			plants_noise(&r.pd, r.ty, r.tmp, r.rn, 3);
			plants_shuffle(&r.pd, ~(r.ty >> 8));
		} else {
			memset(r.pd.id, 0, 3);
			r.pd.n = 0;
		}
		chunks_bio_raw[i + j] = r;
	}
}

static inline uint32_t
view_idx(point_t pos)
{
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

	for (y = 0; y < VIEW_SIZE; y++, bo += ol - VIEW_SIZE)
		for (x = 0; x < VIEW_SIZE; x++, bd++, bo++)
			*bd = *bo;
}

static inline void __attribute__((hot))
spread(unsigned ny)
{
	static const size_t bio_line_size = sizeof(struct bio) << CHUNK_Y;
	static const unsigned nx = 2;
	unsigned w, p;

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

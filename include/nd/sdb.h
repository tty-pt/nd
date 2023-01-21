#ifndef SDB_H
#define SDB_H

// see http://www.vision-tools.com/h-tropf/multidimensionalrangequery.pdf
// TODO support duplicates
// TODO use cursors instead of a callback

#include <db4/db.h>
/* value returned when no position is found */
#define SDB_INVALID 130056652770671ULL

typedef void (*sdb_callback_t)(int16_t *pv, void *ptr);

typedef struct {
	DB *primary;
	DB *point;
	uint8_t dim, y;
	size_t ekl; // extra key length
	size_t vl; // value length
	sdb_callback_t callback;
	uint8_t max_dim;
	DBTYPE type;
} sdb_t;

int sdb_init(sdb_t *, uint8_t dim, uint8_t y, size_t ekl, size_t len, DB_ENV *dbe, char *fname, DBTYPE type);
void sdb_search(sdb_t *, int16_t *min, int16_t *max);
int sdb_close(sdb_t *);
void sdb_where(int16_t *p, sdb_t *, void *thing);
int sdb_delete(sdb_t *, void *what);
void *sdb_get(sdb_t *, int16_t *at);
void sdb_put(sdb_t *, void *thing, int flags);

#ifdef SDB_IMPLEMENTATION
#include "debug.h"

typedef uint64_t morton_t; // up to 4d morton
typedef int64_t smorton_t;

static const morton_t m1 = 0x111111111111ULL;
static const morton_t m0 = 0x800000000000ULL;

morton_t
pos_morton(int16_t *p, uint8_t dim)
{
	morton_t res = 0;
	int i, j;

	for (i = 0; i < dim; i++)
		for (j = 0; j < 16; j++) {
			register uint16_t v = (smorton_t) p[i] + SHRT_MAX;
			res |= ((morton_t) (v >> j) & 1) << (j * dim + i);
		}

	/* res <<= (4 - dim) * 16; */

	return res;
}

/* leave me alone */
void
morton_pos(int16_t *p, morton_t code, uint8_t dim)
{
	uint16_t up[dim];
	int i, j;

	for (i = 0; i < dim; i++) {
		up[i] = 0;
		for (j = 0; j < 16; j++) {
			int16_t v = (code >> (j * dim + i)) & 1;
			/* int16_t v = (code >> (j * dim + i + (4 - dim) * 16)) & 1; */
			up[i] |= v << j;
		}
	}

	for (i = 0; i < dim; i++) {
		p[i] = (smorton_t) up[i] - SHRT_MAX;
	}
}

static inline void
point_debug(int16_t *p, char *label, int8_t dim)
{
	warn("point %s", label);
	for (int i = 0; i < dim; i++)
		warn(" %d", p[i]);
	warn("\n");
}

static inline void
morton_debug(morton_t x, uint8_t dim)
{
	int16_t p[dim];
	morton_pos(p, x, dim);
	warn("%llx ", x);
	point_debug(p, "morton_debug", dim);
}

static inline int
morton_cmp(morton_t a, morton_t b, uint8_t dim, uint8_t cdim) {
	int	shift_start = 64 - (4 - dim) * 16 - (dim - cdim),
		shift;

	for (shift = shift_start; shift >= 0; shift -= dim) {
		if ((b >> shift) & 1) {
			if (!((a >> shift) & 1))
				return 1;
		}
		else if ((a >> shift) & 1)
			return -1;
	}

	return 0;
}

static inline morton_t
sdb_qload_0(morton_t c, morton_t lm0, morton_t lm1) // LOAD(0111...
{
	return (c & (~ lm0)) | lm1;
}

static inline morton_t
sdb_qload_1(morton_t c, morton_t lm0, morton_t lm1) // LOAD(1000...
{
	return (c | lm0) & (~ lm1);
}

static inline int
sdb_inrange(morton_t dr, morton_t min, morton_t max, uint8_t dim)
{
	for (int i = 0; i < dim; i++)
		if (morton_cmp(min, dr, dim, i) < 0
		    || morton_cmp(dr, max, dim, i) < 0)
		    return 0;

	return 1;
}

static inline void
sdb_compute_bmlm(morton_t *bm, morton_t *lm,
	     morton_t dr, morton_t min, morton_t max)
{
	register morton_t lm0 = m0, lm1 = m1;

	for (; lm0; lm0 >>= 1, lm1 >>= 1)
	{
		register morton_t a = dr & lm0,
			 b = min & lm0,
			 c = max & lm0;

		if (b) { // ? 1 ?
			if (!a && c) { // 0 1 1
				*bm = min;
				break;
			}

		} else if (a) // 1 0 ?
			if (c) { // 1 0 1
				*lm = sdb_qload_0(max, lm0, lm1);
				min = sdb_qload_1(min, lm0, lm1);
			} else { // 1 0 0
				*lm = max;
				break;
			}

		else if (c) { // 0 0 1
			// BIGMIN
			*bm = sdb_qload_1(min, lm0, lm1);
			max = sdb_qload_0(max, lm0, lm1);
		}
	}
}

static inline int
sdb_range_unsafe(sdb_t *sdb, morton_t min, morton_t max)
{
	morton_t lm = 0, code;
	DBC *cur;
	DBT key, pkey, data;
	int ret = 0, res = 0;
	int dbflags;

	/* warn("sdb_range_unsafe min %llx max %llx\n", min, max); */
	/* morton_debug(min, sdb->dim); */
	/* morton_debug(max, sdb->dim); */
	/* CBUG((smorton_t) min > (smorton_t) max); */
	if (sdb->point->cursor(sdb->point, NULL, &cur, 0))
		return -1;

	memset(&data, 0, sizeof(DBT));
	memset(&pkey, 0, sizeof(DBT));

	dbflags = DB_SET_RANGE;
	memset(&key, 0, sizeof(DBT));
	key.data = &min;
	key.size = sizeof(min);

next:	ret = cur->c_pget(cur, &key, &pkey, &data, dbflags);
	dbflags = DB_NEXT;

	switch (ret) {
	case 0: break;
	case DB_NOTFOUND:
		ret = 0;
		goto out;
	default:
		BUG("%s", db_strerror(ret));
		goto out;
	}

	code = *(morton_t*) key.data;

	if ((smorton_t) code > (smorton_t) max)
		goto out;

	if (sdb_inrange(code, min, max, sdb->dim)) {
		int16_t pv[sdb->dim];
		morton_pos(pv, code, sdb->dim);
		sdb->callback(pv, pkey.data);
		res++;
	} else
		sdb_compute_bmlm(&min, &lm, code, min, max);

	goto next;

out:	if (cur->close(cur) || ret)
		return -1;

	return res;
}

static inline int16_t
morton_get(morton_t code, uint8_t i, uint8_t dim)
{
	uint16_t u = 0;
	int j;

	for (j = 0; j < 16; j++) {
		int16_t v = (code >> (j * dim + i)) & 1;
		u |= v << j;
	}

	return (smorton_t) u - SHRT_MAX;
}

static inline morton_t
morton_set(morton_t x, uint8_t i, int16_t value, uint8_t dim)
{
	morton_t res = x;
	int j;

	for (j = 0; j < 16; j++) {
		register unsigned shift = j * dim + i;
		register uint16_t v = (smorton_t) value + SHRT_MAX;
		res &= ~(1 << shift);
		res |= ((morton_t) (v >> j) & 1) << shift;
	}

	return res;
}

/* this version accounts for type limits,
 * and wraps around them in each direction, if necessary
 * TODO iterative form
 */

static int
sdb_range_safe(sdb_t *sdb, morton_t min, morton_t max, int dim)
{
	int i, ret, aux;
	morton_t lmin = min,
		 lmax = max;

	/* warn("sdb_range_safe %d %llx %llx\n", dim, min, max); */
	/* morton_debug(min, sdb->dim); */
	/* morton_debug(max, sdb->dim); */

	for (i = dim; i < sdb->dim; i++) {
		if (morton_cmp(lmin, lmax, sdb->dim, i) >= 0)
			continue;

		int16_t lmaxi = morton_get(lmax, i, sdb->dim);

		// lmin is correct
		lmax = morton_set(lmax, i, SHRT_MAX, sdb->dim);
		aux = sdb_range_safe(sdb, lmin, lmax, i + 1);

		if (aux < 0)
			return aux;

		lmin = morton_set(lmin, i, SHRT_MIN, sdb->dim);
		lmax = morton_set(lmax, i, lmaxi, sdb->dim);
		ret = sdb_range_safe(sdb, lmin, lmax, i + 1);

		if (ret < 0)
			return ret;

		return aux + ret;
	}

	return sdb_range_unsafe(sdb, lmin, lmax);
}

// TODO make this actually "safe" aka working across data type boundaries
static int
sdb_hrange_safe(sdb_t *sdb, int16_t *min, int16_t *max, uint8_t dim)
{
	int16_t lmin[sdb->dim], lmax[sdb->dim];
	int16_t mini, maxi, j, step = 1 << sdb->y;
	int ret = 0;

	memcpy(lmin, min, sdb->dim * sizeof(int16_t));
	memcpy(lmax, max, sdb->dim * sizeof(int16_t));

	mini = (min[dim] >> sdb->y) << sdb->y;
	maxi = (max[dim] >> sdb->y) << sdb->y;

	/* warn("sdb_hrange_safe %hhu\n", dim); */
	/* point_debug(min, "min", sdb->dim); */
	/* point_debug(max, "max", sdb->dim); */

	if (mini > maxi) {
		if (dim == 0)
			for (j = mini; j <= SHRT_MAX; j++) {
				lmin[dim] = j;
				void *ptr = sdb_get(sdb, lmin);
				if (ptr)
					ret++;
				sdb->callback(lmin, ptr);
			}
		else {
			lmax[dim] = SHRT_MAX;
			for (j = mini; j <= SHRT_MAX; j += step) {
				lmin[dim] = j;
				ret += sdb_hrange_safe(sdb, lmin, lmax, dim - 1);
			}

			lmax[dim] = maxi;
			for (j = SHRT_MIN; j <= maxi; j += step) {
				lmin[dim] = j;
				ret += sdb_hrange_safe(sdb, lmin, lmax, dim - 1);
			}
		}
	} else {
		if (dim == 0)
			for (j = mini; j <= maxi; j += step) {
				lmin[dim] = j;
				void *ptr = sdb_get(sdb, lmin);
				if (ptr)
					ret++;
				sdb->callback(lmin, ptr);
			}
		else {
			lmax[dim] = maxi;
			for (j = mini; j <= maxi; j += step) {
				lmin[dim] = j;
				ret += sdb_hrange_safe(sdb, lmin, lmax, dim - 1);
			}
		}
	}

	return ret;
}

void
sdb_search(sdb_t *sdb, int16_t *min, int16_t *max)
{
	/* warn("sdb_search %llx %llx\n", morton_min, morton_max); */

	if (sdb->type == DB_BTREE) {
		morton_t morton_min = pos_morton(min, 4),
			 morton_max = pos_morton(max, 4);

		sdb_range_safe(sdb, morton_min, morton_max, 0);
	} else {
		sdb_hrange_safe(sdb, min, max, sdb->dim - 1);
	}
	/* warn("sdb_search result %d\n", ret); */

}

static int
sdb_cmp(DB *sec, const DBT *a_r, const DBT *b_r)
{
	morton_t a = * (morton_t*) a_r->data,
		 b = * (morton_t*) b_r->data;

	return b > a ? -1 : (a > b ? 1 : 0);
}

static int
sdb_mki_code(DB *sec, const DBT *key, const DBT *data, DBT *result)
{
	morton_t *code = (morton_t *) malloc(sizeof(morton_t));
	*code = pos_morton(data->data, 4);
	result->size = sizeof(morton_t);
	result->data = code;
	result->flags = DB_DBT_APPMALLOC;
	return 0;
}

int
sdb_init(sdb_t *sdb, uint8_t dim, uint8_t y, size_t ekl, size_t len, DB_ENV *dbe, char *fname, DBTYPE type) {
	int ret = 0;

	sdb->dim = dim;
	sdb->ekl = ekl;
	sdb->vl = len - ekl - sizeof(int16_t) * 4;
	sdb->type = type;
	sdb->y = y;
	sdb->max_dim = 4; // change this if coords can be int8_t for example (a lot of other fixed needed for that to work)

	if ((ret = db_create(&sdb->primary, dbe, 0))
	    || (ret = sdb->primary->open(sdb->primary, NULL, fname, fname, DB_HASH, DB_CREATE, 0664)))
		return ret;

	if (type == DB_BTREE) {
		if ((ret = db_create(&sdb->point, dbe, 0))
		    || (ret = sdb->point->set_bt_compare(sdb->point, sdb_cmp))
		    || (ret = sdb->point->open(sdb->point, NULL, fname, fname, DB_BTREE, DB_CREATE, 0664))
		    || (ret = sdb->primary->associate(sdb->primary, NULL, sdb->point, sdb_mki_code, DB_CREATE)))

			sdb->point->close(sdb->point, 0);
		else
			return 0;
	} else {
		CBUG(type != DB_HASH);

		if ((ret = db_create(&sdb->point, dbe, 0))
		    || (ret = sdb->point->open(sdb->point, NULL, fname, fname, DB_HASH, DB_CREATE, 0664))
		    || (ret = sdb->primary->associate(sdb->primary, NULL, sdb->point, sdb_mki_code, DB_CREATE)))

			sdb->point->close(sdb->point, 0);
		else
			return 0;
	}

	sdb->primary->close(sdb->primary, 0);
	return ret;
}

int
sdb_close(sdb_t *sdb)
{
	return sdb->point->close(sdb->point, 0)
		|| sdb->primary->close(sdb->primary, 0);
}

void
sdb_put(sdb_t *sdb, void *thing, int flags)
{
	DBT key;
	DBT data;
	int ret;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.data = thing;
	key.size = sizeof(int16_t) * 4 + sdb->ekl;
	data.data = thing;
	data.size = key.size + sdb->vl;

	ret = sdb->primary->put(sdb->primary, NULL, &key, &data, flags);
	CBUG(ret);
}

morton_t
_sdb_where(sdb_t *sdb, void *thing)
{
	int bad;
	DBT key;
	DBT data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = thing;
	key.size = sizeof(int16_t) * 4 + sdb->ekl;

	if ((bad = sdb->primary->get(sdb->primary, NULL, &key, &data, 0))) {
		static morton_t code = SDB_INVALID;
		if (bad && bad != DB_NOTFOUND)
			warn("sdb_where %p %s", thing, db_strerror(bad));
		return code;
	}

	return * (morton_t *) data.data;
}

void
sdb_where(int16_t *p, sdb_t *sdb, void *thing)
{
	morton_t x = _sdb_where(sdb, thing);
	morton_pos(p, x, sdb->dim);
}

void *
sdb_get(sdb_t *sdb, int16_t *p)
{
	morton_t at = pos_morton(p, 4);
	DBC *cur;
	DBT pkey;
	DBT data;
	DBT key;
	int res;

	if (sdb->point->cursor(sdb->point, NULL, &cur, 0))
		return NULL;

	memset(&pkey, 0, sizeof(DBT));
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.size = sizeof(at);
	key.data = &at;

	while (1) {
		res = cur->c_pget(cur, &key, &pkey, &data, DB_SET);
		switch (res) {
		case 0:
			cur->close(cur);
			return data.data;
		case DB_NOTFOUND:
			cur->close(cur);
			return NULL;
		default:
			BUG("%s", db_strerror(res));
			return NULL;
		}
	}
}

int
sdb_delete(sdb_t *sdb, void *what)
{
	DBT key;
	memset(&key, 0, sizeof(key));
	key.data = what;
	key.size = sizeof(int16_t) * 4 + sdb->ekl;
	return sdb->primary->del(sdb->primary, NULL, &key, 0);
}

#endif

#endif

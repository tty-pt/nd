#include "uapi/map.h"

#include <stdlib.h>

#include "config.h"
#include "nddb.h"
#include "st.h"

#define GEO_DB "/var/nd/geo.db"

// #define PRECOVERY

typedef struct {
	unsigned what;
	morton_t where;
} map_range_t;

// see http://www.vision-tools.com/h-tropf/multidimensionalrangequery.pdf

static DB *pdb = NULL, *ipdb = NULL;

static const morton_t m1 = 0x111111111111ULL;
static const morton_t m0 = 0x800000000000ULL;

static inline morton_t
qload_0(morton_t c, morton_t lm0, morton_t lm1) // LOAD(0111...
{
	return (c & (~ lm0)) | lm1;
}

static inline morton_t
qload_1(morton_t c, morton_t lm0, morton_t lm1) // LOAD(1000...
{
	return (c | lm0) & (~ lm1);
}

static inline int
inrange(morton_t dr, pos_t min, pos_t max)
{
	pos_t drp;

	// TODO use ffs
	morton_pos(drp, dr);

	if (
			drp[0] < min[0] || drp[0] >= max[0]
			|| drp[1] < min[1] || drp[1] >= max[1]
			|| drp[2] < min[2] || drp[2] >= max[2]
			|| drp[3] < min[3] || drp[3] >= max[3]
	   )
		return 0;

	return 1;
}

static inline void
compute_bmlm(morton_t *bm, morton_t *lm,
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
				*lm = qload_0(max, lm0, lm1);
				min = qload_1(min, lm0, lm1);
			} else { // 1 0 0
				*lm = max;
				break;
			}

		else if (c) { // 0 0 1
			// BIGMIN
			*bm = qload_1(min, lm0, lm1);
			max = qload_0(max, lm0, lm1);
		}
	}
}

static inline int
map_range_unsafe(map_range_t *res,
		     size_t n,
		     struct rect4D *rect)
{
	pos_t rect_e = {
		rect->s[0] + rect->l[0],
		rect->s[1] + rect->l[1],
		rect->s[2] + rect->l[2],
		rect->s[3] + rect->l[3],
	};

	morton_t rmin = pos_morton(rect->s),
		 rmax = pos_morton(rect_e),
		 lm = 0,
		 code;

	map_range_t *r = res;
	DBC *cur;
	DBT key, pkey, data;
	size_t nn = n;
	int ret = 0;
	int dbflags;

	if (pdb->cursor(pdb, txnl_peek(&qdb_config.txnl), &cur, 0))
		return -1;

	memset(&data, 0, sizeof(DBT));
	memset(&pkey, 0, sizeof(DBT));

	dbflags = DB_SET_RANGE;
	memset(&key, 0, sizeof(DBT));
	key.data = &rmin;
	key.size = sizeof(rmin);

next:	ret = cur->c_pget(cur, &key, &pkey, &data, dbflags);
	dbflags = DB_NEXT;

	switch (ret) {
	case 0: break;
	case DB_NOTFOUND:
		ret = 0;
		goto out;
	default: goto out;
	}

	code = *(morton_t*) key.data;

	if (code > rmax)
		goto out;

	if (inrange(code, rect->s, rect_e)) {
		r->what = * (unsigned *) pkey.data;
		r->where = code;
		r++;
		nn--;
		if (nn == 0)
			goto out;
	} else
		compute_bmlm(&rmin, &lm, code, rmin, rmax);

	goto next;

out:	if (cur->close(cur) || ret)
		return -1;

	return r - res;
}

/* this version accounts for type limits,
 * and wraps around them in each direction, if necessary
 * TODO iterative form
 */

static int
map_range_safe(map_range_t *res,
		   size_t n,
		   struct rect4D *rect,
		   int dim)
{
	map_range_t *re = res;
	int i, aux;
	size_t nn = n;
	struct rect4D r = *rect;

	for (i = dim; i < DIM; i++) {
		if (rect->s[i] + rect->l[i] <= COORD_MAX)
			continue;

		r.l[i] = COORD_MAX - rect->s[i];
		r.s[i] = rect->s[i];
		aux = map_range_safe(re, nn, &r, i + 1);

		if (aux < 0)
			return -1;

		nn -= aux;
		re += aux;

		r.l[i] = rect->s[i] + rect->l[i] - COORD_MAX - 1;
		r.s[i] = COORD_MIN + 1;
		aux = map_range_safe(re, nn, &r, i + 1); 

		if (aux < 0)
			return -1;

		nn -= aux;

		return n - nn;
	}

	return map_range_unsafe(re, nn, rect);
}

void
map_search(unsigned *mat, pos_t pos, unsigned radius)
{
	const unsigned side = 2 * radius + 1;
	const unsigned m = side * side;

	struct rect4D vpr = {
		{ pos[0] - radius, pos[1] - radius, pos[2], pos[3] },
		{ side, side, 1, 1 },
	};

	/* static const size_t m = 49; */
	map_range_t buf[m];
	size_t n, i;

	n =  map_range_safe(buf, m, &vpr, 0);
	memset(mat, -1, sizeof(unsigned) * m);

	for (i = 0; i < n; i++) {
		pos_t p;
		morton_pos(p, buf[i].where);
		mat[point_rel_idx(p, vpr.s, side)] = buf[i].what;
	}
}

static int
map_cmp(DB *sec __attribute__((unused)), const DBT *a_r, const DBT *b_r)
{
	morton_t a = * (morton_t*) a_r->data,
		 b = * (morton_t*) b_r->data;

	return b > a ? -1 : (a > b ? 1 : 0);
}

static int
map_mki_code(
		DB *sec __attribute__((unused)),
		const DBT *key __attribute__((unused)),
		const DBT *data, DBT *result )
{
	result->size = sizeof(morton_t);
	result->data = data->data;
	return 0;
}

void
map_init(void) {
	int ret = 0;
	DB_TXN *txn = qdb_begin();

	if ((ret = db_create(&ipdb, qdb_config.env, 0))
	    || (ret = ipdb->open(ipdb, txn, STD_DB, "map_dp", DB_HASH, DB_CREATE, 0644))) {
		qdb_abort();
		ndclog(LOG_ERR, "map_init %s\n", db_strerror(ret));
		exit(EXIT_FAILURE);
		return;
	}

	if ((ret = db_create(&pdb, qdb_config.env, 0))
	    || (ret = pdb->set_bt_compare(pdb, map_cmp))
	    || (ret = pdb->open(pdb, txn, STD_DB, "map_pd", DB_BTREE, DB_CREATE, 0644))
	    || (ret = ipdb->associate(ipdb, txn, pdb, map_mki_code, DB_CREATE)))
	{
		pdb->close(pdb, 0);
		qdb_abort();
		ndclog(LOG_ERR, "map_init2 %s\n", db_strerror(ret));
		exit(EXIT_FAILURE);
	}

	qdb_commit();
}

int
map_close(unsigned flags)
{
	return pdb->close(pdb, flags)
		|| ipdb->close(ipdb, flags);
}

int
map_sync(void)
{
	return pdb->sync(pdb, 0)
		|| ipdb->sync(ipdb, 0);
}

static void
_map_put(morton_t code, unsigned thing_ref, int flags)
{
	DBT key;
	DBT data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.data = &thing_ref;
	key.size = sizeof(thing_ref);
	data.data = &code;
	data.size = sizeof(code);

	ipdb->put(ipdb, txnl_peek(&qdb_config.txnl), &key, &data, flags);
}

void
map_put(pos_t p, unsigned thing, int flags)
{
	morton_t code = pos_morton(p);
	_map_put(code, thing, flags);
}

morton_t
map_mwhere(unsigned where)
{
	unsigned room = where;
	int bad;
	DBT key;
	DBT data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &room;
	key.size = sizeof(room);
	data.flags = DB_DBT_MALLOC;

	if ((bad = ipdb->get(ipdb, txnl_peek(&qdb_config.txnl), &key, &data, 0))) {
		static morton_t code = 130056652770671ULL;
		return code;
	}

	return * (morton_t *) data.data;
}

int
map_has(unsigned room) {
	return map_mwhere(room) != 130056652770671ULL;
}

void
map_where(pos_t p, unsigned room)
{
	morton_t x = map_mwhere(room);
	morton_pos(p, x);
}

unsigned
map_get(pos_t p)
{
	morton_t at = pos_morton(p);
	DBC *cur;
	DBT pkey;
	DBT data;
	DBT key;
	unsigned ref;
	int res;

	if (pdb->cursor(pdb, txnl_peek(&qdb_config.txnl), &cur, 0))
		return NOTHING;

	memset(&pkey, 0, sizeof(DBT));
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.size = sizeof(at);
	key.data = &at;

	while (1) {
		res = cur->c_pget(cur, &key, &pkey, &data, DB_SET);
		switch (res) {
		case 0:
			ref = * (unsigned *) pkey.data;
#ifdef PRECOVERY
			OBJ obj = obj_get(ref);
			if (obj.type != TYPE_ROOM) {
				debug("GARBAGE %d REMOVED ;)", ref);
				cur->c_del(cur, 0);
				continue;
			}
#endif
			cur->close(cur);
			return ref;
		case DB_NOTFOUND:
			cur->close(cur);
			return NOTHING;
		}
	}
}

int
map_delete(unsigned what)
{
	DBT key;
	morton_t code = map_mwhere(what);
	memset(&key, 0, sizeof(key));
	key.data = &code;
	key.size = sizeof(code);
	return pdb->del(pdb, txnl_peek(&qdb_config.txnl), &key, 0);
}

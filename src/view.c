#include "view.h"

#include <stdio.h>
#include <string.h>

#include "mcp.h"
#include "noise.h"
#include "params.h"
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/map.h"
#include "uapi/type.h"

static unsigned *biome_map;

unsigned vtf_entity, vtf_shop, vtf_max;

static inline char * biome_bg(unsigned i) {
	static char ret[16];
	if (NIGHT_IS)
		return ANSI_RESET;

	SKEL skel;
	qdb_get(skel_hd, &skel, &biome_map[i]);
	memcpy(ret, ansi_bg[skel.sp.biome.bg], sizeof(ret));
	return ret;
}

static const char * v = "|";
static const char * l = ANSI_FG_WHITE "+";
static const char *h_open	= "   ";
static const char *h_closed	= "---";

void base_vtf_init(void) {
	vtf_shop = vtf_register('$', GREEN, BOLD);
	vtf_entity = vtf_register('!', YELLOW, BOLD);
}

static inline char *
empty_tile(char *b, view_tile_t *t, unsigned side) {
	sic_str_t ss = { .str = "" };
	SIC_CALL(&ss, sic_empty_tile, *t, side, ss);
	b += sprintf(b, "%s", ss.str);
	return b;
}

static inline char *
dr_room(char *buf, view_tile_t *t, const char *bg)
{
	register char *b = buf;
	vtf_t vtf;
	unsigned i;

	if (t->exits & E_DOWN)
		b = stpcpy(b, ANSI_FG_WHITE "<");
	else
		b = empty_tile(b, t, 0);


	int got = 0;
	for (i = 0; i < vtf_max; i++) {
		unsigned ref = (1 << i);

		if (!(t->flags & ref))
			continue;

		if (!qdb_get(vtf_hd, &vtf, &i)) {
			got = 1;
			break;
		}
	}

	b = stpcpy(b, ANSI_RESET);
	b = stpcpy(b, bg);

	if (got) {
		if (vtf.pi.flags & BOLD)
			b = stpcpy(b, ANSI_BOLD);
		b = stpcpy(b, ansi_fg[vtf.pi.fg]);
		*b++ = vtf.emp;
		if (vtf.pi.flags & BOLD)
			b = stpcpy(b, ANSI_RESET_BOLD);
	} else
		b = empty_tile(b, t, 1);


	if (t->exits & E_UP)
		b = stpcpy(b, ANSI_FG_WHITE ">");
	else
		b = empty_tile(b, t, 2);

	b = stpcpy(b, ANSI_RESET);
	b = stpcpy(b, bg);

	return b;
}

static inline unsigned
floor_get(unsigned what_ref) {
	OBJ what;
	qdb_get(obj_hd, &what, &what_ref);
	ROO *rwhat = &what.sp.room;
	unsigned char floor = rwhat->floor;

	if (floor > BIOME_VOLCANIC)
		return BIOME_VOLCANIC;
	else
		return floor;
}

static inline const char *
wall_paint(int floor)
{
	switch (floor) {
	case 11: /* desert */
	case -8:
		return ANSI_FG_RED;
	default:
		return ANSI_FG_YELLOW;
	}
}

static inline char *
dr_v(char *b, view_tile_t *t, enum exit exit,
		const char *wp) {
	if (t->doors & exit)
		return stpcpy(b, l);
	else if (t->exits & exit)
		return stpcpy(b, " ");
	else {
		b = stpcpy(b, wp);
		return stpcpy(b, v);
	}
}

static char *
dr_vs(char *b, view_tile_t *t)
{
	view_tile_t * const t_max = &t[VIEW_SIZE];
	const char *bg, *wp;
	view_tile_t *tp;
	unsigned floor;
	enum exit exit; // loop cache

	if (t->room == NOTHING) {
		floor = t->bio_idx;
		wp = "";
	} else {
		floor = t->bio_idx;
		wp = wall_paint(floor);
	}

	bg = biome_bg(floor);
	b = stpcpy(b, " ");
	for (;;) {
		b = stpcpy(b, bg);
		b = dr_room(b, t, bg);

		tp = t;
		t++;

		if (t >= t_max)
			break;

		if (t->room == NOTHING) {
			floor = t->bio_idx;
			bg = biome_bg(floor);
		} else {
			floor = t->bio_idx;
			wp = wall_paint(floor);
			bg = biome_bg(floor);
			b = stpcpy(b, bg);
		}

		if (t->room != NOTHING) {
			exit = E_WEST;
			tp = t;
		} else if (tp->room != NOTHING) {
			exit = E_EAST;
		} else {
			*b++ = ' ';
			continue;
		}

		b = dr_v(b, tp, exit, wp);
	}

        b = stpcpy(b, ANSI_RESET"\r\n");
        return b;
}

static inline char *
dr_h(char *b, char const *bg, char const *wp, char *w, int toggle)
{
	if (toggle) {
		b = stpcpy(b, " ");
		b = stpcpy(b, bg);
	} else {
		b = stpcpy(b, bg);
		b = stpcpy(b, " ");
	}
	b = stpcpy(b, wp);
	b = stpcpy(b, w);
	return b;
}

static inline char *
dr_hs_n(char *b, view_tile_t *t)
{
	view_tile_t * const t_max = &t[VIEW_SIZE];
	const char *wp, *bg;
	char wb[32], *w;
	unsigned floor;
	int toggle;

	for (wp = "",
	     bg = biome_bg(
		     t->room == NOTHING
		     ? t->bio_idx
		     : floor_get(t->room)) ;
	     t < t_max ;
	     bg = biome_bg(floor),
	     b = dr_h(b, bg, wp, w, toggle),
	     t++)
	{
		view_tile_t *tn = &t[VIEW_SIZE];
		unsigned curr = t->room,
		      next = tn->room;

		floor = t->bio_idx;
		if (next != NOTHING) {
			floor = tn->bio_idx;
			toggle = 0;
		} else if (curr != NOTHING) {
			floor = t->bio_idx;
			toggle = 0;
		} else {
			floor = t->bio_idx;
			w = (char *) h_open;
			toggle = 1;
			continue;
		}

		wp = wall_paint(floor);

		if (!(t->exits & E_SOUTH) || !(tn->exits & E_NORTH))
			w = (char *) h_closed;
		else if ((t->doors & E_SOUTH) || (tn->doors & E_NORTH)) {
			snprintf(wb, sizeof(wb), "-"ANSI_FG_WHITE"+%s-", wp);
			w = wb;
		} else
			w = (char *) h_open;
	}

        b = stpcpy(b, ANSI_RESET"\r\n");
        return b;
}

static char *
view_draw(view_t view) {
	static char view_buf[8 * BUFSIZ];
	view_tile_t * const t_max = &view[VIEW_BDI];
	view_tile_t *t = view;
	char *p = view_buf;
        memset(view_buf, 0, sizeof(view_buf));

	p = dr_vs(p, t);

	for (; t < t_max;) {
		p = dr_hs_n(p, t);
		t += VIEW_SIZE;
		p = dr_vs(p, t);
	}

	return view_buf;
}

static inline void
view_build_exit(view_tile_t *t, ROO *rloc, enum exit e) {
	if (rloc->exits & e) {
		t->exits |= e;
		t->doors |= rloc->doors & e;
	}
}

static inline void
view_build_exit_z(view_tile_t *t, ROO *rloc, enum exit e) {
	if (rloc->exits & e)
		t->exits |= e;
}

static inline void
view_build_exit_s(view_tile_t *t, pos_t p, enum exit e) {
	pos_t pa;
	pos_move(pa, p, e);
	unsigned tmp_ref = map_get(pa);

	if (tmp_ref == NOTHING) {
		t->exits |= e;
		return;
	}

	OBJ tmp;
	qdb_get(obj_hd, &tmp, &tmp_ref);
	ROO *rtmp = &tmp.sp.room;

	if (rtmp->exits & e_simm(e)) {
		t->exits |= e;
		if (rtmp->doors & e_simm(e))
			t->doors |= e;
	}
}

static inline ucoord_t
view_build_flags(unsigned loc_ref) {
	qdb_cur_t c = qdb_iter(contents_hd, &loc_ref);
	unsigned tmp_ref;
	ucoord_t flags = 0;

	while (qdb_next(&loc_ref, &tmp_ref, &c)) {
		OBJ tmp;
		qdb_get(obj_hd, &tmp, &tmp_ref);
		switch (tmp.type) {
		case TYPE_ENTITY:
			flags |= vtf_entity;

			if (ent_get(tmp_ref).flags & EF_SHOP)
				flags |= vtf_shop;

			break;
		}

		SIC_CALL(&flags, sic_view_flags, flags, tmp_ref);
	}

	return flags;
}

static void
view_build_tile(struct bio *n, unsigned loc_ref, view_tile_t *t, pos_t p)
{
	t->room = loc_ref;
	if (loc_ref != NOTHING) {
		OBJ loc;
		qdb_get(obj_hd, &loc, &loc_ref);
		ROO *rloc = &loc.sp.room;
		view_build_exit(t, rloc, E_EAST);
		view_build_exit(t, rloc, E_NORTH);
		view_build_exit(t, rloc, E_WEST);
		view_build_exit(t, rloc, E_SOUTH);
		view_build_exit_z(t, rloc, E_UP);
		view_build_exit_z(t, rloc, E_DOWN);
		t->flags = view_build_flags(loc_ref);
		t->bio_idx = floor_get(loc_ref);
		if (rloc->flags & RF_TEMP)
			t->room = NOTHING;
	} else {
		view_build_exit_s(t, p, E_EAST);
		view_build_exit_s(t, p, E_NORTH);
		view_build_exit_s(t, p, E_WEST);
		view_build_exit_s(t, p, E_SOUTH);
		/* view_build_exit_sz(t, descr, player, loc, E_UP); */
		/* view_build_exit_sz(t, descr, player, loc, E_DOWN); */
		t->bio_idx = n->bio_idx;
	}

	memcpy(&t->raw, &n->raw, sizeof(n->raw));
}

static inline view_tile_t *
_view_build(struct bio *n, unsigned *g, view_tile_t *b, pos_t p)
{
	unsigned *g_max = g + VIEW_SIZE;

	// TODO vary p vertically and horizontally
	for (; g < g_max ; n++, g++, b++, p[1]++)
		view_build_tile(n, *g, b, p);

        return b;
}

void map_search(unsigned *mat, pos_t pos, unsigned radius);
void noise_view(struct bio to[VIEW_M], pos_t pos);

void
view(unsigned player_ref)
{
	struct bio bd[VIEW_M], *n_p = bd,
		   *n_max = &bd[VIEW_BDI + 1];
	pos_t pos, opos;
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	map_where(opos, player.location);
	view_t view;
        view_tile_t *p = view;
	unsigned o[VIEW_M], *o_p = o;
	uint64_t position = map_mwhere(player.location);
	biome_map = biome_map_get(position);

	memcpy(pos, opos, sizeof(pos));
	noise_view(bd, pos);
	map_search(o, pos, VIEW_AROUND);
	memset(view, 0, sizeof(view));

	for (; n_p < n_max;) {
		_view_build(n_p, o_p, p, pos);
		p += VIEW_SIZE;
		o_p += VIEW_SIZE;
		n_p += VIEW_SIZE;
		pos[0]++;
		pos[1] = opos[1];
	}

	char *view_buf = view_draw(view);
        /* if (mcp_view(eplayer, &view)) */
        /* if (fbcp_view(eplayer, &view)) */
        fbcp_view_buf(player_ref, view_buf);
}

void
do_view(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
        view(fd_player(fd));
}

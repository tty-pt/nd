#include "io.h"

#include "entity.h"
#include "view.h"

#include "map.h"
#include "noise.h"

#include "params.h"
#include "debug.h"
#include "command.h"

#ifndef __OpenBSD__
#include "bsd/string.h"
#endif

#define BIOME_BG(i) (NIGHT_IS \
		? ANSI_RESET : biomes[i].sp.biome.bg)

// global buffer for map? // FIXME d bio_limit
static const char * v = "|";
static const char * l = ANSI_FG_WHITE "+";
static const char *h_open	= "   ";
static const char *h_closed	= "---";

vtf_t vtf_map[] = {
	[VTF_SHOP] = {
		.pre = ANSI_BOLD ANSI_FG_GREEN,
		.emp = '$',
	},
	[VTF_ENTITY] = {
		.pre = ANSI_BOLD ANSI_FG_YELLOW,
		.emp = '!',
	},
	[VTF_POND] = {
		.pre = ANSI_BOLD ANSI_FG_BLUE,
		.emp = '~',
	},
};

static inline char *
dr_tree(struct plant_data pd, int n, char *b) {
	if (PLANT_N(pd.n, n)) {
		struct object_skeleton *obj_skel = PLANT_SKELETON(pd.id[n]);
		struct plant_skeleton *pl = &obj_skel->sp.plant;
		b = stpcpy(b, pl->pre);
		*b++ = PLANT_N(pd.n, n) > PLANT_HALF
			? pl->big : pl->small;
	} else
		*b++ = ' ';
	return b;
}

// TODO remove phantom trees

static inline char *
dr_room(char *buf, view_tile_t *t, const char *bg)
{
	register char *b = buf;
	vtf_t *vtf = NULL;
	int i;

	if (t->exits & E_DOWN)
		b = stpcpy(b, ANSI_FG_WHITE "<");
	else
		b = dr_tree(t->pd, 0, b);


	for (i = 0; i < VTF_MAX; i++) {
		if (!(t->flags & (1 << i)))
			continue;

		vtf = &vtf_map[1 << i];
		break;
	}

	b = stpcpy(b, ANSI_RESET);
	b = stpcpy(b, bg);

	if (vtf) {
		b = stpcpy(b, vtf->pre);
		*b++ = vtf->emp;
	} else
		b = dr_tree(t->pd, 1, b);


	if (t->exits & E_UP)
		b = stpcpy(b, ANSI_FG_WHITE ">");
	else
		b = dr_tree(t->pd, 2, b);

	b = stpcpy(b, ANSI_RESET);
	b = stpcpy(b, bg);

	return b;
}

static inline unsigned
floor_get(OBJ *what) {
	CBUG(what->type != TYPE_ROOM);
	ROO *rwhat = &what->sp.room;
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

	if (t->room < 0) {
		floor = t->bio_idx;
		wp = "";
	} else {
		floor = t->bio_idx;
		wp = wall_paint(floor);
	}

	bg = BIOME_BG(floor);
	b = stpcpy(b, " ");
	for (;;) {
		b = stpcpy(b, bg);
		b = dr_room(b, t, bg);

		tp = t;
		t++;

		if (t >= t_max)
			break;

		if (t->room < 0) {
			floor = t->bio_idx;
			bg = BIOME_BG(floor);
		} else {
			floor = t->bio_idx;
			wp = wall_paint(floor);
			bg = BIOME_BG(floor);
			b = stpcpy(b, bg);
		}

		if (t->room >= 0) {
			exit = E_WEST;
			tp = t;
		} else if (tp->room >= 0) {
			exit = E_EAST;
		} else {
			*b++ = ' ';
			continue;
		}

		b = dr_v(b, tp, exit, wp);
	}

        b = stpcpy(b, ANSI_RESET"\n");
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
	     bg = BIOME_BG(
		     t->room < 0
		     ? t->bio_idx
		     : floor_get(object_get(t->room))) ;
	     t < t_max ;
	     bg = BIOME_BG(floor),
	     b = dr_h(b, bg, wp, w, toggle),
	     t++)
	{
		view_tile_t *tn = &t[VIEW_SIZE];
		dbref curr = t->room,
		      next = tn->room;

		floor = t->bio_idx;
		if (next >= 0) {
			floor = tn->bio_idx;
			toggle = 0;
		} else if (curr >= 0) {
			floor = t->bio_idx;
			toggle = 0;
		} else {
			floor = t->bio_idx;
			w = (char *) h_open;
			toggle = 1;
			continue;
		}

		wp = wall_paint(floor);

		// TODO fix t exits
		//  if (!(t->exits & E_SOUTH)) should work
		// see FIXME below

		if (!(t->exits & E_SOUTH) || !(tn->exits & E_NORTH))
			w = (char *) h_closed;
		else if ((t->doors & E_SOUTH) || (tn->doors & E_NORTH)) {
			snprintf(wb, sizeof(wb), "-"ANSI_FG_WHITE"+%s-", wp);
			w = wb;
		} else
			w = (char *) h_open;
	}

        b = stpcpy(b, ANSI_RESET"\n");
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
view_build_exit(view_tile_t *t, OBJ *loc, enum exit e) {
	ROO *rloc = &loc->sp.room;

	if (rloc->exits & e) {
		t->exits |= e;
		t->doors |= rloc->doors & e;
	}
}

static inline void
view_build_exit_z(view_tile_t *t, OBJ *loc, enum exit e) {
	ROO *rloc = &loc->sp.room;

	if (rloc->exits & e)
		t->exits |= e;
}

static inline void
view_build_exit_s(OBJ *player, view_tile_t *t,
		  OBJ *loc, pos_t p, enum exit e) {
	pos_t pa;
	pos_move(pa, p, e);
	OBJ *tmp = map_get(pa);

	if (!tmp) {
		t->exits |= e;
		return;
	}

	ROO *rtmp = &tmp->sp.room;

	// FIXME not returning correct room (at least for E_SOUTH)
	/* debug("here 0x%llx -> 0x%llx (%d)\n", */
	/* 		* (morton_t *) p, */
	/* 		* (morton_t *) pa, */
	/* 		tmp); */

	if (rtmp->exits & e_simm(e)) {
		t->exits |= e;
		if (rtmp->doors & e_simm(e))
			t->doors |= e;
	}
}

static inline ucoord_t
view_build_flags(OBJ *loc) {
	OBJ *tmp = loc->contents;
	ucoord_t flags = 0;

	FOR_LIST(tmp, tmp) {
		switch (tmp->type) {
		case TYPE_ENTITY:
			{
				ENT *etmp = &tmp->sp.entity;
				flags |= VTF_ENTITY;

				if (etmp->flags & EF_SHOP)
					flags |= VTF_SHOP;
			}

			break;

		case TYPE_CONSUMABLE:
			flags |= VTF_POND;
			break;
		}
	}

	return flags;
}

static void
view_build_tile(OBJ *player,
		struct bio *n, OBJ *loc,
		view_tile_t *t, pos_t p)
{
	t->room = object_ref(loc);
	if (loc) {
		ROO *rloc = &loc->sp.room;
		view_build_exit(t, loc, E_EAST);
		view_build_exit(t, loc, E_NORTH);
		view_build_exit(t, loc, E_WEST);
		view_build_exit(t, loc, E_SOUTH);
		view_build_exit_z(t, loc, E_UP);
		view_build_exit_z(t, loc, E_DOWN);
		t->flags = view_build_flags(loc);
		t->bio_idx = floor_get(loc);
		if (rloc->flags & RF_TEMP)
			t->room = -1;
	} else {
		view_build_exit_s(player, t, loc, p, E_EAST);
		view_build_exit_s(player, t, loc, p, E_NORTH);
		view_build_exit_s(player, t, loc, p, E_WEST);
		view_build_exit_s(player, t, loc, p, E_SOUTH);
		/* view_build_exit_sz(t, descr, player, loc, E_UP); */
		/* view_build_exit_sz(t, descr, player, loc, E_DOWN); */
		t->bio_idx = n->bio_idx;
	}

	memcpy(&t->pd, &n->pd, sizeof(n->pd));
}

static inline view_tile_t *
_view_build(OBJ *player,
		struct bio *n, dbref *g,
		view_tile_t *b, pos_t p)
{
	dbref *g_max = g + VIEW_SIZE;

	// TODO vary p vertically and horizontally
	for (; g < g_max ; n++, g++, b++, p[1]++)
		view_build_tile(player, n, object_get(*g), b, p);

        return b;
}

void
view(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	struct bio bd[VIEW_M], *n_p = bd,
		   *n_max = &bd[VIEW_BDI + 1];
	pos_t pos, opos;
	map_where(opos, player->location);
	view_t view;
        view_tile_t *p = view;
	dbref o[VIEW_M], *o_p = o;

	memcpy(pos, opos, sizeof(pos));
	noise_view(bd, pos);
	map_search(o, pos, VIEW_AROUND);
	memset(view, 0, sizeof(view));

	for (; n_p < n_max;) {
		_view_build(player, n_p, o_p, p, pos);
		p += VIEW_SIZE;
		o_p += VIEW_SIZE;
		n_p += VIEW_SIZE;
		pos[0]++;
		pos[1] = opos[1];
	}

	char *view_buf = view_draw(view);
        if (mcp_view(eplayer, view_buf))
		notify(eplayer, view_buf);
}

void
do_view(command_t *cmd)
{
        view(cmd->player);
}

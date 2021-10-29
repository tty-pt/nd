#include "geometry.h"
#include "mdb.h"
#include "props.h"
#include "externs.h"
#include "search.h"
#include "web.h"

enum exit e_map[] = {
	[0 ... 254] = E_NULL,
	['h'] = E_WEST,
	['w'] = E_WEST,
	['j'] = E_SOUTH,
	['s'] = E_SOUTH,
	['k'] = E_NORTH,
	['n'] = E_NORTH,
	['l'] = E_EAST,
	['e'] = E_EAST,
	['K'] = E_UP,
	['u'] = E_UP,
	['J'] = E_DOWN,
	['d'] = E_DOWN,
};

exit_t exit_map[] = {
	[0 ... E_ALL] = {
		.simm = E_NULL,
		.name = "",
		.fname = "",
		.other = "",
		.dim = 5,
	},
	[E_EAST] = {
		.simm = E_WEST,
		.name = "east",
		.fname = "e;east",
		.other = "wnsud",
		.dim = 1, .dis = 1,
	},
	[E_SOUTH] = {
		.simm = E_NORTH,
		.name = "south",
		.fname = "s;south",
		.other = "ewudn",
		.dim = 0, .dis = 1,
	},
	[E_WEST] = {
		.simm = E_EAST,
		.name = "west",
		.fname = "w;west",
		.other = "nsude",
		.dim = 1, .dis = -1,
	}, 
	[E_NORTH] = {
		.simm = E_SOUTH,
		.name = "north",
		.fname = "n;north",
		.other = "sewud",
		.dim = 0, .dis = -1,
	},
	[E_UP] = {
		.simm = E_DOWN,
		.name = "up",
		.fname = "u;up",
		.other = "dnsew",
		.dim = 2, .dis = 1,
	},
	[E_DOWN] = {
		.simm = E_UP,
		.name = "down",
		.fname = "d;down",
		.other = "nsewu",
		.dim = 2, .dis = -1,
	},
};

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
pos_morton(pos_t p)
{
	upoint3D_t up;
	morton_t res = ((morton_t) p[3]) << 48;
	int i;

	POOP3D up[I] = unsign(p[I]);

	for (i = 0; i < 16; i ++) {
		POOP3D res |= ((morton_t) ((up[I] >> i) & 1)) << (I + (3 * i));
	}

	/* debug("encoded point %d %d %d %d -> x%llx", p[0], p[1], p[2], p[3], res); */
	return res;
}

static inline coord_t
sign(ucoord_t n)
{
	return (smorton_t) n - COORD_MAX;
}

void
morton_pos(pos_t p, morton_t code)
{
	morton_t up[3] = { 0, 0, 0 };
	int i;

	for (i = 0; i < 16; i ++) {
		POOP3D up[I] |= ((code >> (I + 3 * i)) & 1) << i;
	}

	POOP3D p[I] = sign(up[I]);
	p[3] = OBITS(code);
	/* debug("decoded point x%llx -> %d %d %d %d", code, p[0], p[1], p[2], p[3]); */
}

dbref
obj_add(struct obj o, dbref where)
{
	CBUG(where < 0);
	dbref nu = new_object();
	NAME(nu) = alloc_string(o.name);
	SETART(nu, alloc_string(o.art));
        SETAVATAR(nu, o.avatar);
	SETDESC(nu, alloc_string(o.description));
	ALLOC_THING_SP(nu);
	DBFETCH(nu)->location = where;
	OWNER(nu) = 1;
	FLAGS(nu) = TYPE_THING;
	THING_SET_HOME(nu, where);
	PUSH(nu, DBFETCH(where)->contents);
	DBDIRTY(where);
        web_content_in(where, nu);
	return nu;
}

dbref
obj_stack_add(struct obj o, dbref where, unsigned char n)
{
	CBUG(n <= 0);
	dbref nu = obj_add(o, where);
	if (n > 1)
		SETSTACK(nu, n);
	return nu;
}

dbref
contents_find(command_t *cmd, dbref where, const char *name)
{
	struct match_data md;
	init_match_remote(cmd, where, name, TYPE_THING, &md);
	match_possession(&md);
	return md.exact_match;
}

dbref
e_exit_where(command_t *cmd, dbref loc, enum exit e)
{
	struct match_data md;
	init_match_remote(cmd, loc, e_name(e), TYPE_EXIT, &md),
	match_room_exits(loc, &md);
	return match_result(&md);
}

int
e_exit_can(dbref player, dbref exit) {
	enum exit e = exit_e(exit);
	CBUG(exit < 0);
	CBUG(e_exit_dest(exit) >= 0);
	return e_ground(getloc(player), e);
}

int
e_ground(dbref room, enum exit e)
{
	pos_t pos;

	if (e & (E_UP | E_DOWN))
		return 0;

	map_where(pos, room);
	return pos[2] == 0;
}

void
e_exit_dest_set(dbref exit, dbref dest)
{
	union specific *sp = &DBFETCH(exit)->sp;
#ifdef PRECOVERY
	if (!sp->exit.ndest) {
		sp->exit.dest = (dbref *)malloc(sizeof(dbref));
		sp->exit.ndest = 1;
	}
#else
	CBUG(!sp->exit.ndest);
#endif
	sp->exit.dest[0] = dest;
}

dbref
e_exit_here(command_t *cmd, enum exit e)
{
	return e_exit_where(cmd, getloc(cmd->player), e);
}

void
pos_move(pos_t d, pos_t o, enum exit e) {
	exit_t *ex = &exit_map[e];
	POOP4D d[I] = o[I];
	d[ex->dim] += ex->dis;
}

enum exit
dir_e(const char dir) {
	return e_map[(int) dir];
}

const char
e_dir(enum exit e) {
	return exit_map[e].name[0];
}

enum exit
e_simm(enum exit e) {
	return exit_map[e].simm;
}

const char *
e_name(enum exit e) {
	return exit_map[e].name;
}

const char *
e_fname(enum exit e) {
	return exit_map[e].fname;
}

const char *
e_other(enum exit e) {
	return exit_map[e].other;
}

morton_t
point_rel_idx(point_t p, point_t s, smorton_t w)
{
	smorton_t s0 = s[Y_COORD],
		s1 = s[X_COORD];
	if (s0 > p[Y_COORD])
		s0 -= UCOORD_MAX;
	if (s1 > p[X_COORD])
		s1 -= UCOORD_MAX;
	return (p[Y_COORD] - s0) * w + (p[X_COORD] - s1);
}

enum exit
exit_e(dbref exit) {
	const char dir = NAME(exit)[0];
	return dir_e(dir);
}

int
e_exit_is(dbref exit)
{ 
	char const *fname = e_fname(exit_e(exit));
	return *fname && !strncmp(NAME(exit), fname, 4);
}

dbref
e_exit_dest(dbref exit)
{
	if (!DBFETCH(exit)->sp.exit.ndest)
		return NOTHING;

	else
		return DBFETCH(exit)->sp.exit.dest[0];
}

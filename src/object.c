#include "uapi/object.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qdb.h>

#include "config.h"
#include "mcp.h"
#include "noise.h"
#include "params.h"
#include "player.h"
#include "nddb.h"
#include "uapi/entity.h"
#include "uapi/io.h"
#include "uapi/map.h"
#include "uapi/match.h"

enum actions {
        ACT_LOOK = 1,
        ACT_KILL = 2,
        ACT_SHOP = 4,
        ACT_CONSUME = 8,
        ACT_OPEN = 16,
        ACT_CHOP = 32,
        ACT_FILL = 64,
        ACT_GET = 128,
        ACT_TALK = 256,
};

char std_db[BUFSIZ];
char std_db_ok[BUFSIZ];
unsigned obj_hd, contents_hd, obs_hd;

int obj_exists(unsigned ref) {
	return qdb_exists(obj_hd, &ref);
}

unsigned
object_new(OBJ *newobj)
{
	memset(newobj, 0, sizeof(OBJ));
	newobj->location = NOTHING;
	newobj->owner = ROOT;
	return qdb_put(obj_hd, NULL, newobj);
}

static inline int
rarity_get(void) {
	register int r = random();
	if (r > RAND_MAX >> 1)
		return 0; // POOR
	if (r > RAND_MAX >> 2)
		return 1; // COMMON
	if (r > RAND_MAX >> 6)
		return 2; // UNCOMMON
	if (r > RAND_MAX >> 10)
		return 3; // RARE
	if (r > RAND_MAX >> 14)
		return 4; // EPIC
	return 5; // MYTHICAL
}

ucoord_t biome_rain_floor[16] = {
	0, 0, 0, 0,
	128, 128, 128, 128,
	256, 256, 256, 256,
	384, 384, 384, 384,
};

ucoord_t biome_rain_ceil[16] = {
	128, 128, 128, 128,
	256, 256, 256, 256,
	384, 384, 384, 384,
	512, 512, 512, 512,    
};

coord_t biome_temp_floor[16] = {
	-41, -41, -41, -41,
	11, 11, 11, 11,
	64, 64, 64, 64,
	117, 117, 117, 117,
};

coord_t biome_temp_ceil[16] = {
	11, 11, 11, 11,
	64, 64, 64, 64,
	117, 117, 117, 117,
	170, 170, 170, 170,
};

unsigned _bio_idx(coord_t tmp_f, coord_t tmp_c, ucoord_t rain_f, ucoord_t rain_c, coord_t tmp, ucoord_t rain);

static inline unsigned
biome_art_idx(struct bio *bio) {
	return 1 + _bio_idx(biome_rain_floor[bio->bio_idx],
		biome_rain_ceil[bio->bio_idx],
		biome_temp_floor[bio->bio_idx],
		biome_temp_ceil[bio->bio_idx],
		bio->rn,
		bio->tmp);
}

struct core_art {
	unsigned max;
	char name[32];
};

struct core_art core_art[] = {
	// ENTITIES
	{ 1, "dolphin" },
	{ 6, "eagle" },
	{ 5, "firebird" },
	{ 2, "goldfish" },
	{ 1, "icebird" },
	{ 3, "koifish" },
	{ 9, "owl" },
	{ 8, "parrot" },
	{ 6, "rainbowfish" },
	{ 6, "salmon" },
	{ 7, "shark" },
	{ 15, "sparrow" },
	{ 16, "starling" },
	{ 9, "swallow" },
	{ 13, "tuna" },
	{ 2, "vulture" },
	{ 6, "wolf" },
	{ 7, "woodpecker" },
	{ 300, "avatar" },

	// PLANTS
	{ 10, "abies alba" },
	{ 12, "acacia senegal" },
	{ 14, "betula pendula" },
	{ 6, "daucus carota" },
	{ 19, "pinus sylvestris" },
	{ 21, "pseudotsuga menziesii" },
	{ 9, "arthrocereus rondonianus" },
	{ 11, "solanum lycopersicum" },
	{ 7, "linum usitatissimum" },

	// MINERALS
	{ 17, "stone" },

	// OTHER
	{ 14, "stick" },
};

unsigned art_hd = -1;

unsigned art_max(char *name) {
	unsigned max;
	qdb_get(art_hd, &max, name);
	return max;
}

static inline unsigned
art_idx(OBJ *obj) {
	return 1 + (random() % art_max(obj->name));
}

void stats_init(ENT *enu, SENT *sk);

unsigned
object_add(OBJ *nu, unsigned skel_id, unsigned where_ref, void *arg)
{
	SKEL skel;
	qdb_get(skel_hd, &skel, &skel_id);
	unsigned nu_ref = object_new(nu);
	strcpy(nu->name, skel.name);
	nu->location = where_ref;
	nu->owner = ROOT;
	nu->type = TYPE_THING;
	if (where_ref != NOTHING)
		qdb_put(contents_hd, &nu->location, &nu_ref);

	switch (skel.type) {
	case STYPE_SPELL:
		break;
	case STYPE_EQUIPMENT:
		{
			EQU *enu = &nu->sp.equipment;
			nu->type = TYPE_EQUIPMENT;
			enu->eqw = skel.sp.equipment.eqw;
			enu->msv = skel.sp.equipment.msv;
			enu->rare = rarity_get();
			equip(where_ref, nu_ref);
		}

		break;
	case STYPE_CONSUMABLE:
		{
			CON *cnu = &nu->sp.consumable;
			nu->type = TYPE_CONSUMABLE;
			cnu->food = skel.sp.consumable.food;
			cnu->drink = skel.sp.consumable.drink;
		}

		break;
	case STYPE_ENTITY:
		{
			ENT ent;
			memset(&ent, 0, sizeof(ent));
			nu->type = TYPE_ENTITY;
			stats_init(&ent, &skel.sp.entity);
			ent.flags = skel.sp.entity.flags;
			ent.wtso = skel.sp.entity.wt;
			ent.sat = NOTHING;
			ent.last_observed = NOTHING;
			birth(&ent);
			object_drop(nu_ref, skel_id);
			ent.home = where_ref;
			ent_set(nu_ref, &ent);
			nu->art_id = art_idx(nu);
		}

		break;
	case STYPE_PLANT:
		{
			uint32_t v = * (uint32_t *) arg;
			nu->type = TYPE_PLANT;
			object_drop(nu_ref, skel_id);
			nu->owner = ROOT;
			unsigned max = art_max(nu->name);
			nu->art_id = max ? 1 + (v & 0xf) % max : 0;
		}

		break;
        case STYPE_BIOME:
		{
			struct bio *bio = arg;
			ROO *rnu = &nu->sp.room;
			nu->type = TYPE_ROOM;
			rnu->exits = rnu->doors = 0;
			rnu->flags = RF_TEMP;
			nu->art_id = biome_art_idx(bio);
		}

		break;
	case STYPE_MINERAL:
		{
			uint32_t v = * (uint32_t *) arg;
			nu->type = TYPE_MINERAL;
			unsigned max = art_max(nu->name);
			nu->art_id = max ? 1 + (v & 0xf) % max : 0;
		}

		break;

	case STYPE_OTHER:
		if (arg) {
			uint32_t v = * (uint32_t *) arg;
			unsigned max = art_max(nu->name);
			nu->art_id = max ? 1 + (v & 0xf) % max : 0;
		}
		nu->type = TYPE_THING;

		break;
	}

	qdb_put(obj_hd, &nu_ref, nu);
	if (skel.type != STYPE_BIOME)
		mcp_content_in(where_ref, nu_ref);

	return nu_ref;
}

char *
object_art(unsigned thing_ref)
{
	OBJ thing;
	qdb_get(obj_hd, &thing, &thing_ref);
	static char art[BUFSIZ];
	char *type = NULL;

	switch (thing.type) {
	case TYPE_ENTITY:
		type = "entity";
		snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.art_id && ent_get(thing_ref).flags & EF_PLAYER ? "avatar" : thing.name, thing.art_id);
		return art;
	case TYPE_ROOM: type = "biome"; break;
	case TYPE_PLANT: type = "plant"; break;
	case TYPE_MINERAL: type = "mineral"; break;
	default: type = "other"; break;
	}

	snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.name, thing.art_id);
	return art;
}

void
objects_init(void)
{
	unsigned ref;

	size_t hash_i = 0;
	for (; hash_i < sizeof(core_art) / sizeof(struct core_art); hash_i++) {
		struct core_art *art = &core_art[hash_i];
		qdb_put(art_hd, art->name, &art->max);
	}

	qdb_cur_t c = qdb_iter(obj_hd, NULL);
	OBJ oi;
	while (qdb_next(&ref, &oi, &c)) {
		qdb_put(contents_hd, &oi.location, &ref);
		if (oi.type != TYPE_ENTITY)
			continue;

		ENT ent = ent_get(ref);
		ent.last_observed = NOTHING;
		qdb_put(ent_hd, &ref, &ent);
		if (ent.flags & EF_PLAYER)
			player_put(oi.name, ref);
	}
}

unsigned
object_copy(OBJ *nu, unsigned old_ref)
{
	OBJ old;
	qdb_get(obj_hd, &old, &old_ref);
	unsigned ref = object_new(nu);
	strcpy(nu->name, old.name);
	nu->location = NOTHING;
	nu->owner = old.owner;
        return ref;
}

void entity_update(unsigned player_ref, double dt);

void
objects_update(double dt)
{
	OBJ obj;
	unsigned obj_ref;
	qdb_cur_t c = qdb_iter(obj_hd, NULL);
	while (qdb_next(&obj_ref, &obj, &c))
		if (!*obj.name)
			continue;
		else if (obj.type == TYPE_ENTITY)
			entity_update(obj_ref, dt);
}

void
object_move(unsigned what_ref, unsigned where_ref)
{
	unsigned last_loc;
	OBJ what;
	qdb_get(obj_hd, &what, &what_ref);

	last_loc = what.location;
        if (last_loc != NOTHING)
		qdb_del(contents_hd, &what.location, &what_ref);

	qdb_cur_t c = qdb_iter(obs_hd, &what_ref);
	unsigned first_ref;
	while (qdb_next(&what_ref, &first_ref, &c)) {
		ENT efirst = ent_get(first_ref);
		efirst.last_observed = what.location;
		qdb_cdel(&c);
		ent_set(first_ref, &efirst);
	}

	/* test for special cases */
	if (where_ref == NOTHING) {
		unsigned first_ref;

		qdb_cur_t c = qdb_iter(contents_hd, &what_ref);
		while (qdb_next(&what_ref, &first_ref, &c))
			object_move(first_ref, NOTHING);

		switch (what.type) {
		case TYPE_ENTITY:
			ent_del(what_ref);
			break;
		case TYPE_ROOM:
			map_delete(what_ref);
		}
		qdb_del(obj_hd, &what_ref, NULL);
		mcp_content_out(last_loc, what_ref);
		return;
	}

	mcp_content_in(where_ref, what_ref);
	what.location = where_ref;
	qdb_put(obj_hd, &what_ref, &what);
	mcp_content_out(last_loc, what_ref);

	if (what.type == TYPE_ENTITY) {
		ENT ewhat = ent_get(what_ref);
		if (ewhat.last_observed != NOTHING)
			qdb_del(obs_hd, &ewhat.last_observed, &what_ref);
		if ((ewhat.flags & EF_SITTING)) {
			stand(what_ref, &ewhat);
			ent_set(what_ref, &ewhat);
		}
	}

	qdb_put(contents_hd, &where_ref, &what_ref);
}

struct icon
object_icon(unsigned what_ref)
{
	OBJ what;
	qdb_get(obj_hd, &what, &what_ref);

        struct icon ret = {
                .actions = ACT_LOOK,
                .ch = '?',
                .pi = { .fg = WHITE, .flags = BOLD, },
        };
        unsigned aux;
        switch (what.type) {
        case TYPE_ROOM:
                ret.ch = '-';
                ret.pi.fg = YELLOW;
                break;
        case TYPE_ENTITY:
		ret.actions |= ACT_KILL;
		ret.pi.flags = BOLD;
		if (ent_get(what_ref).flags & EF_SHOP) {
			ret.actions |= ACT_SHOP;
			ret.pi.fg = GREEN;
			ret.ch = '$';
		} else {
			ret.ch = '!';
			ret.pi.fg = YELLOW;
		}
                break;
	case TYPE_CONSUMABLE:
		{
			CON *cwhat = &what.sp.consumable;
			if (cwhat->drink) {
				ret.actions |= ACT_FILL;
				ret.pi.fg = BLUE;
				ret.ch = '~';
			} else {
				ret.pi.fg = RED;
				ret.ch = 'o';
			}
			ret.actions |= ACT_CONSUME;
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pwhat = &what.sp.plant;
			aux = pwhat->plid;
			SKEL skel;
			qdb_get(skel_hd, &skel, &aux);
			SPLA *pl = &skel.sp.plant;

			ret.actions |= ACT_CHOP;
			ret.pi = pl->pi;
			ret.ch = pwhat->size > PLANT_HALF ? pl->big : pl->small;
		}
		break;
	default:
                ret.actions |= ACT_GET;
                break;
        }
        return ret;
}

static inline int
ok_name(const char *name)
{
	return (name
			&& *name
			&& *name != NUMBER_TOKEN
			&& !strchr(name, ' ')
			&& !strchr(name, '\r')
			&& !strchr(name, ESCAPE_CHAR)
			&& strcmp(name, "me")
			&& strcmp(name, "home")
			&& strcmp(name, "here"));
}

void
do_clone(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	char *name = argv[1];

	if (!(ent_get(player_ref).flags & EF_WIZARD) || !*name || !ok_name(name)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}
	
	if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ thing;
	qdb_get(obj_hd, &thing, &thing_ref);

	if(!controls(player_ref, thing_ref)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	OBJ clone;
	unsigned clone_ref = object_new(&clone);

	strcpy(clone.name, thing.name);
	clone.location = player_ref;
	clone.owner = player.owner;
	clone.value = thing.value;

	switch (thing.type) {
		case TYPE_ROOM:
			{
				ROO *rclone = &clone.sp.room;
				rclone->exits = rclone->doors = 0;
			}
			break;
		case TYPE_ENTITY:
			{
				ENT eclone = ent_get(clone_ref);
				eclone.home = ent_get(thing_ref).home;
				ent_set(clone_ref, &eclone);
			}
			break;
	}
	clone.type = thing.type;

	qdb_put(obj_hd, &clone_ref, &clone);
	object_move(clone_ref, player_ref);
}

void
do_create(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	unsigned pflags = ent_get(player_ref).flags;
	char *name = argv[1];
	int cost = 30;

	if (!(pflags & EF_WIZARD) || *name == '\0' || !ok_name(name)) {
		nd_writef(player_ref, "You can't do that.\n");
		return;
	}

	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);

	OBJ thing;
	thing_ref = object_new(&thing);

	strcpy(thing.name, name);
	thing.location = player_ref;
	thing.owner = player.owner;
	thing.value = cost;
	thing.type = TYPE_THING;

	qdb_put(obj_hd, &thing_ref, &thing);
	object_move(thing_ref, player_ref);
}
void
do_name(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	char *newname = argv[2];
	unsigned thing_ref = ematch_all(player_ref, name);

	if (thing_ref == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	if (!controls(player_ref, thing_ref) || !*newname || !ok_name(newname)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	OBJ thing;
	qdb_get(obj_hd, &thing, &thing_ref);
	strcpy(thing.name, newname);
	qdb_put(obj_hd, &thing_ref, &thing);
}

void
do_chown(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), owner_ref, thing_ref;
	int wizard = ent_get(player_ref).flags & EF_WIZARD;
	char *name = argv[1];
	char *newowner = argv[2];

	if (!*name || (thing_ref = ematch_all(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	OBJ player, thing;
	qdb_get(obj_hd, &player, &player_ref);

	owner_ref = *newowner && strcmp(newowner, "me") ? player_get(newowner) : player.owner;
	if (owner_ref == NOTHING)
		goto error;

	qdb_get(obj_hd, &thing, &thing_ref);

	if (thing.type == TYPE_ENTITY ||
			(!wizard && ((thing.type == TYPE_ROOM && player.location != thing_ref)
				     || (thing.type != TYPE_ROOM && thing.location != player_ref ))))
		goto error;

	thing.owner = owner_ref;
	qdb_put(obj_hd, &thing_ref, &thing);
	return;

error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

void
do_recycle(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	unsigned thing_ref;
	OBJ thing;

	if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	qdb_get(obj_hd, &thing, &thing_ref);

	if (!controls(player_ref, thing_ref) || thing.owner != player_ref) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	object_move(thing_ref, NOTHING);
}

void
do_get(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref, cont_ref;
	char *what = argv[1];
	char *obj = argv[2];

	if (
			(thing_ref = ematch_near(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, what)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	cont_ref = thing_ref;
	OBJ player, thing, cont;
	qdb_get(obj_hd, &player, &player_ref);
	qdb_get(obj_hd, &thing, &cont_ref);
	cont = thing;

	if (obj && *obj) {
		thing_ref = ematch_at(player_ref, cont_ref, obj);
		if (thing_ref == NOTHING) {
			nd_writef(player_ref, NOMATCH_MESSAGE);
			return;
		}
		qdb_get(obj_hd, &thing, &thing_ref);
		if (cont.type == TYPE_ENTITY)
			goto error;
	}

	if (thing.location == player_ref
			|| thing_ref == player_ref
			|| thing_ref == player.location)
		goto error;

	switch (thing.type) {
	case TYPE_ENTITY:
	case TYPE_PLANT: if (player_ref != ROOT)
				 goto error;
			 break;
	case TYPE_SEAT: if (thing.owner != player_ref)
				goto error;
			break;
	case TYPE_ROOM: goto error;
	default: break;
	}

	object_move(thing_ref, player_ref);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

void
do_drop(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref, cont_ref;
	OBJ player, cont, thing;
	qdb_get(obj_hd, &player, &player_ref);
	char *name = argv[1];
	char *obj = argv[2];

	if ((thing_ref = ematch_mine(player_ref, name)) == NOTHING) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	cont_ref = player.location;
	if (
			obj && *obj
			&& (cont_ref = ematch_mine(player_ref, obj)) == NOTHING
			&& (cont_ref = ematch_near(player_ref, obj)) == NOTHING
	   )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}
        
	if (thing_ref == cont_ref)
		goto error;

	qdb_get(obj_hd, &cont, &cont_ref);

	if (cont.type != TYPE_ROOM && cont.type != TYPE_ENTITY && !object_item(cont_ref))
		goto error;

	object_move(thing_ref, cont_ref);
	qdb_get(obj_hd, &thing, &thing_ref);

	if (object_item(cont_ref))
		return;

	if (cont.type == TYPE_ENTITY) {
		nd_writef(cont_ref, "%s hands you %s.\n", player.name, thing.name);
		return;
	}

	nd_owritef(player_ref, "%s drops %s.\n", player.name, thing.name);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

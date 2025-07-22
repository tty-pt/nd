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
#include "uapi/type.h"
#include "papi/nd.h"

unsigned act_look, act_fight, act_shop,
	 act_open, act_chop,
	 act_get, act_talk;

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

// there are 6 wolf art images

static inline unsigned
art_idx(OBJ *obj) {
	SKEL skel;
	qdb_get(skel_hd, &skel, &obj->skid);
	return 1 + (random() % skel.max_art);
}

void stats_init(ENT *enu, SENT *sk);

unsigned
object_add(OBJ *nu, unsigned skel_id, unsigned where_ref, uint64_t v)
{
	SKEL skel;
	qdb_get(skel_hd, &skel, &skel_id);
	unsigned nu_ref = object_new(nu);
	strlcpy(nu->name, skel.name, sizeof(nu->name));
	nu->skid = skel_id;
	nu->location = where_ref;
	nu->owner = ROOT;
	nu->type = skel.type;
	if (where_ref != NOTHING)
		qdb_put(contents_hd, &nu->location, &nu_ref);

	switch (skel.type) {
	case TYPE_ENTITY:
		{
			ENT ent;
			memset(&ent, 0, sizeof(ent));
			stats_init(&ent, &skel.sp.entity);
			ent.flags = skel.sp.entity.flags;
			ent.wtso = skel.sp.entity.wt;
			ent.sat = NOTHING;
			ent.last_observed = NOTHING;
			birth(nu_ref, &ent);
			object_drop(nu_ref, skel_id);
			ent.home = where_ref;
			ent_set(nu_ref, &ent);
			nu->art_id = art_idx(nu);
		}

		break;
        case TYPE_ROOM:
		{
			struct bio *bio = (struct bio *) v;
			ROO *rnu = &nu->sp.room;
			rnu->exits = rnu->doors = 0;
			rnu->flags = RF_TEMP;
			nu->art_id = biome_art_idx(bio);
		}

		break;

	case TYPE_THING:
		nu->art_id = skel.max_art ? 1 + (v & 0xf) % skel.max_art : 0;
		break;
	default:
		 break;
	}

	qdb_put(obj_hd, &nu_ref, nu);
	SIC_CALL(NULL, sic_add, nu_ref, v);

	if (skel.type != TYPE_ROOM)
		mcp_content_in(where_ref, nu_ref);

	// needed for retrieval (FIXME)
	qdb_get(obj_hd, nu, &nu_ref);
	return nu_ref;
}

char *
object_art(unsigned thing_ref)
{
	OBJ thing;
	qdb_get(obj_hd, &thing, &thing_ref);
	static char art[BUFSIZ];
	char typestr[BUFSIZ];
	char *type = NULL;

	switch (thing.type) {
	case TYPE_ENTITY:
		type = "entity";
		snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.art_id && ent_get(thing_ref).flags & EF_PLAYER ? "avatar" : thing.name, thing.art_id);
		return art;
	case TYPE_ROOM: type = "biome"; break;
	case TYPE_THING: type = "other"; break;
	default:
			 qdb_get(type_hd + 1, typestr, &thing.type);
			 type = typestr; break;
	}

	snprintf(art, sizeof(art), "%s/%s/%u.jpeg", type, thing.name, thing.art_id);
	return art;
}

void
objects_init(void)
{
	unsigned ref;

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
	strlcpy(nu->name, old.name, sizeof(nu->name));
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
	while (qdb_next(&obj_ref, &obj, &c)) {
		if (!*obj.name)
			continue;
		else if (obj.type == TYPE_ENTITY)
			entity_update(obj_ref, dt);

		SIC_CALL(NULL, sic_update, obj_ref, dt);
	}
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

	SIC_CALL(NULL, sic_leave, what_ref, last_loc);

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

		SIC_CALL(NULL, sic_del, what_ref);
		return;
	}

	what.location = where_ref;
	qdb_put(obj_hd, &what_ref, &what);
	mcp_content_in(where_ref, what_ref);
	mcp_content_out(last_loc, what_ref);

	if (what.type == TYPE_ENTITY) {
		ENT ewhat = ent_get(what_ref);
		if (ewhat.last_observed != NOTHING)
			qdb_del(obs_hd, &ewhat.last_observed, &what_ref);
	}

	qdb_put(contents_hd, &where_ref, &what_ref);
	SIC_CALL(NULL, sic_enter, what_ref, where_ref);
}

void
base_actions_register(void) {
	act_look = action_register("look", "👁");
	act_fight = action_register("fight", "⚔️");
	act_open = action_register("open", "📦");
	act_get = action_register("get", "🖐️");
	act_talk = action_register("talk", "👄");
}

struct icon
object_icon(unsigned what_ref)
{
	OBJ what;
	qdb_get(obj_hd, &what, &what_ref);

        struct icon ret = {
                .actions = act_look,
                .ch = '?',
                .pi = { .fg = WHITE, .flags = BOLD, },
        };
        switch (what.type) {
        case TYPE_ROOM:
                ret.ch = '-';
                ret.pi.fg = YELLOW;
                break;
        case TYPE_ENTITY:
		ret.actions |= act_fight;
		ret.pi.flags = BOLD;
		ret.ch = '!';
		ret.pi.fg = YELLOW;
                break;
	default: {
                ret.actions |= act_get;
                break;
        }}

	SIC_CALL(&ret, sic_icon, ret, what_ref);
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

	strlcpy(clone.name, thing.name, sizeof(clone.name));
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
	SIC_CALL(NULL, sic_clone, thing_ref, clone_ref);
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

	strlcpy(thing.name, name, sizeof(thing.name));
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
	strlcpy(thing.name, newname, sizeof(thing.name));
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
	case TYPE_ENTITY: if (player_ref == ROOT)
				  break;
	case TYPE_ROOM: goto error;
	default: break;
	}

	int ret;
	SIC_CALL(&ret, sic_get, player_ref, thing_ref);
	if (ret)
		goto error;

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

	object_move(thing_ref, cont_ref);
	qdb_get(obj_hd, &thing, &thing_ref);

	if (cont.type == TYPE_ENTITY) {
		nd_writef(cont_ref, "%s hands you %s.\n", player.name, thing.name);
		return;
	}

	nd_owritef(player_ref, "%s drops %s.\n", player.name, thing.name);
	return;
error:
	nd_writef(player_ref, CANTDO_MESSAGE);
}

#include "uapi/entity.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <qdb.h>

#include "config.h"
#include "mcp.h"
#include "params.h"
#include "st.h"
#include "uapi/io.h"
#include "uapi/match.h"
#include "uapi/wts.h"
#include "view.h"

#include "papi/nd.h"

unsigned ent_hd = -1;
unsigned me = -1;

unsigned me_get(void) {
	return me;
}

ENT ent_get(unsigned ref) {
	ENT ent;
	qdb_get(ent_hd, &ent, &ref);
	return ent;
}

void ent_set(unsigned ref, ENT *tmp) {
	qdb_put(ent_hd, &ref, tmp);
}

void ent_del(unsigned ref) {
	qdb_del(ent_hd, &ref, NULL);
}

void
enter(unsigned player_ref, unsigned loc_ref, enum exit e)
{
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	unsigned old_loc_ref = player.location;

	SIC_CALL(NULL, on_before_leave, player_ref);

	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports out.\n", player.name);
	else {
		nd_writef(player_ref, "You go %s%s%s.\n", ANSI_FG_BLUE ANSI_BOLD, e_name(e), ANSI_RESET);
		nd_owritef(player_ref, "%s goes %s.\n", player.name, e_name(e));
	}
	object_move(player_ref, loc_ref);
	room_clean(old_loc_ref);
	if (e == E_NULL) {
		nd_writef(player_ref, "Teleported\n");
		nd_owritef(player_ref, "%s teleports in.\n", player.name);
	} else
		nd_owritef(player_ref, "%s comes in from the %s.\n", player.name, e_name(e_simm(e)));

	SIC_CALL(NULL, on_after_enter, player_ref);
}

int
payfor(unsigned who_ref, OBJ *who, unsigned cost)
{
	if (ent_get(who_ref).flags & EF_WIZARD)
		return 1;

	if (who->value >= cost) {
		who->value -= cost;
		return 1;
	} else {
		return 0;
	}
}

int
controls(unsigned who_ref, unsigned what_ref)
{
	if (what_ref == NOTHING)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	OBJ who, what;

	qdb_get(obj_hd, &who, &who_ref);
	if (who.type != TYPE_ENTITY)
		who_ref = who.owner;

	qdb_get(obj_hd, &what, &what_ref);

	/* Wizard controls everything */
	if (ent_get(who_ref).flags & EF_WIZARD) {
		if(what.owner == ROOT && who_ref == ROOT)
			return 0;
		else
			return 1;
	}

	/* owners control their own stuff */
	return (who_ref == what.owner);
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l, __VA_ARGS__)

const char *
unparse(unsigned loc_ref)
{
	static char buf[BUFSIZ];
        size_t buf_l = 0;

	if (loc_ref == NOTHING)
		return "*NOTHING*";

	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);

	BUFF("%s(#%u)", loc.name, loc_ref);

	buf[buf_l] = '\0';
	return buf;
}

void
look_at(unsigned player_ref, unsigned loc_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);
	unsigned thing_ref;

	fbcp_item(player_ref, loc_ref, 1);
	switch (loc.type) {
	case TYPE_ROOM:
		break;
	case TYPE_ENTITY: // falls through
	default:
		eplayer.last_observed = loc_ref;
		ent_set(player_ref, &eplayer);
		qdb_put(obs_hd, &loc_ref, &player_ref);
	}

        if (loc_ref != player_ref && loc.type == TYPE_ENTITY && !(eplayer.flags & EF_WIZARD))
                return;

	// use callbacks for mcp like this versus telnet
	qdb_cur_t c = qdb_iter(contents_hd, &loc_ref);
	while (qdb_next(&loc_ref, &thing_ref, &c))
		fbcp_item(player_ref, thing_ref, 0);

	nd_twritef(player_ref, "%s\n", unparse(loc_ref));

        char buf[BUFSIZ];
        size_t buf_l = 0;

	qdb_cur_t c2 = qdb_iter(contents_hd, &loc_ref);
	while (qdb_next(&loc_ref, &thing_ref, &c2)) {
	/* check to see if there is anything there */
			if (thing_ref == player_ref)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"%s\r\n", unparse(thing_ref));
	}

        buf[buf_l] = '\0';
        nd_twritef(player_ref, "Contents: %s", buf);
}

void
look_around(unsigned player_ref)
{
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	look_at(player_ref, player.location);
	view(player_ref);
}

#define ADAM_SKEL_REF 0

void
do_look_at(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd), thing_ref;
	OBJ player, thing;
	char *name = argv[1];

	if (*name == '\0') {
		qdb_get(obj_hd, &player, &player_ref);
		thing_ref = player.location;
	} else if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_here(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_me(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
		  )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	qdb_get(obj_hd, &thing, &thing_ref);
	switch (thing.type) {
	case TYPE_ROOM:
		view(player_ref);
	default:
		look_at(player_ref, thing_ref);
		break;
	}
}

int
do_status(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	SIC_CALL(NULL, on_status, player_ref);
	return 0;
}

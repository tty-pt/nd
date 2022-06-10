/* $Header$ */


#include "copyright.h"
#include "config.h"

#include <string.h>
#include "mdb.h"
#include "externs.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "props.h"
#include "item.h"
#include "plant.h"

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

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

struct icon
icon(dbref what)
{
        static char buf[BUFSIZ];
        struct icon ret = {
                .actions = ACT_LOOK,
                .icon = ANSI_RESET ANSI_BOLD "?",
        };
        dbref aux;
        switch (OBJECT(what)->type) {
        case TYPE_ROOM:
                ret.icon = ANSI_FG_YELLOW "-";
                break;
        case TYPE_ENTITY:
                ret.actions |= ACT_KILL;
		ret.icon = ANSI_BOLD ANSI_FG_YELLOW "!";
		if (dialog_exists(what)) {
			ret.actions |= ACT_TALK;
		}
		if (ENTITY(what)->flags & EF_SHOP) {
                        ret.actions |= ACT_SHOP;
                        ret.icon = ANSI_BOLD ANSI_FG_GREEN "$";
		}
                break;
	case TYPE_CONSUMABLE:
		if (CONSUM(what)->drink) {
			ret.actions |= ACT_FILL;
			ret.icon = ANSI_BOLD ANSI_FG_BLUE "~";
		} else {
			ret.icon = ANSI_BOLD ANSI_FG_RED "o";
		}
		ret.actions |= ACT_CONSUME;
		break;
	case TYPE_PLANT:
		aux = PLANT(what)->plid;
		struct object_skeleton *obj_skel = PLANT_SKELETON(aux);
		struct plant_skeleton *pl = &obj_skel->sp.plant;

		ret.actions |= ACT_CHOP;
		snprintf(buf, sizeof(buf), "%s%c%s", pl->pre,
				PLANT(what)->size > PLANT_HALF ? pl->big : pl->small,
				pl->post); 

		// use the icon immediately
		ret.icon = buf;
		break;
	case TYPE_EQUIPMENT:
        case TYPE_THING:
                ret.actions |= ACT_GET;
                break;
        }
        return ret;
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFFER_LEN - buf_l, __VA_ARGS__)

static inline int
controls_link(dbref who, dbref what)
{
	switch (OBJECT(what)->type) {
	case TYPE_ROOM:
		if (controls(who, ROOM(what)->dropto))
			return 1;
		return 0;

	case TYPE_ENTITY:
		if (controls(who, ENTITY(what)->home))
			return 1;
		return 0;

	default:
		return 0;
	}
}

const char *
unparse_object(dbref player, dbref loc)
{
	static char buf[BUFFER_LEN];
        size_t buf_l = 0;
	if (player != NOTHING && OBJECT(player)->type != TYPE_ENTITY)
		player = OBJECT(player)->owner;

	switch (loc) {
	case NOTHING:
		return "*NOTHING*";
	default:
		if (loc < 0 || loc >= db_top)
			return "*INVALID*";

		if (OBJECT(loc)->type == TYPE_EQUIPMENT && EQUIPMENT(loc)->eqw) {
			unsigned n = EQUIPMENT(loc)->rare;

			if (n != 1)
                                BUFF("(%s) ", rarity_str[n]);
		}

                BUFF("%s", OBJECT(loc)->name);

		if (player == NOTHING || controls_link(player, loc))

                        BUFF("(#%d)", loc);

                buf[buf_l] = '\0';
		return buf;
	}
}

static const char *unparse_c_version = "$RCSfile$ $Revision: 1.11 $";
const char *get_unparse_c_version(void) { return unparse_c_version; }

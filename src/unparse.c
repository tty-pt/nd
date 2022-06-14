#include "object.h"
#include "plant.h"
#include "params.h"

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
icon(OBJ *what)
{
        static char buf[BUFSIZ];
        struct icon ret = {
                .actions = ACT_LOOK,
                .icon = ANSI_RESET ANSI_BOLD "?",
        };
        dbref aux;
        switch (what->type) {
        case TYPE_ROOM:
                ret.icon = ANSI_FG_YELLOW "-";
                break;
        case TYPE_ENTITY:
		{
			ENT *ewhat = &what->sp.entity;

			ret.actions |= ACT_KILL;
			ret.icon = ANSI_BOLD ANSI_FG_YELLOW "!";
			if (dialog_exists(what)) {
				ret.actions |= ACT_TALK;
			}
			if (ewhat->flags & EF_SHOP) {
				ret.actions |= ACT_SHOP;
				ret.icon = ANSI_BOLD ANSI_FG_GREEN "$";
			}
		}
                break;
	case TYPE_CONSUMABLE:
		{
			CON *cwhat = &what->sp.consumable;
			if (cwhat->drink) {
				ret.actions |= ACT_FILL;
				ret.icon = ANSI_BOLD ANSI_FG_BLUE "~";
			} else {
				ret.icon = ANSI_BOLD ANSI_FG_RED "o";
			}
			ret.actions |= ACT_CONSUME;
		}
		break;
	case TYPE_PLANT:
		{
			PLA *pwhat = &what->sp.plant;
			aux = pwhat->plid;
			struct object_skeleton *obj_skel = PLANT_SKELETON(aux);
			struct plant_skeleton *pl = &obj_skel->sp.plant;

			ret.actions |= ACT_CHOP;
			snprintf(buf, sizeof(buf), "%s%c%s", pl->pre,
					pwhat->size > PLANT_HALF ? pl->big : pl->small,
					pl->post); 

			// use the icon immediately
			ret.icon = buf;
		}
		break;
	case TYPE_EQUIPMENT:
        case TYPE_THING:
                ret.actions |= ACT_GET;
                break;
        }
        return ret;
}

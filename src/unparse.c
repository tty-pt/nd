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

const char *
unparse_flags(dbref thing)
{
	static char buf[BUFFER_LEN];
	char *p;
	const char *type_codes = "R-EPFG";

	p = buf;
	if (Typeof(thing) != TYPE_THING)
		*p++ = type_codes[Typeof(thing)];
	if (FLAGS(thing) & ~TYPE_MASK) {
		/* print flags */
		if (FLAGS(thing) & WIZARD)
			*p++ = 'W';
		if (FLAGS(thing) & LINK_OK)
			*p++ = 'L';

		if (FLAGS(thing) & KILL_OK)
			*p++ = 'K';

		if (FLAGS(thing) & DARK)
			*p++ = 'D';
		if (FLAGS(thing) & STICKY)
			*p++ = 'S';
		if (FLAGS(thing) & QUELL)
			*p++ = 'Q';
		if (FLAGS(thing) & BUILDER)
			*p++ = 'B';
		if (FLAGS(thing) & CHOWN_OK)
			*p++ = 'C';
		if (FLAGS(thing) & JUMP_OK)
			*p++ = 'J';
		if (FLAGS(thing) & HAVEN)
			*p++ = 'H';
		if (FLAGS(thing) & ABODE)
			*p++ = 'A';
	}
	*p = '\0';
	return buf;
}

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
        ACT_DRINK = 8,
        ACT_OPEN = 16,
        ACT_CHOP = 32,
        ACT_FILL = 64,
        ACT_GET = 128,
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
        switch (Typeof(what)) {
        case TYPE_ROOM:
                ret.icon = ANSI_FG_YELLOW "-";
                break;
        case TYPE_PLAYER:
                ret.actions |= ACT_KILL;
                ret.icon = ANSI_BOLD ANSI_FG_BLUE "#";
                break;
        case TYPE_THING:
                ret.actions |= ACT_GET;
                if (GETSHOP(what)) {
                        ret.actions |= ACT_SHOP;
                        ret.icon = ANSI_BOLD ANSI_FG_GREEN "$";
                        break;
                } else if (GETLID(what) >= 0) {
                        ret.actions |= ACT_KILL;
                        ret.icon = ANSI_BOLD ANSI_FG_YELLOW "!";
                        break;
                } else if (GETDRINK(what) >= 0) {
                        ret.actions |= ACT_DRINK | ACT_FILL;
                        ret.icon = ANSI_BOLD ANSI_FG_BLUE "~";
                        break;
                } else if ((aux = GETPLID(what)) >= 0) {
			struct object_skeleton *obj_skel = PLANT_SKELETON(aux);
                        struct plant_skeleton *pl = &obj_skel->sp.plant;

                        ret.actions |= ACT_CHOP | ACT_OPEN;
                        snprintf(buf, sizeof(buf), "%s%c%s", pl->pre,
                                 GETSTACK(what) > PLANT_HALF ? pl->big : pl->small,
                                 pl->post); 

                        // use the icon immediately
                        ret.icon = buf;
                        break;
                }
                break;
        }
        return ret;
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFFER_LEN - buf_l, __VA_ARGS__)

const char *
unparse_object(dbref player, dbref loc)
{
	static char buf[BUFFER_LEN];
        size_t buf_l = 0;
	if (player != NOTHING && Typeof(player) != TYPE_PLAYER)
		player = OWNER(player);

	switch (loc) {
	case NOTHING:
		return "*NOTHING*";
	case AMBIGUOUS:
		return "*AMBIGUOUS*";
	case HOME:
		return "*HOME*";
	default:
		if (loc < 0 || loc >= db_top)
			return "*INVALID*";

		unsigned n = GETSTACK(loc);
		if (n)
			BUFF("(%ux) ", n);

		if (GETEQW(loc)) {
			n = GETRARE(loc);

			if (n != 1)
                                BUFF("(%s) ", rarity_str[n]);
		}

                BUFF("%s", NAME(loc));

		if ((player == NOTHING) || (!(FLAGS(player) & STICKY) &&
			(can_link_to(player, NOTYPE, loc) ||
			 ((Typeof(loc) != TYPE_PLAYER) &&
			  (controls_link(player, loc) || (FLAGS(loc) & CHOWN_OK))))))

                        BUFF("(#%d%s)", loc, unparse_flags(loc));

                buf[buf_l] = '\0';
		return buf;
	}
}

static char boolexp_buf[BUFFER_LEN];
static char *buftop;

static void
unparse_boolexp1(dbref player, struct boolexp *b, boolexp_type outer_type, int fullname)
{
	if ((buftop - boolexp_buf) > (BUFFER_LEN / 2))
		return;
	if (b == TRUE_BOOLEXP) {
		strlcpy(buftop, "*UNLOCKED*", sizeof(boolexp_buf) - (buftop - boolexp_buf));
		buftop += strlen(buftop);
	} else {
		switch (b->type) {
		case BOOLEXP_AND:
			if (outer_type == BOOLEXP_NOT) {
				*buftop++ = '(';
			}
			unparse_boolexp1(player, b->sub1, b->type, fullname);
			*buftop++ = AND_TOKEN;
			unparse_boolexp1(player, b->sub2, b->type, fullname);
			if (outer_type == BOOLEXP_NOT) {
				*buftop++ = ')';
			}
			break;
		case BOOLEXP_OR:
			if (outer_type == BOOLEXP_NOT || outer_type == BOOLEXP_AND) {
				*buftop++ = '(';
			}
			unparse_boolexp1(player, b->sub1, b->type, fullname);
			*buftop++ = OR_TOKEN;
			unparse_boolexp1(player, b->sub2, b->type, fullname);
			if (outer_type == BOOLEXP_NOT || outer_type == BOOLEXP_AND) {
				*buftop++ = ')';
			}
			break;
		case BOOLEXP_NOT:
			*buftop++ = '!';
			unparse_boolexp1(player, b->sub1, b->type, fullname);
			break;
		case BOOLEXP_CONST:
			if (fullname) {
				strlcpy(buftop, unparse_object(player, b->thing), sizeof(boolexp_buf) - (buftop - boolexp_buf));
			} else {
				snprintf(buftop, sizeof(boolexp_buf) - (buftop - boolexp_buf), "#%d", b->thing);
			}
			buftop += strlen(buftop);
			break;
		case BOOLEXP_PROP:
			strlcpy(buftop, PropName(b->prop_check), sizeof(boolexp_buf) - (buftop - boolexp_buf));
			strlcat(buftop, ":", sizeof(boolexp_buf) - (buftop - boolexp_buf));
			if (PropType(b->prop_check) == PROP_STRTYP)
				strlcat(buftop, PropDataStr(b->prop_check), sizeof(boolexp_buf) - (buftop - boolexp_buf));
			buftop += strlen(buftop);
			break;
		default:
			abort();			/* bad type */
			break;
		}
	}
}

const char *
unparse_boolexp(dbref player, struct boolexp *b, int fullname)
{
	buftop = boolexp_buf;
	unparse_boolexp1(player, b, BOOLEXP_CONST, fullname);	/* no outer type */
	*buftop++ = '\0';

	return boolexp_buf;
}
static const char *unparse_c_version = "$RCSfile$ $Revision: 1.11 $";
const char *get_unparse_c_version(void) { return unparse_c_version; }

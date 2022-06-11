#include "web.h"
#include "mcp.h"
#include "externs.h"
#include "interface.h"
#include "geography.h"

void *
web_frame(int descr)
{
	McpFrame *mfr = descr_mcpframe(descr);

	if (!mfr || !mfr->enabled)
                return NULL;

        return mfr;
}

int
web_support(int descr)
{
        return !!web_frame(descr);
}

int
web_geo_view(OBJ *player, char *buf)
{
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
	if (!mfr)
                return 1;
        mcp_mesg_init(&msg, MCP_WEB_PKG, "view");
        mcp_mesg_arg_append(&msg, "data", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_art(int descr, char const *art)
{
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "art");
        mcp_mesg_arg_append(&msg, "src", art);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_look(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
        char buf2[BUFSIZ];
	char const *description = "";
	OBJ *thing;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "look");
        snprintf(buf2, sizeof(buf2), "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "dbref", buf2);

	OBJ *last_observed = eplayer->last_observed;
	if (last_observed)
		observer_remove(last_observed, player);

        if (loc->type == TYPE_ROOM) {
		ROO *rloc = &loc->sp.room;
                mcp_mesg_arg_append(&msg, "room", "1");
                snprintf(buf, sizeof(buf), "%u", rloc->exits);
                mcp_mesg_arg_append(&msg, "exits", buf);
        } else {
		if (loc->type == TYPE_ENTITY) {
			ENT *eloc = &loc->sp.entity;
			if (eloc->flags & EF_SHOP)
				mcp_mesg_arg_append(&msg, "shop", "1");
		}

		eplayer->last_observed = loc;
		observer_add(loc, player);
	}

        mcp_mesg_arg_append(&msg, "art", loc->art);
        mcp_mesg_arg_append(&msg, "name", loc->name);
        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, loc));
	if (loc->description)
		description = loc->description;
        mcp_mesg_arg_append(&msg, "description", description);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);

        if (loc != player && loc->type == TYPE_ENTITY && !(eplayer->flags & EF_WIZARD))
                return 0;

	// use callbacks for mcp like this versus telnet
        FOR_LIST(thing, loc->contents) {
		if (thing == player)
			continue;

		struct icon ico = icon(thing);
		mcp_mesg_init(&msg, MCP_WEB_PKG, "look-content");
		mcp_mesg_arg_append(&msg, "loc", buf2);
		snprintf(buf, sizeof(buf), "%d", object_ref(thing));
		mcp_mesg_arg_append(&msg, "dbref", buf);
		mcp_mesg_arg_append(&msg, "name", thing->name);
		mcp_mesg_arg_append(&msg, "pname", unparse_object(player, thing));
		mcp_mesg_arg_append(&msg, "icon", ico.icon);
		snprintf(buf, sizeof(buf), "%d", thing->value);
		mcp_mesg_arg_append(&msg, "price", buf);
		snprintf(buf, sizeof(buf), "%d", ico.actions);
		mcp_mesg_arg_append(&msg, "avatar", thing->avatar);
		mcp_mesg_arg_append(&msg, "actions", buf);
		mcp_frame_output_mesg(mfr, &msg);
		mcp_mesg_clear(&msg);
        }

        return 0;
}

void web_room_mcp(OBJ *room, void *msg) {
	OBJ *tmp;

	for (tmp = room->contents;
	     tmp;
	     tmp = tmp->next)
	{
		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;
		McpFrame *mfr = web_frame(etmp->fd);

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

void web_obs_mcp(OBJ *thing, void *msg) {
	struct observer_node *node;

	for (node = thing->first_observer; node; node = node->next)
	{
		OBJ *tmp = node->who;

		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;

		McpFrame *mfr = web_frame(etmp->fd);

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

void do_meme(command_t *cmd) {
        OBJ *player = object_get(cmd->player);
        const char *url = cmd->argv[1];
	McpMesg msg;
        // FIXME should also work in the terminal
        mcp_mesg_init(&msg, MCP_WEB_PKG, "meme");
        mcp_mesg_arg_append(&msg, "who", player->name);
        mcp_mesg_arg_append(&msg, "url", url);
        web_room_mcp(player->location, &msg);
        mcp_mesg_clear(&msg);
}

void
web_content_out(OBJ *loc, OBJ *thing) {
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "out");
	snprintf(buf, BUFSIZ, "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", object_ref(thing));
        mcp_mesg_arg_append(&msg, "dbref", buf);
        /* mcp_mesg_arg_append(&msg, "name", unparse_object(player, thing)); */

	if (loc->type == TYPE_ROOM)
		web_room_mcp(loc, &msg);
	else
		web_obs_mcp(loc, &msg);

        mcp_mesg_clear(&msg);
}

void
web_content_in(OBJ *loc, OBJ *thing) {
	struct icon ico = icon(thing);
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "in");
	snprintf(buf, BUFSIZ, "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", object_ref(thing));
        mcp_mesg_arg_append(&msg, "dbref", buf);
	mcp_mesg_arg_append(&msg, "name", thing->name);
	mcp_mesg_arg_append(&msg, "pname", unparse_object(object_get(HUMAN_BEING), thing));
	mcp_mesg_arg_append(&msg, "icon", ico.icon);
	snprintf(buf, sizeof(buf), "%d", ico.actions);
        mcp_mesg_arg_append(&msg, "avatar", thing->avatar);
	mcp_mesg_arg_append(&msg, "actions", buf);

	if (loc->type == TYPE_ROOM)
		web_room_mcp(loc, &msg);
	else
		web_obs_mcp(loc, &msg);

        mcp_mesg_clear(&msg);
}

int
web_auth_fail(int descr, int reason) {
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "auth-fail");
	snprintf(buf, sizeof(buf), "%d", reason);
        mcp_mesg_arg_append(&msg, "reason", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_auth_success(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "auth-success");
	snprintf(buf, sizeof(buf), "%d", object_ref(player));
        mcp_mesg_arg_append(&msg, "player", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_stats(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "stats");
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_STR]);
        mcp_mesg_arg_append(&msg, "str", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_CON]);
        mcp_mesg_arg_append(&msg, "con", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_DEX]);
        mcp_mesg_arg_append(&msg, "dex", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_INT]);
        mcp_mesg_arg_append(&msg, "int", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_WIZ]);
        mcp_mesg_arg_append(&msg, "wiz", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DODGE).value);
        mcp_mesg_arg_append(&msg, "dodge", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DMG).value);
        mcp_mesg_arg_append(&msg, "dmg", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, MDMG).value);
        mcp_mesg_arg_append(&msg, "mdmg", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DEF).value);
        mcp_mesg_arg_append(&msg, "def", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, MDEF).value);
        mcp_mesg_arg_append(&msg, "mdef", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_bars(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "bars");
	snprintf(buf, sizeof(buf), "%d", eplayer->hp);
        mcp_mesg_arg_append(&msg, "hp", buf);
	snprintf(buf, sizeof(buf), "%d", HP_MAX(eplayer));
        mcp_mesg_arg_append(&msg, "hpMax", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->mp);
        mcp_mesg_arg_append(&msg, "mp", buf);
	snprintf(buf, sizeof(buf), "%d", MP_MAX(eplayer));
        mcp_mesg_arg_append(&msg, "mpMax", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_dialog_start(OBJ *player, OBJ *npc, const char *dialog)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	const char *text;
	int i, n;
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "dialog-start");
	snprintf(buf, sizeof(buf), "%d", object_ref(npc));
        mcp_mesg_arg_append(&msg, "npc", buf);
	/* snprintf(buf, sizeof(buf), "%d", npc); */
        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/text", dialog);
        text = GETMESG(npc, buf);
        mcp_mesg_arg_append(&msg, "text", text);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/n", dialog);
	n = get_property_value(npc, buf);

	for (i = 0; i < n; i++) {
		const char *answer;
		mcp_mesg_init(&msg, MCP_WEB_PKG, "dialog-answer");
		snprintf(buf, sizeof(buf), "%d", i);
		mcp_mesg_arg_append(&msg, "id", buf);
		snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/text", dialog, i);
		answer = GETMESG(npc, buf);
		mcp_mesg_arg_append(&msg, "text", answer);
		mcp_frame_output_mesg(mfr, &msg);
		mcp_mesg_clear(&msg);
	}

	return 0;
}

int
web_dialog_stop(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "dialog-stop");
	snprintf(buf, sizeof(buf), "%d", 1);
        mcp_mesg_arg_append(&msg, "ignore", buf);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);
	return 0;
}

static inline void
web_equipment_item(OBJ *player, enum equipment_slot eql)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);
        dbref eq = EQUIP(eplayer, eql);
        if (eq == NOTHING || !eq)
                return;
	OBJ *oeq = object_get(eq);
        CBUG(!mfr);
        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment-item");
	snprintf(buf, sizeof(buf), "%d", eq);
        mcp_mesg_arg_append(&msg, "dbref", buf);
	snprintf(buf, sizeof(buf), "%d", eql);
        mcp_mesg_arg_append(&msg, "eql", buf);
        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, oeq));
        mcp_mesg_arg_append(&msg, "avatar", oeq->avatar);
        struct icon ico = icon(oeq);
        mcp_mesg_arg_append(&msg, "icon", ico.icon);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);
}

int
web_equipment(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(eplayer->fd);

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment");
	snprintf(buf, sizeof(buf), "%d", 1);
        mcp_mesg_arg_append(&msg, "ignore", buf);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);

        web_equipment_item(player, ES_HEAD);
        web_equipment_item(player, ES_NECK);
        web_equipment_item(player, ES_CHEST);
        web_equipment_item(player, ES_BACK);
        web_equipment_item(player, ES_RHAND);
        web_equipment_item(player, ES_LFINGER);
        web_equipment_item(player, ES_RFINGER);
        web_equipment_item(player, ES_PANTS);

	return 0;
}

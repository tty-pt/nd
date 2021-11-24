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
web_geo_view(int descr, char *buf)
{
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
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
web_look(command_t *cmd, dbref loc, char const *description)
{
	dbref player = cmd->player;
	int descr = cmd->fd;
        char buf[BUFSIZ];
        char buf2[BUFSIZ];
        dbref thing, can_see_loc;
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "look");
        snprintf(buf2, sizeof(buf2), "%d", loc);
        mcp_mesg_arg_append(&msg, "dbref", buf2);

	dbref last_observed = PLAYER_SP(player)->last_observed;
	if (last_observed != NOTHING)
		db_obs_remove(last_observed, player);

        if (Typeof(loc) == TYPE_ROOM) {
                mcp_mesg_arg_append(&msg, "room", "1");
                snprintf(buf, sizeof(buf), "%d", gexits(cmd, loc));
                mcp_mesg_arg_append(&msg, "exits", buf);
        } else {
		if (GETSHOP(loc))
			mcp_mesg_arg_append(&msg, "shop", "1");

		PLAYER_SP(player)->last_observed = loc;
		db_obs_add(loc, player);
	}

        mcp_mesg_arg_append(&msg, "art", GETMESG(loc, MESGPROP_ART));
        mcp_mesg_arg_append(&msg, "name", NAME(loc));
        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, loc));
        mcp_mesg_arg_append(&msg, "description", description);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);

        if (Typeof(loc) == TYPE_EXIT)
                return 0;

        if (loc != player && Typeof(loc) == TYPE_PLAYER && !Wizard(player))
                return 0;

	can_see_loc = (!Dark(loc) || controls(player, loc));

	// use callbacks for mcp like this versus telnet
        DOLIST(thing, DBFETCH(loc)->contents) {
                if (can_see(player, thing, can_see_loc)) {
                        struct icon ico = icon(thing);
                        mcp_mesg_init(&msg, MCP_WEB_PKG, "look-content");
                        mcp_mesg_arg_append(&msg, "loc", buf2);
                        snprintf(buf, sizeof(buf), "%d", thing);
                        mcp_mesg_arg_append(&msg, "dbref", buf);
                        mcp_mesg_arg_append(&msg, "name", NAME(thing));
                        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, thing));
                        mcp_mesg_arg_append(&msg, "icon", ico.icon);
                        snprintf(buf, sizeof(buf), "%d", GETVALUE(thing));
                        mcp_mesg_arg_append(&msg, "price", buf);
                        snprintf(buf, sizeof(buf), "%d", ico.actions);
                        mcp_mesg_arg_append(&msg, "avatar", GETAVATAR(thing));
                        mcp_mesg_arg_append(&msg, "actions", buf);
                        mcp_frame_output_mesg(mfr, &msg);
                        mcp_mesg_clear(&msg);
                }
        }

        return 0;
}

void web_room_mcp(dbref room, void *msg) {
	dbref tmp;

	for (tmp = DBFETCH(room)->contents;
	     tmp > 0;
	     tmp = DBFETCH(tmp)->next)
	{
		if (Typeof(tmp) != TYPE_PLAYER)
			continue;

		McpFrame *mfr = web_frame(PLAYER_FD(OWNER(tmp)));

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

void web_obs_mcp(dbref thing, void *msg) {
	struct object *o = DBFETCH(thing);
	struct observer_node *node = o->first_observer;

	for (node = o->first_observer; node; node = node->next)
	{
		dbref tmp = node->who;

		if (Typeof(tmp) != TYPE_PLAYER)
			continue;

		McpFrame *mfr = web_frame(PLAYER_FD(OWNER(tmp)));

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

void do_meme(command_t *cmd) {
        dbref player = cmd->player;
        const char *url = cmd->argv[1];
	McpMesg msg;
        // FIXME should also work in the terminal
        mcp_mesg_init(&msg, MCP_WEB_PKG, "meme");
        mcp_mesg_arg_append(&msg, "who", NAME(player));
        mcp_mesg_arg_append(&msg, "url", url);
        web_room_mcp(getloc(player), &msg);
        mcp_mesg_clear(&msg);
}

void
web_content_out(dbref loc, dbref thing) {
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "out");
	snprintf(buf, BUFSIZ, "%d", loc);
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", thing);
        mcp_mesg_arg_append(&msg, "dbref", buf);
        /* mcp_mesg_arg_append(&msg, "name", unparse_object(player, thing)); */

	if (Typeof(loc) == TYPE_ROOM)
		web_room_mcp(loc, &msg);
	else
		web_obs_mcp(loc, &msg);

        mcp_mesg_clear(&msg);
}

void
web_content_in(dbref loc, dbref thing) {
	/* dbref loc = DBFETCH(thing)->location; */
	struct icon ico = icon(thing);
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "in");
	snprintf(buf, BUFSIZ, "%d", loc);
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", thing);
        mcp_mesg_arg_append(&msg, "dbref", buf);
	mcp_mesg_arg_append(&msg, "name", NAME(thing));
	mcp_mesg_arg_append(&msg, "pname", unparse_object(HUMAN_BEING, thing));
	mcp_mesg_arg_append(&msg, "icon", ico.icon);
	snprintf(buf, sizeof(buf), "%d", ico.actions);
        mcp_mesg_arg_append(&msg, "avatar", GETAVATAR(thing));
	mcp_mesg_arg_append(&msg, "actions", buf);

	if (Typeof(loc) == TYPE_ROOM)
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
web_auth_success(int descr, dbref player) {
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "auth-success");
	snprintf(buf, sizeof(buf), "%d", player);
        mcp_mesg_arg_append(&msg, "player", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_stats(dbref player) {
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));
	struct mob *mob = MOB(player);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "stats");
	snprintf(buf, sizeof(buf), "%d", GETSTAT(player, STR));
        mcp_mesg_arg_append(&msg, "str", buf);
	snprintf(buf, sizeof(buf), "%d", GETSTAT(player, CON));
        mcp_mesg_arg_append(&msg, "con", buf);
	snprintf(buf, sizeof(buf), "%d", GETSTAT(player, DEX));
        mcp_mesg_arg_append(&msg, "dex", buf);
	snprintf(buf, sizeof(buf), "%d", GETSTAT(player, INT));
        mcp_mesg_arg_append(&msg, "int", buf);
	snprintf(buf, sizeof(buf), "%d", GETSTAT(player, WIZ));
        mcp_mesg_arg_append(&msg, "wiz", buf);
	snprintf(buf, sizeof(buf), "%d", MOB_EV(mob, DODGE));
        mcp_mesg_arg_append(&msg, "dodge", buf);
	snprintf(buf, sizeof(buf), "%d", MOB_EV(mob, DMG));
        mcp_mesg_arg_append(&msg, "dmg", buf);
	snprintf(buf, sizeof(buf), "%d", MOB_EV(mob, MDMG));
        mcp_mesg_arg_append(&msg, "mdmg", buf);
	snprintf(buf, sizeof(buf), "%d", MOB_EV(mob, DEF));
        mcp_mesg_arg_append(&msg, "def", buf);
	snprintf(buf, sizeof(buf), "%d", MOB_EV(mob, MDEF));
        mcp_mesg_arg_append(&msg, "mdef", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_bars(dbref player) {
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));
	struct mob *mob = MOB(player);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "bars");
	snprintf(buf, sizeof(buf), "%d", mob->hp);
        mcp_mesg_arg_append(&msg, "hp", buf);
	snprintf(buf, sizeof(buf), "%d", HP_MAX(player));
        mcp_mesg_arg_append(&msg, "hpMax", buf);
	snprintf(buf, sizeof(buf), "%d", mob->mp);
        mcp_mesg_arg_append(&msg, "mp", buf);
	snprintf(buf, sizeof(buf), "%d", MP_MAX(player));
        mcp_mesg_arg_append(&msg, "mpMax", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
web_dialog_start(dbref player, dbref npc, const char *dialog)
{
        char buf[BUFSIZ];
	const char *text;
	int i, n;
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "dialog-start");
	snprintf(buf, sizeof(buf), "%d", npc);
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
web_dialog_stop(dbref player)
{
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));

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
web_equipment_item(dbref player, enum eq eql)
{
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));
        dbref eq = GETEQ(player, eql);
        if (!eq)
                return;
        CBUG(!mfr);
        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment-item");
	snprintf(buf, sizeof(buf), "%d", eq);
        mcp_mesg_arg_append(&msg, "dbref", buf);
	snprintf(buf, sizeof(buf), "%d", eql);
        mcp_mesg_arg_append(&msg, "eql", buf);
        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, eq));
        mcp_mesg_arg_append(&msg, "avatar", GETAVATAR(eq));
        struct icon ico = icon(eq);
        mcp_mesg_arg_append(&msg, "icon", ico.icon);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);
}

int
web_equipment(dbref player)
{
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = web_frame(PLAYER_FD(player));

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment");
	snprintf(buf, sizeof(buf), "%d", 1);
        mcp_mesg_arg_append(&msg, "ignore", buf);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);

        web_equipment_item(player, HEAD);
        web_equipment_item(player, NECK);
        web_equipment_item(player, CHEST);
        web_equipment_item(player, BACK);
        web_equipment_item(player, RHAND);
        web_equipment_item(player, LFINGER);
        web_equipment_item(player, RFINGER);
        web_equipment_item(player, PANTS);

	return 0;
}

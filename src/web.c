#include "web.h"
#include "mcp.h"
#include "externs.h"
#include "interface.h"
#include "geography.h"

void *
web_frame(int descr)
{
	McpFrame *mfr = descr_mcpframe(descr);
	McpVer supp;

	if (!mfr)
                return NULL;

        if (!mfr->enabled)
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
web_art(int descr, char const *art, char *buf, size_t n)
{
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	if (!mfr)
                return 1;

        snprintf(buf, n, "../art/%s.png", art);
        if (access(buf, F_OK) == -1)
                return 1;

        snprintf(buf, n, "./art/%s.png", art);
        mcp_mesg_init(&msg, MCP_WEB_PKG, "art");
        mcp_mesg_arg_append(&msg, "src", buf);
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
        dbref thing, can_see_loc;
	McpMesg msg;
	McpFrame *mfr = web_frame(descr);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "look");
        if (Typeof(loc) == TYPE_ROOM) {
                mcp_mesg_arg_append(&msg, "room", "1");
                snprintf(buf, sizeof(buf), "%d", gexits(cmd, loc));
                mcp_mesg_arg_append(&msg, "exits", buf);

        }
        mcp_mesg_arg_append(&msg, "name", unparse_object(player, loc));
        mcp_mesg_arg_append(&msg, "description", description);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);

	can_see_loc = (!Dark(loc) || controls(player, loc));

	// use callbacks for mcp like this versus telnet
        DOLIST(thing, DBFETCH(loc)->contents) {
                if (can_see(player, thing, can_see_loc)) {
                        struct icon ico = icon(thing);
                        mcp_mesg_init(&msg, MCP_WEB_PKG, "look-content");
                        snprintf(buf, sizeof(buf), "%d", thing);
                        mcp_mesg_arg_append(&msg, "dbref", buf);
                        mcp_mesg_arg_append(&msg, "name", NAME(thing));
                        mcp_mesg_arg_append(&msg, "pname", unparse_object(player, thing));
                        mcp_mesg_arg_append(&msg, "icon", ico.icon);
                        snprintf(buf, sizeof(buf), "%d", ico.actions);
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
web_content_out(dbref thing) {
	dbref loc = DBFETCH(thing)->location;
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "out");
	snprintf(buf, BUFSIZ, "%d", thing);
        mcp_mesg_arg_append(&msg, "dbref", buf);
        /* mcp_mesg_arg_append(&msg, "name", unparse_object(player, thing)); */
        web_room_mcp(loc, &msg);
        mcp_mesg_clear(&msg);
}

void
web_content_in(dbref thing) {
	dbref loc = DBFETCH(thing)->location;
	struct icon ico = icon(thing);
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "in");
	snprintf(buf, BUFSIZ, "%d", thing);
        mcp_mesg_arg_append(&msg, "dbref", buf);
	mcp_mesg_arg_append(&msg, "name", NAME(thing));
	mcp_mesg_arg_append(&msg, "pname", unparse_object(HUMAN_BEING, thing));
	mcp_mesg_arg_append(&msg, "icon", ico.icon);
	snprintf(buf, sizeof(buf), "%d", ico.actions);
	mcp_mesg_arg_append(&msg, "actions", buf);

        web_room_mcp(loc, &msg);
        mcp_mesg_clear(&msg);
}

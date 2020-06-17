#include "web.h"
#include "externs.h"
#include "interface.h"
#include "geography.h"
#undef NDEBUG
#include "debug.h"

extern int auth(int descr, char *user, char *password);
extern void identify(int descr, unsigned ip, unsigned old);

/* void * */
/* web_frame(int descr) */
/* { */
/* 	McpFrame *mfr = descr_mcpframe(descr); */
/* 	McpVer supp; */

/* 	if (!mfr) */
/*                 return NULL; */

/*         supp = mcp_frame_package_supported(mfr, MCP_WEB_PKG); */

/* 	if (supp.verminor == 0 && supp.vermajor == 0) */
/* 		return NULL; */

/*         return mfr; */
/* } */

/* int */
/* web_support(int descr) */
/* { */
/*         return !!web_frame(descr); */
/* } */

/* int */
/* web_geo_view(int descr, char *buf) */
/* { */
/* 	McpMesg msg; */
/* 	McpFrame *mfr = web_frame(descr); */
/* 	if (!mfr) */
/*                 return 1; */
/*         mcp_mesg_init(&msg, MCP_WEB_PKG, "view"); */
/*         mcp_mesg_arg_append(&msg, "data", buf); */
/*         mcp_frame_output_mesg(mfr, &msg); */
/*         mcp_mesg_clear(&msg); */
/*         return 0; */
/* } */

/* int */
/* web_art(int descr, char const *art, char *buf, size_t n) */
/* { */
/* 	McpMesg msg; */
/* 	McpFrame *mfr = web_frame(descr); */
/* 	if (!mfr) */
/*                 return 1; */

/*         snprintf(buf, n, "../art/%s.png", art); */
/*         if (access(buf, F_OK) == -1) */
/*                 return 1; */

/*         snprintf(buf, n, "./art/%s.png", art); */
/*         mcp_mesg_init(&msg, MCP_WEB_PKG, "art"); */
/*         mcp_mesg_arg_append(&msg, "src", buf); */
/*         mcp_frame_output_mesg(mfr, &msg); */
/*         mcp_mesg_clear(&msg); */
/*         return 0; */
/* } */

/* int */
/* web_look(int descr, dbref player, dbref loc, char const *description) */
/* { */
/*         char buf[BUFSIZ]; */
/*         dbref thing, can_see_loc; */
/* 	McpMesg msg; */
/* 	McpFrame *mfr = web_frame(descr); */
/* 	if (!mfr) */
/*                 return 1; */

/*         mcp_mesg_init(&msg, MCP_WEB_PKG, "look"); */
/*         if (Typeof(loc) == TYPE_ROOM) { */
/*                 mcp_mesg_arg_append(&msg, "room", "1"); */
/*                 snprintf(buf, sizeof(buf), "%d", gexits(descr, player, loc)); */
/*                 mcp_mesg_arg_append(&msg, "exits", buf); */

/*         } */
/*         mcp_mesg_arg_append(&msg, "name", unparse_object(player, loc)); */
/*         mcp_mesg_arg_append(&msg, "description", description); */
/*         mcp_frame_output_mesg(mfr, &msg); */
/*         mcp_mesg_clear(&msg); */

/* 	can_see_loc = (!Dark(loc) || controls(player, loc)); */

/* 	// use callbacks for mcp like this versus telnet */
/*         DOLIST(thing, DBFETCH(loc)->contents) { */
/*                 if (can_see(player, thing, can_see_loc)) { */
/*                         struct icon ico = icon(thing); */
/*                         mcp_mesg_init(&msg, MCP_WEB_PKG, "look-content"); */
/*                         mcp_mesg_arg_append(&msg, "name", NAME(thing)); */
/*                         mcp_mesg_arg_append(&msg, "pname", unparse_object(player, thing)); */
/*                         mcp_mesg_arg_append(&msg, "icon", ico.icon); */
/*                         snprintf(buf, sizeof(buf), "%d", ico.actions); */
/*                         mcp_mesg_arg_append(&msg, "actions", buf); */
/*                         mcp_frame_output_mesg(mfr, &msg); */
/*                         mcp_mesg_clear(&msg); */
/*                 } */
/*         } */

/*         return 0; */
/* } */

/* void web_room_mcp(dbref room, void *msg) { */
/* 	dbref tmp; */

/* 	for (tmp = DBFETCH(room)->contents; */
/* 	     tmp > 0; */
/* 	     tmp = DBFETCH(tmp)->next) */
/* 	{ */
/* 		int *darr, dcount, di; */

/* 		if (Typeof(tmp) != TYPE_PLAYER) */
/* 			continue; */

/* 		darr = get_player_descrs(OWNER(tmp), &dcount); */
/* 		for (di = 0; di < dcount; di++) { */
/* 			McpFrame *mfr = web_frame(darr[di]); */
/* 			if (!mfr) */
/* 				continue; */
/* 			mcp_frame_output_mesg(mfr, msg); */
/* 		} */
/* 	} */
/* } */

/* void do_meme(int descr, dbref player, char const *url) { */
/* 	McpMesg msg; */
/*         // FIXME should also work in the terminal */
/*         mcp_mesg_init(&msg, MCP_WEB_PKG, "meme"); */
/*         mcp_mesg_arg_append(&msg, "who", NAME(player)); */
/*         mcp_mesg_arg_append(&msg, "url", url); */
/*         web_room_mcp(getloc(player), &msg); */
/*         mcp_mesg_clear(&msg); */
/* } */

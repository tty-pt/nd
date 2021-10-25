#ifndef WEB_H
#define WEB_H
#include <sys/types.h>
#include "mdb.h"
#include "command.h"
#define MCP_WEB_PKG "web"

/* void web_logout(int descr); */
int web_geo_view(int descr, char *buf);
int web_art(int descr, const char *art);
int web_support(int descr);
int web_look(command_t *cmd, dbref loc, char const *description);
void web_room_mcp(dbref room, void *msg);
void * web_frame(int descr);
void do_meme(command_t *cmd);
void web_content_out(dbref thing);
void web_content_in(dbref thing);
int web_auth_fail(int descr, int reason);
int web_auth_success(int descr, dbref player);
#endif

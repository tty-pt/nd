#ifndef WEB_H
#define WEB_H
#include <sys/types.h>
#include "mdb.h"
#include "command.h"
#define MCP_WEB_PKG "web"

/* void web_logout(int descr); */
int web_geo_view(OBJ *player, char *buf);
int web_art(int descr, const char *art);
int web_support(int descr);
int web_look(OBJ *player, OBJ *target);
void web_room_mcp(OBJ *room, void *msg);
void * web_frame(int descr);
void do_meme(command_t *cmd);
void web_content_out(OBJ *loc, OBJ *thing);
void web_content_in(OBJ *loc, OBJ *thing);
int web_auth_fail(int descr, int reason);
int web_auth_success(OBJ *player);
int web_stats(OBJ *player);
int web_bars(OBJ *player);
int web_dialog_start(OBJ *player, OBJ *npc, const char *dialog);
int web_dialog_stop(OBJ *player);
int web_equipment(OBJ *player);

#endif

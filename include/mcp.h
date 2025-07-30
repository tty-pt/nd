#ifndef MCP_H
#define MCP_H

#include "uapi/object.h"
#include "uapi/mcp.h"
#include "view.h"

void mcp_view(ENT *eplayer, char *buf);
void mcp_art(int descr, const char *art);
void mcp_look(unsigned player_ref, unsigned target_ref);
void mcp_auth_fail(int descr, int reason);
void mcp_auth_success(unsigned player_ref);
void mcp_tod(unsigned player_ref, unsigned tod);
/*
void mcp_dialog_start(OBJ *player, OBJ *npc, const char *dialog);
void mcp_dialog_stop(OBJ *player);
*/
void mcp_equipment(unsigned player_ref);
void fbcp_view(unsigned player_ref, view_t *view);
void fbcp_view_buf(unsigned player_ref, char *view);

void fbcp_item(unsigned player_ref, unsigned obj_ref, unsigned char dynflags);

void mcp_action(unsigned player_ref, unsigned id,
		char *label, char *icon);

#endif							/* MCP_H */

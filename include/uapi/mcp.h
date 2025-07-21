#ifndef UAPI_MCP_H
#define UAPI_MCP_H

#include <stddef.h>

typedef void fbcp_item_t(unsigned player_ref, unsigned obj_ref, unsigned char dynflags);
fbcp_item_t fbcp_item;

typedef void fbcp_t(unsigned player_ref, size_t len, unsigned char iden, void *data);
fbcp_t fbcp;

typedef void mcp_content_out_t(unsigned loc_ref, unsigned thing_ref);
mcp_content_out_t mcp_content_out;

typedef void mcp_content_in_t(unsigned loc_ref, unsigned thing_ref);
mcp_content_in_t mcp_content_in;

typedef void mcp_stats_t(unsigned player_ref);
mcp_stats_t mcp_stats;

#endif

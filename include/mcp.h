#ifndef MCP_H
#define MCP_H

#include "object.h"
#include "view.h"

typedef void *connection_t;

typedef struct McpVersion_T {
	unsigned short vermajor;	/* major version number */
	unsigned short verminor;	/* minor version number */
} McpVer;

typedef struct McpArgPart_T {
	struct McpArgPart_T *next;
	char *value;
} McpArgPart;


typedef struct McpArg_T {
	struct McpArg_T *next;
	char *name;
	McpArgPart *value;
	McpArgPart *last;
	int was_shown;
} McpArg;


typedef struct McpMesg_T {
	struct McpMesg_T *next;
	char *package;
	char *mesgname;
	char *datatag;
	McpArg *args;
	int incomplete;
	int bytes;
} McpMesg;

struct McpFrame_T;
typedef void (*McpPkg_CB) (struct McpFrame_T * mfr, McpMesg * mesg, McpVer version, void *context);
typedef void (*ContextCleanup_CB) (void *context);

typedef struct McpPkg_T {
	char *pkgname;				/* Name of the package */
	McpVer minver;				/* min supported version number */
	McpVer maxver;				/* max supported version number */
	McpPkg_CB callback;			/* function to call with mesgs */
	void *context;				/* user defined callback context */
	ContextCleanup_CB cleanup;  /* callback to use to free context */
	struct McpPkg_T *next;
} McpPkg;

typedef struct McpFrame_T {
	void *descriptor;			/* The descriptor to send output to */
	unsigned int enabled;		/* Flag denoting if MCP is enabled. */
	char *authkey;				/* Authorization key. */
	McpVer version;				/* Supported MCP version number. */
	McpPkg *packages;			/* Pkgs supported on this connection. */
	McpMesg *messages;			/* Partial messages, under construction. */
} McpFrame;

void mcp_frame_init(McpFrame * mfr, connection_t con);

int mcp_view(ENT *eplayer, char *buf);
int mcp_art(int descr, const char *art);
int mcp_look(OBJ *player, OBJ *target);
void mcp_content_out(OBJ *loc, OBJ *thing);
void mcp_content_in(OBJ *loc, OBJ *thing);
int mcp_auth_fail(int descr, int reason);
int mcp_auth_success(OBJ *player);
int mcp_stats(OBJ *player);
int mcp_bars(ENT *eplayer);
int mcp_dialog_start(OBJ *player, OBJ *npc, const char *dialog);
int mcp_dialog_stop(OBJ *player);
int mcp_equipment(OBJ *player);
int fbcp_view(ENT *eplayer, view_t *view);
int fbcp_view_buf(ENT *eplayer, char *view);

#endif							/* MCP_H */

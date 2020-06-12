#ifndef _P_MCP_H
#define _P_MCP_H

extern void prim_mcp_register(PRIM_PROTOTYPE);
extern void prim_mcp_register_event(PRIM_PROTOTYPE);
extern void prim_mcp_bind(PRIM_PROTOTYPE);
extern void prim_mcp_supports(PRIM_PROTOTYPE);
extern void prim_mcp_send(PRIM_PROTOTYPE);


#define PRIMS_MCP_FUNCS prim_mcp_register, prim_mcp_bind, prim_mcp_supports, \
		prim_mcp_send, prim_mcp_register_event

#define PRIMS_MCP_NAMES "MCP_REGISTER", "MCP_BIND", "MCP_SUPPORTS", \
		"MCP_SEND", "GUI_AVAILABLE", "GUI_DLOG_SHOW", \
		"GUI_DLOG_CLOSE", "GUI_VALUES_GET", "GUI_VALUE_GET", \
		"GUI_VALUE_SET", "GUI_CTRL_CREATE", "GUI_CTRL_COMMAND", \
		"GUI_DLOG_CREATE", "MCP_REGISTER_EVENT"


#define PRIMS_MCP_CNT 14

#endif /* _P_MCP_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef p_mcph_version
#define p_mcph_version
const char *p_mcp_h_version = "$RCSfile$ $Revision: 1.9 $";
#endif
#else
extern const char *p_mcp_h_version;
#endif


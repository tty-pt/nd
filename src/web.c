#include "web.h"
#include "mcp.h"
#include "externs.h"
#include "interface.h"
#undef NDEBUG
#include "debug.h"

#define MCP_WEB_PKG "com-qnixsoft-web"

extern int auth(int descr, char *user, char *password);
extern void identify(int descr, unsigned ip, unsigned old);

void
mcppkg_web(McpFrame * mfr, McpMesg * msg, McpVer ver, void *context)
{
	McpVer supp = mcp_frame_package_supported(mfr, MCP_WEB_PKG);
	McpMesg omsg;
        int descr = mcpframe_to_descr(mfr);

	if (supp.verminor == 0 && supp.vermajor == 0) {
		notify(descr, "MCP: "MCP_WEB_PKG" not supported.");
		return;
	}

        if (!string_compare(msg->mesgname, "auth")) {
                char *username, *password;
                username = mcp_mesg_arg_getline(msg, "username", 0);
                password = mcp_mesg_arg_getline(msg, "password", 0);
                if (auth(descr, username, password))
                        mcp_mesg_init(&omsg, MCP_WEB_PKG, "auth-error");
                else
                        mcp_mesg_init(&omsg, MCP_WEB_PKG, "auth-ok");
                mcp_frame_output_mesg(mfr, &omsg);
                mcp_mesg_clear(&omsg);
        } else if (!string_compare(msg->mesgname, "identify")) {
                char *ips = mcp_mesg_arg_getline(msg, "ip", 0);
                char *olds = mcp_mesg_arg_getline(msg, "old", 0);
                unsigned ip = strtoul(ips, NULL, 0);
                unsigned old = strtoul(olds, NULL, 0);
                identify(descr, ip, old);
        }
}

static inline McpFrame *
web_frame(int descr)
{
	McpFrame *mfr = descr_mcpframe(descr);
	McpVer supp;

	if (!mfr)
                return NULL;

        supp = mcp_frame_package_supported(mfr, MCP_WEB_PKG);

	if (supp.verminor == 0 && supp.vermajor == 0)
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

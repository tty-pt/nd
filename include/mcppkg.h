#ifndef _MCPPKG_H
#define _MCPPKG_H

void mcppkg_help_request(McpFrame * mfr, McpMesg * msg, McpVer ver, void *context);
void mcppkg_web(McpFrame * mfr, McpMesg * msg, McpVer ver, void *context);

#endif /* _MCPPKG_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef mcppkgh_version
#define mcppkgh_version
const char *mcppkg_h_version = "$RCSfile$ $Revision: 1.7 $";
#endif
#else
extern const char *mcppkg_h_version;
#endif


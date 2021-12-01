/* mcp.c: MUD Client Protocol.
   Part of the FuzzBall distribution. */

#include "config.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mdb.h"
#include "externs.h"
#include "mcp.h"

#define MCP_MESG_PREFIX		"#$#"
#define MCP_QUOTE_PREFIX	"#$\""
#define MCP_ARG_EMPTY		"\"\""
#define MCP_INIT_PKG		"mcp"
#define MCP_DATATAG			"_data-tag"
#define MCP_INIT_MESG		"mcp "
#define MCP_NEGOTIATE_PKG	"mcp-negotiate"

/* Defined elsewhere.  Used to send text to a connection */
void SendText(McpFrame * mfr, const char *mesg);

McpPkg *mcp_PackageList = NULL;

int
strcmp_nocase(const char *s1, const char *s2)
{
	while (*s1 && tolower(*s1) == tolower(*s2))
		s1++, s2++;
	return (tolower(*s1) - tolower(*s2));
}

/* Used internally to escape and quote argument values. */
int
msgarg_escape(char* buf, int bufsize, const char* in)
{
	char *out = buf;
	int len = 0;

	if (bufsize < 3) {
		return 0;
	}
	*out++ = '"';
	len++;
	while (*in && len < bufsize - 3) {
		if (*in == '"' || *in == '\\') {
			*out++ = '\\';
			len++;
		}
		*out++ = *in++;
		len++;
	}
	*out++ = '"';
	*out = '\0';
	len++;

	return len;
}

/*****************************************************************/
/***                       ***************************************/
/*** MCP CONNECTION FRAMES ***************************************/
/***                       ***************************************/
/*****************************************************************/

struct McpFrameList_t {
	McpFrame* mfr;
	struct McpFrameList_t* next;
};
typedef struct McpFrameList_t McpFrameList;
McpFrameList* mcp_frame_list;


/*****************************************************************
 *
 * void mcp_frame_init(McpFrame* mfr, connection_t con);
 *
 *   Initializes an McpFrame for a new connection.
 *   You MUST call this to initialize a new McpFrame.
 *   The caller is responsible for the allocation of the McpFrame.
 *
 *****************************************************************/

void
mcp_frame_init(McpFrame * mfr, connection_t con)
{
	McpFrameList* mfrl;

	mfr->descriptor = con;
	mfr->version.verminor = 0;
	mfr->version.vermajor = 0;
	mfr->authkey = NULL;
	mfr->packages = NULL;
	mfr->messages = NULL;
	mfr->enabled = 0;

	mfrl = (McpFrameList*)malloc(sizeof(McpFrameList));
	mfrl->mfr = mfr;
	mfrl->next = mcp_frame_list;
	mcp_frame_list = mfrl;
}

/*****************************************************************
 *
 * McpVer mcp_frame_package_supported(
 *              McpFrame* mfr,
 *              char* package
 *          );
 *
 *   Returns the supported version of the given package.
 *   Returns {0,0} if the package is not supported.
 *
 *****************************************************************/

McpVer
mcp_frame_package_supported(McpFrame * mfr, const char *package)
{
	McpPkg *ptr;
	McpVer errver = { 0, 0 };

	if (!mfr->enabled) {
		return errver;
	}

	for (ptr = mfr->packages; ptr; ptr = ptr->next) {
		if (!strcmp_nocase(ptr->pkgname, package)) {
			break;
		}
	}
	if (!ptr) {
		return errver;
	}

	return (ptr->minver);
}

/*****************************************************************
 *
 * int mcp_frame_output_mesg(
 *             McpFrame* mfr,
 *             McpMesg* msg
 *         );
 *
 *   Sends an MCP message to the given connection.
 *   Returns EMCP_SUCCESS if successful.
 *   Returns EMCP_NOMCP if MCP isn't supported on this connection.
 *   Returns EMCP_NOPACKAGE if this connection doesn't support the needed package.
 *
 *****************************************************************/

int
mcp_frame_output_mesg(McpFrame * mfr, McpMesg * msg)
{
	char outbuf[BUFFER_LEN * 2];
	int bufrem = sizeof(outbuf);
	char mesgname[128];
	char datatag[32];
	McpArg *anarg = NULL;
	int mlineflag = 0;
	char *p;
	char *out;

	/* if (!mfr->enabled && strcmp_nocase(msg->package, MCP_INIT_PKG)) { */
	/* 	return EMCP_NOMCP; */
	/* } */

	/* Create the message name from the package and message subnames */
	if (msg->mesgname && *msg->mesgname) {
		snprintf(mesgname, sizeof(mesgname), "%s-%s", msg->package, msg->mesgname);
	} else {
		snprintf(mesgname, sizeof(mesgname), "%s", msg->package);
	}

	strlcpy(outbuf, MCP_MESG_PREFIX, sizeof(outbuf));
	strlcat(outbuf, mesgname, sizeof(outbuf));
	/* if (strcmp_nocase(mesgname, MCP_INIT_PKG)) { */
	/* 	McpVer nullver = { 0, 0 }; */

	/* 	strlcat(outbuf, " ", sizeof(outbuf)); */
	/* 	strlcat(outbuf, mfr->authkey, sizeof(outbuf)); */
	/* 	if (strcmp_nocase(msg->package, MCP_NEGOTIATE_PKG)) { */
	/* 		McpVer ver = mcp_frame_package_supported(mfr, msg->package); */

	/* 		if (!mcp_version_compare(ver, nullver)) { */
	/* 			return EMCP_NOPACKAGE; */
	/* 		} */
	/* 	} */
	/* } */

	/* If the argument lines contain newlines, split them into separate lines. */
	for (anarg = msg->args; anarg; anarg = anarg->next) {
		if (anarg->value) {
			McpArgPart *ap = anarg->value;

			while (ap) {
				p = ap->value;
				while (*p) {
					if (*p == '\n' || *p == '\r') {
						McpArgPart *nu = (McpArgPart *) malloc(sizeof(McpArgPart));

						nu->next = ap->next;
						ap->next = nu;
						*p++ = '\0';
						nu->value = strdup(p);
						ap->value = (char *) realloc(ap->value, strlen(ap->value) + 1);
						ap = nu;
						p = nu->value;
					} else {
						p++;
					}
				}
				ap = ap->next;
			}
		}
	}

	/* Build the initial message string */
	out = outbuf;
	bufrem = outbuf + sizeof(outbuf) - out;
	for (anarg = msg->args; anarg; anarg = anarg->next) {
		out += strlen(out);
		bufrem = outbuf + sizeof(outbuf) - out;
		if (!anarg->value) {
			anarg->was_shown = 1;
			snprintf(out, bufrem, " %s: %s", anarg->name, MCP_ARG_EMPTY);
			out += strlen(out);
			bufrem = outbuf + sizeof(outbuf) - out;
		} else {
			int totlen = strlen(anarg->value->value) + strlen(anarg->name) + 5;

			if (anarg->value->next || totlen > ((BUFFER_LEN - (out - outbuf)) / 2)) {
				/* Value is multi-line or too long.  Send on separate line(s). */
				mlineflag = 1;
				anarg->was_shown = 0;
				snprintf(out, bufrem, " %s*: %s", anarg->name, MCP_ARG_EMPTY);
			} else {
				anarg->was_shown = 1;
				snprintf(out, bufrem, " %s: ", anarg->name);
				out += strlen(out);
				bufrem = outbuf + sizeof(outbuf) - out;

				msgarg_escape(out, bufrem, anarg->value->value);
				out += strlen(out);
				bufrem = outbuf + sizeof(outbuf) - out;
			}
			out += strlen(out);
			bufrem = outbuf + sizeof(outbuf) - out;
		}
	}

	/* If the message is multi-line, make sure it has a _data-tag field. */
	if (mlineflag) {
		snprintf(datatag, sizeof(datatag), "%.8lX", (unsigned long)(RANDOM() ^ RANDOM()));
		snprintf(out, bufrem, " %s: %s", MCP_DATATAG, datatag);
		out += strlen(out);
		bufrem = outbuf + sizeof(outbuf) - out;
	}

	/* Send the initial line. */
	SendText(mfr, outbuf);
	SendText(mfr, "\r\n");

	if (mlineflag) {
		/* Start sending arguments whose values weren't already sent. */
		/* This is usually just multi-line argument values. */
		for (anarg = msg->args; anarg; anarg = anarg->next) {
			if (!anarg->was_shown) {
				McpArgPart *ap = anarg->value;

				while (ap) {
					*outbuf = '\0';
					snprintf(outbuf, sizeof(outbuf), "%s* %s %s: %s", MCP_MESG_PREFIX, datatag, anarg->name, ap->value);
					SendText(mfr, outbuf);
					SendText(mfr, "\r\n");
					ap = ap->next;
				}
			}
		}

		/* Let the other side know we're done sending multi-line arg vals. */
		snprintf(outbuf, sizeof(outbuf), "%s: %s", MCP_MESG_PREFIX, datatag);
		SendText(mfr, outbuf);
		SendText(mfr, "\r\n");
	}

	return EMCP_SUCCESS;
}

/*****************************************************************/
/***                   *******************************************/
/***  MESSAGE METHODS  *******************************************/
/***                   *******************************************/
/*****************************************************************/

/*****************************************************************
 *
 * void mcp_mesg_init(
 *         McpMesg*    msg,
 *         const char* package,
 *         const char* mesgname
 *     );
 *
 *   Initializes an MCP message.
 *   You MUST initialize a message before using it.
 *   You MUST also mcp_mesg_clear() a mesg once you are done using it.
 *
 *****************************************************************/

void
mcp_mesg_init(McpMesg * msg, const char *package, const char *mesgname)
{
	msg->package = strdup(package);
	msg->mesgname = strdup(mesgname);
	msg->datatag = NULL;
	msg->args = NULL;
	msg->incomplete = 0;
	msg->bytes = 0;
	msg->next = NULL;
}

/*****************************************************************
 *
 * void mcp_mesg_clear(
 *              McpMesg* msg
 *          );
 *
 *   Clear the given MCP message.
 *   You MUST clear every message after you are done using it, to
 *     free the memory used by the message.
 *
 *****************************************************************/

void
mcp_mesg_clear(McpMesg * msg)
{
	if (msg->package)
		free(msg->package);
	if (msg->mesgname)
		free(msg->mesgname);
	if (msg->datatag)
		free(msg->datatag);

	while (msg->args) {
		McpArg *tmp = msg->args;

		msg->args = tmp->next;
		if (tmp->name)
			free(tmp->name);
		while (tmp->value) {
			McpArgPart *ptr2 = tmp->value;

			tmp->value = tmp->value->next;
			if (ptr2->value)
				free(ptr2->value);
			free(ptr2);
		}
		free(tmp);
	}
	msg->bytes = 0;
}

/*****************************************************************/
/***                  ********************************************/
/*** ARGUMENT METHODS ********************************************/
/***                  ********************************************/
/*****************************************************************/

/*****************************************************************
 *
 * char* mcp_mesg_arg_getline(
 *         McpMesg* msg,
 *         const char* argname
 *         int linenum;
 *     );
 *
 *   Gets the value of a named argument in the given message.
 *
 *****************************************************************/

char *
mcp_mesg_arg_getline(McpMesg * msg, const char *argname, int linenum)
{
	McpArg *ptr = msg->args;

	while (ptr && strcmp_nocase(ptr->name, argname)) {
		ptr = ptr->next;
	}
	if (ptr) {
		McpArgPart *ptr2 = ptr->value;

		while (linenum > 0 && ptr2) {
			ptr2 = ptr2->next;
			linenum--;
		}
		if (ptr2) {
			return (ptr2->value);
		}
		return NULL;
	}
	return NULL;
}




/*****************************************************************
 *
 * int mcp_mesg_arg_append(
 *         McpMesg* msg,
 *         const char* argname,
 *         const char* argval
 *     );
 *
 *   Appends to the list value of the named arg in the given mesg.
 *   If that named argument doesn't exist yet, it will be created.
 *   This is used to construct arguments that have lists as values.
 *   Returns the success state of the call.  EMCP_SUCCESS if the
 *   call was successful.  EMCP_ARGCOUNT if this would make too
 *   many arguments in the message.  EMCP_ARGLENGTH is this would
 *   cause an argument to exceed the max allowed number of lines.
 *
 *****************************************************************/

int
mcp_mesg_arg_append(McpMesg * msg, const char *argname, const char *argval)
{
	McpArg *ptr = msg->args;
	int namelen = strlen(argname);
	int vallen = argval? strlen(argval) : 0;

	if (namelen > MAX_MCP_ARGNAME_LEN) {
		return EMCP_ARGNAMELEN;
	}
	if (vallen + msg->bytes > MAX_MCP_MESG_SIZE) {
		return EMCP_MESGSIZE;
	}
	while (ptr && strcmp_nocase(ptr->name, argname)) {
		ptr = ptr->next;
	}
	if (!ptr) {
		if (namelen + vallen + msg->bytes > MAX_MCP_MESG_SIZE) {
			return EMCP_MESGSIZE;
		}
		ptr = (McpArg *) malloc(sizeof(McpArg));
		ptr->name = (char *) malloc(namelen + 1);
		strlcpy(ptr->name, argname, namelen+1);
		ptr->value = NULL;
		ptr->last = NULL;
		ptr->next = NULL;
		if (!msg->args) {
			msg->args = ptr;
		} else {
			int limit = MAX_MCP_MESG_ARGS;
			McpArg *lastarg = msg->args;

			while (lastarg->next) {
				if (limit-- <= 0) {
					free(ptr->name);
					free(ptr);
					return EMCP_ARGCOUNT;
				}
				lastarg = lastarg->next;
			}
			lastarg->next = ptr;
		}
		msg->bytes += sizeof(McpArg) + namelen + 1;
	}

	if (argval) {
		McpArgPart *nu = (McpArgPart *) malloc(sizeof(McpArgPart));

		nu->value = (char *) malloc(vallen + 1);
		strlcpy(nu->value, argval, vallen+1);
		nu->next = NULL;

		if (!ptr->last) {
			ptr->value = ptr->last = nu;
		} else {
			ptr->last->next = nu;
			ptr->last = nu;
		}
		msg->bytes += sizeof(McpArgPart) + vallen + 1;
	}
	ptr->was_shown = 0;
	return EMCP_SUCCESS;
}

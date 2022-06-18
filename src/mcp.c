/* mcp.c: MUD Client Protocol.
   Part of the FuzzBall distribution. */

#include "mcp.h"
#include "entity.h"
#include "config.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mdb.h"
#include "externs.h"

#define MCP_WEB_PKG "web"
#define MCP_MESG_PREFIX		"#$#"
#define MCP_ARG_EMPTY		"\"\""
#define MCP_INIT_PKG		"mcp"
#define MCP_DATATAG			"_data-tag"
#define MCP_INIT_MESG		"mcp "
#define MCP_NEGOTIATE_PKG	"mcp-negotiate"

#define EMCP_SUCCESS		 0	/* successful result */
#define EMCP_ARGCOUNT		-3	/* Too many arguments in mesg. */
#define EMCP_ARGNAMELEN		-5	/* Arg name is too long. */
#define EMCP_MESGSIZE		-6	/* Message is too large. */

#define MAX_MCP_ARGNAME_LEN    30 /* max length of argument name. */
#define MAX_MCP_MESG_ARGS      30 /* max number of args per mesg. */
#define MAX_MCP_MESG_SIZE  262144 /* max mesg size in bytes. */

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

static int
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

	/* Create the message name from the package and message subnames */
	if (msg->mesgname && *msg->mesgname) {
		snprintf(mesgname, sizeof(mesgname), "%s-%s", msg->package, msg->mesgname);
	} else {
		snprintf(mesgname, sizeof(mesgname), "%s", msg->package);
	}

	strlcpy(outbuf, MCP_MESG_PREFIX, sizeof(outbuf));
	strlcat(outbuf, mesgname, sizeof(outbuf));

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
		snprintf(datatag, sizeof(datatag), "%.8lX", (unsigned long)(random() ^ random()));
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

static void
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

static int
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

static inline McpFrame *
mcp_frame(int descr)
{
	McpFrame *mfr = descr_mcpframe(descr);

	if (!mfr || !mfr->enabled)
                return NULL;

        return mfr;
}

int
mcp_view(ENT *eplayer, char *buf)
{
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
	if (!mfr)
                return 1;
        mcp_mesg_init(&msg, MCP_WEB_PKG, "view");
        mcp_mesg_arg_append(&msg, "data", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
mcp_art(int descr, char const *art)
{
	McpMesg msg;
	McpFrame *mfr = mcp_frame(descr);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "art");
        mcp_mesg_arg_append(&msg, "src", art);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
mcp_look(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
        char buf2[BUFSIZ];
	char const *description = "";
	OBJ *thing;
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "look");
        snprintf(buf2, sizeof(buf2), "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "dbref", buf2);

	OBJ *last_observed = eplayer->last_observed;
	if (last_observed)
		observer_remove(last_observed, player);

        if (loc->type == TYPE_ROOM) {
		ROO *rloc = &loc->sp.room;
                mcp_mesg_arg_append(&msg, "room", "1");
                snprintf(buf, sizeof(buf), "%u", rloc->exits);
                mcp_mesg_arg_append(&msg, "exits", buf);
        } else {
		if (loc->type == TYPE_ENTITY) {
			ENT *eloc = &loc->sp.entity;
			if (eloc->flags & EF_SHOP)
				mcp_mesg_arg_append(&msg, "shop", "1");
		}

		eplayer->last_observed = loc;
		observer_add(loc, player);
	}

        mcp_mesg_arg_append(&msg, "art", loc->art);
        mcp_mesg_arg_append(&msg, "name", loc->name);
        mcp_mesg_arg_append(&msg, "pname", unparse(player, loc));
	if (loc->description)
		description = loc->description;
        mcp_mesg_arg_append(&msg, "description", description);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);

        if (loc != player && loc->type == TYPE_ENTITY && !(eplayer->flags & EF_WIZARD))
                return 0;

	// use callbacks for mcp like this versus telnet
        FOR_LIST(thing, loc->contents) {
		if (thing == player)
			continue;

		struct icon ico = icon(thing);
		mcp_mesg_init(&msg, MCP_WEB_PKG, "look-content");
		mcp_mesg_arg_append(&msg, "loc", buf2);
		snprintf(buf, sizeof(buf), "%d", object_ref(thing));
		mcp_mesg_arg_append(&msg, "dbref", buf);
		mcp_mesg_arg_append(&msg, "name", thing->name);
		mcp_mesg_arg_append(&msg, "pname", unparse(player, thing));
		mcp_mesg_arg_append(&msg, "icon", ico.icon);
		snprintf(buf, sizeof(buf), "%d", thing->value);
		mcp_mesg_arg_append(&msg, "price", buf);
		snprintf(buf, sizeof(buf), "%d", ico.actions);
		mcp_mesg_arg_append(&msg, "avatar", thing->avatar);
		mcp_mesg_arg_append(&msg, "actions", buf);
		mcp_frame_output_mesg(mfr, &msg);
		mcp_mesg_clear(&msg);
        }

        return 0;
}

static void
mcp_room(OBJ *room, void *msg)
{
	OBJ *tmp;

	for (tmp = room->contents;
	     tmp;
	     tmp = tmp->next)
	{
		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;
		McpFrame *mfr = mcp_frame(etmp->fd);

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

static void
mcp_observers(OBJ *thing, void *msg)
{
	struct observer_node *node;

	for (node = thing->first_observer; node; node = node->next)
	{
		OBJ *tmp = node->who;

		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;

		McpFrame *mfr = mcp_frame(etmp->fd);

		if (!mfr)
			continue;

		mcp_frame_output_mesg(mfr, msg);
	}
}

void do_meme(command_t *cmd) {
        OBJ *player = cmd->player;
        const char *url = cmd->argv[1];
	McpMesg msg;
        // FIXME should also work in the terminal
        mcp_mesg_init(&msg, MCP_WEB_PKG, "meme");
        mcp_mesg_arg_append(&msg, "who", player->name);
        mcp_mesg_arg_append(&msg, "url", url);
        mcp_room(player->location, &msg);
        mcp_mesg_clear(&msg);
}

void
mcp_content_out(OBJ *loc, OBJ *thing) {
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "out");
	snprintf(buf, BUFSIZ, "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", object_ref(thing));
        mcp_mesg_arg_append(&msg, "dbref", buf);

	if (loc->type == TYPE_ROOM)
		mcp_room(loc, &msg);
	else
		mcp_observers(loc, &msg);

        mcp_mesg_clear(&msg);
}

void
mcp_content_in(OBJ *loc, OBJ *thing) {
	struct icon ico = icon(thing);
	char buf[BUFSIZ];
	McpMesg msg;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "in");
	snprintf(buf, BUFSIZ, "%d", object_ref(loc));
        mcp_mesg_arg_append(&msg, "loc", buf);
	snprintf(buf, BUFSIZ, "%d", object_ref(thing));
        mcp_mesg_arg_append(&msg, "dbref", buf);
	mcp_mesg_arg_append(&msg, "name", thing->name);
	mcp_mesg_arg_append(&msg, "pname", unparse(object_get(HUMAN_BEING), thing));
	mcp_mesg_arg_append(&msg, "icon", ico.icon);
	snprintf(buf, sizeof(buf), "%d", ico.actions);
        mcp_mesg_arg_append(&msg, "avatar", thing->avatar);
	mcp_mesg_arg_append(&msg, "actions", buf);

	if (loc->type == TYPE_ROOM)
		mcp_room(loc, &msg);
	else
		mcp_observers(loc, &msg);

        mcp_mesg_clear(&msg);
}

int
mcp_auth_fail(int descr, int reason) {
	McpMesg msg;
	McpFrame *mfr = mcp_frame(descr);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "auth-fail");
	snprintf(buf, sizeof(buf), "%d", reason);
        mcp_mesg_arg_append(&msg, "reason", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
mcp_auth_success(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "auth-success");
	snprintf(buf, sizeof(buf), "%d", object_ref(player));
        mcp_mesg_arg_append(&msg, "player", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
mcp_stats(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "stats");
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_STR]);
        mcp_mesg_arg_append(&msg, "str", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_CON]);
        mcp_mesg_arg_append(&msg, "con", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_DEX]);
        mcp_mesg_arg_append(&msg, "dex", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_INT]);
        mcp_mesg_arg_append(&msg, "int", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->attr[ATTR_WIZ]);
        mcp_mesg_arg_append(&msg, "wiz", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DODGE).value);
        mcp_mesg_arg_append(&msg, "dodge", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DMG).value);
        mcp_mesg_arg_append(&msg, "dmg", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, MDMG).value);
        mcp_mesg_arg_append(&msg, "mdmg", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, DEF).value);
        mcp_mesg_arg_append(&msg, "def", buf);
	snprintf(buf, sizeof(buf), "%d", EFFECT(eplayer, MDEF).value);
        mcp_mesg_arg_append(&msg, "mdef", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

int
mcp_bars(ENT *eplayer) {
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
	char buf[BUFSIZ];
	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "bars");
	snprintf(buf, sizeof(buf), "%d", eplayer->hp);
        mcp_mesg_arg_append(&msg, "hp", buf);
	snprintf(buf, sizeof(buf), "%d", HP_MAX(eplayer));
        mcp_mesg_arg_append(&msg, "hpMax", buf);
	snprintf(buf, sizeof(buf), "%d", eplayer->mp);
        mcp_mesg_arg_append(&msg, "mp", buf);
	snprintf(buf, sizeof(buf), "%d", MP_MAX(eplayer));
        mcp_mesg_arg_append(&msg, "mpMax", buf);
        mcp_frame_output_mesg(mfr, &msg);
        mcp_mesg_clear(&msg);
        return 0;
}

static inline void
_mcp_equipment(OBJ *player, enum equipment_slot eql)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);
        dbref eq = EQUIP(eplayer, eql);
        if (eq == NOTHING || !eq)
                return;
	OBJ *oeq = object_get(eq);
        CBUG(!mfr);
        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment-item");
	snprintf(buf, sizeof(buf), "%d", eq);
        mcp_mesg_arg_append(&msg, "dbref", buf);
	snprintf(buf, sizeof(buf), "%d", eql);
        mcp_mesg_arg_append(&msg, "eql", buf);
        mcp_mesg_arg_append(&msg, "pname", unparse(player, oeq));
        mcp_mesg_arg_append(&msg, "avatar", oeq->avatar);
        struct icon ico = icon(oeq);
        mcp_mesg_arg_append(&msg, "icon", ico.icon);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);
}

int
mcp_equipment(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
	McpMesg msg;
	McpFrame *mfr = mcp_frame(eplayer->fd);

	if (!mfr)
                return 1;

        mcp_mesg_init(&msg, MCP_WEB_PKG, "equipment");
	snprintf(buf, sizeof(buf), "%d", 1);
        mcp_mesg_arg_append(&msg, "ignore", buf);
	mcp_frame_output_mesg(mfr, &msg);
	mcp_mesg_clear(&msg);

        _mcp_equipment(player, ES_HEAD);
        _mcp_equipment(player, ES_NECK);
        _mcp_equipment(player, ES_CHEST);
        _mcp_equipment(player, ES_BACK);
        _mcp_equipment(player, ES_RHAND);
        _mcp_equipment(player, ES_LFINGER);
        _mcp_equipment(player, ES_RFINGER);
        _mcp_equipment(player, ES_PANTS);

	return 0;
}

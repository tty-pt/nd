#include "mcp.h"
#include "io.h"
#include <ndc.h>
#include "entity.h"
#include "config.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <qhash.h>
#include "debug.h"
#include "io.h"

#ifndef __OpenBSD__
#include <bsd/string.h>
#endif

enum bcp_type {
	BCP_ATTR = 0,
	BCP_BARS = 1,
	BCP_EQUIPMENT = 2,
	BCP_STATS = 3,
	BCP_ITEM = 4,
	BCP_VIEW = 5,
	BCP_VIEW_BUFFER = 6,
	BCP_ROOM = 7,
	BCP_ENTITY = 8,
	BCP_AUTH_FAILURE = 9,
	BCP_AUTH_SUCCESS = 10,
	BCP_OUT = 11,
};

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

static void
nd_tdwritef(OBJ *player, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	if (!player->sp.entity.fds)
		return;
	struct hash_cursor c = hash_iter_start(player->sp.entity.fds);
	while (hash_iter_get(&c)) {
		int fd = * (int *) c.key;
		if (ndc_flags(fd) & DF_WEBSOCKET)
			continue;
		ndc_write(fd, buf, len);
	}
}

static void
nd_twritef(OBJ *player, const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_tdwritef(player, fmt, va);
	va_end(va);
}

static void
nd_wwrite(OBJ *player, void *msg, size_t len) {
	ENT *eplayer = &player->sp.entity;

	if (!eplayer->fds)
		return;

	struct hash_cursor c = hash_iter_start(eplayer->fds);

	while (hash_iter_get(&c)) {
		register int fd = * (int *) c.key;
		if ((ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, msg, len);
	}
}

static void
fbcp(OBJ *player, size_t len, unsigned char iden, void *data)
{
	char bcp_buf[2 + sizeof(iden) + len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + 2, &iden, sizeof(iden));
	memcpy(bcp_buf + 2 + sizeof(iden), data, len);
	nd_wwrite(player, bcp_buf, 1 + sizeof(iden) + len);
}

static int
_fbcp_item(char *bcp_buf, OBJ *obj, unsigned char dynflags)
{
	int aux, aux1;
	struct icon ico = object_icon(obj);
	unsigned char iden = BCP_ITEM;
	char *p = bcp_buf, *aux2;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = object_ref(obj);
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	aux = object_ref(obj->location);
	memcpy(p += sizeof(aux), &aux, sizeof(aux));
	memcpy(p += sizeof(aux), &dynflags, sizeof(dynflags));
	memcpy(p += sizeof(dynflags), &obj->type, sizeof(obj->type));
	memcpy(p += sizeof(obj->type), &ico.actions, sizeof(ico.actions));
	memcpy(p += sizeof(ico.actions), obj->name, aux = strlen(obj->name) + 1);
	aux2 = (char *) unparse(object_get(HUMAN_BEING), obj);
	memcpy(p += aux, aux2, aux1 = strlen(aux2) + 1);
	memcpy(p += aux1, ico.icon, aux = strlen(ico.icon) + 1);
	aux2 = object_art(obj);
	memcpy(p += aux, aux2, aux1 = strlen(aux2) + 1);
	switch (obj->type) {
	case TYPE_ROOM: {
		ROO *roo = &obj->sp.room;
		memcpy(p += aux1, &roo->exits, sizeof(roo->exits));
		p += sizeof(roo->exits);
		break;
	}
	case TYPE_ENTITY: {
		ENT *ent = &obj->sp.entity;
		memcpy(p += aux1, &ent->flags, sizeof(ent->flags));
		p += sizeof(ent->flags);
		break;
	}
	default: {
		p += aux1;
	}}
	return p - bcp_buf;
}

static void
fbcp_item(OBJ *player, OBJ *obj, unsigned char dynflags)
{
	static char bcp_buf[SUPERBIGSIZ];
	int ret = _fbcp_item(bcp_buf, obj, dynflags);
	nd_wwrite(player, bcp_buf, ret);
}

static int // 2 + sizeof(iden) + sizeof(int) * 2
_fbcp_out(char *bcp_buf, OBJ *obj)
{
	int aux;
	unsigned char iden = BCP_OUT;
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = object_ref(obj);
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	aux = object_ref(obj->location);
	memcpy(p += sizeof(aux), &aux, sizeof(aux));
	p += sizeof(aux);
	return p - bcp_buf;
}

static void
fbcp_auth_success(OBJ *player, dbref who)
{
	unsigned char iden = BCP_AUTH_SUCCESS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(who)];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &who, sizeof(who));
	p += sizeof(who);
	nd_wwrite(player, bcp_buf, p - bcp_buf);
}

static void
fbcp_auth_failure(int fd, int reason)
{
	unsigned char iden = BCP_AUTH_FAILURE;
	static char bcp_buf[2 + sizeof(iden) + sizeof(reason)];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &reason, sizeof(reason));
	p += sizeof(reason);
	ndc_write(fd, bcp_buf, p - bcp_buf);
}

static void
fbcp_stats(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	unsigned char iden = BCP_STATS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(eplayer->attr) + sizeof(short) * 7];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), eplayer->attr, sizeof(eplayer->attr));
	memcpy(p += sizeof(eplayer->attr), &eplayer->e[AF_HP].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_MOV].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_MDMG].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_MDEF].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_DODGE].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_DMG].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer->e[AF_DEF].value, sizeof(short));
	p += sizeof(short);
	nd_write(player, bcp_buf, p - bcp_buf);
}

static void
fbcp_bars(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	unsigned char iden = BCP_BARS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(int) * 4];
	char *p = bcp_buf;
	unsigned short aux;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &eplayer->hp, sizeof(eplayer->hp));
	aux = HP_MAX(eplayer);
	memcpy(p += sizeof(eplayer->hp), &aux, sizeof(aux));
	memcpy(p += sizeof(aux), &eplayer->mp, sizeof(eplayer->mp));
	aux = MP_MAX(eplayer);
	memcpy(p += sizeof(eplayer->mp), &aux, sizeof(aux));
	p += sizeof(aux);
	nd_write(player, bcp_buf, p - bcp_buf);
}

void
fbcp_view(OBJ *player, view_t *view)
{
	fbcp(player, sizeof(view_t), BCP_VIEW, view);
}

void
fbcp_view_buf(OBJ *player, char *view_buf)
{
	fbcp(player, strlen(view_buf), BCP_VIEW_BUFFER, view_buf);
	nd_twritef(player, view_buf);
}

void
mcp_look(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *thing;

	fbcp_item(player, loc, 1);
	switch (loc->type) {
	case TYPE_ROOM: break;
	case TYPE_ENTITY: // falls through
	default:
		eplayer->last_observed = loc;
		observer_add(loc, player);
	}

        if (loc != player && loc->type == TYPE_ENTITY && !(eplayer->flags & EF_WIZARD))
                return;

	// use callbacks for mcp like this versus telnet
        FOR_LIST(thing, loc->contents)
		fbcp_item(player, thing, 0);

	nd_twritef(player, "%s\n", unparse(player, loc));
	if (loc->description && *loc->description)
		nd_twritef(player, loc->description);

        char buf[BUFSIZ];
        size_t buf_l = 0;

	/* check to see if there is anything there */
	if (loc->contents) {
                FOR_LIST(thing, loc->contents) {
			if (thing == player)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"%s\r\n", unparse(player, thing));
                }
	}

        buf[buf_l] = '\0';
        nd_twritef(player, "Contents: %s", buf);
}

static void
fbcp_room(OBJ *room, char *msg, size_t len)
{
	OBJ *tmp;

	for (tmp = room->contents; tmp; tmp = tmp->next) {
		if (tmp->type != TYPE_ENTITY)
			continue;

		nd_wwrite(tmp, msg, len);
	}
}

static void
fbcp_observers(OBJ *thing, char *msg, size_t len)
{
	struct observer_node *node;

	for (node = thing->first_observer; node; node = node->next) {
		OBJ *tmp = node->who;

		if (tmp->type != TYPE_ENTITY)
			continue;

		nd_wwrite(tmp, msg, len);
	}
}

void
mcp_content_out(OBJ *loc, OBJ *thing) {
	char bcp_buf[2 + sizeof(unsigned char) + sizeof(int) * 2];
	size_t len = _fbcp_out(bcp_buf, thing);

	if (loc->type == TYPE_ROOM)
		fbcp_room(loc, bcp_buf, len);
	else
		fbcp_observers(loc, bcp_buf, len);
}

void
mcp_content_in(OBJ *loc, OBJ *thing) {
	static char bcp_buf[SUPERBIGSIZ];
	size_t len = _fbcp_item(bcp_buf, thing, 2);

	if (loc->type == TYPE_ROOM)
		fbcp_room(loc, bcp_buf, len);
	else
		fbcp_observers(loc, bcp_buf, len);
}

void
mcp_auth_fail(int descr, int reason) {
	fbcp_auth_failure(descr, reason);
}

void
mcp_auth_success(OBJ *player) {
	fbcp_auth_success(player, object_ref(player));
}

void
mcp_stats(OBJ *player) {
	fbcp_stats(player);
}

void
mcp_bars(OBJ *player) {
	fbcp_bars(player);
}

void
mcp_equipment(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	int aux;

	if (!eplayer->fds)
		return;

	fbcp(player, sizeof(eplayer->equipment), BCP_EQUIPMENT, eplayer->equipment);
	
	aux = eplayer->equipment[ES_HEAD];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_NECK];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_CHEST];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_BACK];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_RHAND];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_LFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_RFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
	aux = eplayer->equipment[ES_PANTS];
	if (aux && aux != NOTHING)
		fbcp_item(player, object_get(aux), 0);
}

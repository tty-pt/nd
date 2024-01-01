#include "mcp.h"
#include "io.h"
#include <ndc.h>
#include "entity.h"
#include "config.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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

static int
fbcp(int fd, size_t len, unsigned char iden, void *data)
{
	char bcp_buf[2 + sizeof(iden) + len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + 2, &iden, sizeof(iden));
	memcpy(bcp_buf + 2 + sizeof(iden), data, len);
	return ndc_write(fd, bcp_buf, 1 + sizeof(iden) + len);
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
		warn("BCP_ROOM %u\n", roo->exits);
		p += sizeof(roo->exits);
		break;
	}
	case TYPE_ENTITY: {
		ENT *ent = &obj->sp.entity;
		warn("BCP_ENTITY %u %lu\n", ent->flags, sizeof(ent->flags));
		memcpy(p += aux1, &ent->flags, sizeof(ent->flags));
		p += sizeof(ent->flags);
		break;
	}
	default: {
		p += aux1;
	}}
	return p - bcp_buf;
}

static int
fbcp_item(int fd, OBJ *obj, unsigned char dynflags)
{
	static char bcp_buf[SUPERBIGSIZ];
	return ndc_write(fd, bcp_buf, _fbcp_item(bcp_buf, obj, dynflags));
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

static int
fbcp_auth_success(int fd, dbref who)
{
	unsigned char iden = BCP_AUTH_SUCCESS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(who)];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &who, sizeof(who));
	p += sizeof(who);
	return ndc_write(fd, bcp_buf, p - bcp_buf);
}

static int
fbcp_auth_failure(int fd, int reason)
{
	unsigned char iden = BCP_AUTH_FAILURE;
	static char bcp_buf[2 + sizeof(iden) + sizeof(reason)];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &reason, sizeof(reason));
	p += sizeof(reason);
	return ndc_write(fd, bcp_buf, p - bcp_buf);
}

static int
fbcp_stats(ENT *eplayer)
{
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
	return ndc_write(eplayer->fd, bcp_buf, p - bcp_buf);
}

static int
fbcp_bars(ENT *eplayer)
{
	unsigned char iden = BCP_BARS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(int) * 4];
	char *p = bcp_buf;
	int aux;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &eplayer->hp, sizeof(eplayer->hp));
	aux = HP_MAX(eplayer);
	memcpy(p += sizeof(eplayer->hp), &aux, sizeof(aux));
	memcpy(p += sizeof(aux), &eplayer->mp, sizeof(eplayer->mp));
	aux = HP_MAX(eplayer);
	memcpy(p += sizeof(eplayer->mp), &aux, sizeof(aux));
	p += sizeof(aux);
	return ndc_write(eplayer->fd, bcp_buf, p - bcp_buf);
}

int
fbcp_view(ENT *eplayer, view_t *view)
{
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp(eplayer->fd, sizeof(view_t), BCP_VIEW, view);
        return 0;
}

int
fbcp_view_buf(ENT *eplayer, char *view_buf)
{
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp(eplayer->fd, strlen(view_buf), BCP_VIEW_BUFFER, view_buf);
        return 0;
}

int
mcp_look(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *thing;

	int fd = eplayer->fd;
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp_item(fd, loc, 1);
	switch (loc->type) {
	case TYPE_ROOM: break;
	case TYPE_ENTITY: // falls through
	default:
		eplayer->last_observed = loc;
		observer_add(loc, player);
	}

        if (loc != player && loc->type == TYPE_ENTITY && !(eplayer->flags & EF_WIZARD))
                return 0;

	// use callbacks for mcp like this versus telnet
        FOR_LIST(thing, loc->contents)
		fbcp_item(fd, thing, 0);

        return 0;
}

static void
fbcp_room(OBJ *room, char *msg, size_t len)
{
	OBJ *tmp;

	for (tmp = room->contents; tmp; tmp = tmp->next) {
		if (tmp->type != TYPE_ENTITY)
			continue;

		int fd = tmp->sp.entity.fd;

		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			continue;

		ndc_write(fd, msg, len);
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

		int fd = tmp->sp.entity.fd;

		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			continue;

		ndc_write(fd, msg, len);
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

int
mcp_auth_fail(int descr, int reason) {
	if (!(ndc_flags(descr) & DF_WEBSOCKET))
                return 1;
	fbcp_auth_failure(descr, reason);
        return 0;
}

int
mcp_auth_success(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp_auth_success(eplayer->fd, object_ref(player));
        return 0;
}

int
mcp_stats(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp_stats(eplayer);
        return 0;
}

int
mcp_bars(ENT *eplayer) {
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp_bars(eplayer);
        return 0;
}

int
mcp_equipment(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	int fd = eplayer->fd;
	int aux;
	if (!(ndc_flags(eplayer->fd) & DF_WEBSOCKET))
                return 1;
	fbcp(fd, sizeof(eplayer->equipment), BCP_EQUIPMENT, eplayer->equipment);
	
	aux = eplayer->equipment[ES_HEAD];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_NECK];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_CHEST];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_BACK];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_RHAND];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_LFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_RFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);
	aux = eplayer->equipment[ES_PANTS];
	if (aux && aux != NOTHING)
		fbcp_item(fd, object_get(aux), 0);

	return 0;
}

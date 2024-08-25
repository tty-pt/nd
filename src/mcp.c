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
nd_tdwritef(dbref player_ref, const char *fmt, va_list args) {
	static char buf[BUFSIZ];
	ssize_t len = vsnprintf(buf, sizeof(buf), fmt, args);
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c)))
		if (!(ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, buf, len);
}

static void
nd_twritef(dbref player_ref, const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_tdwritef(player_ref, fmt, va);
	va_end(va);
}

static void
nd_wwrite(dbref player_ref, void *msg, size_t len) {
	struct hash_cursor c = fds_iter(player_ref);
	int fd;

	while ((fd = fds_next(&c))) {
		if ((ndc_flags(fd) & DF_WEBSOCKET))
			ndc_write(fd, msg, len);
	}
}

static void
fbcp(dbref player_ref, size_t len, unsigned char iden, void *data)
{
	char bcp_buf[2 + sizeof(iden) + len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + 2, &iden, sizeof(iden));
	memcpy(bcp_buf + 2 + sizeof(iden), data, len);
	nd_wwrite(player_ref, bcp_buf, 1 + sizeof(iden) + len);
}

static int
_fbcp_item(char *bcp_buf, dbref obj_ref, unsigned char dynflags)
{
	int aux, aux1;
	OBJ obj = obj_get(obj_ref);
	struct icon ico = object_icon(obj_ref);
	unsigned char iden = BCP_ITEM;
	char *p = bcp_buf, *aux2;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = obj_ref;
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	aux = obj.location;
	memcpy(p += sizeof(aux), &aux, sizeof(aux));
	memcpy(p += sizeof(aux), &dynflags, sizeof(dynflags));
	memcpy(p += sizeof(dynflags), &obj.type, sizeof(obj.type));
	memcpy(p += sizeof(obj.type), &ico.actions, sizeof(ico.actions));
	memcpy(p += sizeof(ico.actions), obj.name, aux = strlen(obj.name) + 1);
	aux2 = (char *) unparse(obj_ref);
	memcpy(p += aux, aux2, aux1 = strlen(aux2) + 1);
	memcpy(p += aux1, ico.icon, aux = strlen(ico.icon) + 1);
	aux2 = object_art(obj_ref);
	memcpy(p += aux, aux2, aux1 = strlen(aux2) + 1);
	switch (obj.type) {
	case TYPE_ROOM: {
		ROO *roo = &obj.sp.room;
		memcpy(p += aux1, &roo->exits, sizeof(roo->exits));
		p += sizeof(roo->exits);
		break;
	}
	case TYPE_ENTITY: {
		unsigned flags = ent_get(obj_ref).flags;
		memcpy(p += aux1, &flags, sizeof(flags));
		p += sizeof(flags);
		break;
	}
	default: {
		p += aux1;
	}}
	return p - bcp_buf;
}

static void
fbcp_item(dbref player_ref, dbref obj_ref, unsigned char dynflags)
{
	static char bcp_buf[SUPERBIGSIZ];
	int ret = _fbcp_item(bcp_buf, obj_ref, dynflags);
	nd_wwrite(player_ref, bcp_buf, ret);
}

static int // 2 + sizeof(iden) + sizeof(int) * 2
_fbcp_out(char *bcp_buf, dbref obj_ref)
{
	int aux;
	unsigned char iden = BCP_OUT;
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = obj_ref;
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	aux = obj_get(obj_ref).location;
	memcpy(p += sizeof(aux), &aux, sizeof(aux));
	p += sizeof(aux);
	return p - bcp_buf;
}

static void
fbcp_auth_success(dbref player_ref)
{
	unsigned char iden = BCP_AUTH_SUCCESS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(player_ref)];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &player_ref, sizeof(player_ref));
	p += sizeof(player_ref);
	nd_wwrite(player_ref, bcp_buf, p - bcp_buf);
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
fbcp_stats(dbref player_ref)
{
	ENT eplayer = ent_get(player_ref);
	unsigned char iden = BCP_STATS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(eplayer.attr) + sizeof(short) * 7];
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), eplayer.attr, sizeof(eplayer.attr));
	memcpy(p += sizeof(eplayer.attr), &eplayer.e[AF_HP].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_MOV].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_MDMG].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_MDEF].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_DODGE].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_DMG].value, sizeof(short));
	memcpy(p += sizeof(short), &eplayer.e[AF_DEF].value, sizeof(short));
	p += sizeof(short);
	nd_write(player_ref, bcp_buf, p - bcp_buf);
}

static void
fbcp_bars(dbref player_ref)
{
	ENT eplayer = ent_get(player_ref);
	unsigned char iden = BCP_BARS;
	static char bcp_buf[2 + sizeof(iden) + sizeof(int) * 4];
	char *p = bcp_buf;
	unsigned short aux;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &eplayer.hp, sizeof(eplayer.hp));
	aux = HP_MAX(&eplayer);
	memcpy(p += sizeof(eplayer.hp), &aux, sizeof(aux));
	memcpy(p += sizeof(aux), &eplayer.mp, sizeof(eplayer.mp));
	aux = MP_MAX(&eplayer);
	memcpy(p += sizeof(eplayer.mp), &aux, sizeof(aux));
	p += sizeof(aux);
	nd_write(player_ref, bcp_buf, p - bcp_buf);
}

void
fbcp_view(dbref player_ref, view_t *view)
{
	fbcp(player_ref, sizeof(view_t), BCP_VIEW, view);
}

void
fbcp_view_buf(dbref player_ref, char *view_buf)
{
	fbcp(player_ref, strlen(view_buf), BCP_VIEW_BUFFER, view_buf);
	nd_twritef(player_ref, view_buf);
}

void
mcp_look(dbref player_ref, dbref loc_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ loc = obj_get(loc_ref);
	dbref thing_ref;

	fbcp_item(player_ref, loc_ref, 1);
	switch (loc.type) {
	case TYPE_ROOM: break;
	case TYPE_ENTITY: // falls through
	default:
		eplayer.last_observed = loc_ref;
		ent_set(player_ref, &eplayer);
		observer_add(loc_ref, player_ref);
	}

        if (loc_ref != player_ref && loc.type == TYPE_ENTITY && !(eplayer.flags & EF_WIZARD))
                return;

	// use callbacks for mcp like this versus telnet
	struct hash_cursor c = contents_iter(loc_ref);
	while ((thing_ref = contents_next(&c)))
		fbcp_item(player_ref, thing_ref, 0);

	nd_twritef(player_ref, "%s\n", unparse(loc_ref));
	if (loc.description && *loc.description)
		nd_twritef(player_ref, loc.description);

        char buf[BUFSIZ];
        size_t buf_l = 0;

	struct hash_cursor c2 = contents_iter(loc_ref);
	while ((thing_ref = contents_next(&c2))) {
	/* check to see if there is anything there */
			if (thing_ref == player_ref)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"%s\r\n", unparse(thing_ref));
	}

        buf[buf_l] = '\0';
        nd_twritef(player_ref, "Contents: %s", buf);
}

static void
fbcp_room(dbref room_ref, char *msg, size_t len)
{
	struct hash_cursor c = contents_iter(room_ref);
	dbref tmp_ref;

	while ((tmp_ref = contents_next(&c))) {
		if (obj_get(tmp_ref).type != TYPE_ENTITY)
			continue;

		nd_wwrite(tmp_ref, msg, len);
	}
}

static void
fbcp_observers(dbref thing_ref, char *msg, size_t len)
{
	struct hash_cursor c = hash_citer(observable_hd, &thing_ref, sizeof(thing_ref));
	dbref tmp_ref = 0, ignore;

	while (hash_next(&tmp_ref, &ignore, &c) && tmp_ref) {
		OBJ tmp = obj_get(tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;
		nd_wwrite(tmp_ref, msg, len);
	}
}

void
mcp_content_out(dbref loc_ref, dbref thing_ref) {
	char bcp_buf[2 + sizeof(unsigned char) + sizeof(int) * 2];
	size_t len = _fbcp_out(bcp_buf, thing_ref);

	if (obj_get(loc_ref).type == TYPE_ROOM)
		fbcp_room(loc_ref, bcp_buf, len);
	else
		fbcp_observers(loc_ref, bcp_buf, len);
}

void
mcp_content_in(dbref loc_ref, dbref thing_ref) {
	static char bcp_buf[SUPERBIGSIZ];
	size_t len = _fbcp_item(bcp_buf, thing_ref, 2);

	if (obj_get(loc_ref).type == TYPE_ROOM)
		fbcp_room(loc_ref, bcp_buf, len);
	else
		fbcp_observers(loc_ref, bcp_buf, len);
}

void
mcp_auth_fail(int descr, int reason) {
	fbcp_auth_failure(descr, reason);
}

void
mcp_auth_success(dbref player_ref) {
	fbcp_auth_success(player_ref);
}

void
mcp_stats(dbref player_ref) {
	fbcp_stats(player_ref);
}

void
mcp_bars(dbref player_ref) {
	fbcp_bars(player_ref);
}

void
mcp_equipment(dbref player_ref)
{
	ENT eplayer = ent_get(player_ref);
	int aux;

	fbcp(player_ref, sizeof(eplayer.equipment), BCP_EQUIPMENT, eplayer.equipment);
	
	aux = eplayer.equipment[ES_HEAD];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_NECK];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_CHEST];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_BACK];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_RHAND];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_LFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_RFINGER];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
	aux = eplayer.equipment[ES_PANTS];
	if (aux && aux != NOTHING)
		fbcp_item(player_ref, aux, 0);
}

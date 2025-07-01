#include "mcp.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <ndc.h>

#include "uapi/io.h"
#include "uapi/entity.h"

#define SUPERBIGSIZ 80000 * 8192

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
	BCP_TOD = 12,
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
fbcp(unsigned player_ref, size_t len, unsigned char iden, void *data)
{
	char bcp_buf[2 + sizeof(iden) + len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + 2, &iden, sizeof(iden));
	memcpy(bcp_buf + 2 + sizeof(iden), data, len);
	nd_wwrite(player_ref, bcp_buf, 1 + sizeof(iden) + len);
}

static int
_fbcp_item(char *bcp_buf, unsigned obj_ref, unsigned char dynflags)
{
	int aux, aux1;
	OBJ obj;
	qdb_get(obj_hd, &obj, &obj_ref);
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
	memcpy(p += aux1, &ico.pi.fg, sizeof(ico.pi.fg));
	memcpy(p += sizeof(ico.pi.fg), &ico.pi.flags, sizeof(ico.pi.flags));
	memcpy(p += sizeof(ico.pi.flags), &ico.ch, sizeof(ico.ch));
	aux2 = object_art(obj_ref);
	memcpy(p += sizeof(ico.ch), aux2, aux1 = strlen(aux2) + 1);
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

void
fbcp_item(unsigned player_ref, unsigned obj_ref, unsigned char dynflags)
{
	static char bcp_buf[SUPERBIGSIZ];
	int ret = _fbcp_item(bcp_buf, obj_ref, dynflags);
	nd_wwrite(player_ref, bcp_buf, ret);
}

static int // 2 + sizeof(iden) + sizeof(int) * 2
_fbcp_out(char *bcp_buf, unsigned obj_ref)
{
	int aux;
	unsigned char iden = BCP_OUT;
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = obj_ref;
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	OBJ obj;
	qdb_get(obj_hd, &obj, &obj_ref);
	aux = obj.location;
	memcpy(p += sizeof(aux), &aux, sizeof(aux));
	p += sizeof(aux);
	return p - bcp_buf;
}

static void
fbcp_auth_success(unsigned player_ref)
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
fbcp_stats(unsigned player_ref)
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
	nd_wwrite(player_ref, bcp_buf, p - bcp_buf);
}

static void
fbcp_bars(unsigned player_ref)
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
	nd_wwrite(player_ref, bcp_buf, p - bcp_buf);
}

void
fbcp_view(unsigned player_ref, view_t *view)
{
	fbcp(player_ref, sizeof(view_t), BCP_VIEW, view);
}

void
fbcp_view_buf(unsigned player_ref, char *view_buf)
{
	fbcp(player_ref, strlen(view_buf), BCP_VIEW_BUFFER, view_buf);
	nd_twritef(player_ref, view_buf);
}

extern unsigned fds_hd;

static void
fbcp_all(size_t len, unsigned char iden, void *msg)
{
	qdb_cur_t c = qdb_iter(fds_hd, NULL);
	unsigned fd, tmp_ref;
	char buf[len + sizeof(iden)];
	memcpy(buf, &iden, sizeof(iden));
	memcpy(buf + sizeof(iden), msg, len);

	while (qdb_next(&tmp_ref, &fd, &c)) {
		if ((ndc_flags(fd) & DF_WEBSOCKET)) {
		fprintf(stderr, "fbcp_all %u %u\n", tmp_ref, fd);
			ndc_write(fd, buf, sizeof(buf));
		}
	}
}

static void
fbcp_room(unsigned room_ref, char *msg, size_t len)
{
	qdb_cur_t c = qdb_iter(contents_hd, &room_ref);
	unsigned tmp_ref;

	while (qdb_next(&room_ref, &tmp_ref, &c)) {
		OBJ tmp;
		qdb_get(obj_hd, &tmp, &tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;

		nd_wwrite(tmp_ref, msg, len);
	}
}

static void
fbcp_observers(unsigned thing_ref, char *msg, size_t len)
{
	qdb_cur_t c = qdb_iter(obs_hd, &thing_ref);;
	unsigned tmp_ref = 0;

	while (qdb_next(&thing_ref, &tmp_ref, &c)) {
		OBJ tmp;
		qdb_get(obj_hd, &tmp, &tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;
		nd_wwrite(tmp_ref, msg, len);
	}
}

void
mcp_content_out(unsigned loc_ref, unsigned thing_ref) {
	char bcp_buf[2 + sizeof(unsigned char) + sizeof(int) * 2];
	size_t len = _fbcp_out(bcp_buf, thing_ref);
	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);

	if (loc.type == TYPE_ROOM)
		fbcp_room(loc_ref, bcp_buf, len);
	else
		fbcp_observers(loc_ref, bcp_buf, len);
}

void
mcp_content_in(unsigned loc_ref, unsigned thing_ref) {
	static char bcp_buf[SUPERBIGSIZ];
	size_t len = _fbcp_item(bcp_buf, thing_ref, 2);
	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);

	if (loc.type == TYPE_ROOM)
		fbcp_room(loc_ref, bcp_buf, len);
	else
		fbcp_observers(loc_ref, bcp_buf, len);
}

void
mcp_auth_fail(int descr, int reason) {
	fbcp_auth_failure(descr, reason);
}

void
mcp_auth_success(unsigned player_ref) {
	fbcp_auth_success(player_ref);
}

void
mcp_stats(unsigned player_ref) {
	fbcp_stats(player_ref);
}

void
mcp_bars(unsigned player_ref) {
	fbcp_bars(player_ref);
}

void
mcp_tod(unsigned player_ref, unsigned tod) {
	fbcp(player_ref, sizeof(tod), BCP_TOD, &tod);
}

void
mcp_equipment(unsigned player_ref)
{
	ENT eplayer = ent_get(player_ref);
	unsigned aux;

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

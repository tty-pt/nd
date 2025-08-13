#include "mcp.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <ndc.h>

#include "uapi/io.h"
#include "uapi/entity.h"
#include "uapi/type.h"

#define SUPERBIGSIZ 80000 * 8192

enum bcp_type {
	BCP_ITEM,
	BCP_VIEW,
	BCP_VIEW_BUFFER,
	BCP_ROOM,
	BCP_ENTITY,
	BCP_AUTH_FAILURE,
	BCP_AUTH_SUCCESS,
	BCP_OUT,
	BCP_TOD,
	BCP_ACTION,
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

void
fbcp(unsigned player_ref, size_t len, unsigned char iden, void *data)
{
	char bcp_buf[2 + sizeof(iden) + len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + 2, &iden, sizeof(iden));
	memcpy(bcp_buf + 2 + sizeof(iden), data, len);
	nd_wwrite(player_ref, bcp_buf, 1 + sizeof(iden) + len);
}

void
mcp_action(unsigned player_ref, unsigned id,
		char *label, char *icon)
{
	unsigned char iden = BCP_ACTION;
	size_t label_len = strlen(label) + 1,
	       icon_len = strlen(icon) + 1,
	       pos = 2;
	char bcp_buf[2 + sizeof(iden) + sizeof(id) + label_len + icon_len];
	memcpy(bcp_buf, "#b", 2);
	memcpy(bcp_buf + pos, &iden, sizeof(iden));
	pos += sizeof(iden);
	memcpy(bcp_buf + pos, &id, sizeof(id));
	pos += sizeof(id);
	memcpy(bcp_buf + pos, label, label_len);
	pos += label_len;
	memcpy(bcp_buf + pos, icon, icon_len);
	pos += icon_len;
	nd_wwrite(player_ref, bcp_buf, pos);
}

typedef int _fbcp_item_t(char *, unsigned, unsigned, unsigned char);

int
_fbcp_item(char *bcp_buf, unsigned player_ref, unsigned obj_ref, unsigned char dynflags)
{
	int aux, aux1;
	OBJ obj;
	qmap_get(obj_hd, &obj, &obj_ref);
	struct icon ico = object_icon(player_ref, obj_ref);
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
	p += aux1;
	switch (obj.type) {
	case TYPE_ROOM: {
		ROO *roo = (ROO *) &obj.data;
		memcpy(p, &roo->exits, sizeof(roo->exits));
		p += sizeof(roo->exits);
		break;
	}
	case TYPE_ENTITY: {
		unsigned flags = ent_get(obj_ref).flags;
		memcpy(p, &flags, sizeof(flags));
		p += sizeof(flags);
		break;
	}
	}

	return p - bcp_buf;
}

void
fbcp_item(unsigned player_ref, unsigned obj_ref, unsigned char dynflags)
{
	static char bcp_buf[SUPERBIGSIZ];
	int ret = _fbcp_item(bcp_buf, player_ref, obj_ref, dynflags);
	nd_wwrite(player_ref, bcp_buf, ret);
}

static int // 2 + sizeof(iden) + sizeof(int) * 2
_fbcp_out(char *bcp_buf,
		unsigned player_ref __attribute__((unused)),
		unsigned obj_ref,
		unsigned char dynflags __attribute__((unused)))
{
	int aux;
	unsigned char iden = BCP_OUT;
	char *p = bcp_buf;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	aux = obj_ref;
	memcpy(p += sizeof(iden), &aux, sizeof(aux));
	OBJ obj;
	qmap_get(obj_hd, &obj, &obj_ref);
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

void
mcp_bar(unsigned char iden, unsigned player_ref, unsigned short val, unsigned short max)
{
	static char bcp_buf[2 + sizeof(iden) + sizeof(int) * 4];
	char *p = bcp_buf;
	unsigned short aux;
	memcpy(p, "#b", 2);
	memcpy(p += 2, &iden, sizeof(iden));
	memcpy(p += sizeof(iden), &val, sizeof(val));
	aux = max;
	memcpy(p += sizeof(val), &aux, sizeof(aux));
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
fbcp_observers(unsigned loc_ref, unsigned thing_ref, unsigned dynflags)
{
	unsigned hd, tmp_ref = 0, c;
	_fbcp_item_t *callback = &_fbcp_item;

	hd = obs_hd;

	if (dynflags & 4) {
		dynflags &= ~4;
		callback = &_fbcp_out;
	}

	c = qmap_iter(hd, &loc_ref);;

	while (qmap_next(&loc_ref, &tmp_ref, c)) {
		static char bcp_buf[SUPERBIGSIZ];
		OBJ tmp;
		size_t len;

		qmap_get(obj_hd, &tmp, &tmp_ref);
		if (tmp.type != TYPE_ENTITY)
			continue;

		len = callback(bcp_buf, tmp_ref, thing_ref, dynflags);
		nd_wwrite(tmp_ref, bcp_buf, len);
	}
}

void
mcp_content_out(unsigned loc_ref, unsigned thing_ref) {
	fbcp_observers(loc_ref, thing_ref, 2 | 4);
}

void
mcp_content_in(unsigned loc_ref, unsigned thing_ref) {
	fbcp_observers(loc_ref, thing_ref, 2);
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
mcp_tod(unsigned player_ref, unsigned tod) {
	fbcp(player_ref, sizeof(tod), BCP_TOD, &tod);
}


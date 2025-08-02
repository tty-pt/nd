#include "papi/nd.h"
#include <string.h>
#include <stdio.h>

struct nd nd;

/* IO */

unsigned fd_player(unsigned fd) {
	return nd.fd_player(fd);
}

/* int fds_has(unsigned player) { */
/* 	return nd.fds_has(player); */
/* } */

void nd_close(unsigned player) {
	nd.nd_close(player);
}

void nd_write(unsigned player_ref, char *str, size_t len) {
	nd.nd_write(player_ref, str, len);
}

void nd_dwritef(unsigned player_ref, const char *fmt, va_list args) {
	nd.nd_dwritef(player_ref, fmt, args);
}

void nd_rwrite(unsigned room_ref, unsigned exception_ref, char *str, size_t len) {
	nd.nd_rwrite(room_ref, exception_ref, str, len);
}

void nd_dowritef(unsigned player_ref, const char *fmt, va_list args) {
	nd.nd_dowritef(player_ref, fmt, args);
}

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args) {
	nd.nd_tdwritef(player_ref, fmt, args);
}

void nd_wwrite(unsigned player_ref, void *msg, size_t len) {
	nd.nd_wwrite(player_ref, msg, len);
}

/* MAP */

int map_has(unsigned thing) {
	return nd.map_has(thing);
}

morton_t map_mwhere(unsigned thing) {
	return nd.map_mwhere(thing);
}

void map_where(pos_t p, unsigned thing) {
	nd.map_where(p, thing);
}

int map_delete(unsigned what) {
	return nd.map_delete(what);
}

unsigned map_get(pos_t at) {
	return nd.map_get(at);
}

/* ST */

void st_teleport(unsigned player_ref, uint64_t pos) {
	nd.st_teleport(player_ref, pos);
}

void st_run(unsigned player_ref, char *symbol) {
	nd.st_run(player_ref, symbol);
}

/* WTS */

char *plural(char *singular) {
	return nd.plural(singular);
}

/* SKEL */

/* void skel_init(); */

/* void drop_init(); */

/* OBJECT */

/* void drop_init(); */

unsigned object_new(OBJ *obj) {
	return nd.object_new(obj);
}

unsigned object_copy(OBJ *nu, unsigned old_ref) {
	return nd.object_copy(nu, old_ref);
}

void object_move(unsigned what_ref, unsigned where_ref) {
	nd.object_move(what_ref, where_ref);
}

unsigned object_add(OBJ *nu, unsigned skel_id, unsigned where, uint64_t v, unsigned flags) {
	return nd.object_add(nu, skel_id, where, v, flags);
}

void object_drop(unsigned where_ref, unsigned skel_id) {
	nd.object_drop(where_ref, skel_id);
}

/* ENTITY */

unsigned me_get(void) {
	return nd.me_get();
}

ENT ent_get(unsigned ref) {
	return nd.ent_get(ref);
}

void ent_set(unsigned ref, ENT *tmp) {
	nd.ent_set(ref, tmp);
}

void ent_del(unsigned ref) {
	nd.ent_del(ref);
}

int controls(unsigned who_ref, unsigned what_ref) {
	return nd.controls(who_ref, what_ref);
}

int payfor(unsigned who_ref, OBJ *who, unsigned cost) {
	return nd.payfor(who_ref, who, cost);
}

void enter(unsigned player_ref, unsigned loc_ref, enum exit e) {
	nd.enter(player_ref, loc_ref, e);
}

void look_at(unsigned player_ref, unsigned thing_ref) {
	nd.look_at(player_ref, thing_ref);
}

unsigned room_clean(unsigned loc_ref) {
	return nd.room_clean(loc_ref);
}

inline static unsigned hd_get(unsigned hd) {
	return hd >= HD_MAX ? hd : nd.hds[hd];
}

unsigned nd_put(unsigned hd, void *key, void *value) {
	return nd.nd_put(hd_get(hd), key, value);
}

unsigned nd_get(unsigned hd, void *value, void *key) {
	return nd.nd_get(hd_get(hd), value, key);
}

int nd_open(char *database, char *key_sip, char *value_sip, unsigned flags) {
	unsigned hd = nd.nd_open(database, key_sip, value_sip, flags);
	nd.nd_put(nd.hds[HD_HD], database, &hd);
	return hd;
}

nd_cur_t nd_iter(unsigned hd, void *key) {
	return nd.nd_iter(hd_get(hd), key);
}

int nd_next(void *key, void *value, nd_cur_t *cur) {
	return nd.nd_next(key, value, cur);
}

void nd_fin(nd_cur_t *cur) {
	nd.nd_fin(cur);
}

void nd_len_reg(char *iden, size_t len) {
	nd.nd_len_reg(iden, len);
}

void nd_register(char *cmd, nd_cb_t *cb, unsigned flags) {
	nd.nd_register(cmd, cb, flags);
}

unsigned ematch_at(unsigned player_ref, unsigned where_ref, char *name) {
	return nd.ematch_at(player_ref, where_ref, name);
}

unsigned ematch_player(char *name) {
	return nd.ematch_player(name);
}

unsigned ematch_absolute(char *name) {
	return nd.ematch_absolute(name);
}

unsigned ematch_me(unsigned player_ref, char *str) {
	return nd.ematch_me(player_ref, str);
}

unsigned ematch_here(unsigned player_ref, char *str) {
	return nd.ematch_here(player_ref, str);
}

unsigned ematch_mine(unsigned player_ref, char *str) {
	return nd.ematch_mine(player_ref, str);
}

unsigned ematch_near(unsigned player_ref, char *str) {
	return nd.ematch_near(player_ref, str);
}

unsigned ematch_all(unsigned player_ref, char *str) {
	return nd.ematch_all(player_ref, str);
}

void mod_load(char *fname) {
	nd.mod_load(fname);
}

unsigned action_register(char *label, char *icon) {
	return nd.action_register(label, icon);
}

unsigned vtf_register(char emp, enum color fg, unsigned flags) {
	return nd.vtf_register(emp, fg, flags);
}

unsigned sic_areg(char *name, sic_adapter_t *adapter) {
	return nd.sic_areg(name, adapter);
}

void sic_call(void *retp, unsigned id, void *args) {
	nd.sic_call(retp, id, args);
}

struct bio noise_point(pos_t pos) {
	return nd.noise_point(pos);
}

void fbcp(unsigned player_ref, size_t len, unsigned char iden, void *data) {
	nd.fbcp(player_ref, len, iden, data);
}

void fbcp_item(unsigned player_ref, unsigned obj_ref, unsigned char dynflags) {
	nd.fbcp_item(player_ref, obj_ref, dynflags);
}

void mcp_content_out(unsigned loc_ref, unsigned thing_ref) {
	nd.mcp_content_out(loc_ref, thing_ref);
}

void mcp_content_in(unsigned loc_ref, unsigned thing_ref) {
	nd.mcp_content_in(loc_ref, thing_ref);
}

int sic_last(void *ret) {
	if (!nd.adapter->ran)
		return 1;
	memcpy(ret, nd.adapter->ret, nd.adapter->ret_size);
	return 0;
}

void mcp_bar(unsigned char iden, unsigned player_ref,
		unsigned short val, unsigned short max)
{
	nd.mcp_bar(iden, player_ref, val, max);
}

unsigned sic_get(char *name) {
	return nd.sic_get(name);
}

void mod_auto_init(void) {
    extern void (*__start_sic_auto_init)(void);
    extern void (*__stop_sic_auto_init)(void);

    for (void (**fn)(void) = &__start_sic_auto_init; fn < &__stop_sic_auto_init; ++fn)
        (*fn)();
}

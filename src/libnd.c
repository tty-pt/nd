#include "papi/nd.h"

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

void dnotify_wts(unsigned player_ref, char const *a, char const *b, char *format, va_list args) {
	nd.dnotify_wts(player_ref, a, b, format, args);
}

void dnotify_wts_to(unsigned who_ref, unsigned tar_ref, char const *a, char const *b, char *format, va_list args) {
	nd.dnotify_wts_to(who_ref, tar_ref, a, b, format, args);
}

void notify_attack(unsigned player_ref, unsigned target_ref, char *wts, short val, enum color color, short mval) {
	nd.notify_attack(player_ref, target_ref, wts, val, color, mval);
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

int st_teleport(unsigned player_ref, struct cmd_dir cd) {
	return nd.st_teleport(player_ref, cd);
}

void st_run(unsigned player_ref, char *symbol) {
	nd.st_run(player_ref, symbol);
}

/* WTS */

char *wts_plural(char *singular) {
	return nd.wts_plural(singular);
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

unsigned object_add(OBJ *nu, unsigned skel_id, unsigned where, void *arg) {
	return nd.object_add(nu, skel_id, where, arg);
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

void ent_reset(ENT *ent) {
	nd.ent_reset(ent);
}

void birth(ENT *eplayer) {
	nd.birth(eplayer);
}

void sit(unsigned player_ref, ENT *eplayer, char *what) {
	nd.sit(player_ref, eplayer, what);
}

int stand_silent(unsigned player_ref, ENT *eplayer) {
	return nd.stand_silent(player_ref, eplayer);
}

void stand(unsigned player_ref, ENT *eplayer) {
	nd.stand(player_ref, eplayer);
}

int controls(unsigned who_ref, unsigned what_ref) {
	return nd.controls(who_ref, what_ref);
}

int payfor(unsigned who_ref, OBJ *who, unsigned cost) {
	return nd.payfor(who_ref, who, cost);
}

void look_around(unsigned player_ref) {
	nd.look_around(player_ref);
}

int equip_affect(ENT *ewho, EQU *equ) {
	return nd.equip_affect(ewho, equ);
}

int equip(unsigned player_ref, unsigned eq_ref) {
	return nd.equip(player_ref, eq_ref);
}

unsigned unequip(unsigned player_ref, unsigned eql) {
	return nd.unequip(player_ref, eql);
}

unsigned mask_element(ENT *ref, register unsigned char a) {
	return nd.mask_element(ref, a);
}

int entity_damage(unsigned player_ref, ENT *eplayer, unsigned target_ref, ENT *etarget, short amt) {
	return nd.entity_damage(player_ref, eplayer, target_ref, etarget, amt);
}

void enter(unsigned player_ref, unsigned loc_ref, enum exit e) {
	nd.enter(player_ref, loc_ref, e);
}

int kill_dodge(unsigned player_ref, char *wts) {
	return nd.kill_dodge(player_ref, wts);
}

short kill_dmg(unsigned dmg_type, short dmg, short def, unsigned def_type) {
	return nd.kill_dmg(dmg_type, dmg, def, def_type);
}

int spell_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref, unsigned slot) {
	return nd.spell_cast(player_ref, eplayer, target_ref, slot);
}

void debufs_end(ENT *player) {
	nd.debufs_end(player);
}

void look_at(unsigned player_ref, unsigned thing_ref) {
	nd.look_at(player_ref, thing_ref);
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

unsigned nd_open(char *database, char *key_sip, char *value_sip, unsigned flags) {
	return nd.nd_open(database, key_sip, value_sip, flags);
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

void nd_mod_load(char *fname) {
	nd.nd_mod_load(fname);
}

#include "papi/nd.h"

struct nd nd;

/* IO */

unsigned fd_player(int fd) {
	return nd.fd_player(fd);
}

unsigned fds_hd_get() {
	return nd.fds_hd;
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

void notify_attack(unsigned player_ref, unsigned target_ref, char *wts, short val, char const *color, short mval) {
	nd.notify_attack(player_ref, target_ref, wts, val, color, mval);
}

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args) {
	return nd.nd_tdwritef(player_ref, fmt, args);
}

void nd_wwrite(unsigned player_ref, void *msg, size_t len) {
	return nd.nd_wwrite(player_ref, msg, len);
}

/* MAP */

int map_has(unsigned thing) {
	return nd.map_has(thing);
}

morton_t map_mwhere(unsigned thing) {
	return nd.map_mwhere(thing);
}

void map_where(pos_t p, unsigned thing) {
	return nd.map_where(p, thing);
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

unsigned skel_hd_get() {
	return nd.skel_hd;
}

/* void drop_init(); */

unsigned drop_hd_get() {
	return nd.drop_hd;
}

unsigned adrop_hd_get() {
	return nd.adrop_hd;
}

/* OBJECT */

/* void drop_init(); */

unsigned obj_hd_get() {
	return nd.obj_hd;
}

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
	return nd.object_drop(where_ref, skel_id);
}

unsigned obs_hd_get() {
	return nd.obs_hd;
}

unsigned contents_hd_get() {
	return nd.contents_hd;
}

/* ENTITY */

unsigned me_get() {
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

int payfor(unsigned who_ref, OBJ *who, int cost) {
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

enum element mask_element(ENT *ref, register unsigned char a) {
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

short kill_dmg(enum element dmg_type, short dmg, short def, enum element def_type) {
	return nd.kill_dmg(dmg_type, dmg, def, def_type);
}

int spell_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref, unsigned slot) {
	return nd.spell_cast(player_ref, eplayer, target_ref, slot);
}

void debufs_end(ENT *player) {
	nd.debufs_end(player);
}

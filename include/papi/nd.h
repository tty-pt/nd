#ifndef PAPI_ND_H
#define PAPI_ND_H

#include "uapi/nd.h"

struct nd {
	/* io */
	fd_player_t *fd_player;

	unsigned fds_hd;

	/* fds_has_t *fds_has; */
	nd_close_t *nd_close;
	nd_write_t *nd_write;
	nd_dwritef_t *nd_dwritef;
	nd_rwrite_t *nd_rwrite;
	nd_dowritef_t *nd_dowritef;
	dnotify_wts_t *dnotify_wts;
	dnotify_wts_to_t *dnotify_wts_to;
	notify_attack_t *notify_attack;
	nd_tdwritef_t *nd_tdwritef;
	nd_wwrite_t *nd_wwrite;

	/* map */
	map_has_t *map_has;
	map_mwhere_t *map_mwhere;
	map_where_t *map_where;
	map_delete_t *map_delete;
	map_get_t *map_get;

	/* st */
	st_teleport_t *st_teleport;
	st_run_t *st_run;

	/* wts */
	wts_plural_t *wts_plural;

	/* skel */
	unsigned skel_hd;
	/* skel_init_t *skel_init; */
	unsigned drop_hd;
	/* drop_init_t *drop_init; */
	unsigned adrop_hd;

	/* object */
	/* obj_init_t *obj_init; */
	unsigned obj_hd;
	obj_exists_t *obj_exists;
	object_new_t *object_new;
	object_copy_t *object_copy;
	object_move_t *object_move;
	object_add_t *object_add;
	object_drop_t *object_drop;
	unsigned obs_hd;
	unsigned contents_hd;
	object_icon_t *object_icon;
	art_max_t *art_max;
	object_art_t *object_art;
	unparse_t *unparse;

	/* entity */
	me_get_t *me_get;
	ent_get_t *ent_get;
	ent_set_t *ent_set;
	ent_del_t *ent_del;
	ent_reset_t *ent_reset;
	birth_t *birth;
	sit_t *sit;
	stand_silent_t *stand_silent;
	stand_t *stand;
	controls_t *controls;
	payfor_t *payfor;
	look_around_t *look_around;
	equip_affect_t *equip_affect;
	equip_t *equip;
	unequip_t *unequip;
	mask_element_t *mask_element;
	entity_damage_t *entity_damage;
	enter_t *enter;
	kill_dodge_t *kill_dodge;
	kill_dmg_t *kill_dmg;
	spell_cast_t *spell_cast;
	debufs_end_t *debufs_end;
};

extern struct nd nd;

#endif

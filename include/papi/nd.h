#ifndef PAPI_ND_H
#define PAPI_ND_H

#include "uapi/nd.h"

struct nd {
	/* io */
	fd_player_t *fd_player;
	fds_iter_t *fds_iter;
	fds_next_t *fds_next;
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
	/* skel_init_t *skel_init; */
	skel_new_t *skel_new;
	skel_get_t *skel_get;
	skel_set_t *skel_set;
	lhash_iter_t *skel_iter;
	skel_next_t *skel_next;
	/* drop_init_t *drop_init; */
	drop_new_t *drop_new;
	drop_get_t *drop_get;
	drop_set_t *drop_set;
	lhash_iter_t *drop_iter;
	drop_next_t *drop_next;
	alhash_add_t *adrop_add;
	alhash_iter_t *adrop_iter;
	alhash_next_t *adrop_next;
	alhash_remove_t *adrop_remove;

	/* object */
	/* obj_init_t *obj_init; */
	obj_new_t *obj_new;
	obj_get_t *obj_get;
	obj_set_t *obj_set;
	lhash_iter_t *obj_iter;
	obj_next_t *obj_next;
	obj_exists_t *obj_exists;
	object_new_t *object_new;
	object_copy_t *object_copy;
	object_move_t *object_move;
	object_add_t *object_add;
	object_drop_t *object_drop;
	alhash_add_t *obs_add;
	alhash_iter_t *obs_iter;
	alhash_next_t *obs_next;
	alhash_remove_t *obs_remove;
	alhash_add_t *contents_add;
	alhash_iter_t *contents_iter;
	alhash_next_t *contents_next;
	alhash_remove_t *contents_remove;
	object_icon_t *object_icon;
	art_max_t *art_max;
	object_art_t *object_art;
	unparse_t *unparse;

	/* entity */
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

#endif

#ifndef PAPI_ND_H
#define PAPI_ND_H

#include "uapi/nd.h"

struct nd {
	/* io */
	fd_player_t *fd_player;

	nd_close_t *nd_close;
	nd_write_t *nd_write;
	nd_dwritef_t *nd_dwritef;
	nd_rwrite_t *nd_rwrite;
	nd_dowritef_t *nd_dowritef;
	dnotify_wts_t *dnotify_wts;
	dnotify_wts_to_t *dnotify_wts_to;
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

	/* object */
	obj_exists_t *obj_exists;
	object_new_t *object_new;
	object_copy_t *object_copy;
	object_move_t *object_move;
	object_add_t *object_add;
	object_drop_t *object_drop;
	object_icon_t *object_icon;
	object_art_t *object_art;
	unparse_t *unparse;

	/* entity */
	me_get_t *me_get;
	ent_get_t *ent_get;
	ent_set_t *ent_set;
	ent_del_t *ent_del;
	ent_reset_t *ent_reset;
	controls_t *controls;
	payfor_t *payfor;
	look_around_t *look_around;
	enter_t *enter;
	look_at_t *look_at;
	room_clean_t *room_clean;

	nd_put_t *nd_put, *nd_get;
	nd_open_t *nd_open;
	nd_iter_t *nd_iter;
	nd_next_t *nd_next;
	nd_fin_t *nd_fin;
	nd_len_reg_t *nd_len_reg;

	nd_register_t *nd_register;

	ematch_at_t *ematch_at;
	ematch_player_t *ematch_player;
	ematch_absolute_t *ematch_absolute;
	ematch_me_t *ematch_me;
	ematch_here_t *ematch_here;
	ematch_mine_t *ematch_mine;
	ematch_near_t *ematch_near;
	ematch_all_t *ematch_all;

	mod_load_t *mod_load;

	action_register_t *action_register;
	vtf_register_t *vtf_register;
	sic_call_t *sic_call;
	sic_areg_t *sic_areg;

	noise_point_t *noise_point;

	fbcp_item_t *fbcp_item;
	fbcp_t *fbcp;
	mcp_content_in_t *mcp_content_in;
	mcp_content_out_t *mcp_content_out;
	mcp_bar_t *mcp_bar;

	unsigned hds[HD_MAX];
	unsigned hd_hd;
	char ret[5096];
};

extern struct nd nd;

#endif

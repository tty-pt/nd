/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#define NOMATCH_MESSAGE "I don't know what you mean.\n"
#define CANTDO_MESSAGE "You can't do that.\n"

typedef unsigned ematch_at_t(unsigned player_ref, unsigned where_ref, char *name);
ematch_at_t ematch_at;

typedef unsigned ematch_player_t(char *name);
ematch_player_t ematch_player;

typedef unsigned ematch_absolute_t(char *name);
ematch_absolute_t ematch_absolute;

typedef unsigned ematch_me_t(unsigned player_ref, char *str);
ematch_me_t ematch_me;

typedef unsigned ematch_here_t(unsigned player_ref, char *str);
ematch_here_t ematch_here;

typedef unsigned ematch_mine_t(unsigned player_ref, char *str);
ematch_mine_t ematch_mine;

typedef unsigned ematch_near_t(unsigned player_ref, char *str);
ematch_near_t ematch_near;

typedef unsigned ematch_all_t(unsigned player_ref, char *name);
ematch_all_t ematch_all;

#endif /* _MATCH_H */

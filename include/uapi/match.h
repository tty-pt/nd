/* $Header$ */

#ifndef _MATCH_H
#define _MATCH_H

#define NOMATCH_MESSAGE "I don't know what you mean.\n"

unsigned ematch_at(unsigned player_ref, unsigned where_ref, char *name);
unsigned ematch_player(char *name);
unsigned ematch_absolute(char *name);
unsigned ematch_me(unsigned player_ref, char *str);
unsigned ematch_here(unsigned player_ref, char *str);
unsigned ematch_mine(unsigned player_ref, char *str);
unsigned ematch_near(unsigned player_ref, char *str);
unsigned ematch_all(unsigned player_ref, char *name);

#endif /* _MATCH_H */

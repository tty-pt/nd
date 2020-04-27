#ifndef WEB_H
#define WEB_H
#include <sys/types.h>
#include <db.h>
/* void web_logout(int descr); */
int web_geo_view(int descr, char *buf);
int web_art(int descr, const char *art, char *buf, size_t n);
int web_support(int descr);
int web_look(int descr, dbref player, dbref loc, char const *description);
void do_meme(int descr, dbref player, char const *url);
#endif

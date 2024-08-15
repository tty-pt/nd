#ifndef UAPI_IO_H
#define UAPI_IO_H

#include <stdarg.h>
#include <stddef.h>
#include <qhash.h>

unsigned fd_player(int fd);
struct hash_cursor fds_iter(unsigned player);
int fds_next(struct hash_cursor *c);
int fds_has(unsigned player);
void nd_close(unsigned player);
void nd_write(unsigned player_ref, char *str, size_t len);
void nd_dwritef(unsigned player_ref, const char *fmt, va_list args);
void nd_writef(unsigned player_ref, const char *fmt, ...);
void nd_rwrite(unsigned room_ref, unsigned exception_ref, char *str, size_t len);
void nd_owritef(unsigned player_ref, char *format, ...);
void notify_wts(unsigned player_ref, char const *a, char const *b, char *format, ...);
void notify_wts_to(unsigned player_ref, unsigned target_ref, char const *a, char const *b, char *format, ...);
void notify_attack(unsigned player_ref, unsigned target_ref, char *wts, short val, char const *color, short mval);

void nd_tdwritef(unsigned player_ref, const char *fmt, va_list args);
void nd_twritef(unsigned player_ref, const char *fmt, ...);
void nd_wwrite(unsigned player_ref, void *msg, size_t len);

#endif


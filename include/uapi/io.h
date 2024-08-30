#ifndef UAPI_IO_H
#define UAPI_IO_H

#include <stdarg.h>
#include <stddef.h>
#include <qhash.h>

typedef unsigned fd_player_t(int fd);
fd_player_t fd_player;

typedef struct hash_cursor fds_iter_t(unsigned player);
fds_iter_t fds_iter;

typedef int fds_next_t(struct hash_cursor *c);
fds_next_t fds_next;

/* typedef int fds_has_t(unsigned player); */
/* fds_has_t fds_has; */

typedef void nd_close_t(unsigned player);
nd_close_t nd_close;

typedef void nd_write_t(unsigned player_ref, char *str, size_t len);
nd_write_t nd_write;

typedef void nd_dwritef_t(unsigned player_ref, const char *fmt, va_list args);
nd_dwritef_t nd_dwritef;

static inline void
nd_writef(unsigned player_ref, const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_dwritef(player_ref, fmt, va);
	va_end(va);
}

typedef void nd_rwrite_t(unsigned room_ref, unsigned exception_ref, char *str, size_t len);
nd_rwrite_t nd_rwrite;

typedef void nd_dowritef_t(unsigned player_ref, const char *format, va_list args);
nd_dowritef_t nd_dowritef;

static inline void
nd_owritef(unsigned player_ref, char *format, ...)
{
	va_list args;
	va_start(args, format);
	nd_dowritef(player_ref, format, args);
	va_end(args);
}

typedef void dnotify_wts_t(unsigned who_ref, char const *a, char const *b, char *format, va_list args);
dnotify_wts_t dnotify_wts;

static inline void
notify_wts(unsigned who_ref, char const *a, char const *b, char *format, ...)
{
	va_list args;
	va_start(args, format);
	dnotify_wts(who_ref, a, b, format, args);
	va_end(args);
}

typedef void dnotify_wts_to_t(unsigned who_ref, unsigned tar_ref, char const *a, char const *b, char *format, va_list args);
dnotify_wts_to_t dnotify_wts_to;

static inline void
notify_wts_to(unsigned who_ref, unsigned tar_ref, char const *a, char const *b, char *format, ...)
{
	va_list args;
	va_start(args, format);
	dnotify_wts_to(who_ref, tar_ref, a, b, format, args);
	va_end(args);
}

typedef void notify_attack_t(unsigned player_ref, unsigned target_ref, char *wts, short val, char const *color, short mval);
notify_attack_t notify_attack;

typedef void nd_tdwritef_t(unsigned player_ref, const char *fmt, va_list args);
nd_tdwritef_t nd_tdwritef;

static inline void nd_twritef(unsigned player_ref, const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	nd_tdwritef(player_ref, fmt, va);
	va_end(va);
}

typedef void nd_wwrite_t(unsigned player_ref, void *msg, size_t len);
nd_wwrite_t nd_wwrite;

#endif

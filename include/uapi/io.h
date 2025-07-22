#ifndef UAPI_IO_H
#define UAPI_IO_H

#include <stdarg.h>
#include <stddef.h>
#include "azoth.h"

enum hd {
	HD_FD,
	HD_SKEL,
	HD_DROP,
	HD_ADROP,
	HD_BIOME,
	HD_WTS,
	HD_RWTS,
	HD_OBJ,
	HD_OBS,
	HD_CONTENTS,
	HD_TYPE,
	HD_RTYPE,
	HD_BCP,
	HD_ELEMENT,
	HD_MAX,
};

typedef unsigned fd_player_t(unsigned fd);
fd_player_t fd_player;

/* typedef int fds_has_t(unsigned player); */
/* fds_has_t fds_has; */

typedef void nd_close_t(unsigned player);
nd_close_t nd_close, nd_flush;

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

typedef void notify_attack_t(unsigned player_ref, unsigned target_ref, char *wts, short val, enum color color, short mval);
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

extern unsigned fds_hd;

typedef unsigned (nd_put_t)(unsigned, void *, void *);
nd_put_t nd_put, nd_get;

typedef int (nd_open_t)(char *, char *, char *, unsigned);
nd_open_t nd_open;

typedef void nd_len_reg_t(char *iden, size_t len);
nd_len_reg_t nd_len_reg;

typedef struct {
	int flags;
	void *internal;
} nd_cur_t;

typedef nd_cur_t (nd_iter_t)(unsigned, void *);
nd_iter_t nd_iter;

typedef int (nd_next_t)(void *, void *, nd_cur_t *cur);
nd_next_t nd_next;

typedef void (nd_fin_t)(nd_cur_t *cur);
nd_fin_t nd_fin;

typedef void nd_cb_t(int fd, int argc, char *argv[]);
typedef void nd_register_t(char *, nd_cb_t *, unsigned);
nd_register_t nd_register;

typedef void mod_load_t(char *fname);
mod_load_t mod_load;

#endif

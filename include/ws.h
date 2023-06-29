#ifndef WS_H
#define WS_H

#include <unistd.h>
#include <stdarg.h>

int
ws_handshake(int cfd, char *buf);

int
ws_read(int cfd, char *data, size_t len);

int
_ws_write(int cfd, const void *data, size_t n, int start, int cont, int fin);

int
ws_write(int cfd, const void *data, size_t n);

int
wsdprintf(int cfd, const char *format, va_list ap);
int wsprintf(int fd, const char *format, ...);

#endif

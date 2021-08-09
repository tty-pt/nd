#ifndef WS_H
#define WS_H

#include <unistd.h>

int
ws_handshake(int cfd, char *buf);

int
ws_read(int cfd, char *data, size_t len);

int
ws_write(int cfd, void *data, size_t n);

#endif

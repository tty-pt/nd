#ifndef ND_H
#define ND_H

typedef void (*echo_t)(char *, ...);

struct nd {
	echo_t echo, oecho;
} nd;

#endif

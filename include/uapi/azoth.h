#ifndef AZOTH_H
#define AZOTH_H

enum color {
	BLACK,
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE
};

enum pi_flags {
	BOLD = 1,
};

struct print_info {
	enum color fg;
	unsigned flags;
};

#endif

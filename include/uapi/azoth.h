#ifndef AZOTH_H
#define AZOTH_H

#define ANSI_BOLD	"\033[1m"
#define ANSI_DIM      	"\033[2m"
#define ANSI_ITALIC   	"\033[3m"
#define ANSI_UNDERLINE	"\033[4m"
#define ANSI_FLASH	"\033[5m"
#define ANSI_REVERSE	"\033[7m"
#define ANSI_OSTRIKE	"\033[9m"

#define ANSI_RESET_BOLD	ANSI_DIM

#define ANSI_FG_BLACK	"\033[30m"
#define ANSI_FG_RED	"\033[31m"
#define ANSI_FG_YELLOW	"\033[33m"
#define ANSI_FG_GREEN	"\033[32m"
#define ANSI_FG_CYAN	"\033[36m"
#define ANSI_FG_BLUE	"\033[34m"
#define ANSI_FG_MAGENTA	"\033[35m"
#define ANSI_FG_WHITE	"\033[37m"

#define ANSI_BG_BLACK	"\033[40m"
#define ANSI_BG_RED	"\033[41m"
#define ANSI_BG_YELLOW	"\033[43m"
#define ANSI_BG_GREEN	"\033[42m"
#define ANSI_BG_CYAN	"\033[46m"
#define ANSI_BG_BLUE	"\033[44m"
#define ANSI_BG_MAGENTA	"\033[45m"
#define ANSI_BG_WHITE	"\033[47m"

#define ANSI_RESET	"\033[0m"

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

static char *ansi_bg[] = {
	[BLACK] = ANSI_BG_BLACK,
	[RED] = ANSI_BG_RED,
	[YELLOW] = ANSI_BG_YELLOW,
	[GREEN] = ANSI_BG_GREEN,
	[CYAN] = ANSI_BG_CYAN,
	[BLUE] = ANSI_BG_BLUE,
	[MAGENTA] = ANSI_BG_MAGENTA,
	[WHITE] = ANSI_BG_WHITE,
};

static char *ansi_fg[] = {
	[BLACK] = ANSI_FG_BLACK,
	[RED] = ANSI_FG_RED,
	[YELLOW] = ANSI_FG_YELLOW,
	[GREEN] = ANSI_FG_GREEN,
	[CYAN] = ANSI_FG_CYAN,
	[BLUE] = ANSI_FG_BLUE,
	[MAGENTA] = ANSI_FG_MAGENTA,
	[WHITE] = ANSI_FG_WHITE,
};

#endif

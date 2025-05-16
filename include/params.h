#ifndef _PARAMS_H
#define _PARAMS_H

#define GLOBAL_ENVIRONMENT ((unsigned) 0)	/* parent of all rooms.  Always #0 */

/* magic cookies (not chocolate chip) :) */

#define ESCAPE_CHAR 27
#define NUMBER_TOKEN '#'

/* ANSI attributes and color codes */

#define ANSI_RESET	"\033[0m"

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

#define ENABLE_TRANSACTIONS 0

extern char *ansi_bg[];
extern char *ansi_fg[];

#endif /* _PARAMS_H */

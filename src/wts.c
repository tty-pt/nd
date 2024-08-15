#include "uapi/wts.h"
#include <string.h>
#include <stdio.h>

char *wts_map[] = {
	"punch",
	"peck",
	"slash",
	"bite",
	"strike",
	"slam",
	"kick",
	"thrust",
	"stab",
	"hit",
	"crush",
	"chop",
	"smash",
	"throw",
	"pound",
	"bludgeon",
	"lash",
	"rip",
	"tear",
	"bash",
	"trample",
	"headbutt",
	"backhand",
	"uppercut",
	"shove",
	"knee",
	"elbow",
	"flail at",
	"gnaw",
	"pierce",
	"whip",
	"slap",
	"claw",
	"maul",
	"mace",
	"batter",
	"hammer",
	"spin",
	"tackle",
	"pin",
	"grapple",
	"hook",
	"jab",
	"slice",
	"pummel",
	"fling",
	"stomp",
	"blast",
	"zap",
	"sizzle",
	"burn",
	"scald",
	"peel",
	"bruise",
	"sting",
};

char *wts_plural(char *singular) {
	static char plural[BUFSIZ], *last;
	char *space = strchr(singular, ' ');
	size_t len = space ? space - singular : strlen(singular);
	last = singular + len - 1;
	strncpy(plural, singular, len + 1);

	switch (*last) {
		case 'y':
			*last = 'i';
		case 's':
		case 'x':
		case 'z':
		case 'h':
			strcat(plural, "es");
			break;
		default:
			strcat(plural, "s");
	}

	return plural;
}

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

// TODO improve wts. We want no hardcoded ones
char *buf_wts[] = {
	// HP
	"heal",

	// - HP
	[16] =
	"bleed",
	"burn",

	// 32 MOV

	// - MOV
	[48] =
	"stunned",
	"",
	"frozen",

	// MDMG
	[64] =
	"focus on attacking",
	"focus on fire",

	// - MDMG
	[80] =
	"weaken",

	// MDEF
	[96] =
	"focus on defending",
	"are protected by flames",

	// - MDEF
	[112] =
	"are distracted",

	// 128 DODGE

	// 144 - DDGE
};

enum wt {
	WT_PUNCH,
	WT_PECK,
	WT_SLASH,
	WT_BITE,
	WT_STRIKE,
	WT_SLAM,
	WT_KICK,
	WT_THRUST,
	WT_STAB,
	WT_HIT,
	WT_CRUSH,
	WT_CHOP,
	WT_SMASH,
	WT_THROW,
	WT_POUND,
	WT_BLUDGEON,
	WT_LASH,
	WT_RIP,
	WT_TEAR,
	WT_BASH,
	WT_TRAMPLE,
	WT_HEADBUTT,
	WT_BACKHAND,
	WT_UPPERCUT,
	WT_SHOVE,
	WT_KNEE,
	WT_ELBOW,
	WT_FLAIL,
	WT_GNAW,
	WT_PIERCE,
	WT_WHIP,
	WT_SLAP,
	WT_CLAW,
	WT_MAUL,
	WT_MACE,
	WT_BATTER,
	WT_HAMMER,
	WT_SPIN,
	WT_TACKLE,
	WT_PIN,
	WT_GRAPPLE,
	WT_HOOK,
	WT_JAB,
	WT_SLICE,
	WT_PUMMEL,
	WT_FLING,
	WT_STOMP,
	WT_BLAST,
	WT_ZAP,
	WT_SIZZLE,
	WT_BURN,
	WT_SCALD,
	WT_PEEL,
	WT_BRUISE,
	WT_STING,
};

void mod_init(void *arg __attribute__((unused))) {
}

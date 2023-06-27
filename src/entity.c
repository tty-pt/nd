#include "entity.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "spacetime.h"
#include "mcp.h"
#include "map.h"
#include "defaults.h"
#include "params.h"
#include "match.h"
#include "view.h"
#include "room.h"
#include "equipment.h"
#include "spell.h"
#include "debug.h"
#include "mob.h"
#include "utils.h"

#define HUNGER_Y	4
#define THIRST_Y	2
#define HUNGER_INC	(1 << (DAYTICK_Y - HUNGER_Y))
#define THIRST_INC	(1 << (DAYTICK_Y - THIRST_Y))

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

struct wts phys_wts[] = {
	{ "punch", "punches" },
	{ "peck", "pecks at" },
	{ "slash", "slashes" },
	{ "bite", "bites" },
	{ "strike", "strikes" },
};

ENT *
birth(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	eplayer->wts = phys_wts[eplayer->wtso];
	eplayer->hunger = eplayer->thirst = 0;
	eplayer->combo = 0;
	eplayer->hp = HP_MAX(eplayer);
	eplayer->mp = MP_MAX(eplayer);
	eplayer->sat = eplayer->target = NULL;

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);

	int i;

	for (i = 0; i < ES_MAX; i++) {
		register dbref eq = EQUIP(eplayer, i);

		if (eq <= 0)
			continue;

		OBJ *oeq = object_get(eq);
		EQU *eeq = &oeq->sp.equipment;
		CBUG(equip_affect(eplayer, eeq));
	}

        return eplayer;
}

void
recycle(OBJ *player, OBJ *thing)
{
	extern OBJ *recyclable;
	ENT *eplayer = &player->sp.entity;
	static int depth = 0;
	OBJ *first, *rest;
	int looplimit;

	CBUG(!thing->owner);
	OBJ *owner = thing->owner;
	ENT *eowner = &owner->sp.entity;

	depth++;
	switch (thing->type) {
	case TYPE_ROOM:
		{
			ROO *rthing = &thing->sp.room;
			if (!(eowner->flags & EF_WIZARD))
				owner->value += ROOM_COST;
			if (rthing->flags & RF_TEMP)
				for (first = thing->contents; first; first = rest) {
					rest = first->next;

					if (first->type == TYPE_ENTITY) {
						ENT *efirst = &first->sp.entity;
						if (efirst->flags & EF_PLAYER)
							continue;
					}

					recycle(player, first);
				}
			anotifyf(thing, "You feel a wrenching sensation...");
			map_delete(thing);
		}

		break;
	case TYPE_PLANT:
	case TYPE_SEAT:
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
	case TYPE_MINERAL:
		if (!(eowner->flags & EF_WIZARD))
			owner->value += thing->value;

		OBJ *thingloc = thing->location;

		if (thingloc->type != TYPE_ROOM)
			break;

		ROO *rthingloc = &thingloc->sp.room;

		if (rthingloc->flags & RF_TEMP) {
			for (first = thing->contents; first; first = rest) {
				rest = first->next;
				recycle(player, first);
			}
		}

		break;
	}

	FOR_ALL(rest) {
		switch (rest->type) {
		case TYPE_ROOM:
			{
				ROO *rrest = &rest->sp.room;
				if (rrest->dropto == thing)
					rrest->dropto = NULL;
				if (rest->owner == thing)
					rest->owner = object_get(GOD);
			}

			break;
		case TYPE_PLANT:
		case TYPE_SEAT:
		case TYPE_CONSUMABLE:
		case TYPE_EQUIPMENT:
		case TYPE_THING:
		case TYPE_MINERAL:
			if (rest->owner == thing)
				rest->owner = object_get(GOD);
			break;
		case TYPE_ENTITY:
			{
				ENT *erest = &rest->sp.entity;
				if (erest->home == thing)
					erest->home = object_get(PLAYER_START);
			}

			break;
		}
		if (rest->contents == thing)
			rest->contents = thing->next;
		if (rest->next == thing)
			rest->next = thing->next;
	}

	looplimit = db_top;
	while ((looplimit-- > 0) && (first = thing->contents)) {
		if (first->type == TYPE_ENTITY) {
			ENT *efirst = &first->sp.entity;
			if (efirst->flags & EF_PLAYER) {
				enter(first, efirst->home, E_NULL);
				if (first->location == thing) {
					notifyf(eplayer, "Escaping teleport loop!  Going home.");
					object_move(first, object_get(PLAYER_START));
				}
				continue;
			}
		}

		recycle(player, first);
	}


	object_move(thing, NULL);

	depth--;

	object_free(thing);
	object_clear(thing);
	thing->name = (char*) strdup("<garbage>");
	thing->description = (char *) strdup("<recyclable>");
	thing->type = TYPE_GARBAGE;
	thing->next = recyclable;
	recyclable = thing;
}

static inline int
entities_aggro(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *here = player->location;
	OBJ *tmp;
	int klock = 0;

	FOR_LIST(tmp, here->contents) {
		if (tmp->type != TYPE_ENTITY)
			continue;

		ENT *etmp = &tmp->sp.entity;

		if (etmp->flags & EF_AGGRO) {
			etmp->target = player;
			klock++;
		}
	}

	eplayer->klock += klock;
	return klock;
}

#if 0
void
enter_room(OBJ *player, enum exit e)
{
	ENT *eplayer = &player->sp.entity;
	char contents[BIGBUFSIZ];
	OBJ *oi;
	int count = 0;

	if (eplayer->gpt) {
		free(eplayer->gpt);
		eplayer->gpt = NULL;
	}

	sprintf(contents, "This is a fantasy story narrated by an artificial intelligence.\n");
	sprintf(contents, "%sIt takes place in a '%s'",
			contents, player->location->name);
	if (player->location->description && *player->location->description)
		sprintf(contents, "%s that is described as '%s' and ",
			contents, player->location->description);
	else
		sprintf(contents, "%s that ", contents);
	sprintf(contents, "%shas the following contents:\n", contents);

	FOR_LIST(oi, player->location->contents) {
		count++;
		switch (oi->type) {
		case TYPE_ENTITY:
			{
				ENT *ei = &oi->sp.entity;
				if (ei->flags & EF_PLAYER) {
					sprintf(contents, "%sA player named %s", contents, oi->name);
					if (e != E_ALL) {
						sprintf(contents, "%s, who just ", contents);
						if (e == E_NULL)
							sprintf(contents, "%steleported in", contents);
						else
							sprintf(contents, "%scame in from the %s", contents, e_name(e_simm(e)));
					}
				} else
					sprintf(contents, "%sA %s", contents, oi->name);

				if (player->description)
					sprintf(contents, "%s, described as '%s'", contents, oi->name);
				if (eplayer->flags & EF_SITTING)
					sprintf(contents, "%s, who is sitting down", contents);

				sprintf(contents, "%s.\n", contents);
			}
			break;
		case TYPE_PLANT:
			sprintf(contents, "%sA plant or tree named %s.\n", contents, oi->name);
			break;
		default:
			sprintf(contents, "%sA %s.\n", contents, oi->name);
		}
	}

	/* sprintf(contents, "%s\n" */
	/* 		"Narrate this information as if in a fantasy novel.\n" */
	/* 		, contents); */
	sprintf(contents, "%s\n"
			"Describe this.\n"
			, contents);

	entity_gpt(player, 1, contents);
}

enum SPEAK {
	ME = 1,
	OTHER = 2,
	BOTH = 3,
};

/* struct speech { */
/* 	OBJ *player; */
/* 	char *me, *other; */
/* 	size_t me_rem, other_rem; */
/* } */

/* void */
/* speak(struct *speech, int flags, char *fmt_str, ...) { */
/* 	va_list args; */
/* 	va_start(args, format); */
/* 	if (flags & ME) */
/* 		vsnprintf(speech->me, me_rem, format, args); */
/* 	else if (flags & OTHER) */
/* 		vsnprintf(speech->other, other_rem, format, args); */
/* 	va_end(args); */
/* } */

/* void */
/* tell(struct *speech, char *fmt_str, ...) { */
/* 	va_list args; */
/* 	va_start(args, format); */
/* 	if (flags & ME) { */
/* 		char *name = "you"; */
/* 		vsnprintf(speech->me, speech->me_rem, format, name, args); */
/* 	} else if (flags & OTHER) { */
/* 		char *name = speech->player->name; */
/* 		vsnprintf(speech->other, speech->other_rem, format, name, args); */
/* 	} */
/* 	va_end(args); */
/* } */

/* void */
/* verb(struct *speech, char *verb) { */
/* 	// there could be an fd that writes to all players or all players in that location */
/* 	fprintf(fd */
/* 	vsnprintf(speech->me, speech->me_rem, format, name, args); */
/* } */

enum word_type {
	WOT_VERB = 0,
	WOT_NOUN = 1,
	WOT_ADJECTIVE = 2,
	WOT_ADVERB = 3,
	WOT_PRONOUN = 4,
	WOT_PREPOSITION = 5,
	WOT_CONJUNCTION = 6,
	WOT_INTERJECTION = 7,
	WOT_SPECIAL = 8,
};

struct core_word {
	char *str;
	enum word_type type;
	char *antonym;
	/* char *replacer(OBJ *player, char *word); */
};

/* enum verb_tense = { */
/* 	VT_PRESENT = 0; */
/* }; */
	/* union { */
	/* 	enum verb_tense; */
	/* } sp; */

struct core_word core_words[] = {
	/* { */
	/* 	.str = "me", */
	/* 	.type = WOT_SPECIAL, replace_me */
	/* }, */
	{
		.str = "teleport",
		.type = WOT_VERB,
	},
	{
		.str = "go",
		.antonym = "come",
		.type = WOT_VERB
	},
	{
		.str = "come",
		.antonym = "go",
		.type = WOT_VERB
	},
	{
		.str = "out",
		.antonym = "in",
		.type = WOT_PREPOSITION
	},
	{
		.str = "in",
		.antonym = "out",
		.type = WOT_PREPOSITION
	},
	{
		.str = "from",
		.antonym = "into",
		.type = WOT_PREPOSITION
	},
	{
		.str = "into",
		.antonym = "from",
		.type = WOT_PREPOSITION
	},
	{
		.str = "the",
		.type = WOT_PREPOSITION
	},
};

char *third[] = { "ss", "sh", "ch", " x", " z" };

DB *words_db = NULL;

static inline struct core_word *word_get(char *word) {
	return hash_get(words_db, word);
}

// keep a notion of who the players are in the client
static inline word_process(OBJ *player, int flags, char *word) {
	ENT *eplayer = &player->sp.entity;
	struct core_word *core_word = word_get(word);
	size_t len = strlen(word);

	switch (core_word->type) {
		case (WOT_VERB) {
			if (flags & ME) {
				const ch = eplayer->proc->final ? 'Y' : 'y';
				dprintf(eplayer->proc->fd, "%cou %s", ch, word);
			} else if (flags & OTHER) {
				char last[3] = { len > 1 ? word[len - 2] : ' ', word[len - 1], '\0' };
				dprintf(player->location->proc->fd, "%s %s%ss", player->name, word, hash_get(third_db, last) ? "e" : "");
			}
		}
	}
}

static inline text_process(OBJ *player, int flags, char *text) {
	struct speech *speech = flags & ME ? player->speech : player->location->speech;
	char word[32], *w = word;

	do {
		switch (*text) {
			case '\0': break;
			case ' ':
				*text = '\0';
				word_process(player, flags, word);
				w = word;
				text++;
				continue;
			case '.':
			case ';':
				player->speech->final = 1;
				continue;
			default:
				player->speech->final = 0;
				*w++ = *text++;
		}
		break;
	} while (true);
}

void proc(OBJ *player, char *fmt, ...) {
	ENT *eplayer = &player->sp.entity;
	va_list args;
	/* if (flags & ME) { */
		/* char *name = "you"; */
		va_start(args, format);
		dprintf(eplayer->proc->fd, format, args);
		va_end(args);
	/* } else if (flags & OTHER) { */
		/* char *other_name = speech->player->name; */
		va_start(args, format);
		dprintf(player->location->proc->fd, format, args);
		va_end(args);
	/* } */
	va_end(args);
}

void
enter(OBJ *player, OBJ *loc, enum exit e)
{
	OBJ *old = player->location;

	/* if (eplayer->gpt) { */
	/* 	free(eplayer->gpt); */
	/* 	eplayer->gpt = NULL; */
	/* } */

	if (e == E_NULL)
		proc(player, "%d teleport out", object_ref(player));
	else
		proc(player, "%d go %s", object_ref(player), e_name(e));

	object_move(player, loc);
	room_clean(player, old);

	if (e == E_NULL)
		proc(player, "%d teleport in", object_ref(player)); // invalidated by the above
	else
		proc(player, "%d come in from the %s", object_ref(player), e_name(e)); // invalidated by the above

	proc(player, "into %s.%s", player->location->name, player->location->description);
	flush(player);

	entities_aggro(player);
	look_around(player);
}

#endif

void
enter(OBJ *player, OBJ *loc, enum exit e)
{
	OBJ *old = player->location;

	if (e == E_NULL)
		onotifyf(player, "%s teleports out.", player->name);
	object_move(player, loc);
	room_clean(player, old);
	if (e == E_NULL)
		onotifyf(player, "%s teleports in.", player->name);
}

int
payfor(OBJ *who, int cost)
{
	ENT *ewho = &who->sp.entity;

	if (ewho->flags & EF_WIZARD)
		return 1;

	if (who->value >= cost) {
		who->value -= cost;
		return 1;
	} else {
		return 0;
	}
}

int
controls(OBJ *who, OBJ *what)
{
	if (!what)
		return 0;

	if (what->type == TYPE_GARBAGE)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	if (who->type != TYPE_ENTITY)
		who = who->owner;

	ENT *ewho = &who->sp.entity;

	/* Wizard controls everything */
	if (ewho->flags & EF_WIZARD) {
		if(God(what->owner) && !God(who))
			/* Only God controls God's objects */
			return 0;
		else
			return 1;
	}

	/* owners control their own stuff */
	return (who == what->owner);
}

static inline int
controls_link(OBJ *who, OBJ *what)
{
	switch (what->type) {
	case TYPE_ROOM:
		if (controls(who, what->sp.room.dropto))
			return 1;
		return 0;

	case TYPE_ENTITY:
		if (controls(who, what->sp.entity.home))
			return 1;
		return 0;

	default:
		return 0;
	}
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l, __VA_ARGS__)

const char *
unparse(OBJ *player, OBJ *loc)
{
	static char buf[BUFSIZ];
        size_t buf_l = 0;

	if (player && player->type != TYPE_ENTITY)
		player = player->owner;

	if (!loc)
		return "*NOTHING*";

	if (loc->type == TYPE_EQUIPMENT) {
		EQU *eloc = &loc->sp.equipment;

		if (eloc->eqw) {
			unsigned n = eloc->rare;

			if (n != 1)
				BUFF("(%s) ", rarity_str[n]);
		}
	}

	BUFF("%s(#%d)", loc->name, object_ref(loc));

	buf[buf_l] = '\0';
	return buf;
}

void
sit(OBJ *player, const char *name)
{
	ENT *eplayer = &player->sp.entity;

	if (eplayer->flags & EF_SITTING) {
		notify(eplayer, "You are already sitting.");
		return;
	}

	if (!*name) {
		notify_wts(player, "sit", "sits", " on the ground");
		eplayer->flags |= EF_SITTING;
		eplayer->sat = NULL;

		/* warn("%d sits on the ground\n", player); */
		return;
	}

	OBJ *seat = ematch_near(player, name);

	if (!seat || seat->type != TYPE_SEAT) {
		notify(eplayer, "You can't sit on that.");
		return;
	}

	SEA *sseat = &seat->sp.seat;

	if (sseat->quantity >= sseat->capacity) {
		notify(eplayer, "No seats available.");
		return;
	}

	sseat->quantity += 1;
	eplayer->flags |= EF_SITTING;
	eplayer->sat = seat;

	notify_wts(player, "sit", "sits", " on %s", seat->name);
}

int
stand_silent(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;

	if (!(eplayer->flags & EF_SITTING))
		return 1;

	OBJ *chair = eplayer->sat;

	if (chair) {
		CBUG(chair->type != TYPE_SEAT);
		SEA *schair = &chair->sp.seat;
		schair->quantity--;
		eplayer->sat = NULL;
	}

	eplayer->flags ^= EF_SITTING;
	notify_wts(player, "stand", "stands", " up");
	return 0;
}

void
stand(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
	if (stand_silent(player))
		notify(eplayer, "You are already standing.");
}

int
cando(OBJ *player, OBJ *thing, const char *default_fail_msg)
{
	ENT *eplayer = &player->sp.entity;

	if (!thing) {
		notify(eplayer, default_fail_msg);
		return 0;
	}

	CBUG(player->type != TYPE_ENTITY);

	if (eplayer->klock) {
		notify(eplayer, "Not while being targeted.");
		return 0;
	}

	stand_silent(player);

	return 1;
}

static void
look_contents(OBJ *player, OBJ *loc, const char *contents_name)
{
	ENT *eplayer = &player->sp.entity;
        char buf[BUFSIZ];
        size_t buf_l = 0;
	OBJ *thing;

	/* check to see if there is anything there */
	if (loc->contents) {
                FOR_LIST(thing, loc->contents) {
			if (thing == player)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"\n%s", unparse(player, thing));
                }
	}

        buf[buf_l] = '\0';

        notifyf(eplayer, "%s%s", contents_name, buf);
}

void
look_room(OBJ *player, OBJ *loc)
{
	ENT *eplayer = &player->sp.entity;
	char const *description = "";
	CBUG(loc->type != TYPE_ROOM);

	if (mcp_look(player, loc)) {
		notify(eplayer, unparse(player, loc));
		if (*description && description)
			notify(eplayer, description);
		look_contents(player, loc, "You see:");
	}
}

void
look_around(OBJ *player)
{
	look_room(player, player->location);
}

int
equip_affect(ENT *ewho, EQU *equ)
{
	register int msv = equ->msv,
		 eqw = equ->eqw,
		 eql = EQL(eqw),
		 eqt = EQT(eqw);

	unsigned aux = 0;

	switch (eql) {
	case ES_RHAND:
		if (ewho->attr[ATTR_STR] < msv)
			return 1;
		EFFECT(ewho, DMG).value += DMG_WEAPON(equ);
		ewho->wts = WTS_WEAPON(equ);
		break;

	case ES_HEAD:
	case ES_PANTS:
		aux = 1;
	case ES_CHEST:

		switch (eqt) {
		case ARMOR_LIGHT:
			if (ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 2;
			break;
		case ARMOR_MEDIUM:
			msv /= 2;
			if (ewho->attr[ATTR_STR] < msv
			    || ewho->attr[ATTR_DEX] < msv)
				return 1;
			aux += 1;
			break;
		case ARMOR_HEAVY:
			if (ewho->attr[ATTR_STR] < msv)
				return 1;
		}
		aux = DEF_ARMOR(equ, aux);
		EFFECT(ewho, DEF).value += aux;
		int pd = EFFECT(ewho, DODGE).value - DODGE_ARMOR(aux);
		EFFECT(ewho, DODGE).value = pd > 0 ? pd : 0;
	}

	return 0;
}

int
equip(OBJ *who, OBJ *eq)
{
	CBUG(who->type != TYPE_ENTITY);
	ENT *ewho = &who->sp.entity;
	CBUG(eq->type != TYPE_EQUIPMENT);
	EQU *eeq = &eq->sp.equipment;
	unsigned eql = EQL(eeq->eqw);

	if (!eql || EQUIP(ewho, eql) > 0
	    || equip_affect(ewho, eeq))
		return 1;

	EQUIP(ewho, eql) = object_ref(eq);
	eeq->flags |= EQF_EQUIPPED;

	notifyf(ewho, "You equip %s.", eq->name);
	mcp_stats(who);
	mcp_content_out(who, eq);
	mcp_equipment(who);
	return 0;
}

dbref
unequip(OBJ *player, unsigned eql)
{
	ENT *eplayer = &player->sp.entity;
	dbref eq = EQUIP(eplayer, eql);
	unsigned eqt, aux;

	if (!eq)
		return NOTHING;

	OBJ *oeq = object_get(eq);
	EQU *eeq = &oeq->sp.equipment;
	eqt = EQT(eeq->eqw);
	aux = 0;

	switch (eql) {
	case ES_RHAND:
		EFFECT(eplayer, DMG).value -= DMG_WEAPON(eeq);
		eplayer->wts = phys_wts[eplayer->wtso];
		break;
	case ES_PANTS:
	case ES_HEAD:
		aux = 1;
	case ES_CHEST:
		switch (eqt) {
		case ARMOR_LIGHT: aux += 2; break;
		case ARMOR_MEDIUM: aux += 1; break;
		}
		aux = DEF_ARMOR(eeq, aux);
		EFFECT(eplayer, DEF).value -= aux;
		EFFECT(eplayer, DODGE).value += DODGE_ARMOR(aux);
	}

	EQUIP(eplayer, eql) = NOTHING;
	eeq->flags &= ~EQF_EQUIPPED;
	mcp_content_in(player, oeq);
	mcp_stats(player);
	mcp_equipment(player);
	return eq;
}

static inline unsigned
entity_xp(ENT *eplayer, ENT *etarget)
{
	// alternatively (2000/x)*y/x
	if (!eplayer->lvl)
		return 0;

	unsigned r = 254 * etarget->lvl / (eplayer->lvl * eplayer->lvl);
	if (r < 0)
		return 0;
	else
		return r;
}

static inline void
_entity_award(ENT *eplayer, unsigned const xp)
{
	unsigned cxp = eplayer->cxp;
	notifyf(eplayer, "You gain %u xp!", xp);
	cxp += xp;
	while (cxp >= 1000) {
		notify(eplayer, "You level up!");
		cxp -= 1000;
		eplayer->lvl += 1;
		eplayer->spend += 2;
	}
	eplayer->cxp = cxp;
}

static inline void
entity_award(ENT *eplayer, ENT *etarget)
{
	_entity_award(eplayer, entity_xp(eplayer, etarget));
}

static inline OBJ *
entity_body(OBJ *player, OBJ *mob)
{
	char buf[32];
	struct object_skeleton o = { .name = "", .description = "" };
	snprintf(buf, sizeof(buf), "%s's body.", mob->name);
	o.name = buf;
	OBJ *dead_mob = object_add(&o, mob->location, NULL);
	OBJ *tmp;
	unsigned n = 0;

	for (; (tmp = mob->contents); ) {
		/* CBUG(tmp->type != TYPE_GARBAGE); */
		if (tmp->type == TYPE_GARBAGE)
			continue;
		if (tmp->type == TYPE_EQUIPMENT) {
			EQU *etmp = &tmp->sp.equipment;
			unequip(mob, EQL(etmp->eqw));
		}
		object_move(tmp, dead_mob);
		n++;
	}

	if (n > 0) {
		onotifyf(mob, "%s's body drops to the ground.", mob->name);
		return dead_mob;
	} else {
		recycle(player, dead_mob);
		return NULL;
	}
}

static inline OBJ *
entity_kill(OBJ *player, OBJ *target)
{
	ENT *eplayer = player ? &player->sp.entity : NULL;
	ENT *etarget = &target->sp.entity;

	notify_wts(target, "die", "dies", "");

	entity_body(player, target);

	if (player) {
		CBUG(player->type != TYPE_ENTITY)
		entity_award(eplayer, etarget);
		eplayer->target = NULL;
	}

	CBUG(target->type != TYPE_ENTITY);

	if (etarget->target && (etarget->flags & EF_AGGRO)) {
		OBJ *tartar = etarget->target;
		ENT *etartar = &tartar->sp.entity;
		etartar->klock --;
	}

	OBJ *loc = target->location;
	ROO *rloc = &loc->sp.room;

	if ((rloc->flags & RF_TEMP) && !(etarget->flags & EF_PLAYER)) {
		recycle(player, target);
		room_clean(target, loc);
		return NULL;
	}

	etarget->klock = 0;
	etarget->target = NULL;
	etarget->hp = 1;
	etarget->mp = 1;
	etarget->thirst = etarget->hunger = 0;

	debufs_end(etarget);
	object_move(target, object_get((dbref) 0));
	room_clean(target, loc);
	look_around(target);
	mcp_bars(etarget);

	return target;
}

int
entity_damage(OBJ *player, OBJ *target, short amt)
{
	ENT *etarget = &target->sp.entity;
	short hp = etarget->hp;
	int ret = 0;
	hp += amt;

	if (!amt)
		return ret;

	if (hp <= 0) {
		hp = 1;
		ret = 1;
		target = entity_kill(player, target);
	} else {
		register unsigned short max = HP_MAX(etarget);
		if (hp > max)
			hp = max;
	}

	if (target) {
		etarget->hp = hp;
		CBUG(target->type != TYPE_ENTITY);
		mcp_bars(etarget);
	}

	if (player && (player->type != TYPE_GARBAGE)) {
		CBUG(player->type != TYPE_ENTITY);
		ENT *eplayer = &player->sp.entity;
		mcp_bars(eplayer);
	}

	return ret;
}

void
art(int fd, OBJ *thing)
{
	char art[BUFSIZ];
	size_t rem = sizeof(art);

	rem -= snprintf(art, rem, "../art/");

	switch (thing->type) {
	case TYPE_ROOM:
		rem -= snprintf(art, rem, "biome/%s/1.jpeg", thing->name);
		break;
	case TYPE_PLANT:
		rem -= snprintf(art, rem, "plant/%s/1.jpeg", thing->name);
		break;
	case TYPE_ENTITY:
		rem -= snprintf(art, rem, "entity/%s/1.jpeg", thing->name);
		break;
	default:
		rem -= snprintf(art, rem, "unknown.jpeg");
		break;

	}

	mcp_art(fd, art);
}

static void
look_simple(ENT *eplayer, OBJ *thing)
{
	art(eplayer->fd, thing);

	if (thing->description)
		notify(eplayer, thing->description);
}

void
do_look_at(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *name = cmd->argv[1];
	OBJ *thing;

	if (*name == '\0') {
		thing = player->location;
	} else if (
			!(thing = ematch_absolute(name))
			&& !(thing = ematch_here(player, name))
			&& !(thing = ematch_me(player, name))
			&& !(thing = ematch_near(player, name))
			&& !(thing = ematch_mine(player, name))
		  )
	{
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	switch (thing->type) {
	case TYPE_ROOM:
		look_room(player, thing);
		view(player);
		break;
	case TYPE_ENTITY:
		if (mcp_look(player, thing)) {
			look_simple(eplayer, thing);
			look_contents(player, thing, "Carrying:");
		}
		break;
	case TYPE_CONSUMABLE:
	case TYPE_EQUIPMENT:
	case TYPE_THING:
		if (mcp_look(player, thing))
			look_simple(eplayer, thing);
		break;
	default:
		if (mcp_look(player, thing))
			look_simple(eplayer, thing);
		break;
	}
}

static void
respawn(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *where;

	onotifyf(player, "%s disappears.", player->name);

	if (eplayer->flags & EF_PLAYER) {
		struct cmd_dir cd;
		cd.rep = STARTING_POSITION;
		cd.dir = '\0';
		st_teleport(player, cd);
		where = player->location;
	} else {
		where = eplayer->home;
		/* warn("respawning %d to %d\n", who, where); */
		object_move(player, where);
	}

	onotifyf(player, "%s appears.", player->name);
}

static inline int
huth_notify(OBJ *player, unsigned v, unsigned char y, char const *m[4])
{
	ENT *eplayer = &player->sp.entity;

	static unsigned const n[] = {
		1 << (DAY_Y - 1),
		1 << (DAY_Y - 2),
		1 << (DAY_Y - 3)
	};

	register unsigned aux;

	if (v == n[2])
		notify(eplayer, m[0]);
	else if (v == (aux = n[1]))
		notify(eplayer, m[1]);
	else if (v == (aux += n[0]))
		notify(eplayer, m[2]);
	else if (v == (aux += n[2]))
		notify(eplayer, m[3]);
	else if (v > aux) {
		short val = -(HP_MAX(eplayer) >> 3);
		return entity_damage(NULL, player, val);
	}

        return 0;
}

static inline unsigned char
d20()
{
	return (random() % 20) + 1;
}

static unsigned char
entity_ac(ENT *eplayer)
{
	return 10 + MODIFIER(eplayer, ATTR_DEX);
}

// returns 1 if target dodges
static inline int
dodge_get(ENT *eplayer)
{
	return d20() < entity_ac(eplayer);
}

int
kill_dodge(OBJ *player, struct wts wts)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *target = eplayer->target;
	ENT *etarget = &target->sp.entity;
	if (!EFFECT(etarget, MOV).value && dodge_get(eplayer)) {
		notify_wts_to(target, player, "dodge", "dodges", "'s %s", wts.a);
		return 1;
	} else
		return 0;
}

static inline short
randd_dmg(short dmg)
{
	register unsigned short xx = 1 + (random() & 7);
	return xx = dmg + ((dmg * xx * xx * xx) >> 9);
}

short
kill_dmg(enum element dmg_type, short dmg,
	short def, enum element def_type)
{
	if (dmg > 0) {
		if (dmg_type == ELEMENT(def_type)->weakness)
			dmg *= 2;
		else if (ELEMENT(dmg_type)->weakness == def_type)
			dmg /= 2;

		if (dmg < def)
			return 0;

	} else
		// heal TODO make type matter
		def = 0;

	return randd_dmg(dmg - def);
}

static inline void
attack(OBJ *player)
{
	ENT *eplayer = &player->sp.entity,
	    *etarget;

	register unsigned char mask;

	mask = EFFECT(eplayer, MOV).mask;

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(player, &eplayer->debufs[i], 0);
		return;
	}

	OBJ *target = eplayer->target;

	if (!target)
		return;

	etarget = &target->sp.entity;

	if (!spells_cast(player, target) && !kill_dodge(player, eplayer->wts)) {
		enum element at = ELEMENT_NEXT(eplayer, MDMG);
		enum element dt = ELEMENT_NEXT(etarget, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(eplayer, DMG).value, EFFECT(etarget, DEF).value + EFFECT(etarget, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(eplayer, MDMG).value, EFFECT(etarget, MDEF).value, dt);
		char const *color = ELEMENT(at)->color;
		notify_attack(player, target, eplayer->wts, aval, color, bval);
		entity_damage(player, target, aval + bval);
	}
}

static inline void
kill_update(OBJ *player)
{
	ENT *eplayer = &player->sp.entity;
	OBJ *target = eplayer->target;

	if (!target
            || target->type == TYPE_GARBAGE
	    || target->location != player->location) {
		eplayer->target = NULL;
		return;
	}

	ENT *etarget = &target->sp.entity;

	if (!etarget->target)
		etarget->target = player;

	stand_silent(player);
	attack(player);
}

void
entity_update(OBJ *player)
{
	CBUG(player->type != TYPE_ENTITY);
	ENT *eplayer = &player->sp.entity;

	static char const *thirst_msg[] = {
		"You are thirsty.",
		"You are very thirsty.",
		"you are dehydrated.",
		"You are dying of thirst."
	};

	static char const *hunger_msg[] = {
		"You are hungry.",
		"You are very hungry.",
		"You are starving.",
		"You are starving to death."
	};

	if (!(eplayer->flags & EF_PLAYER)) {
		/* warn("%d hp %d/%d\n", player, eplayer->hp, HP_MAX(player)); */
		if (eplayer->hp == HP_MAX(eplayer)) {
			/* warn("%d's hp is maximum\n", player); */

			if ((eplayer->flags & EF_SITTING)) {
				/* warn("%d is sitting so they shall stand\n", player); */
				stand(player);
			}

			if (player->location == 0) {
				/* warn("%d is located at 0, so they shall respawn\n", player); */
				respawn(player);
			}
		} else {
			/* warn("%d's hp is not maximum\n", player); */
			if (!eplayer->target && !(eplayer->flags & EF_SITTING)) {
				/* warn("%d is not sitting so they shall sit\n", player); */
				sit(player, "");
			}
		}
	}

	if (get_tick() % 16 == 0 && (eplayer->flags & EF_SITTING)) {
		int div = 10;
		int max, cur;
		entity_damage(NULL, player, HP_MAX(eplayer) / div);

		max = MP_MAX(eplayer);
		cur = eplayer->mp + (max / div);
		eplayer->mp = cur > max ? max : cur;

	}

        CBUG(player->type == TYPE_GARBAGE);

	if (player->location == 0)
		return;

        /* if mob dies, return */
	if (huth_notify(player, eplayer->thirst += THIRST_INC, THIRST_Y, thirst_msg)
		|| huth_notify(player, eplayer->hunger += HUNGER_INC, HUNGER_Y, hunger_msg)
                || debufs_process(player))
                        return;

	kill_update(player);
}

int
entity_boot(ENT *eplayer)
{
	int fd = eplayer->fd;

	if (fd > 0) {
		descr_close(DESCR(fd));
		return 1;
	}

	return 0;
}

void
stats_init(ENT *enu, SENT *sk)
{
	unsigned char stat = sk->stat;
	int lvl = sk->lvl, spend, i, sp,
	    v = sk->lvl_v ? sk->lvl_v : 0xf;

	lvl += random() & v;

	if (!stat)
		stat = 0x1f;

	spend = 1 + lvl;
	for (i = 0; i < ATTR_MAX; i++)
		if (stat & (1<<i)) {
			sp = random() % spend;
			enu->attr[i] = sp;
		}

	enu->lvl = lvl;
}

static inline int
bird_is(SENT *sk)
{
	return sk->wt == WT_PECK;
}

static inline OBJ *
entity_add(enum mob_type mid, OBJ *where, enum biome biome, long long pdn) {
	struct object_skeleton *obj_skel = ENTITY_SKELETON(mid);
	CBUG(obj_skel->type != S_TYPE_ENTITY);
	struct entity_skeleton *mob_skel = &obj_skel->sp.entity;

	if ((bird_is(mob_skel) && !pdn)
	    || (!NIGHT_IS && (mob_skel->type == ELM_DARK || mob_skel->type == ELM_VAMP))
	    || random() >= (RAND_MAX >> mob_skel->y))
		return NULL;

	if (!((1 << biome) & mob_skel->biomes))
		return NULL;

	return object_add(obj_skel, where, NULL);
}

void
entities_add(OBJ *where, enum biome biome, long long pdn) {
	unsigned mid;

	for (mid = 1; mid < MOB_MAX; mid++)
		entity_add(mid, where, biome, pdn);
}

void
do_reroll(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *what = cmd->argv[1];
	OBJ *thing;
	int i;

	if (
			!(thing = ematch_me(player, what))
			&& !(thing = ematch_near(player, what))
			&& !(thing = ematch_mine(player, what))
	   ) {
		notify(eplayer, NOMATCH_MESSAGE);
		return;
	}

	if (player != thing && !(eplayer->flags & EF_WIZARD)) {
		notify(eplayer, "You can not do that.");
		return;
	}

	for (i = 0; i < ATTR_MAX; i++) {
		eplayer->attr[i] = d20();
	}

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);
	mcp_stats(player);
}

/* void */
/* entity_gpt(OBJ *player, int echo_off, char *add_prompt) */
/* { */
/* 	ENT *eplayer = &player->sp.entity; */
/* 	int len = strlen(add_prompt); */
/* 	char *has_previous = eplayer->gpt; */
/* 	char *pgpt = has_previous ? eplayer->gpt : ""; */
/* 	int glen = strlen(pgpt); */
/* 	/1* char *prompt = malloc(glen + len + 3); *1/ */
/* 	char *prompt = malloc(glen + len + 1); */
/* 	strcpy(prompt, pgpt); */
/* 	strcat(prompt, add_prompt); */
/* 	/1* strcat(prompt, "\n\n"); *1/ */
/* 	warn("GPT prompt:\n%s\n", prompt); */
/* 	char *result = gpt(prompt); */
/* 	warn("GPT result:\n%s\n", result); */
/* 	int rlen = strlen(result); */
/* 	/1* eplayer->gpt = realloc(has_previous, glen + (echo_off ? 0 : len) + rlen + 3); *1/ */
/* 	eplayer->gpt = realloc(has_previous, glen + len + rlen + 1); */
/* 	if (!has_previous) */
/* 		*eplayer->gpt = '\0'; */
/* 	/1* strcpy(eplayer->gpt, pgpt); *1/ */
/* 	strcat(eplayer->gpt, add_prompt); */
/* 	strcat(eplayer->gpt, result); */
/* 	warn("Updated GPT:\n%s\n", eplayer->gpt); */
/* 	/1* if (has_previous && has_previous != eplayer->gpt) *1/ */
/* 	/1* 	free((void *) has_previous); *1/ */
/* 	notifyf(eplayer, "%s", eplayer->gpt + glen + (echo_off ? len : 0)); */
/* } */

/* void */
/* entity_gptcat(OBJ *player, char *str) */
/* { */
/* 	ENT *eplayer = &player->sp.entity; */
/* 	char *has_previous = eplayer->gpt; */
/* 	char *pgpt = has_previous ? eplayer->gpt : ""; */
/* 	int glen = strlen(pgpt); */
/* 	int len = strlen(str); */
/* 	eplayer->gpt = realloc(has_previous, glen + len + 1); */
/* 	strcpy(eplayer->gpt, pgpt); */
/* 	strcat(eplayer->gpt, str); */
/* } */

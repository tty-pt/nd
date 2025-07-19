#include "uapi/entity.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <qdb.h>

#include "config.h"
#include "mcp.h"
#include "params.h"
#include "st.h"
#include "uapi/io.h"
#include "uapi/match.h"
#include "uapi/wts.h"
#include "view.h"

#include "papi/nd.h"

#define MODIFIER(ent, a) ((ent->attr[a] - 10) >> 1) // / 2
#define EQT(x)		(x>>6)
#define EQL(x)		(x & 15)
#define WTS_WEAPON(equ) phys_wts[EQT(equ->eqw)]

unsigned huth_y[2] = {
	DAYTICK_Y + 2,
	DAYTICK_Y + 3
};

unsigned ent_hd = -1;
unsigned me = -1;

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

unsigned me_get(void) {
	return me;
}

ENT ent_get(unsigned ref) {
	ENT ent;
	qdb_get(ent_hd, &ent, &ref);
	return ent;
}

void ent_set(unsigned ref, ENT *tmp) {
	qdb_put(ent_hd, &ref, tmp);
}

void ent_del(unsigned ref) {
	qdb_del(ent_hd, &ref, NULL);
}

void ent_reset(ENT *ent) {
	ent->last_observed = ent->target = NOTHING;
	ent->klock = ent->select = 0;
	ent->huth_n[HUTH_THIRST] = ent->huth_n[HUTH_HUNGER] = 0;
	ent->huth[HUTH_THIRST] = ent->huth[HUTH_HUNGER] = 0;
}

void spells_birth(ENT *entity);

void birth(ENT *eplayer)
{
	eplayer->huth[HUTH_THIRST] = eplayer->huth[HUTH_HUNGER] = eplayer->select =
		eplayer->huth_n[HUTH_THIRST] = eplayer->huth_n[HUTH_HUNGER] = 0;
	eplayer->combo = 0;
	eplayer->hp = HP_MAX(eplayer);
	eplayer->mp = MP_MAX(eplayer);
	eplayer->target = NOTHING;
	eplayer->sat = NOTHING;

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);

	int i;

	for (i = 0; i < ES_MAX; i++) {
		unsigned eq = EQUIP(eplayer, i);

		if (eq <= 0)
			continue;

		OBJ oeq;
		qdb_get(obj_hd, &oeq, &eq);
		EQU *eeq = &oeq.sp.equipment;
		equip_affect(eplayer, eeq);
	}

	spells_birth(eplayer);
}

static inline int
entities_aggro(unsigned player_ref)
{
	OBJ player;
	ENT eplayer = ent_get(player_ref);
	unsigned tmp_ref;
	int klock = 0;

	qdb_get(obj_hd, &player, &player_ref);
	
	qdb_cur_t c = qdb_iter(contents_hd, &player.location);
	while (qdb_next(&player.location, &tmp_ref, &c)) {
		OBJ tmp;
		qdb_get(obj_hd, &tmp, &tmp_ref);

		if (tmp.type != TYPE_ENTITY)
			continue;

		ENT etmp = ent_get(tmp_ref);

		if (etmp.flags & EF_AGGRO) {
			etmp.target = player_ref;
			klock++;
		}

		ent_set(tmp_ref, &etmp);
	}

	eplayer.klock += klock;
	ent_set(player_ref, &eplayer);
	return klock;
}

void
enter(unsigned player_ref, unsigned loc_ref, enum exit e)
{
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	unsigned old_loc_ref = player.location;

	ENT eplayer = ent_get(player_ref);

	if (eplayer.target != NOTHING) {
		nd_writef(player_ref, "%s stops fighting.\n", player.name);
		eplayer.target = NOTHING;
		ent_set(player_ref, &eplayer);
	}

	SIC_CALL(NULL, sic_leave, player_ref, old_loc_ref);
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports out.\n", player.name);
	else {
		nd_writef(player_ref, "You go %s%s%s.\n", ANSI_FG_BLUE ANSI_BOLD, e_name(e), ANSI_RESET);
		nd_owritef(player_ref, "%s goes %s.\n", player.name, e_name(e));
	}
	object_move(player_ref, loc_ref);
	room_clean(old_loc_ref);
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports in.\n", player.name);
	else
		nd_owritef(player_ref, "%s comes in from the %s.\n", player.name, e_name(e_simm(e)));
	SIC_CALL(NULL, sic_enter, player_ref, loc_ref);
	entities_aggro(player_ref);
}

int
payfor(unsigned who_ref, OBJ *who, unsigned cost)
{
	if (ent_get(who_ref).flags & EF_WIZARD)
		return 1;

	if (who->value >= cost) {
		who->value -= cost;
		return 1;
	} else {
		return 0;
	}
}

int
controls(unsigned who_ref, unsigned what_ref)
{
	if (what_ref == NOTHING)
		return 0;

	/* Zombies and puppets use the permissions of their owner */
	OBJ who, what;

	qdb_get(obj_hd, &who, &who_ref);
	if (who.type != TYPE_ENTITY)
		who_ref = who.owner;

	qdb_get(obj_hd, &what, &what_ref);

	/* Wizard controls everything */
	if (ent_get(who_ref).flags & EF_WIZARD) {
		if(what.owner == ROOT && who_ref == ROOT)
			return 0;
		else
			return 1;
	}

	/* owners control their own stuff */
	return (who_ref == what.owner);
}

#define BUFF(...) buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l, __VA_ARGS__)

const char *
unparse(unsigned loc_ref)
{
	static char buf[BUFSIZ];
        size_t buf_l = 0;

	if (loc_ref == NOTHING)
		return "*NOTHING*";

	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);

	if (loc.type == TYPE_EQUIPMENT) {
		EQU *eloc = &loc.sp.equipment;

		if (eloc->eqw) {
			unsigned n = eloc->rare;

			if (n != 1)
				BUFF("(%s) ", rarity_str[n]);
		}
	}

	BUFF("%s(#%u)", loc.name, loc_ref);

	buf[buf_l] = '\0';
	return buf;
}

void
sit(unsigned player_ref, ENT *eplayer, char *name)
{
	if (eplayer->flags & EF_SITTING) {
		nd_writef(player_ref, "You are already sitting.\n");
		return;
	}

	if (!*name) {
		notify_wts(player_ref, "sit", "sits", " on the ground");
		eplayer->flags |= EF_SITTING;
		eplayer->sat = NOTHING;
		return;
	}

	unsigned seat_ref = ematch_near(player_ref, name);
	if (seat_ref == NOTHING) {
		nd_writef(player_ref, "Invalid target.\n");
		return;
	}

	OBJ seat;
	qdb_get(obj_hd, &seat, &seat_ref);

	if (seat.type != TYPE_SEAT) {
		nd_writef(player_ref, "You can't sit on that.\n");
		return;
	}

	SEA *sseat = &seat.sp.seat;

	if (sseat->quantity >= sseat->capacity) {
		nd_writef(player_ref, "No seats available.\n");
		return;
	}

	sseat->quantity += 1;
	eplayer->flags |= EF_SITTING;
	eplayer->sat = seat_ref;
	notify_wts(player_ref, "sit", "sits", " on %s", seat.name);
}

int
stand_silent(unsigned player_ref, ENT *eplayer)
{
	if (!(eplayer->flags & EF_SITTING))
		return 1;

	if (eplayer->sat != NOTHING) {
		OBJ chair;
		qdb_get(obj_hd, &chair, &eplayer->sat);
		SEA *schair = &chair.sp.seat;
		schair->quantity--;
		qdb_put(obj_hd, &eplayer->sat, &chair);
		eplayer->sat = NOTHING;
	}

	eplayer->flags ^= EF_SITTING;
	notify_wts(player_ref, "stand", "stands", " up");
	return 0;
}

void
stand(unsigned player_ref, ENT *eplayer) {
	if (stand_silent(player_ref, eplayer))
		nd_writef(player_ref, "You are already standing.\n");
}

void
look_at(unsigned player_ref, unsigned loc_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ loc;
	qdb_get(obj_hd, &loc, &loc_ref);
	unsigned thing_ref;

	fbcp_item(player_ref, loc_ref, 1);
	switch (loc.type) {
	case TYPE_ROOM:
		break;
	case TYPE_ENTITY: // falls through
	default:
		eplayer.last_observed = loc_ref;
		ent_set(player_ref, &eplayer);
		qdb_put(obs_hd, &loc_ref, &player_ref);
	}

        if (loc_ref != player_ref && loc.type == TYPE_ENTITY && !(eplayer.flags & EF_WIZARD))
                return;

	// use callbacks for mcp like this versus telnet
	qdb_cur_t c = qdb_iter(contents_hd, &loc_ref);
	while (qdb_next(&loc_ref, &thing_ref, &c))
		fbcp_item(player_ref, thing_ref, 0);

	nd_twritef(player_ref, "%s\n", unparse(loc_ref));

        char buf[BUFSIZ];
        size_t buf_l = 0;

	qdb_cur_t c2 = qdb_iter(contents_hd, &loc_ref);
	while (qdb_next(&loc_ref, &thing_ref, &c2)) {
	/* check to see if there is anything there */
			if (thing_ref == player_ref)
				continue;
			buf_l += snprintf(&buf[buf_l], BUFSIZ - buf_l,
					"%s\r\n", unparse(thing_ref));
	}

        buf[buf_l] = '\0';
        nd_twritef(player_ref, "Contents: %s", buf);
}

void
look_around(unsigned player_ref)
{
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	look_at(player_ref, player.location);
}

int
equip_affect(ENT *ewho, EQU *equ)
{
	register unsigned msv = equ->msv,
		 eqw = equ->eqw,
		 eql = EQL(eqw),
		 eqt = EQT(eqw);

	unsigned aux = 0;

	switch (eql) {
	case ES_RHAND:
		if (ewho->attr[ATTR_STR] < msv)
			return 1;
		EFFECT(ewho, DMG).value += DMG_WEAPON(equ);
		/* ewho->wts = WTS_WEAPON(equ); */
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
equip(unsigned who_ref, unsigned eq_ref)
{
	OBJ eq;
	qdb_get(obj_hd, &eq, &eq_ref);
	ENT ewho = ent_get(who_ref);
	EQU *eeq = &eq.sp.equipment;
	unsigned eql = EQL(eeq->eqw);

	if (!eql || EQUIP(&ewho, eql) > 0
	    || equip_affect(&ewho, eeq))
		return 1;

	EQUIP(&ewho, eql) = eq_ref;
	eeq->flags |= EQF_EQUIPPED;

	nd_writef(who_ref, "You equip %s.\n", eq.name);
	ent_set(who_ref, &ewho);
	mcp_stats(who_ref);
	mcp_content_out(who_ref, eq_ref);
	mcp_equipment(who_ref);
	return 0;
}

unsigned
unequip(unsigned player_ref, unsigned eql)
{
	ENT eplayer = ent_get(player_ref);
	unsigned eq_ref = EQUIP(&eplayer, eql);
	unsigned eqt, aux;

	if (eq_ref == NOTHING)
		return NOTHING;

	OBJ eq;
	qdb_get(obj_hd, &eq, &eq_ref);
	EQU *eeq = &eq.sp.equipment;
	eqt = EQT(eeq->eqw);
	aux = 0;

	switch (eql) {
	case ES_RHAND:
		EFFECT(&eplayer, DMG).value -= DMG_WEAPON(eeq);
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
		EFFECT(&eplayer, DEF).value -= aux;
		EFFECT(&eplayer, DODGE).value += DODGE_ARMOR(aux);
	}

	EQUIP(&eplayer, eql) = NOTHING;
	eeq->flags &= ~EQF_EQUIPPED;
	ent_set(player_ref, &eplayer);
	mcp_content_in(player_ref, eq_ref);
	mcp_stats(player_ref);
	mcp_equipment(player_ref);
	return eq_ref;
}

static inline unsigned
entity_xp(ENT *eplayer, unsigned target_ref)
{
	ENT etarget = ent_get(target_ref);

	// alternatively (2000/x)*y/x
	if (!eplayer->lvl)
		return 0;

	unsigned r = 254 * etarget.lvl / (eplayer->lvl * eplayer->lvl);
	if (r < 0)
		return 0;
	else
		return r;
}

static inline void
_entity_award(unsigned player_ref, ENT *eplayer, unsigned const xp)
{
	unsigned cxp = eplayer->cxp;
	nd_writef(player_ref, "You gain %u xp!\n", xp);
	cxp += xp;
	while (cxp >= 1000) {
		nd_writef(player_ref, "You level up!\n");
		cxp -= 1000;
		eplayer->lvl += 1;
		eplayer->spend += 2;
	}
	eplayer->cxp = cxp;
}

static inline void
entity_award(unsigned player_ref, ENT *eplayer, unsigned target_ref)
{
	_entity_award(player_ref, eplayer, entity_xp(eplayer, target_ref));
}

#define ADAM_SKEL_REF 0

static inline unsigned
entity_body(unsigned mob_ref)
{
	unsigned tmp_ref;
	char buf[32];
	OBJ mob, dead_mob;
	qdb_get(obj_hd, &mob, &mob_ref);
	snprintf(buf, sizeof(buf), "%s's body.", mob.name);
	unsigned dead_mob_ref = object_add(&dead_mob, ADAM_SKEL_REF, mob.location, 0);
	unsigned n = 0;

	qdb_cur_t c = qdb_iter(contents_hd, &mob_ref);
	while (qdb_next(&mob_ref, &tmp_ref, &c)) {
		OBJ tmp;
		qdb_get(obj_hd, &tmp, &tmp_ref);
		if (tmp.type == TYPE_EQUIPMENT) {
			EQU *etmp = &tmp.sp.equipment;
			unequip(mob_ref, EQL(etmp->eqw));
		}
		object_move(tmp_ref, dead_mob_ref);
		n++;
	}

	if (n > 0) {
		strlcpy(dead_mob.name, buf, sizeof(dead_mob.name));
		qdb_put(obj_hd, &dead_mob_ref, &dead_mob);
		nd_owritef(mob_ref, "%s's body drops to the ground.\n", mob.name);
		return dead_mob_ref;
	} else {
		object_move(dead_mob_ref, NOTHING);
		return NOTHING;
	}
}

static inline unsigned
entity_kill(unsigned player_ref, ENT *eplayer, unsigned target_ref, ENT *etarget)
{
	notify_wts(target_ref, "die", "dies", "");

	entity_body(target_ref);

	if (player_ref != NOTHING) {
		entity_award(player_ref, eplayer, target_ref);
		eplayer->target = NOTHING;
	}

	OBJ target;
	qdb_get(obj_hd, &target, &target_ref);

	if (etarget->target && (etarget->flags & EF_AGGRO)) {
		ENT etartar = ent_get(etarget->target);
		etartar.klock --;
		ent_set(etarget->target, &etartar);
	}

	unsigned loc_ref = target.location;
	OBJ loc;

	qdb_get(obj_hd, &loc, &loc_ref);

	if ((loc.sp.room.flags & RF_TEMP) && !(etarget->flags & EF_PLAYER)) {
		object_move(target_ref, NOTHING);
		room_clean(loc_ref);
		return NOTHING;
	}

	etarget->hp = 1;
	etarget->mp = 1;
	ent_reset(etarget);
	ent_set(target_ref, etarget);
	debufs_end(etarget);
	st_start(target_ref);
	look_around(target_ref);
	mcp_bars(target_ref);
	view(target_ref);

	return target_ref;
}

int
entity_damage(unsigned player_ref, ENT *eplayer, unsigned target_ref, ENT *etarget, short amt)
{
	long hp = etarget->hp;
	hp += amt;

	if (!amt)
		return 0;

	if (hp <= 0) {
		entity_kill(player_ref, eplayer, target_ref, etarget);
		return 1;
	}

	register unsigned short max = HP_MAX(etarget);
	if (hp > max)
		hp = max;

	etarget->hp = hp;
	ent_set(target_ref, etarget);
	mcp_bars(target_ref);

	return 0;
}

void
do_look_at(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd), thing_ref;
	OBJ player, thing;
	char *name = argv[1];

	if (*name == '\0') {
		qdb_get(obj_hd, &player, &player_ref);
		thing_ref = player.location;
	} else if (
			(thing_ref = ematch_absolute(name)) == NOTHING
			&& (thing_ref = ematch_here(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_me(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, name)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, name)) == NOTHING
		  )
	{
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	qdb_get(obj_hd, &thing, &thing_ref);
	switch (thing.type) {
	case TYPE_ROOM:
		view(player_ref);
	default:
		look_at(player_ref, thing_ref);
		break;
	}
}

static void
respawn(unsigned player_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);

	nd_owritef(player_ref, "%s disappears.\n", player.name);

	if (eplayer.flags & EF_PLAYER) {
		struct cmd_dir cd;
		cd.rep = 0;
		cd.dir = '\0';
		st_teleport(player_ref, cd);
	} else {
		object_move(player_ref, eplayer.home);
	}

	nd_owritef(player_ref, "%s appears.\n", player.name);
}

static inline int
huth_notify(unsigned player_ref, ENT *eplayer, enum huth type)
{
	static char const *msg[] = {
		"You are thirsty.\n",
		"You are very thirsty.\n",
		"you are dehydrated.\n",
		"You are dying of thirst.\n",
		"You are hungry.\n",
		"You are very hungry.\n",
		"You are starving.\n",
		"You are starving to death.\n"
	};

	unsigned thirst_y = huth_y[HUTH_THIRST],
		hunger_y = huth_y[HUTH_HUNGER];

	unsigned long long const on[] = {
		1ULL << (thirst_y - 1),
		1ULL << (thirst_y - 2),
		1ULL << (thirst_y - 3),
		1ULL << (hunger_y - 1),
		1ULL << (hunger_y - 2),
		1ULL << (hunger_y - 3)
	};

	unsigned long long const *n = on + 3 * type;
	unsigned long long v;
	unsigned char rn = eplayer->huth_n[type];

	v = eplayer->huth[type] += 1ULL << (DAYTICK_Y - 10);

	char const **m = msg + 4 * type;

	if (rn >= 4) {
		return HP_MAX(eplayer) >> 3;
	} else if (rn >= 3) {
		if (v >= 2 * n[1] + n[2]) {
			nd_writef(player_ref, m[3]);
			rn += 1;
		}
	} else if (rn >= 2) {
		if (v >= 2 * n[1]) {
			nd_writef(player_ref, m[2]);
			rn += 1;
		}
	} else if (rn >= 1) {
		if (v >= n[1]) {
			nd_writef(player_ref, m[1]);
			rn += 1;
		}
	} else if (v >= n[2]) {
		nd_writef(player_ref, m[0]);
		rn += 1;
	}

	eplayer->huth_n[type] = rn;
	return 0;
}

static inline unsigned char
d20(void)
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
kill_dodge(unsigned player_ref, char *wts)
{
	ENT eplayer = ent_get(player_ref);
	ENT etarget = ent_get(eplayer.target);
	if (!EFFECT(&etarget, MOV).value && dodge_get(&eplayer)) {
		notify_wts_to(eplayer.target, player_ref, "dodge", "dodges", "'s %s", wts);
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
kill_dmg(unsigned dmg_type, short dmg,
	short def, unsigned def_type)
{
	if (dmg > 0) {
		element_t element;
		qdb_get(element_hd, &element, &def_type);
		if (dmg_type == element.weakness)
			dmg *= 2;
		else {
			qdb_get(element_hd, &element, &dmg_type);
			if (element.weakness == def_type)
				dmg /= 2;
		}

		if (dmg < def)
			return 0;

	} else
		// heal TODO make type matter
		def = 0;

	return randd_dmg(dmg - def);
}

void debuf_notify(unsigned player_ref, struct debuf *d, short val);
int spells_cast(unsigned player_ref, ENT *eplayer, unsigned target_ref);

static inline void
attack(unsigned player_ref, ENT *eplayer)
{
	register unsigned char mask;

	mask = EFFECT(eplayer, MOV).mask;

	if (mask) {
		register unsigned i = __builtin_ffs(mask) - 1;
		debuf_notify(player_ref, &eplayer->debufs[i], 0);
		return;
	}

	if (eplayer->target == NOTHING)
		return;

	ENT etarget = ent_get(eplayer->target);
	char wts[BUFSIZ];
	extern unsigned wts_hd;
	unsigned wts_ref = eplayer->wtso;

	unsigned eq_ref = EQUIP(eplayer, ES_RHAND);

	if (eq_ref) {
		OBJ eq;
		qdb_get(obj_hd, &eq, &eq_ref);
		wts_ref = EQT(eq.sp.equipment.eqw);
	}

	qdb_get(wts_hd, wts, &wts_ref);

	if (spells_cast(player_ref, eplayer, eplayer->target) && !kill_dodge(player_ref, wts)) {
		unsigned at = STAT_ELEMENT(eplayer, MDMG);
		unsigned dt = STAT_ELEMENT(&etarget, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(eplayer, DMG).value, EFFECT(&etarget, DEF).value + EFFECT(&etarget, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(eplayer, MDMG).value, EFFECT(&etarget, MDEF).value, dt);
		element_t element;
		qdb_get(element_hd, &element, &at);
		notify_attack(player_ref, eplayer->target, wts, aval, element.color, bval);
		if (!entity_damage(player_ref, eplayer, eplayer->target, &etarget, aval + bval))
			ent_set(eplayer->target, &etarget);
	}
}

static inline void
kill_update(unsigned player_ref, ENT *eplayer, double dt __attribute__((unused)))
{
	if (eplayer->target == NOTHING)
		return;

	unsigned target_ref = eplayer->target;
	ENT etarget = ent_get(target_ref);

	if (etarget.target == NOTHING) {
		etarget.target = player_ref;
		ent_set(target_ref, &etarget);
	}

	stand_silent(player_ref, eplayer);

	unsigned short ohp = etarget.hp;
	unsigned short omp = eplayer->mp;
	attack(player_ref, eplayer);
	if (eplayer->mp != omp)
		mcp_bars(player_ref);
	if (etarget.hp != ohp)
		mcp_bars(target_ref);
}

int debufs_process(unsigned player_ref, ENT *eplayer);

void
entity_update(unsigned player_ref, double dt)
{
	OBJ player;
	qdb_get(obj_hd, &player, &player_ref);
	ENT eplayer = ent_get(player_ref);
	unsigned short ohp = eplayer.hp;
	unsigned short omp = eplayer.mp;

	if (!(eplayer.flags & EF_PLAYER)) {
		if (eplayer.hp == HP_MAX(&eplayer)) {
			if ((eplayer.flags & EF_SITTING)) {
				stand(player_ref, &eplayer);
			}

			if (player.location == 0) {
				respawn(player_ref);
				qdb_get(obj_hd, &player, &player_ref);
				eplayer = ent_get(player_ref);
			}
		} else {
			if (eplayer.target == NOTHING && !(eplayer.flags & EF_SITTING)) {
				sit(player_ref, &eplayer, "");
			}
		}
	}

	int damage = 0;

	if (eplayer.flags & EF_SITTING) {
		int div = 100;
		int max, cur;
		damage += dt * HP_MAX(&eplayer) / div;
		max = MP_MAX(&eplayer);
		cur = eplayer.mp + (max / div);
		eplayer.mp = cur > max ? max : cur;
	}

	if (player.location == 0)
		return;

	damage -= huth_notify(player_ref, &eplayer, HUTH_THIRST);
	damage -= huth_notify(player_ref, &eplayer, HUTH_HUNGER);
	damage += debufs_process(player_ref, &eplayer);

	if (!entity_damage(NOTHING, NULL, player_ref, &eplayer, damage))
		kill_update(player_ref, &eplayer, dt);

	ent_set(player_ref, &eplayer);
	if (eplayer.hp != ohp || eplayer.mp != omp)
		mcp_bars(player_ref);

	nd_flush(player_ref);
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

void
reroll(ENT *eplayer) {
	int i;

	for (i = 0; i < ATTR_MAX; i++)
		eplayer->attr[i] = d20();

	EFFECT(eplayer, DMG).value = DMG_BASE(eplayer);
	EFFECT(eplayer, DODGE).value = DODGE_BASE(eplayer);
}

void
do_reroll(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd),
	      thing_ref = player_ref;

	char *what = argv[1];

	if (
			argc > 1 && (thing_ref = ematch_me(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_near(player_ref, what)) == NOTHING
			&& (thing_ref = ematch_mine(player_ref, what)) == NOTHING
	   ) {
		nd_writef(player_ref, NOMATCH_MESSAGE);
		return;
	}

	ENT eplayer = ent_get(player_ref);
	reroll(&eplayer);
	ent_set(player_ref, &eplayer);
	mcp_stats(player_ref);
	mcp_bars(player_ref);
}

void
notify_attack(unsigned player_ref, unsigned target_ref, char *wts, short val, enum color color, short mval)
{
	char buf[BUFSIZ];
	unsigned i = 0;

	if (val || mval) {
		buf[i++] = ' ';
		buf[i++] = '(';

		if (val)
			i += snprintf(&buf[i], sizeof(buf) - i, "%d%s", val, mval ? ", " : "");

		if (mval)
			i += snprintf(&buf[i], sizeof(buf) - i, "%s%d%s", ansi_fg[color], mval, ANSI_RESET);

		buf[i++] = ')';
	}
	buf[i] = '\0';

	notify_wts_to(player_ref, target_ref, wts, wts_plural(wts), "%s", buf);
}


void
do_fight(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	OBJ player, loc, target;
	qdb_get(obj_hd, &player, &player_ref);
	unsigned target_ref = strcmp(argv[1], "me")
		? ematch_near(player_ref, argv[1])
		: player_ref;

	qdb_get(obj_hd, &loc, &player.location);
	if (player.location == 0 || (loc.flags & RF_HAVEN)) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	qdb_get(obj_hd, &target, &target_ref);
	if (target_ref == NOTHING
	    || player_ref == target_ref
	    || target.type != TYPE_ENTITY)
	{
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	ENT eplayer = ent_get(player_ref);
	eplayer.target = target_ref;
	ent_set(player_ref, &eplayer);
	/* ndc_writef(fd, "You form a combat pose."); */
}

void
do_status(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
	// TODO optimize MOB_EV / MOB_EM
	nd_writef(player_ref, "hp %u/%u\tmp %u/%u\tstuck 0x%x\n"
		"dodge %d\tcombo 0x%x \tdebuf_mask 0x%x\n"
		"damage %d\tmdamage %d\tmdmg_mask 0x%x\n"
		"defense %d\tmdefense %d\tmdef_mask 0x%x\n"
		"klock   %u\thunger %u\tthirst %u\n",
		eplayer.hp, HP_MAX(&eplayer), eplayer.mp, MP_MAX(&eplayer), EFFECT(&eplayer, MOV).mask,
		EFFECT(&eplayer, DODGE).value, eplayer.combo, eplayer.debuf_mask,
		EFFECT(&eplayer, DMG).value, EFFECT(&eplayer, MDMG).value, EFFECT(&eplayer, MDMG).mask,
		EFFECT(&eplayer, DEF).value, EFFECT(&eplayer, MDEF).value, EFFECT(&eplayer, MDEF).mask,
		eplayer.klock, eplayer.huth[HUTH_HUNGER], eplayer.huth[HUTH_THIRST]);
}

void
do_heal(int fd, int argc __attribute__((unused)), char *argv[])
{
	char *name = argv[1];
	unsigned player_ref = fd_player(fd), target_ref;

	if (strcmp(name, "me")) {
		target_ref = ematch_near(player_ref, name);
	} else
		target_ref = player_ref;

	if (target_ref == NOTHING || !(ent_get(player_ref).flags & EF_WIZARD)) {
                nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	ENT etarget = ent_get(target_ref);
	etarget.hp = HP_MAX(&etarget);
	etarget.mp = MP_MAX(&etarget);
	etarget.huth[HUTH_THIRST] = etarget.huth[HUTH_HUNGER] = 0;
	debufs_end(&etarget);
	ent_set(target_ref, &etarget);
	notify_wts_to(player_ref, target_ref, "heal", "heals", "");
	mcp_bars(target_ref);
}

void
do_advitam(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	char *name = argv[1];
	unsigned target_ref = ematch_near(player_ref, name);

	if (!(ent_get(player_ref).flags & EF_WIZARD) || target_ref == NOTHING) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	OBJ target;
	qdb_get(obj_hd, &target, &target_ref);

	if (target.owner != player_ref) {
		nd_writef(player_ref, CANTDO_MESSAGE);
		return;
	}

	ENT etarget;
	birth(&etarget);
	ent_set(target_ref, &etarget);
	target.type = TYPE_ENTITY;
	qdb_put(obj_hd, &target_ref, &target);
}

void
do_train(int fd, int argc __attribute__((unused)), char *argv[]) {
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
	const char *attrib = argv[1];
	const char *amount_s = argv[2];
	int attr;

	switch (attrib[0]) {
	case 's': attr = ATTR_STR; break;
	case 'c': attr = ATTR_CON; break;
	case 'd': attr = ATTR_DEX; break;
	case 'i': attr = ATTR_INT; break;
	case 'w': attr = ATTR_WIZ; break;
	case 'h': attr = ATTR_CHA; break;
	default:
		  nd_writef(player_ref, "Invalid attribute.\n");
		  return;
	}

	int avail = eplayer.spend;
	int amount = *amount_s ? atoi(amount_s) : 1;

	if (amount > avail) {
		  nd_writef(player_ref, "Not enough points.\n");
		  return;
	}

	unsigned c = eplayer.attr[attr];
	eplayer.attr[attr] += amount;

	switch (attr) {
	case ATTR_STR:
		EFFECT(&eplayer, DMG).value += DMG_G(c + amount) - DMG_G(c);
		break;
	case ATTR_DEX:
		EFFECT(&eplayer, DODGE).value += DODGE_G(c + amount) - DODGE_G(c);
		break;
	}

	eplayer.spend = avail - amount;
	ent_set(player_ref, &eplayer);
	nd_writef(player_ref, "Your %s increases %d time(s).\n", attrib, amount);
        mcp_stats(player_ref);
}

int
kill_v(unsigned player_ref, char const *opcs)
{
	ENT eplayer = ent_get(player_ref);
	char *end;
	if (isdigit(*opcs)) {
		unsigned combo = strtol(opcs, &end, 0);
		eplayer.combo = combo;
		ent_set(player_ref, &eplayer);
		nd_writef(player_ref, "Set combo to 0x%x.\n", combo);
		return end - opcs;
	} else if (*opcs == 'c' && isdigit(opcs[1])) {
		unsigned slot = strtol(opcs + 1, &end, 0);
		OBJ player;
		qdb_get(obj_hd, &player, &player_ref);
		if (player.location == 0)
			nd_writef(player_ref, "You may not cast spells in room 0.\n");
		else {
			spell_cast(player_ref, &eplayer, eplayer.target, slot);
			ent_set(player_ref, &eplayer);
		}
		return end - opcs;
	} else
		return 0;
}

void
do_sit(int fd, int argc __attribute__((unused)), char *argv[])
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
        sit(player_ref, &eplayer, argv[1]);
	ent_set(player_ref, &eplayer);
}

void
do_stand(int fd, int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
	unsigned player_ref = fd_player(fd);
	ENT eplayer = ent_get(player_ref);
        stand(player_ref, &eplayer);
	ent_set(player_ref, &eplayer);
}

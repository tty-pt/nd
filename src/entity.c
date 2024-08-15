#include "uapi/entity.h"

#include <stdio.h>
#include <stdlib.h>

#include "mcp.h"
#include "params.h"
#include "st.h"
#include "uapi/io.h"
#include "uapi/match.h"
#include "uapi/wts.h"
#include "view.h"

#define MODIFIER(ent, a) ((ent->attr[a] - 10) >> 1) // / 2
#define EQT(x)		(x>>6)
#define EQL(x)		(x & 15)
#define WTS_WEAPON(equ) phys_wts[EQT(equ->eqw)]

unsigned huth_y[2] = {
	DAYTICK_Y + 2,
	DAYTICK_Y + 3
};

unsigned tmp_hds = -1;

static const char *rarity_str[] = {
	ANSI_BOLD ANSI_FG_BLACK "Poor" ANSI_RESET,
	"",
	ANSI_BOLD "Uncommon" ANSI_RESET,
	ANSI_BOLD ANSI_FG_CYAN "Rare" ANSI_RESET,
	ANSI_BOLD ANSI_FG_GREEN "Epic" ANSI_RESET,
	ANSI_BOLD ANSI_FG_MAGENTA "Mythical" ANSI_RESET
};

ENT ent_get(unsigned ref) {
	ENT ent;
	hash_cget(tmp_hds, &ent, &ref, sizeof(ref));
	return ent;
}

void ent_set(unsigned ref, ENT *tmp) {
	hash_cput(tmp_hds, &ref, sizeof(ref), tmp, sizeof(ENT));
}

void ent_del(unsigned ref) {
	hash_del(tmp_hds, &ref, sizeof(ref));
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
		register unsigned eq = EQUIP(eplayer, i);

		if (eq <= 0)
			continue;

		OBJ oeq = obj_get(eq);
		EQU *eeq = &oeq.sp.equipment;
		equip_affect(eplayer, eeq);
	}

	spells_birth(eplayer);
}

static inline int
entities_aggro(unsigned player_ref)
{
	ENT eplayer = ent_get(player_ref);
	unsigned tmp_ref;
	int klock = 0;

	struct hash_cursor c = contents_iter(obj_get(player_ref).location);
	while ((tmp_ref = contents_next(&c)) != NOTHING) {
		if (obj_get(tmp_ref).type != TYPE_ENTITY)
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
	OBJ player = obj_get(player_ref);
	unsigned old_loc_ref = player.location;

	st_run(player_ref, "leave");
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports out.\n", player.name);
	object_move(player_ref, loc_ref);
	room_clean(old_loc_ref);
	if (e == E_NULL)
		nd_owritef(player_ref, "%s teleports in.\n", player.name);
	st_run(player_ref, "enter");
	entities_aggro(player_ref);
}

int
payfor(unsigned who_ref, OBJ *who, int cost)
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
	OBJ who = obj_get(who_ref);
	if (who.type != TYPE_ENTITY)
		who_ref = who.owner;

	OBJ what = obj_get(what_ref);

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

	OBJ loc = obj_get(loc_ref);

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

	OBJ seat = obj_get(seat_ref);

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
		OBJ chair = obj_get(eplayer->sat);
		SEA *schair = &chair.sp.seat;
		schair->quantity--;
		obj_set(eplayer->sat, &chair);
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
look_around(unsigned player_ref)
{
	mcp_look(player_ref, obj_get(player_ref).location);
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
	OBJ eq = obj_get(eq_ref);
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

	OBJ eq = obj_get(eq_ref);
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

extern unsigned corpse_ref;

static inline unsigned
entity_body(unsigned mob_ref)
{
	unsigned tmp_ref;
	char buf[32];
	OBJ mob = obj_get(mob_ref), dead_mob;
	snprintf(buf, sizeof(buf), "%s's body.", mob.name);
	unsigned dead_mob_ref = object_add(&dead_mob, corpse_ref, mob.location, NULL);
	unsigned n = 0;

	struct hash_cursor c = contents_iter(mob_ref);
	while ((tmp_ref = contents_next(&c)) != NOTHING) {
		OBJ tmp = obj_get(tmp_ref);
		if (tmp.type == TYPE_EQUIPMENT) {
			EQU *etmp = &tmp.sp.equipment;
			unequip(mob_ref, EQL(etmp->eqw));
		}
		object_move(tmp_ref, dead_mob_ref);
		n++;
	}

	if (n > 0) {
		free(dead_mob.name);
		dead_mob.name = strdup(buf);
		obj_set(dead_mob_ref, &dead_mob);
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

	OBJ target = obj_get(target_ref);

	if (etarget->target && (etarget->flags & EF_AGGRO)) {
		ENT etartar = ent_get(etarget->target);
		etartar.klock --;
		ent_set(etarget->target, &etartar);
	}

	unsigned loc_ref = target.location;

	if ((obj_get(target.location).sp.room.flags & RF_TEMP) && !(etarget->flags & EF_PLAYER)) {
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
	int hp = etarget->hp;
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
do_look_at(int fd, int argc, char *argv[])
{
	unsigned player_ref = fd_player(fd), thing_ref;
	char *name = argv[1];

	if (*name == '\0') {
		thing_ref = obj_get(player_ref).location;
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

	switch (obj_get(thing_ref).type) {
	case TYPE_ROOM:
		view(player_ref);
	default:
		mcp_look(player_ref, thing_ref);
		break;
	}
}

static void
respawn(unsigned player_ref)
{
	ENT eplayer = ent_get(player_ref);
	OBJ player = obj_get(player_ref);

	nd_owritef(player_ref, "%s disappears.\n", player.name);

	if (eplayer.flags & EF_PLAYER) {
		struct cmd_dir cd;
		cd.rep = 0;
		cd.dir = '\0';
		st_teleport(player_ref, cd);
	} else {
		/* warn("respawning %d to %d\n", who, where); */
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
		short val = -(HP_MAX(eplayer) >> 3);
		return entity_damage(NOTHING, NULL, player_ref, eplayer, val);
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
kill_dmg(enum element dmg_type, short dmg,
	short def, enum element def_type)
{
	if (dmg > 0) {
		if (dmg_type == element_get(def_type).weakness)
			dmg *= 2;
		else if (element_get(dmg_type).weakness == def_type)
			dmg /= 2;

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
	char *wts = wts_map[eplayer->wtso];

	unsigned eq_ref = EQUIP(eplayer, ES_RHAND);

	if (eq_ref != NOTHING)
		wts = wts_map[EQT(obj_get(eq_ref).sp.equipment.eqw)];

	if (spells_cast(player_ref, eplayer, eplayer->target) && !kill_dodge(player_ref, wts)) {
		enum element at = STAT_ELEMENT(eplayer, MDMG);
		enum element dt = STAT_ELEMENT(&etarget, MDEF);
		short aval = -kill_dmg(ELM_PHYSICAL, EFFECT(eplayer, DMG).value, EFFECT(&etarget, DEF).value + EFFECT(&etarget, MDEF).value, dt);
		short bval = -kill_dmg(at, EFFECT(eplayer, MDMG).value, EFFECT(&etarget, MDEF).value, dt);
		char const *color = element_get(at).color;
		notify_attack(player_ref, eplayer->target, wts, aval, color, bval);
		if (!entity_damage(player_ref, eplayer, eplayer->target, &etarget, aval + bval))
			ent_set(eplayer->target, &etarget);
	}
}

static inline void
kill_update(unsigned player_ref, ENT *eplayer, double dt)
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
	OBJ player = obj_get(player_ref);
	ENT eplayer = ent_get(player_ref);
	unsigned short ohp = eplayer.hp;
	unsigned short omp = eplayer.mp;

	if (!(eplayer.flags & EF_PLAYER)) {
		/* warn("%d hp %d/%d\n", player, eplayer.hp, HP_MAX(player)); */
		if (eplayer.hp == HP_MAX(&eplayer)) {
			/* warn("%d's hp is maximum\n", player); */

			if ((eplayer.flags & EF_SITTING)) {
				/* warn("%d is sitting so they shall stand\n", player); */
				stand(player_ref, &eplayer);
			}

			if (player.location == 0) {
				/* warn("%d is located at 0, so they shall respawn\n", player); */
				respawn(player_ref);
				player = obj_get(player_ref);
				eplayer = ent_get(player_ref);
			}
		} else {
			/* warn("%d's hp is not maximum\n", player); */
			if (eplayer.target == NOTHING && !(eplayer.flags & EF_SITTING)) {
				/* warn("%d is not sitting so they shall sit\n", player); */
				sit(player_ref, &eplayer, "");
			}
		}
	}

	if (eplayer.flags & EF_SITTING) {
		int div = 100;
		int max, cur;
		entity_damage(NOTHING, NULL, player_ref, &eplayer, dt * HP_MAX(&eplayer) / div);
		max = MP_MAX(&eplayer);
		cur = eplayer.mp + (max / div);
		eplayer.mp = cur > max ? max : cur;
	}

	if (player.location == 0)
		return;

        /* if mob dies, return */
	if (!(huth_notify(player_ref, &eplayer, HUTH_THIRST)
		|| huth_notify(player_ref, &eplayer, HUTH_HUNGER)
                || debufs_process(player_ref, &eplayer)))

		kill_update(player_ref, &eplayer, dt);

	if (!obj_exists(player_ref))
		return;

	ent_set(player_ref, &eplayer);
	if (eplayer.hp != ohp || eplayer.mp != omp)
		mcp_bars(player_ref);
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
reroll(unsigned player_ref, ENT *eplayer) {
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
	reroll(player_ref, &eplayer);
	ent_set(player_ref, &eplayer);
	mcp_stats(player_ref);
	mcp_bars(player_ref);
}

void entities_init() {
	tmp_hds = hash_init();
}

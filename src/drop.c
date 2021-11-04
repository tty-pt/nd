#define ARMORSET(s, _type, min_stat, y) \
	struct object_skeleton s ## _helmet = { \
		.name = #s " armor helmet", \
		.description = "", \
		.art = "", \
		.avatar = "", \
		.type = S_TYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(HEAD, _type), \
			.msv = min_stat, \
		} } \
	}; \
	struct drop s ## _helmet_drop = { \
                .i = & s ## _helmet, y \
	}; \
	struct object_skeleton s ## _chest = { \
		.name = #s " armor chest", \
		.description = "", \
		.art = "", \
		.avatar = "", \
		.type = S_TYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(CHEST, _type), \
			.msv = min_stat, \
		} } \
	}; \
	struct drop s ## _chest_drop = { \
                .i = & s ## _chest, y \
	}; \
	struct object_skeleton  s ## _pants = { \
		.name = #s " armor leggings", \
		.description = "", \
		.art = "", \
		.avatar = "", \
		.type = S_TYPE_EQUIPMENT, \
		.sp = { .equipment = { \
			.eqw = EQ(PANTS, _type), \
			.msv = min_stat, \
		} } \
	}; \
	struct drop s ## _pants_drop = { \
                .i = & s ## _pants, y \
	};


#define ARMORSET_LIGHT(s, min_dex, y) \
	ARMORSET(s, ARMOR_LIGHT, min_dex, y)

#define ARMORSET_MEDIUM(s, min_stat, y) \
	ARMORSET(s, ARMOR_MEDIUM, min_stat, y)

#define ARMORSET_HEAVY(s, min_str, y) \
	ARMORSET(s, ARMOR_HEAVY, min_str, y)

ARMORSET_LIGHT(padded, 15, 5)
ARMORSET_MEDIUM(hide, 15, 5)
ARMORSET_HEAVY(chainmail, 15, 5)

struct object_skeleton dagger = {
	.name = "dagger",
	.description = "",
	.art = "",
	.avatar = "",
	.type = S_TYPE_EQUIPMENT,
	.sp = { .equipment = {
		.eqw = EQ(RHAND, SLASH),
		.msv = 5
	} }
};

struct drop dagger_drop = { .i = &dagger, .y = 0 };

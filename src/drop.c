#define ARMORSET(s, type, min_stat, y) \
	drop_t s ## _helmet = { \
		{ \
			{ #s " armor helmet", "", "", }, \
			EQ(HEAD, type), min_stat, \
		}, y \
	}; \
	drop_t s ## _chest = { \
		{ \
			{ #s " armor chest", "", "", }, \
			EQ(CHEST, type), min_stat, \
		}, y \
	}; \
	drop_t s ## _pants = { \
		{ \
			{ #s " armor leggings", "", "", }, \
			EQ(PANTS, type), min_stat, \
		}, y \
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

drop_t dagger_drop = {
	{
		{ "dagger", "", "", },
		EQ(RHAND, SLASH), 5
	}, 0
};

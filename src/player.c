#include "io.h"
#include "entity.h"

#include "defaults.h"
#include "externs.h"

static hash_tab player_list[PLAYER_HASH_SIZE];

OBJ *
lookup_player(const char *name)
{
	hash_data *hd = find_hash(name, player_list, PLAYER_HASH_SIZE);

	if (!hd)
		return NULL;


	warn("lookup_player! %d\n", hd->dbval);
	return object_get(hd->dbval);
}

OBJ *
connect_player(const char *qsession)
{
	char buf[BUFSIZ];
	FILE *fp;

	snprintf(buf, sizeof(buf), "/sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp)
		return NULL;

	fscanf(fp, "%s", buf);
	fclose(fp);

	return lookup_player(buf);
}

OBJ *
create_player(const char *name)
{
	OBJ *player = object_new();
	ENT *eplayer = &player->sp.entity;

	player->name = strdup(name);
	eplayer->home = player->location = object_get(PLAYER_START);
	player->type = TYPE_ENTITY;
	player->owner = player;
	player->value = START_PENNIES;
	eplayer->flags = EF_PLAYER;

	PUSH(player, object_get(PLAYER_START)->contents);
	add_player(player);

	return player;
}

void
clear_players(void)
{
	kill_hash(player_list, PLAYER_HASH_SIZE, 0);
	return;
}

void
add_player(OBJ *who)
{
	hash_data hd;

	warn("add_player %d\n", object_ref(who));
	hd.dbval = object_ref(who);
	if (add_hash(who->name, hd, player_list, PLAYER_HASH_SIZE) == NULL) {
		BUG("Out of memory");
	} else {
		return;
	}
}


void
delete_player(OBJ *who)
{
	int result;
	char buf[BUFFER_LEN];
	char namebuf[BUFFER_LEN];
	int i, j;
	OBJ *found, *ren;


	result = free_hash(who->name, player_list, PLAYER_HASH_SIZE);

	if (result) {
		wall_wizards("## WARNING: Playername hashtable is inconsistent.  Rebuilding it.  Don't panic.");
		clear_players();
		for (i = 0; i < db_top; i++) {
			OBJ *oi = object_get(i);
			if (oi->type == TYPE_ENTITY) {
				found = lookup_player(oi->name);

				if (!found) {
					add_player(oi);
					continue;
				}

				ren = (i == object_ref(who)) ? found : oi;
				j = 0;
				do {
					snprintf(namebuf, sizeof(namebuf), "%s%d", ren->name, ++j);
				} while (lookup_player(namebuf));

				snprintf(buf, sizeof(buf),
						"## Renaming %s(#%d) to %s to prevent name collision.",
						ren->name, object_ref(ren), namebuf);
				wall_wizards(buf);

				warn("SANITY NAME CHANGE: %s(#%d) to %s", ren->name, object_ref(ren), namebuf);

				if (ren == found) {
					free_hash(ren->name, player_list, PLAYER_HASH_SIZE);
				}
				if (ren->name) {
					free((void *) ren->name);
				}
				ren->name = strdup(namebuf);
				add_player(ren);
			}
		}
		result = free_hash(who->name, player_list, PLAYER_HASH_SIZE);
		if (result) {
			wall_wizards("## WARNING: Playername hashtable still inconsistent.  Now you can panic.");
		}
	}

	return;
}

/* $Header$ */


#include "copyright.h"
#include "config.h"

#include <string.h>

#include "mdb.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"
#include "web.h"

static hash_tab player_list[PLAYER_HASH_SIZE];

dbref
lookup_player(const char *name)
{
	hash_data *hd;

	if ((hd = find_hash(name, player_list, PLAYER_HASH_SIZE)) == NULL) {
		return NOTHING;
	} else {
		return (hd->dbval);
	}
}

dbref
connect_player(const char *qsession)
{
	char buf[BUFSIZ];
	FILE *fp;

	snprintf(buf, sizeof(buf), "/sessions/%s", qsession);
	fp = fopen(buf, "r");

	if (!fp)
		return NOTHING;

	fscanf(fp, "%s", buf);
	fclose(fp);

	return lookup_player(buf);
}

dbref
create_player(const char *name)
{
	dbref ref = object_new();
	OBJ *player = OBJECT(ref);
	ENT *entity = ENTITY(ref);

	player->name = alloc_string(name);
	player->location = PLAYER_START;
	player->flags = TYPE_ENTITY;
	player->owner = ref;
	player->value = START_PENNIES;

	entity->home = PLAYER_START;
	entity->flags = EF_PLAYER;

	PUSH(ref, OBJECT(PLAYER_START)->contents);
	add_player(ref);

	return ref;
}

void
clear_players(void)
{
	kill_hash(player_list, PLAYER_HASH_SIZE, 0);
	return;
}

void
add_player(dbref who)
{
	hash_data hd;

	hd.dbval = who;
	if (add_hash(OBJECT(who)->name, hd, player_list, PLAYER_HASH_SIZE) == NULL) {
		BUG("Out of memory");
	} else {
		return;
	}
}


void
delete_player(dbref who)
{
	int result;
	char buf[BUFFER_LEN];
	char namebuf[BUFFER_LEN];
	int i, j;
	dbref found, ren;


	result = free_hash(OBJECT(who)->name, player_list, PLAYER_HASH_SIZE);

	if (result) {
		wall_wizards
				("## WARNING: Playername hashtable is inconsistent.  Rebuilding it.  Don't panic.");
		clear_players();
		for (i = 0; i < db_top; i++) {
			if (OBJECT(i)->type == TYPE_ENTITY) {
				found = lookup_player(OBJECT(i)->name);
				if (found != NOTHING) {
					ren = (i == who) ? found : i;
					j = 0;
					do {
						snprintf(namebuf, sizeof(namebuf), "%s%d", OBJECT(ren)->name, ++j);
					} while (lookup_player(namebuf) != NOTHING);

					snprintf(buf, sizeof(buf),
							"## Renaming %s(#%d) to %s to prevent name collision.",
							OBJECT(ren)->name, ren, namebuf);
					wall_wizards(buf);

					warn("SANITY NAME CHANGE: %s(#%d) to %s", OBJECT(ren)->name, ren, namebuf);

					if (ren == found) {
						free_hash(OBJECT(ren)->name, player_list, PLAYER_HASH_SIZE);
					}
					if (OBJECT(ren)->name) {
						free((void *) OBJECT(ren)->name);
					}
					OBJECT(ren)->name = alloc_string(namebuf);
					add_player(ren);
				} else {
					add_player(i);
				}
			}
		}
		result = free_hash(OBJECT(who)->name, player_list, PLAYER_HASH_SIZE);
		if (result) {
			wall_wizards
					("## WARNING: Playername hashtable still inconsistent.  Now you can panic.");
		}
	}

	return;
}

static inline void
dialog_start(dbref player, dbref npc, const char *dialog) {
        const char buf[BUFSIZ];
        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/text", dialog);
        const char *text = GETMESG(npc, buf);

        if (!text) {
                /* notifyf(player, "%s stops talking .", OBJECT(npc)->name); */
                web_dialog_stop(player);
                return;
        }

        notify(player, text);

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/n", dialog);
        int i, n = get_property_value(npc, buf);

        for (i = 0; i < n; i++) {
                snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/text", dialog, i);
                const char *answer = GETMESG(npc, buf);
                notifyf(player, "%d) %s", i, answer);
        }
}

void
dialog_stop(dbref player) {
        const char *dialog = ENTITY(player)->dialog;

        if (dialog) {
                free((void *) dialog);
                ENTITY(player)->dialog = NULL;
        }

        ENTITY(player)->dialog_target = NOTHING;
        web_dialog_stop(player);
}

void
do_talk(command_t *cmd) {
        const char buf[BUFSIZ];
        OBJ *player = OBJECT(cmd->player);
        const char *npcs = cmd->argv[1];
	OBJ *npc = *npcs ? ematch_near(player, npcs) : NULL;
	ENT *eplayer = &player->sp.entity;

        if (!npc) {
                notify(REF(player), "Can't find that.");
                return;
        }

        snprintf((char *) buf, sizeof(buf), "_/dialog/%d", REF(npc));

        const char *pdialog = GETMESG(REF(player), buf);
        const char *dialog;

        if (!pdialog)
                dialog = "main";
        else
                dialog = pdialog;

	eplayer->dialog_target = REF(npc);
        if (eplayer->dialog)
                free((void *) eplayer->dialog);
        eplayer->dialog = alloc_string(dialog);

        if (web_dialog_start(REF(player), REF(npc), dialog))
                dialog_start(REF(player), REF(npc), dialog);
}

/* TODO merge this with "copy_props()" */
static void
props_copy(dbref target, dbref what, const char *prefix, int ignore) {
        const char buf[BUFSIZ];
        const char propname[BUFFER_LEN];
        PropPtr propadr, pptr;
        snprintf((char *) buf, sizeof(buf), "%s", prefix);
        propadr = first_prop(what, buf, &pptr, (char *) propname, sizeof(propname));

        while (propadr) {
                snprintf((char *) buf, sizeof(buf), "%s%c%s", prefix, PROPDIR_DELIMITER, propname);
                if (PropDir(pptr))
                        props_copy(target, what, buf, ignore);
                else
                        copy_one_prop(target, what, target, (char *) buf, ignore);

		propadr = next_prop(pptr, propadr, (char *) propname, sizeof(propname));
        }
}

void
do_answer(command_t *cmd) {
        const char buf[BUFSIZ];
        dbref player = cmd->player;
        dbref npc = ENTITY(player)->dialog_target;
        const char *dialog = ENTITY(player)->dialog;
        if (npc <= 0 || !dialog) {
                notify(player, "You are not in a conversation\n");
                return;
        }
        const char *answers = cmd->argv[1];
        int answer = *answers ? atoi(answers) : 0;

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/props", dialog, answer);
        props_copy(player, npc, buf, strlen(buf) + 1);

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/next", dialog, answer);
        dialog = GETMESG(npc, buf);
        free((void *) ENTITY(player)->dialog);
        if (!dialog) {
                ENTITY(player)->dialog = NULL;
                web_dialog_stop(player);
                return;
        }
        ENTITY(player)->dialog = alloc_string(dialog);

        if (web_dialog_start(player, npc, dialog))
                dialog_start(player, npc, dialog);
}

/* TODO make this depend on player aswell */
int
dialog_exists(dbref npc) {
        const char buf[BUFSIZ];
        const char *dialog = "main";
        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/text", dialog);
        const char *text = GETMESG(npc, buf);
        return !!text;
}


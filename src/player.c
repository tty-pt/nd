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

	player->name = alloc_string(name);
	eplayer->home = player->location = object_get(PLAYER_START);
	player->flags = TYPE_ENTITY;
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
		wall_wizards
				("## WARNING: Playername hashtable is inconsistent.  Rebuilding it.  Don't panic.");
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
				ren->name = alloc_string(namebuf);
				add_player(ren);
			}
		}
		result = free_hash(who->name, player_list, PLAYER_HASH_SIZE);
		if (result) {
			wall_wizards
					("## WARNING: Playername hashtable still inconsistent.  Now you can panic.");
		}
	}

	return;
}

static inline void
dialog_start(OBJ *player, OBJ *npc, const char *dialog) {
        const char buf[BUFSIZ];
        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/text", dialog);
        const char *text = GETMESG(npc, buf);

        if (!text) {
                /* notifyf(player, "%s stops talking .", object_get(npc)->name); */
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
dialog_stop(OBJ *player) {
	ENT *eplayer = &player->sp.entity;
        const char *dialog = eplayer->dialog;

        if (dialog) {
                free((void *) dialog);
                eplayer->dialog = NULL;
        }

        eplayer->dialog_target = NULL;
        web_dialog_stop(player);
}

void
do_talk(command_t *cmd) {
        const char buf[BUFSIZ];
        OBJ *player = object_get(cmd->player);
        const char *npcs = cmd->argv[1];
	OBJ *npc = *npcs ? ematch_near(player, npcs) : NULL;
	ENT *eplayer = &player->sp.entity;

        if (!npc) {
                notify(player, "Can't find that.");
                return;
        }

        snprintf((char *) buf, sizeof(buf), "_/dialog/%d", object_ref(npc));

        const char *pdialog = GETMESG(player, buf);
        const char *dialog;

        if (!pdialog)
                dialog = "main";
        else
                dialog = pdialog;

	eplayer->dialog_target = npc;
        if (eplayer->dialog)
                free((void *) eplayer->dialog);
        eplayer->dialog = alloc_string(dialog);

        if (web_dialog_start(player, npc, dialog))
                dialog_start(player, npc, dialog);
}

/* TODO merge this with "copy_props()" */
static void
props_copy(OBJ *target, OBJ *what, const char *prefix, int ignore) {
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
        OBJ *player = object_get(cmd->player);
	ENT *eplayer = &player->sp.entity;
        OBJ *npc = eplayer->dialog_target;
        const char *dialog = eplayer->dialog;
        if (!npc || !dialog) {
                notify(player, "You are not in a conversation\n");
                return;
        }
        const char *answers = cmd->argv[1];
        int answer = *answers ? atoi(answers) : 0;

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/props", dialog, answer);
        props_copy(player, npc, buf, strlen(buf) + 1);

        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/%d/next", dialog, answer);
        dialog = GETMESG(npc, buf);
        free((void *) eplayer->dialog);
        if (!dialog) {
                eplayer->dialog = NULL;
                web_dialog_stop(player);
                return;
        }
        eplayer->dialog = alloc_string(dialog);

        if (web_dialog_start(player, npc, dialog))
                dialog_start(player, npc, dialog);
}

/* TODO make this depend on player aswell */
int
dialog_exists(OBJ *npc) {
        const char buf[BUFSIZ];
        const char *dialog = "main";
        snprintf((char *) buf, sizeof(buf), "_/dialog/%s/text", dialog);
        const char *text = GETMESG(npc, buf);
        return !!text;
}


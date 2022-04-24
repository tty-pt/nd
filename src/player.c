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

int
check_password(dbref player, const char* password)
{
	const char *pword = PLAYER_PASSWORD(player);
#ifdef __OpenBSD__
	return !crypt_checkpass(password, pword);
#else
	char *enc;
	enc = crypt(password, "k?");
	CBUG(!enc);
	return !strcmp(enc, pword);
#endif
}

void
set_password_raw(dbref player, const char* password)
{
	PLAYER_SET_PASSWORD(player, password);
	DBDIRTY(player);
}

void
set_password(dbref player, const char* password)
{
#ifdef __OpenBSD__
	char hash[64];
	if (crypt_newhash(password, "bcrypt,4", hash, sizeof(hash))) {
		perror("crypt_newhash");
	}
	set_password_raw(player, alloc_string(hash));
#else
	char *enc;
	enc = crypt(password, "$5$iamsha-256encryptedwhatashame$");
	CBUG(!enc);
	set_password_raw(player, alloc_string(enc));
#endif
}


dbref
connect_player(const char *name, const char *password)
{
	dbref player;

	if (*name == NUMBER_TOKEN && number(name + 1) && atoi(name + 1)) {
		player = (dbref) atoi(name + 1);
		if ((player < 0) || (player >= db_top) || (Typeof(player) != TYPE_PLAYER))
			player = NOTHING;
	} else {
		player = lookup_player(name);
	}
	if (player == NOTHING)
		return NOTHING;
	if (!check_password(player, password))
		return NOTHING;

	return player;
}

dbref
create_player(const char *name, const char *password)
{
	dbref player;

	if (!ok_player_name(name) || !ok_password(password))
		return NOTHING;

	/* else he doesn't already exist, create him */
	player = object_new();

	/* initialize everything */
	NAME(player) = alloc_string(name);
	DBFETCH(player)->location = PLAYER_START;	/* home */
	FLAGS(player) = TYPE_PLAYER;
	OWNER(player) = player;
	ALLOC_PLAYER_SP(player);
	PLAYER_SET_HOME(player, PLAYER_START);
	DBFETCH(player)->exits = NOTHING;

	SETVALUE(player, START_PENNIES);
	set_password_raw(player, NULL);
	set_password(player, password);

	/* link him to PLAYER_START */
	PUSH(player, DBFETCH(PLAYER_START)->contents);
	add_player(player);
	DBDIRTY(player);
	DBDIRTY(PLAYER_START);

	FLAGS(player) |= CHOWN_OK;

	return player;
}

void
do_password(command_t *cmd)
{
	dbref player = cmd->player;
	const char *old = cmd->argv[1];
	const char *newobj = cmd->argv[2];

	if (!PLAYER_PASSWORD(player) || !check_password(player, old)) {
		notify(player, "Sorry, old password did not match current password.");
	} else if (!ok_password(newobj)) {
		notify(player, "Bad new password (no spaces allowed).");
	} else {
		set_password(player, newobj);
		DBDIRTY(player);
		notify(player, "Password changed.");
	}
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
	if (add_hash(NAME(who), hd, player_list, PLAYER_HASH_SIZE) == NULL) {
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


	result = free_hash(NAME(who), player_list, PLAYER_HASH_SIZE);

	if (result) {
		wall_wizards
				("## WARNING: Playername hashtable is inconsistent.  Rebuilding it.  Don't panic.");
		clear_players();
		for (i = 0; i < db_top; i++) {
			if (Typeof(i) == TYPE_PLAYER) {
				found = lookup_player(NAME(i));
				if (found != NOTHING) {
					ren = (i == who) ? found : i;
					j = 0;
					do {
						snprintf(namebuf, sizeof(namebuf), "%s%d", NAME(ren), ++j);
					} while (lookup_player(namebuf) != NOTHING);

					snprintf(buf, sizeof(buf),
							"## Renaming %s(#%d) to %s to prevent name collision.",
							NAME(ren), ren, namebuf);
					wall_wizards(buf);

					warn("SANITY NAME CHANGE: %s(#%d) to %s", NAME(ren), ren, namebuf);

					if (ren == found) {
						free_hash(NAME(ren), player_list, PLAYER_HASH_SIZE);
					}
					if (NAME(ren)) {
						free((void *) NAME(ren));
					}
					NAME(ren) = alloc_string(namebuf);
					add_player(ren);
				} else {
					add_player(i);
				}
			}
		}
		result = free_hash(NAME(who), player_list, PLAYER_HASH_SIZE);
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
                /* notifyf(player, "%s stops talking .", NAME(npc)); */
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
        const char *dialog = PLAYER_SP(player)->dialog;

        if (dialog) {
                free((void *) dialog);
                PLAYER_SP(player)->dialog = NULL;
        }

        PLAYER_SP(player)->dialog_target = NOTHING;
        web_dialog_stop(player);
}

void
do_talk(command_t *cmd) {
        const char buf[BUFSIZ];
        dbref player = cmd->player;
        dbref loc = getloc(player);
        const char *npcs = cmd->argv[1];
        dbref npc = *npcs ? ematch_near(player, npcs) : NOTHING;

        if (npc <= 0) {
                notify(player, "Can't find that.");
                return;
        }

        snprintf((char *) buf, sizeof(buf), "_/dialog/%d", npc);

        const char *pdialog = GETMESG(player, buf);
        const char *dialog;

        if (!pdialog)
                dialog = "main";
        else
                dialog = pdialog;

        PLAYER_SP(player)->dialog_target = npc;
        if (PLAYER_SP(player)->dialog)
                free((void *) PLAYER_SP(player)->dialog);
        PLAYER_SP(player)->dialog = alloc_string(dialog);

        if (web_dialog_start(player, npc, dialog))
                dialog_start(player, npc, dialog);
}

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
        dbref npc = PLAYER_SP(player)->dialog_target;
        const char *dialog = PLAYER_SP(player)->dialog;
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
        free((void *) PLAYER_SP(player)->dialog);
        if (!dialog) {
                PLAYER_SP(player)->dialog = NULL;
                web_dialog_stop(player);
                return;
        }
        PLAYER_SP(player)->dialog = alloc_string(dialog);

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


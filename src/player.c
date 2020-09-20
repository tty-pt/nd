/* $Header$ */


#include "copyright.h"
#include "config.h"

#include <string.h>

#include "db.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "externs.h"

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
	return !crypt_checkpass(password, pword);
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
	char hash[64];
	if (crypt_newhash(password, "bcrypt,4", hash, sizeof(hash))) {
		perror("crypt_newhash");
	}
	set_password_raw(player, alloc_string(hash));
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
	player = new_object();

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
	NOGUEST("@password",player);

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
static const char *player_c_version = "$RCSfile$ $Revision: 1.13 $";
const char *get_player_c_version(void) { return player_c_version; }

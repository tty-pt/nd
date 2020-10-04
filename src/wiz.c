/* $Header$ */

#include "copyright.h"
#include "config.h"

/* Wizard-only commands */

#include <stdio.h>

#include <sys/resource.h>

#include <stdlib.h>

#include "db.h"
#include "props.h"
#include "params.h"
#include "defaults.h"
#include "interface.h"
#include "match.h"
#include "externs.h"
#include "fbstrings.h"
#include "geography.h"
#include "kill.h"

void
do_teleport(command_t *cmd) {
	dbref player = cmd->player;
	const char *arg1 = cmd->argv[1];
	const char *arg2 = cmd->argv[2];
	dbref victim;
	dbref destination;
	const char *to;
	struct match_data md;

	/* get victim, destination */
	if (*arg2 == '\0') {
		victim = player;
		to = arg1;
	} else {
		init_match(cmd, arg1, NOTYPE, &md);
		match_neighbor(&md);
		match_possession(&md);
		match_me(&md);
		match_here(&md);
		match_absolute(&md);
		match_registered(&md);
		match_player(&md);

		if ((victim = noisy_match_result(&md)) == NOTHING) {
			return;
		}
		to = arg2;
	}
#ifdef GOD_PRIV
	if(!God(player) && God(OWNER(victim))) {
		notify(player, "God has already set that where He wants it to be.");
		return;
	}
#endif

	/* get destination */
	init_match(cmd, to, TYPE_PLAYER, &md);
	match_possession(&md);
	match_me(&md);
	match_here(&md);
	match_home(&md);
	match_absolute(&md);
	match_registered(&md);
	if (Wizard(OWNER(player))) {
		match_neighbor(&md);
		match_player(&md);
	}
	switch (destination = match_result(&md)) {
	case NOTHING:
		notify(player, "Send it where?");
		break;
	case AMBIGUOUS:
		notify(player, "I don't know which destination you mean!");
		break;
	case HOME:
		switch (Typeof(victim)) {
		case TYPE_PLAYER:
			destination = PLAYER_HOME(victim);
			if (parent_loop_check(victim, destination))
				destination = PLAYER_HOME(OWNER(victim));
			break;
		case TYPE_THING:
			destination = THING_HOME(victim);
			if (parent_loop_check(victim, destination)) {
			  destination = PLAYER_HOME(OWNER(victim));
			  if (parent_loop_check(victim, destination)) {
			    destination = (dbref) 0;
			  }
			}
			break;
		case TYPE_ROOM:
			destination = GLOBAL_ENVIRONMENT;
			break;
		default:
			destination = PLAYER_START;	/* caught in the next
											   * switch anyway */
			break;
		}
	default:
		switch (Typeof(victim)) {
		case TYPE_PLAYER:
			if (!controls(player, victim) ||
				!controls(player, destination) ||
				!controls(player, getloc(victim)) ||
				(Typeof(destination) == TYPE_THING && !controls(player, getloc(destination)))) {
				notify(player, "Permission denied. (must control victim, dest, victim's loc, and dest's loc)");
				break;
			}
			if (Typeof(destination) != TYPE_ROOM) {
				notify(player, "Bad destination.");
				break;
			}
			if (parent_loop_check(victim, destination)) {
				notify(player, "Objects can't contain themselves.");
				break;
			}
			notify(victim, "You feel a wrenching sensation...");
			command_t cmd_er = command_new_null(cmd->fd, victim);
			enter_room(&cmd_er, destination, DBFETCH(victim)->location);
			break;
		case TYPE_THING:
			if (parent_loop_check(victim, destination)) {
				notify(player, "You can't make a container contain itself!");
				break;
			}
			if (Typeof(destination) != TYPE_ROOM
				&& Typeof(destination) != TYPE_PLAYER && Typeof(destination) != TYPE_THING) {
				notify(player, "Bad destination.");
				break;
			}
			if (!((controls(player, destination) ||
				   can_link_to(player, NOTYPE, destination)) &&
				  (controls(player, victim) || controls(player, DBFETCH(victim)->location)))) {
				notify(player, "Permission denied. (must control dest and be able to link to it, or control dest's loc)");
				break;
			}
			/* check for non-sticky dropto */
			if (Typeof(destination) == TYPE_ROOM
				&& DBFETCH(destination)->sp.room.dropto != NOTHING
				&& !(FLAGS(destination) & STICKY))
						destination = DBFETCH(destination)->sp.room.dropto;
			moveto(victim, destination);
			notify(player, "Teleported.");
			break;
		case TYPE_ROOM:
			if (Typeof(destination) != TYPE_ROOM) {
				notify(player, "Bad destination.");
				break;
			}
			if (!controls(player, victim)
				|| !can_link_to(player, NOTYPE, destination)
				|| victim == GLOBAL_ENVIRONMENT) {
				notify(player, "Permission denied. (Can't move #0, dest must be linkable, and must control victim)");
				break;
			}
			if (parent_loop_check(victim, destination)) {
				notify(player, "Parent would create a loop.");
				break;
			}
			moveto(victim, destination);
			notify(player, "Parent set.");
			break;
		case TYPE_GARBAGE:
			notify(player, "That object is in a place where magic cannot reach it.");
			break;
		default:
			notify(player, "You can't teleport that.");
			break;
		}
		break;
	}
	return;
}

int
blessprops_wildcard(dbref player, dbref thing, const char *dir, const char *wild, int blessp)
{
	char propname[BUFFER_LEN];
	char wld[BUFFER_LEN];
	char buf[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	char *ptr, *wldcrd = wld;
	PropPtr propadr, pptr;
	int i, cnt = 0;
	int recurse = 0;

#ifdef GOD_PRIV
	if(!God(player) && God(OWNER(thing))) {
		notify(player,"Only God may touch what is God's.");
		return 0;
	}
#endif

	strlcpy(wld, wild, sizeof(wld));
	i = strlen(wld);
	if (i && wld[i - 1] == PROPDIR_DELIMITER)
		strlcat(wld, "*", sizeof(wld));
	for (wldcrd = wld; *wldcrd == PROPDIR_DELIMITER; wldcrd++) ;
	if (!strcmp(wldcrd, "**"))
		recurse = 1;

	for (ptr = wldcrd; *ptr && *ptr != PROPDIR_DELIMITER; ptr++) ;
	if (*ptr)
		*ptr++ = '\0';

	propadr = first_prop(thing, (char *) dir, &pptr, propname, sizeof(propname));
	while (propadr) {
		if (equalstr(wldcrd, propname)) {
			snprintf(buf, sizeof(buf), "%s%c%s", dir, PROPDIR_DELIMITER, propname);
			if (!Prop_System(buf) && ((!Prop_Hidden(buf) && !(PropFlags(propadr) & PROP_SYSPERMS))
				|| Wizard(OWNER(player)))) {
				if (!*ptr || recurse) {
					cnt++;
					if (blessp) {
						set_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Blessed %s", buf);
					} else {
						clear_property_flags(thing, buf, PROP_BLESSED);
						snprintf(buf2, sizeof(buf2), "Unblessed %s", buf);
					}
					notify(player, buf2);
				}
				if (recurse)
					ptr = "**";
				cnt += blessprops_wildcard(player, thing, buf, ptr, blessp);
			}
		}
		propadr = next_prop(pptr, propadr, propname, sizeof(propname));
	}
	return cnt;
}


void
do_unbless(command_t *cmd) {
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	dbref victim;
	struct match_data md;
	char buf[BUFFER_LEN];
	int cnt;

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(player, "Usage is @unbless object=propname.");
		return;
	}

	/* get victim */
	init_match(cmd, what, NOTYPE, &md);
	match_everything(&md);
	if ((victim = noisy_match_result(&md)) == NOTHING) {
		return;
	}

	if (!Wizard(OWNER(player))) {
		notify(player, "Permission denied. (You're not a wizard)");
		return;
	}

	cnt = blessprops_wildcard(player, victim, "", propname, 0);
	snprintf(buf, sizeof(buf), "%d propert%s unblessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(player, buf);
}


void
do_bless(command_t *cmd) {
	dbref player = cmd->player;
	const char *what = cmd->argv[1];
	const char *propname = cmd->argv[2];
	dbref victim;
	struct match_data md;
	char buf[BUFFER_LEN];
	int cnt;

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only Wizard players may use this command.");
		return;
	}

	if (!propname || !*propname) {
		notify(player, "Usage is @bless object=propname.");
		return;
	}

	/* get victim */
	init_match(cmd, what, NOTYPE, &md);
	match_everything(&md);
	if ((victim = noisy_match_result(&md)) == NOTHING) {
		return;
	}

#ifdef GOD_PRIV
	if(!God(player) && God(OWNER(victim))) {
		notify(player, "Only God may touch God's stuff.");
		return;
	}
#endif

	if (!Wizard(OWNER(player))) {
		notify(player, "Permission denied. (you're not a wizard)");
		return;
	}

	cnt = blessprops_wildcard(player, victim, "", propname, 1);
	snprintf(buf, sizeof(buf), "%d propert%s blessed.", cnt, (cnt == 1)? "y" : "ies");
	notify(player, buf);
}

void
do_stats(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	int rooms;
	int exits;
	int things;
	int players;
	int garbage = 0;
	int total;
	int altered = 0;
	dbref i;
	dbref owner=NOTHING;
	char buf[BUFFER_LEN];

	if (!Wizard(OWNER(player)) && (!name || !*name)) {
		snprintf(buf, sizeof(buf), "The universe contains %d objects.", db_top);
		notify(player, buf);
	} else {
		total = rooms = exits = things = players = 0;
		if (name != NULL && *name != '\0') {
			owner = lookup_player(name);
			if (owner == NOTHING) {
				notify(player, "I can't find that player.");
				return;
			}
			if (!Wizard(OWNER(player)) && (OWNER(player) != owner)) {
				notify(player, "Permission denied. (you must be a wizard to get someone else's stats)");
				return;
			}
			for (i = 0; i < db_top; i++) {


				/* count objects marked as changed. */
				if ((OWNER(i) == owner) && (FLAGS(i) & OBJECT_CHANGED))
					altered++;

				switch (Typeof(i)) {
				case TYPE_ROOM:
					if (OWNER(i) == owner)
						total++, rooms++;
					break;

				case TYPE_EXIT:
					if (OWNER(i) == owner)
						total++, exits++;
					break;

				case TYPE_THING:
					if (OWNER(i) == owner)
						total++, things++;
					break;

				case TYPE_PLAYER:
					if (i == owner)
						total++, players++;
					break;

				}
			}
		} else {
			for (i = 0; i < db_top; i++) {


				/* count objects marked as changed. */
				if (FLAGS(i) & OBJECT_CHANGED)
					altered++;

				switch (Typeof(i)) {
				case TYPE_ROOM:
					total++;
					rooms++;
					break;
				case TYPE_EXIT:
					total++;
					exits++;
					break;
				case TYPE_THING:
					total++;
					things++;
					break;
				case TYPE_PLAYER:
					total++;
					players++;
					break;
				case TYPE_GARBAGE:
					total++;
					garbage++;
					break;
				}
			}
		}

		notifyf(player, "%7d room%s        %7d exit%s        %7d thing%s",
			rooms, (rooms == 1) ? " " : "s",
			exits, (exits == 1) ? " " : "s", things, (things == 1) ? " " : "s");

		notifyf(player, "%7d player%s      %7d garbage",
			   players, (players == 1) ? " " : "s", garbage);

		notifyf(player,
			"%7d total object%s",
			total, (total == 1) ? " " : "s");

	}
}


void
do_boot(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	dbref victim;
	char buf[BUFFER_LEN];

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only a Wizard player can boot someone off.");
		return;
	}
	if ((victim = lookup_player(name)) == NOTHING) {
		notify(player, "That player does not exist.");
		return;
	}
	if (Typeof(victim) != TYPE_PLAYER) {
		notify(player, "You can only boot players!");
	}
#ifdef GOD_PRIV
	else if (God(victim)) {
		notify(player, "You can't boot God!");
	}
#endif							/* GOD_PRIV */

	else {
		notify(victim, "You have been booted off the game.");
		if (boot_off(victim)) {
			warn("BOOTED: %s(%d) by %s(%d)", NAME(victim),
					   victim, NAME(player), player);
			if (player != victim) {
				snprintf(buf, sizeof(buf), "You booted %s off!", NAME(victim));
				notify(player, buf);
			}
		} else {
			snprintf(buf, sizeof(buf), "%s is not connected.", NAME(victim));
			notify(player, buf);
		}
	}
}

void
do_toad(command_t *cmd) {
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *recip = cmd->argv[2];
	dbref victim;
	dbref recipient;
	dbref stuff;
	char buf[BUFFER_LEN];

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only a Wizard player can turn a person into a toad.");
		return;
	}
	if ((victim = lookup_player(name)) == NOTHING) {
		notify(player, "That player does not exist.");
		return;
	}
#ifdef GOD_PRIV
	if (God(victim)) {
		notify(player, "You cannot @toad God.");
		if(!God(player)) {
			warn("TOAD ATTEMPT: %s(#%d) tried to toad God.",NAME(player),player);
		}
		return;
	}
#endif
	if(player == victim) {
		/* If GOD_PRIV isn't defined, this could happen: we don't want the
		 * last wizard to be toaded, in any case, so only someone else can
		 * do it. */
		notify(player, "You cannot toad yourself.  Get someone else to do it for you.");
		return;
	}
	if (!*recip) {
		/* FIXME: Make me a tunable parameter! */
		recipient = GOD;
	} else {
		if ((recipient = lookup_player(recip)) == NOTHING || recipient == victim) {
			notify(player, "That recipient does not exist.");
			return;
		}
	}

	if (Typeof(victim) != TYPE_PLAYER) {
		notify(player, "You can only turn players into toads!");
#ifdef GOD_PRIV
	} else if (!(God(player)) && (TrueWizard(victim))) {
#else
	} else if (TrueWizard(victim)) {
#endif
		notify(player, "You can't turn a Wizard into a toad.");
	} else {
		/* we're ok */
		/* do it */
		command_t cmd_sc = command_new_null(cmd->fd, victim);
		send_contents(&cmd_sc, HOME);
		for (stuff = 0; stuff < db_top; stuff++) {
			if (OWNER(stuff) == victim) {
				switch (Typeof(stuff)) {
				case TYPE_ROOM:
				case TYPE_THING:
				case TYPE_EXIT:
					OWNER(stuff) = recipient;
					DBDIRTY(stuff);
					break;
				}
			}
			if (Typeof(stuff) == TYPE_THING && THING_HOME(stuff) == victim) {
				/* FIXME: Set a tunable "lost and found" area! */
				THING_SET_HOME(stuff, PLAYER_START);
			}
		}
		if (PLAYER_PASSWORD(victim)) {
			free((void *) PLAYER_PASSWORD(victim));
			PLAYER_SET_PASSWORD(victim, 0);
		}

		/* notify people */
		notify(victim, "You have been turned into a toad.");
		snprintf(buf, sizeof(buf), "You turned %s into a toad!", NAME(victim));
		notify(player, buf);
		warn("TOADED: %s(%d) by %s(%d)", NAME(victim), victim, NAME(player), player);
		/* reset name */
		delete_player(victim);
		snprintf(buf, sizeof(buf), "A slimy toad named %s", NAME(victim));
		free((void *) NAME(victim));
		NAME(victim) = alloc_string(buf);
		DBDIRTY(victim);

		boot_player_off(victim); /* Disconnect the toad */

		FREE_PLAYER_SP(victim);
		ALLOC_THING_SP(victim);
		THING_SET_HOME(victim, PLAYER_HOME(player));

		FLAGS(victim) = (FLAGS(victim) & ~TYPE_MASK) | TYPE_THING;
		OWNER(victim) = player;	/* you get it */
		SETVALUE(victim, 1);	/* don't let him keep his immense wealth */
	}
}

void
do_newpassword(command_t *cmd)
{
	dbref player = cmd->player;
	const char *name = cmd->argv[1];
	const char *password = cmd->argv[2];
	dbref victim;
	char buf[BUFFER_LEN];

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only a Wizard player can newpassword someone.");
		return;
	} else if ((victim = lookup_player(name)) == NOTHING) {
		notify(player, "No such player.");
	} else if (*password != '\0' && !ok_password(password)) {
		/* Wiz can set null passwords, but not bad passwords */
		notify(player, "Bad password");

#ifdef GOD_PRIV
	} else if (God(victim)) {
		notify(player, "You can't change God's password!");
		return;
	} else {
		if (TrueWizard(victim) && !God(player)) {
			notify(player, "Only God can change a wizard's password.");
			return;
		}
#else							/* GOD_PRIV */
	} else {
#endif							/* GOD_PRIV */

		/* it's ok, do it */
		set_password(victim, password);
		DBDIRTY(victim);
		notify(player, "Password changed.");
		snprintf(buf, sizeof(buf), "Your password has been changed by %s.", NAME(player));
		notify(victim, buf);
		warn("NEWPASS'ED: %s(%d) by %s(%d)", NAME(victim), victim,
				   NAME(player), player);
	}
}

void
do_pcreate(command_t *cmd) {
	dbref player = cmd->player;
	const char *user = cmd->argv[1];
	const char *password = cmd->argv[2];
	dbref newguy;
	char buf[BUFFER_LEN];

	if (!Wizard(player) || Typeof(player) != TYPE_PLAYER) {
		notify(player, "Only a Wizard player can create a player.");
		return;
	}
	newguy = create_player(user, password);
	if (newguy == NOTHING) {
		notify(player, "Create failed.");
	} else {
		warn("PCREATED %s(%d) by %s(%d)", NAME(newguy), newguy, NAME(player), player);
		mob_put(newguy);
		snprintf(buf, sizeof(buf), "Player %s created as object #%d.", user, newguy);
		notify(player, buf);
	}
}

void
do_usage(command_t *cmd) {
	dbref player = cmd->player;
	int pid, psize;

#ifdef HAVE_GETRUSAGE
	struct rusage usage;
#endif

	if (!Wizard(OWNER(player))) {
		notify(player, "Permission denied. (@usage is wizard-only)");
		return;
	}
#ifndef NO_USAGE_COMMAND
	pid = getpid();
#ifdef HAVE_GETRUSAGE
	psize = getpagesize();
	getrusage(RUSAGE_SELF, &usage);
#endif

	notifyf(player, "Compiled on: %s", UNAME_VALUE);
	notifyf(player, "Process ID: %d", pid);
	notifyf(player, "Max descriptors/process: %ld", sysconf(_SC_OPEN_MAX));

#ifdef HAVE_GETRUSAGE
	notifyf(player, "Performed %d input servicings.", usage.ru_inblock);
	notifyf(player, "Performed %d output servicings.", usage.ru_oublock);
	notifyf(player, "Sent %d messages over a socket.", usage.ru_msgsnd);
	notifyf(player, "Received %d messages over a socket.", usage.ru_msgrcv);
	notifyf(player, "Received %d signals.", usage.ru_nsignals);
	notifyf(player, "Page faults NOT requiring physical I/O: %d", usage.ru_minflt);
	notifyf(player, "Page faults REQUIRING physical I/O: %d", usage.ru_majflt);
	notifyf(player, "Swapped out of main memory %d times.", usage.ru_nswap);
	notifyf(player, "Voluntarily context switched %d times.", usage.ru_nvcsw);
	notifyf(player, "Involuntarily context switched %d times.", usage.ru_nivcsw);
	notifyf(player, "User time used: %d sec.", usage.ru_utime.tv_sec);
	notifyf(player, "System time used: %d sec.", usage.ru_stime.tv_sec);
	notifyf(player, "Pagesize for this machine: %d", psize);
	notifyf(player, "Maximum resident memory: %ldk",
			   (long) (usage.ru_maxrss * (psize / 1024)));
	notifyf(player, "Integral resident memory: %ldk",
			   (long) (usage.ru_idrss * (psize / 1024)));
#endif							/* HAVE_GETRUSAGE */

#else							/* NO_USAGE_COMMAND */

	notify(player, "Sorry, this server was compiled with NO_USAGE_COMMAND.");

#endif							/* NO_USAGE_COMMAND */
}

static const char *wiz_c_version = "$RCSfile$ $Revision: 1.38 $";
const char *get_wiz_c_version(void) { return wiz_c_version; }

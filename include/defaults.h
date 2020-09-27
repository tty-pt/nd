#ifndef _DEFAULTS_H
#define _DEFAULTS_H

/* #define MUD_ID "www" */

#define DUMPWARN_MESG   "## Game will pause to save the database in a few minutes. ##"
#define DELTAWARN_MESG  "## Game will pause to save changed objects in a few minutes. ##"
#define DUMPDELTAS_MESG "## Saving changed objects ##"
#define DUMPING_MESG    "## Pausing to save database. This may take a while. ##"
#define DUMPDONE_MESG   "## Save complete. ##"


/* Change this to the name of your muck.  ie: FurryMUCK, or Tapestries */
#define MUCKNAME "NeverDark"

/* name of the monetary unit being used */
#define PENNY "penny"
#define PENNIES "pennies"
#define CPENNY "Penny"
#define CPENNIES "Pennies"

/* message seen when a player enters a line the server doesn't understand */
#define HUH_MESSAGE "Huh?  (Type \"help\" for help.)"

/*  Goodbye message.  */
#define LEAVE_MESSAGE "Come back later!"

/*  Idleboot message.  */
#define IDLEBOOT_MESSAGE "Autodisconnecting for inactivity."

/*  How long someone can idle for.  */
#define MAXIDLE TIME_HOUR(2)

/*  Boot idle players?  */
#define IDLEBOOT 1

/* ping anti-idle time in seconds - just under 1 minute NAT timeout */
#define IDLE_PING_TIME 55

/* Limit max number of players to allow connected?  (wizards are immune) */
#define PLAYERMAX 0

/* How many connections before blocking? */
#define PLAYERMAX_LIMIT 56

/* The mesg to warn users that nonwizzes can't connect due to connect limits */
#define PLAYERMAX_WARNMESG "You likely won't be able to connect right now, since too many players are online."

/* The mesg to display when booting a connection because of connect limit */
#define PLAYERMAX_BOOTMESG "Sorry, but there are too many players online.  Please try reconnecting in a few minutes."

/* command to run when entering a room. */
#define AUTOLOOK_CMD "look"

/* Format of standardized property lists. */
#define PROPLIST_INT_COUNTER 1	/* Specifies that the proplist counter
				   property should be stored as an integer
				   property instead of as a string. */

#define PROPLIST_COUNTER_FORMAT "P#"	/* The prop that has the count of lines. */
									 /* Define as empty string for no counter. */
									 /* P substitutes the proplist name */
									 /* All other chars are literal. */

#define PROPLIST_ENTRY_FORMAT "P#/N"	/* The props that contain the list data. */
									 /* P substitutes the proplist name */
									 /* N substitutes the item number */
									 /* All other chars are literal. */

/* amount of object endowment, based on cost */
#define MAX_OBJECT_ENDOWMENT 100

/* minimum costs for various things */
#define OBJECT_COST 10			/* Amount it costs to make an object    */
#define EXIT_COST 1				/* Amount it costs to make an exit      */
#define LINK_COST 1				/* Amount it costs to make a link       */
#define ROOM_COST 80			/* Amount it costs to dig a room        */
#define LOOKUP_COST 0			/* cost to do a scan                    */
#define MAX_PENNIES 10000		/* amount at which temple gets cheap    */
#define PENNY_RATE 0			/* 1/chance of getting a penny per room */
#define START_PENNIES 50		/* # of pennies a player's created with */

#define PLAYER_START ((dbref) 17)	/* room number of player start location */

/* Server support of @doing (reads the _/do prop on WHOs) */
#define WHO_DOING 1

/* clear out unused programs every so often */

/* Define these to let players set TYPE_THING and TYPE_EXIT objects dark. */
#define EXIT_DARKING 1
#define THING_DARKING 1

/* Define this to 1 if you want DARK players to not show up on the WHO list
 * for mortals, in addition to not showing them in the room contents. */
#define WHO_HIDES_DARK 1

/* Allow a player to use teleport-to-player destinations for exits */
#define TELEPORT_TO_PLAYER 1

/* Enable or diable the global 'HOME' command. */
#define ALLOW_HOME 1

/* With this defined to 1, exits that aren't on TYPE_THING objects will */
/* always act as if they have at least a Priority Level of 1.  */
/* Define this if you want to use this server with an old db, and don't want */
/* to re-set the Levels of all the LOOK, DISCONNECT, and CONNECT actions. */
#define COMPATIBLE_PRIORITIES 1

/* Flags that new players will be created with. */
#define PCREATE_FLAGS "C"

/* Smatch pattern of names that cannot be used. */
#define RESERVED_NAMES ""

/* Smatch pattern of player names that cannot be used. */
#define RESERVED_PLAYER_NAMES ""

/* Enable support for ignoring players */
#define IGNORE_SUPPORT 1

/* Enable bidirectional ignoring for players */
#define IGNORE_BIDIRECTIONAL 1

/* Force 7-bit names */
#if 1 /* ASCII_THING_NAMES */
#define OK_ASCII_THING(name) 1
#define OK_ASCII_OTHER(name) ok_ascii_any(name)
#else
#define OK_ASCII_THING(name) ok_ascii_any(name)
#define OK_ASCII_OTHER(name) 1
#endif

#endif /* _DEFAULTS_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef defaultsh_version
#define defaultsh_version
const char *defaults_h_version = "$RCSfile$ $Revision: 1.34 $";
#endif
#else
extern const char *defaults_h_version;
#endif


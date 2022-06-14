#ifndef _DEFAULTS_H
#define _DEFAULTS_H

/* name of the monetary unit being used */
#define PENNY "penny"
#define PENNIES "pennies"
#define CPENNY "Penny"
#define CPENNIES "Pennies"

/* Limit max number of players to allow connected?  (wizards are immune) */
#define PLAYERMAX 0

/* How many connections before blocking? */
#define PLAYERMAX_LIMIT 56

/* The mesg to warn users that nonwizzes can't connect due to connect limits */
#define PLAYERMAX_WARNMESG "You likely won't be able to connect right now, since too many players are online."

/* amount of object endowment, based on cost */
#define MAX_OBJECT_ENDOWMENT 100

/* minimum costs for various things */
#define OBJECT_COST 10			/* Amount it costs to make an object    */
#define EXIT_COST 1				/* Amount it costs to make an exit      */
#define LINK_COST 1				/* Amount it costs to make a link       */
#define ROOM_COST 80			/* Amount it costs to dig a room        */
#define LOOKUP_COST 0			/* cost to do a scan                    */
#define MAX_PENNIES 10000		/* amount at which temple gets cheap    */
#define START_PENNIES 50		/* # of pennies a player's created with */

#define PLAYER_START ((dbref) 0)	/* room number of player start location */
#define STARTING_POSITION 0x0		/* starting geographical position */

#define OK_ASCII_THING(name) 1
#define OK_ASCII_OTHER(name) ok_ascii_any(name)

#endif /* _DEFAULTS_H */

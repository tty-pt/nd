#!/bin/sh

#
# $Header$
#
# Generates version.c
#
# Copyright (c) 1990 Chelsea Dyerman
# University of California, Berkeley (XCF)
#


if [ ! -f version.c ]
then
        generation=0
else
        generation=`sed -n 's/^const char \*generation = \"\(.*\)\";/\1/p' < version.c`
        if [ ! "$generation" ]; then generation=0; fi
fi

generation=`expr $generation + 1`
creation=`date | awk '{if (NF == 6) { print $1 " "  $2 " " $3 " "  $6 " at " $4 " " $5 } else { print $1 " "  $2 " " $3 " " $7 " at " $4 " " $5 " " $6 }}'`
cat > version.c <<EOF
/*
 * Copyright (c) 1991-2000 by Fuzzball Software
 * under the Gnu Public License
 *
 * Some parts of this code -- in particular the dictionary based compression
 * code is Copyright 1995 by Dragon's Eye Productions
 *
 * Some parts of this code Copyright (c) 1990 Chelsea Dyerman
 * University of California, Berkeley (XCF)
 */

/*
 * This file is generated by mkversion.sh. Any changes made will go away.
 */

#include <sys/time.h>
#include "fb.h"
#include "config.h"
#include "patchlevel.h"
#include "params.h"
#include "externs.h"

#define generation "$generation"
#define creation "$creation"
const char *version = PATCHLEVEL;
#ifdef DEBUG
#define debug "Debug Version, assertions enabled"
#else
#define debug ""
#endif


const char *infotext[] =
{
    VERSION,
    debug,
    generation,
    creation,
    " ",
    "Based on the original code written by these programmers:",
    "  David Applegate    James Aspnes    Timothy Freeman    Bennet Yee",
    " ",
    "Others who have done major coding work along the way:",
    "  Lachesis, ChupChups, FireFoot, Russ 'Random' Smith, and Dr. Cat",
    " ",
    "This is a user-extendible, user-programmable multi-user adventure game.",
    "TinyMUCK was derived from TinyMUD v1.5.2, with extensive modifications.",
    "Because of all the modifications, this program is not in any way, shape,",
    "or form being supported by any of the original authors.  Any bugs, ideas,",
    "suggestions,  etc, should be directed to the persons listed below.",
    "Do not send diff files, send us mail about the bug and describe as best",
    "as you can, where you were at when the bug occured, and what you think",
    "caused the bug to be produced, so we can try to reproduce it and track",
    "it down.",
    " ",
    "The following programmers currently maintain the code:",
    "  Foxen/Revar:   revar@belfry.com    Project Lead, Developer",
    "  Points:                            Bug Finder/Lead/Developer",
    " ",
    "  Our Sourceforge developers (http://sourceforge.net/projects/fbmuck/):",
    "  Akari, Alynna, Arcticwolf (Schneelocke), Cerilus, Fentonator,",
    "  Ferretbun, Foxbird, Jenora, McClure, Naiya, Natasha O'Brien, Sombre,",
    "  Tiger (Fre'ta), Winged, Wog, and WolfWings.",
    " ",
    "The following people helped out a lot along the way:",
    "  Caspian, Kim \\"Bookwyrm\\" Liu, Chris, Jenora, Lynx, WhiteFire,",
    "  Kimi, Cynbe, Myk, Taldin, Howard, darkfox, Moonchilde, Felorin, Xixia,",
    "  Doran, Riss, King_Claudius, and Henri@Voregotten Realm.",
    " ",
    "Alpha and beta test sites, who put up with this nonsense:",
    " FB 5-",
    "  HighSeasMUCK, TygryssMUCK, FurryMUCK, CyberFurry, PendorMUCK, Kalasia,",
    "  AnimeMUCK, Realms, FurryII, Tapestries, Unbridled Desires, TruffleMUCK",
    "  and Brazillian Dreams.",
    " FB 6-",
    "  HereLieMonsters, FurrySpaceMUCK, Sol ]|[, and Voregotten Realm.",
    " ",
    "Places silly enough to give Foxen a wizbit at some time or another:",
    "  ChupMuck, HighSeas, TygMUCK, TygMUCK II, Furry, Pendor, Realms,",
    "  Kalasia, Anime, CrossRoadsMUCK, TestMage, MeadowFaire, TruffleMUCK,",
    "  Tapestries, Brazillian Dreams, SocioPolitical Ramifications and more.",
    " ",
    "Thanks also goes to those persons not mentioned here who have added",
    "their advice, opinions, and code to TinyMUCK FB.",
    0,
};


void
do_credits(dbref player)
{
    int i;

    for (i = 0; infotext[i]; i++) {
        notify(player, infotext[i]);
    }
}

void
do_version(dbref player)
{
	char s[BUFFER_LEN];

	snprintf(s,BUFFER_LEN,"Version: %s Compiled on: %s %s",VERSION,creation,debug);
	notify(player, s);
	return;
}


void
do_showextver(dbref player) {
	notify_nolisten(player, get_array_c_version(), 1);
	notify_nolisten(player, get_boolexp_c_version(), 1);
	notify_nolisten(player, get_compile_c_version(), 1);
	notify_nolisten(player, get_copyright_c_version(), 1);
	notify_nolisten(player, get_create_c_version(), 1);
	notify_nolisten(player, get_crt_malloc_c_version(), 1);
	notify_nolisten(player, get_db_c_version(), 1);
	notify_nolisten(player, get_debugger_c_version(), 1);
	notify_nolisten(player, get_disassem_c_version(), 1);
	notify_nolisten(player, get_diskprop_c_version(), 1);
	notify_nolisten(player, get_edit_c_version(), 1);
	notify_nolisten(player, get_events_c_version(), 1);
	notify_nolisten(player, get_game_c_version(), 1);
	notify_nolisten(player, get_hashtab_c_version(), 1);
	notify_nolisten(player, get_help_c_version(), 1);
	notify_nolisten(player, get_inst_c_version(), 1);
	notify_nolisten(player, get_interface_c_version(), 1);
	notify_nolisten(player, get_interp_c_version(), 1);
	notify_nolisten(player, get_log_c_version(), 1);
	notify_nolisten(player, get_look_c_version(), 1);
	notify_nolisten(player, get_match_c_version(), 1);
	notify_nolisten(player, get_mcp_c_version(), 1);
	notify_nolisten(player, get_mcpgui_c_version(), 1);
	notify_nolisten(player, get_mcppkgs_c_version(), 1);
	notify_nolisten(player, get_mfuns_c_version(), 1);
	notify_nolisten(player, get_mfuns2_c_version(), 1);
	notify_nolisten(player, get_move_c_version(), 1);
	notify_nolisten(player, get_msgparse_c_version(), 1);
	notify_nolisten(player, get_mufevent_c_version(), 1);
	notify_nolisten(player, get_oldcompress_c_version(), 1);
	notify_nolisten(player, get_p_array_c_version(), 1);
	notify_nolisten(player, get_p_connects_c_version(), 1);
	notify_nolisten(player, get_p_db_c_version(), 1);
	notify_nolisten(player, get_p_error_c_version(), 1);
	notify_nolisten(player, get_p_float_c_version(), 1);
	notify_nolisten(player, get_p_math_c_version(), 1);
	notify_nolisten(player, get_p_mcp_c_version(), 1);
	notify_nolisten(player, get_p_misc_c_version(), 1);
	notify_nolisten(player, get_p_props_c_version(), 1);
	notify_nolisten(player, get_p_regex_c_version(), 1);
	notify_nolisten(player, get_p_stack_c_version(), 1);
	notify_nolisten(player, get_p_strings_c_version(), 1);
	notify_nolisten(player, get_player_c_version(), 1);
	notify_nolisten(player, get_predicates_c_version(), 1);
	notify_nolisten(player, get_propdirs_c_version(), 1);
	notify_nolisten(player, get_property_c_version(), 1);
	notify_nolisten(player, get_props_c_version(), 1);
	notify_nolisten(player, get_random_c_version(), 1);
	notify_nolisten(player, get_rob_c_version(), 1);
	notify_nolisten(player, get_sanity_c_version(), 1);
	notify_nolisten(player, get_set_c_version(), 1);
	notify_nolisten(player, get_signal_c_version(), 1);
	notify_nolisten(player, get_smatch_c_version(), 1);
	notify_nolisten(player, get_snprintf_c_version(), 1);
	notify_nolisten(player, get_speech_c_version(), 1);
	notify_nolisten(player, get_strftime_c_version(), 1);
	notify_nolisten(player, get_stringutil_c_version(), 1);
	notify_nolisten(player, get_timequeue_c_version(), 1);
	notify_nolisten(player, get_timestamp_c_version(), 1);
	notify_nolisten(player, get_tune_c_version(), 1);
	notify_nolisten(player, get_unparse_c_version(), 1);
	notify_nolisten(player, get_utils_c_version(), 1);
	notify_nolisten(player, get_wiz_c_version(), 1);
	notify_nolisten(player, array_h_version, 1);
	notify_nolisten(player, config_h_version, 1);
	notify_nolisten(player, copyright_h_version, 1);
#ifdef MALLOC_PROFILING
	notify_nolisten(player, crt_malloc_h_version, 1);
#else
	notify_nolisten(player, "crt_malloc.h not used", 1);
#endif
	notify_nolisten(player, db_h_version, 1);
	notify_nolisten(player, dbsearch_h_version, 1);
	notify_nolisten(player, defaults_h_version, 1);
	notify_nolisten(player, externs_auto_h_version, 1);
	notify_nolisten(player, externs_h_version, 1);
	notify_nolisten(player, fb_h_version, 1);
	notify_nolisten(player, fbstrings_h_version, 1);
	notify_nolisten(player, inst_h_version, 1);
	notify_nolisten(player, interface_h_version, 1);
	notify_nolisten(player, interp_h_version, 1);
	notify_nolisten(player, match_h_version, 1);
	notify_nolisten(player, mcp_h_version, 1);
	notify_nolisten(player, mcpgui_h_version, 1);
	notify_nolisten(player, mcppkg_h_version, 1);
	notify_nolisten(player, mfun_h_version, 1);
	notify_nolisten(player, mfunlist_h_version, 1);
	notify_nolisten(player, mpi_h_version, 1);
	notify_nolisten(player, msgparse_h_version, 1);
	notify_nolisten(player, mufevent_h_version, 1);
	notify_nolisten(player, p_array_h_version, 1);
	notify_nolisten(player, p_connects_h_version, 1);
	notify_nolisten(player, p_db_h_version, 1);
	notify_nolisten(player, p_error_h_version, 1);
	notify_nolisten(player, p_float_h_version, 1);
	notify_nolisten(player, p_math_h_version, 1);
	notify_nolisten(player, p_mcp_h_version, 1);
	notify_nolisten(player, p_misc_h_version, 1);
	notify_nolisten(player, p_props_h_version, 1);
	notify_nolisten(player, p_regex_h_version, 1);
	notify_nolisten(player, p_stack_h_version, 1);
	notify_nolisten(player, p_strings_h_version, 1);
	notify_nolisten(player, params_h_version, 1);
	notify_nolisten(player, patchlevel_h_version, 1);
	notify_nolisten(player, props_h_version, 1);
	notify_nolisten(player, smatch_h_version, 1);
	notify_nolisten(player, speech_h_version, 1);
	notify_nolisten(player, tune_h_version, 1);
	return;
}

EOF

exit 0

#include "config.h"
#include "params.h"
#include "db.h"
#include "defaults.h"
#include "externs.h"
#include "array.h"
#include "interp.h"


const char *tp_dumpwarn_mesg = DUMPWARN_MESG;
const char *tp_deltawarn_mesg = DELTAWARN_MESG;
const char *tp_dumpdeltas_mesg = DUMPDELTAS_MESG;
const char *tp_dumping_mesg = DUMPING_MESG;
const char *tp_dumpdone_mesg = DUMPDONE_MESG;

const char *tp_penny = PENNY;
const char *tp_pennies = PENNIES;
const char *tp_cpenny = CPENNY;
const char *tp_cpennies = CPENNIES;

const char *tp_muckname = MUCKNAME;

const char *tp_huh_mesg = HUH_MESSAGE;
const char *tp_leave_mesg = LEAVE_MESSAGE;
const char *tp_idle_mesg = IDLEBOOT_MESSAGE;
const char *tp_register_mesg = REG_MSG;

const char *tp_playermax_warnmesg = PLAYERMAX_WARNMESG;
const char *tp_playermax_bootmesg = PLAYERMAX_BOOTMESG;

const char *tp_autolook_cmd = AUTOLOOK_CMD;

const char *tp_proplist_counter_fmt = PROPLIST_COUNTER_FORMAT;
const char *tp_proplist_entry_fmt = PROPLIST_ENTRY_FORMAT;

const char *tp_ssl_keyfile_passwd = SSL_KEYFILE_PASSWD;
const char *tp_pcreate_flags = PCREATE_FLAGS;
const char *tp_reserved_names = RESERVED_NAMES;
const char *tp_reserved_player_names = RESERVED_PLAYER_NAMES;

struct tune_str_entry {
	const char *group;
	const char *name;
	const char **str;
	int security;
	int isdefault;
	const char *label;
};

struct tune_str_entry tune_str_list[] = {
	{"Commands",   "autolook_cmd", &tp_autolook_cmd, 0, 1, "Room entry look command"},
	{"Currency",   "penny", &tp_penny, 0, 1, "Currency name"},
	{"Currency",   "pennies", &tp_pennies, 0, 1, "Currency name, plural"},
	{"Currency",   "cpenny", &tp_cpenny, 0, 1, "Currency name, capitalized"},
	{"Currency",   "cpennies", &tp_cpennies, 0, 1, "Currency name, capitalized, plural"},
	{"DB Dumps",   "dumpwarn_mesg", &tp_dumpwarn_mesg, 0, 1, "Full dump warning mesg"},
	{"DB Dumps",   "deltawarn_mesg", &tp_deltawarn_mesg, 0, 1, "Delta dump warning mesg"},
	{"DB Dumps",   "dumping_mesg", &tp_dumping_mesg, 0, 1, "Full dump start mesg"},
	{"DB Dumps",   "dumpdeltas_mesg", &tp_dumpdeltas_mesg, 0, 1, "Delta dump start mesg"},
	{"DB Dumps",   "dumpdone_mesg", &tp_dumpdone_mesg, 0, 1, "Dump completion message"},
	{"Idle Boot",  "idle_boot_mesg", &tp_idle_mesg, 0, 1, "Boot message for idling out"},
	{"Player Max", "playermax_warnmesg", &tp_playermax_warnmesg, 0, 1, "Max. players login warning"},
	{"Player Max", "playermax_bootmesg", &tp_playermax_bootmesg, 0, 1, "Max. players boot message"},
	{"Properties", "proplist_counter_fmt", &tp_proplist_counter_fmt, 0, 1, "Proplist counter name format"},
	{"Properties", "proplist_entry_fmt", &tp_proplist_entry_fmt, 0, 1, "Proplist entry name format"},
	{"Registration", "register_mesg", &tp_register_mesg, 0, 1, "Login registration mesg"},
	{"Misc",       "muckname", &tp_muckname, 0, 1, "Muck name"},
	{"Misc",       "leave_mesg", &tp_leave_mesg, 0, 1, "Logoff message"},
	{"Misc",       "huh_mesg", &tp_huh_mesg, 0, 1, "Command unrecognized warning"},
	{"SSL",        "ssl_keyfile_passwd", &tp_ssl_keyfile_passwd, 4, 1, "Password for SSL keyfile"},
	{"Database",   "pcreate_flags", &tp_pcreate_flags, 0, 1, "Initial Player Flags"},
	{"Database",   "reserved_names", &tp_reserved_names, 0, 1, "Reserved names smatch"},
	{"Database",   "reserved_player_names", &tp_reserved_player_names, 0, 1, "Reserved player names smatch"},

	{NULL, NULL, NULL, 0, 0}
};



/* times */
int tp_dump_interval = DUMP_INTERVAL;
int tp_dump_warntime = DUMP_WARNTIME;
int tp_monolithic_interval = MONOLITHIC_INTERVAL;
int tp_clean_interval = CLEAN_INTERVAL;
int tp_aging_time = AGING_TIME;
int tp_maxidle = MAXIDLE;


struct tune_time_entry {
	const char *group;
	const char *name;
	int *tim;
	int security;
	const char *label;
};

struct tune_time_entry tune_time_list[] = {
	{"Database",  "aging_time", &tp_aging_time, 0, "When to considered an object old and unused"},
	{"DB Dumps",  "dump_interval", &tp_dump_interval, 0, "Interval between delta dumps"},
	{"DB Dumps",  "dump_warntime", &tp_dump_warntime, 0, "Interval between warning and dump"},
	{"DB Dumps",  "monolithic_interval", &tp_monolithic_interval, 0, "Interval between full dumps"},
	{"Idle Boot", "maxidle", &tp_maxidle, 0, "Maximum idle time before booting"},
	{"Tuning",    "clean_interval", &tp_clean_interval, 0, "Interval between memory cleanups."},

	{NULL, NULL, NULL, 0}
};



/* integers */
int tp_max_object_endowment = MAX_OBJECT_ENDOWMENT;
int tp_object_cost = OBJECT_COST;
int tp_exit_cost = EXIT_COST;
int tp_link_cost = LINK_COST;
int tp_room_cost = ROOM_COST;
int tp_lookup_cost = LOOKUP_COST;
int tp_max_pennies = MAX_PENNIES;
int tp_penny_rate = PENNY_RATE;
int tp_start_pennies = START_PENNIES;

int tp_kill_base_cost = KILL_BASE_COST;
int tp_kill_min_cost = KILL_MIN_COST;
int tp_kill_bonus = KILL_BONUS;

int tp_command_burst_size = COMMAND_BURST_SIZE;
int tp_commands_per_time = COMMANDS_PER_TIME;
int tp_command_time_msec = COMMAND_TIME_MSEC;
int tp_max_output = MAX_OUTPUT;

int tp_max_delta_objs = MAX_DELTA_OBJS;
int tp_max_loaded_objs = MAX_LOADED_OBJS;
int tp_max_process_limit = MAX_PROCESS_LIMIT;
int tp_max_plyr_processes = MAX_PLYR_PROCESSES;
int tp_max_instr_count = MAX_INSTR_COUNT;
int tp_max_ml4_preempt_count = MAX_ML4_PREEMPT_COUNT;
int tp_instr_slice = INSTR_SLICE;
int tp_mpi_max_commands = MPI_MAX_COMMANDS;
int tp_pause_min = PAUSE_MIN;
int tp_free_frames_pool = FREE_FRAMES_POOL;
int tp_listen_mlev = LISTEN_MLEV;
int tp_playermax_limit = PLAYERMAX_LIMIT;
int tp_process_timer_limit = PROCESS_TIMER_LIMIT;
int tp_cmd_log_threshold_msec = CMD_LOG_THRESHOLD_MSEC;
int tp_userlog_mlev = USERLOG_MLEV;

int tp_mcp_muf_mlev = MCP_MUF_MLEV;

int tp_movepennies_muf_mlev = MOVEPENNIES_MUF_MLEV;
int tp_addpennies_muf_mlev = ADDPENNIES_MUF_MLEV;
int tp_pennies_muf_mlev = PENNIES_MUF_MLEV;

struct tune_val_entry {
	const char *group;
	const char *name;
	int *val;
	int security;
	const char *label;
};

struct tune_val_entry tune_val_list[] = {
	{"Costs",       "max_object_endowment", &tp_max_object_endowment, 0, "Max value of object"},
	{"Costs",       "object_cost", &tp_object_cost, 0, "Cost to create thing"},
	{"Costs",       "exit_cost", &tp_exit_cost, 0, "Cost to create exit"},
	{"Costs",       "link_cost", &tp_link_cost, 0, "Cost to link exit"},
	{"Costs",       "room_cost", &tp_room_cost, 0, "Cost to create room"},
	{"Costs",       "lookup_cost", &tp_lookup_cost, 0, "Cost to lookup playername"},
	{"Currency",    "max_pennies", &tp_max_pennies, 0, "Player currency cap"},
	{"Currency",    "penny_rate", &tp_penny_rate, 0, "Moves between finding currency, avg"},
	{"Currency",    "start_pennies", &tp_start_pennies, 0, "Player starting currency count"},
	{"Killing",     "kill_base_cost", &tp_kill_base_cost, 0, "Cost to guarentee kill"},
	{"Killing",     "kill_min_cost", &tp_kill_min_cost, 0, "Min cost to kill"},
	{"Killing",     "kill_bonus", &tp_kill_bonus, 0, "Bonus to killed player"},
	{"Listeners",   "listen_mlev", &tp_listen_mlev, 0, "Mucker Level required for Listener progs"},
	{"Logging",     "cmd_log_threshold_msec", &tp_cmd_log_threshold_msec, 0, "Log commands that take longer than X millisecs"},
	{"MUF",         "max_process_limit", &tp_max_process_limit, 0, "Max concurrent processes on system"},
	{"MUF",         "max_plyr_processes", &tp_max_plyr_processes, 0, "Max concurrent processes per player"},
	{"MUF",         "max_instr_count", &tp_max_instr_count, 0, "Max MUF instruction run length for ML1"},
	{"MUF",         "max_ml4_preempt_count", &tp_max_ml4_preempt_count, 0, "Max MUF preempt instruction run length for ML4, (0 = no limit)"},
	{"MUF",         "instr_slice", &tp_instr_slice, 0, "Instructions run per timeslice"},
	{"MUF",         "process_timer_limit", &tp_process_timer_limit, 0, "Max timers per process"},
	{"MUF",         "mcp_muf_mlev", &tp_mcp_muf_mlev, 0, "Mucker Level required to use MCP"},
	{"MUF",		"userlog_mlev", &tp_userlog_mlev, 0, "Mucker Level required to write to userlog"},
	{"MUF",         "movepennies_muf_mlev", &tp_movepennies_muf_mlev, 0, "Mucker Level required to move pennies non-destructively"},
	{"MUF",         "addpennies_muf_mlev", &tp_addpennies_muf_mlev, 0, "Mucker Level required to create/destroy pennies"},
	{"MUF",         "pennies_muf_mlev", &tp_pennies_muf_mlev, 0, "Mucker Level required to read the value of pennies, settings above 1 disable {money}"},
	{"MPI",         "mpi_max_commands", &tp_mpi_max_commands, 0, "Max MPI instruction run length"},
	{"Player Max",  "playermax_limit", &tp_playermax_limit, 0, "Max player connections allowed"},
	{"Spam Limits", "command_burst_size", &tp_command_burst_size, 0, "Commands before limiter engages"},
	{"Spam Limits", "commands_per_time", &tp_commands_per_time, 0, "Commands allowed per time period"},
	{"Spam Limits", "command_time_msec", &tp_command_time_msec, 0, "Millisecs per spam limiter time period"},
	{"Spam Limits", "max_output", &tp_max_output, 0, "Max output buffer size"},
	{"Tuning",      "pause_min", &tp_pause_min, 0, "Min ms to pause between MUF timeslices"},
	{"Tuning",      "free_frames_pool", &tp_free_frames_pool, 0, "Size of MUF process frame pool"},
	{"Tuning",      "max_delta_objs", &tp_max_delta_objs, 0, "Percentage changed objects to force full dump"},
	{"Tuning",      "max_loaded_objs", &tp_max_loaded_objs, 0, "Max proploaded object percentage"},

	{NULL, NULL, NULL, 0}
};




/* dbrefs */
dbref tp_player_start = PLAYER_START;
dbref tp_default_room_parent = GLOBAL_ENVIRONMENT;


struct tune_ref_entry {
	const char *group;
	const char *name;
	int typ;
	dbref *ref;
	int security;
	const char *label;
};

struct tune_ref_entry tune_ref_list[] = {
	{"Database", "default_room_parent", TYPE_ROOM, &tp_default_room_parent, 0, "Place to parent new rooms to"},
	{"Database", "player_start", TYPE_ROOM, &tp_player_start, 0, "Place where new players start"},

	{NULL, NULL, 0, NULL, 0}
};


/* booleans */
int tp_hostnames = HOSTNAMES;
int tp_log_commands = LOG_COMMANDS;
int tp_log_failed_commands = LOG_FAILED_COMMANDS;
int tp_log_programs = LOG_PROGRAMS;
int tp_dbdump_warning = DBDUMP_WARNING;
int tp_deltadump_warning = DELTADUMP_WARNING;
int tp_dumpdone_warning = DUMPDONE_WARNING;
int tp_periodic_program_purge = PERIODIC_PROGRAM_PURGE;
int tp_secure_who = SECURE_WHO;
int tp_who_doing = WHO_DOING;
int tp_realms_control = REALMS_CONTROL;
int tp_listeners = LISTENERS;
int tp_listeners_obj = LISTENERS_OBJ;
int tp_listeners_env = LISTENERS_ENV;
int tp_zombies = ZOMBIES;
int tp_wiz_vehicles = WIZ_VEHICLES;
int tp_force_mlev1_name_notify = FORCE_MLEV1_NAME_NOTIFY;
int tp_restrict_kill = RESTRICT_KILL;
int tp_registration = REGISTRATION;
int tp_teleport_to_player = TELEPORT_TO_PLAYER;
int tp_secure_teleport = SECURE_TELEPORT;
int tp_exit_darking = EXIT_DARKING;
int tp_thing_darking = THING_DARKING;
int tp_dark_sleepers = DARK_SLEEPERS;
int tp_who_hides_dark = WHO_HIDES_DARK;
int tp_compatible_priorities = COMPATIBLE_PRIORITIES;
int tp_do_mpi_parsing = DO_MPI_PARSING;
int tp_look_propqueues = LOOK_PROPQUEUES;
int tp_lock_envcheck = LOCK_ENVCHECK;
int tp_diskbase_propvals = DISKBASE_PROPVALS;
int tp_idleboot = IDLEBOOT;
int tp_playermax = PLAYERMAX;
int tp_allow_home = ALLOW_HOME;
int tp_enable_prefix = ENABLE_PREFIX;
int tp_thing_movement = SECURE_THING_MOVEMENT;
int tp_expanded_debug = EXPANDED_DEBUG_TRACE;
int tp_proplist_int_counter = PROPLIST_INT_COUNTER;
int tp_log_interactive = LOG_INTERACTIVE;
int tp_lazy_mpi_istype_perm = LAZY_MPI_ISTYPE_PERM;
int tp_optimize_muf = OPTIMIZE_MUF;
int tp_ignore_support = IGNORE_SUPPORT;
int tp_ignore_bidirectional = IGNORE_BIDIRECTIONAL;
int tp_verbose_clone = VERBOSE_CLONE;
int tp_muf_comments_strict = MUF_COMMENTS_STRICT;
int tp_starttls_allow = STARTTLS_ALLOW;
int tp_m3_huh = M3_HUH;

struct tune_bool_entry {
	const char *group;
	const char *name;
	int *boolval;
	int security;
	const char *label;
};

struct tune_bool_entry tune_bool_list[] = {
	{"Commands",   "enable_home", &tp_allow_home, 4, "Enable 'home' command"},
	{"Commands",   "enable_prefix", &tp_enable_prefix, 4, "Enable prefix actions"},
	{"Commands",   "verbose_clone", &tp_verbose_clone, 4, "Verbose @clone command"},
	{"Dark",       "exit_darking", &tp_exit_darking, 0, "Allow setting exits dark"},
	{"Dark",       "thing_darking", &tp_thing_darking, 0, "Allow setting things dark"},
	{"Dark",       "dark_sleepers", &tp_dark_sleepers, 0, "Make sleeping players dark"},
	{"Dark",       "who_hides_dark", &tp_who_hides_dark, 4, "Hide dark players from WHO list"},
	{"Database",   "realms_control", &tp_realms_control, 0, "Enable Realms control"},
	{"Database",   "compatible_priorities", &tp_compatible_priorities, 0, "Use legacy exit priority levels on things"},
	{"DB Dumps",   "diskbase_propvals", &tp_diskbase_propvals, 0, "Enable property value diskbasing (req. restart)"},
	{"DB Dumps",   "dbdump_warning", &tp_dbdump_warning, 0, "Enable warning messages for full DB dumps"},
	{"DB Dumps",   "deltadump_warning", &tp_deltadump_warning, 0, "Enable warning messages for delta DB dumps"},
	{"DB Dumps",   "dumpdone_warning", &tp_dumpdone_warning, 0, "Enable notification of DB dump completion"},
	{"Idle Boot",  "idleboot", &tp_idleboot, 0, "Enable booting of idle players"},
	{"Killing",    "restrict_kill", &tp_restrict_kill, 0, "Restrict kill command to players set Kill_OK"},
	{"Listeners",  "allow_listeners", &tp_listeners, 0, "Enable programs to listen to player output"},
	{"Listeners",  "allow_listeners_obj", &tp_listeners_obj, 0, "Allow listeners on things"},
	{"Listeners",  "allow_listeners_env", &tp_listeners_env, 0, "Allow listeners down environment"},
	{"Logging",    "log_commands", &tp_log_commands, 4, "Enable logging of player commands"},
	{"Logging",    "log_failed_commands", &tp_log_failed_commands, 4, "Enable logging of unrecognized commands"},
	{"Logging",    "log_interactive", &tp_log_interactive, 4, "Enable logging of text sent to MUF"},
	{"Logging",    "log_programs", &tp_log_programs, 4, "Log programs every time they are saved"},
	{"Movement",   "teleport_to_player", &tp_teleport_to_player, 0, "Allow teleporting to a player"},
	{"Movement",   "secure_teleport", &tp_secure_teleport, 0, "Restrict actions to Jump_OK or controlled rooms"},
	{"Movement",   "secure_thing_movement", &tp_thing_movement, 4, "Moving things act like player"},
	{"MPI",        "do_mpi_parsing", &tp_do_mpi_parsing, 0, "Enable parsing of mesgs for MPI"},
	{"MPI",        "lazy_mpi_istype_perm", &tp_lazy_mpi_istype_perm, 0, "Enable looser legacy perms for MPI {istype}"},
	{"MUF",        "optimize_muf", &tp_optimize_muf, 0, "Enable MUF bytecode optimizer"},
	{"MUF",        "expanded_debug_trace", &tp_expanded_debug, 0, "MUF debug trace shows array contents"},
	{"MUF",        "force_mlev1_name_notify", &tp_force_mlev1_name_notify, 0, "MUF notify prepends username at ML1"},
	{"MUF",        "muf_comments_strict", &tp_muf_comments_strict, 0, "MUF comments are strict and not recursive"},
	{"Player Max", "playermax", &tp_playermax, 0, "Limit number of concurrent players allowed"},
	{"Properties", "look_propqueues", &tp_look_propqueues, 0, "When a player looks, trigger _look/ propqueues"},
	{"Properties", "lock_envcheck", &tp_lock_envcheck, 0, "Locks check environment for properties"},
	{"Properties", "proplist_int_counter", &tp_proplist_int_counter, 0, "Proplist counter uses an integer property"},
	{"Registration", "registration", &tp_registration, 0, "Require new players to register manually"},
	{"Tuning",     "periodic_program_purge", &tp_periodic_program_purge, 0, "Periodically free unused MUF programs"},
	{"WHO",        "use_hostnames", &tp_hostnames, 0, "Resolve IP addresses into hostnames"},
	{"WHO",        "secure_who", &tp_secure_who, 0, "Disallow WHO command from login screen and programs"},
	{"WHO",        "who_doing", &tp_who_doing, 0, "Show '_/do' property value in WHO lists"},
	{"Misc",       "allow_zombies", &tp_zombies, 0, "Enable Zombie things to relay what they hear"},
	{"Misc",       "wiz_vehicles", &tp_wiz_vehicles, 0, "Only let Wizards set vehicle bits"},
	{"Misc",       "ignore_support", &tp_ignore_support, 3, "Enable support for @ignoring players"},
	{"Misc",       "ignore_bidirectional", &tp_ignore_bidirectional, 3, "Enable bidirectional @ignore"},
	{"Misc",	"m3_huh", &tp_m3_huh, 3, "Enable huh? to call an exit named \"huh?\" and set M3, with full command string"},
	{"SSL",        "starttls_allow", &tp_starttls_allow, 3, "Enable TELNET STARTTLS encryption on plaintext port"},

	{NULL, NULL, NULL, 0}
};


static const char *
timestr_full(long dtime)
{
	static char buf[32];
	int days, hours, minutes, seconds;

	days = dtime / 86400;
	dtime %= 86400;
	hours = dtime / 3600;
	dtime %= 3600;
	minutes = dtime / 60;
	seconds = dtime % 60;

	snprintf(buf, sizeof(buf), "%3dd %2d:%02d:%02d", days, hours, minutes, seconds);

	return buf;
}


int
tune_count_parms(void)
{
	int total = 0;
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;

	while ((tstr++)->name)
		total++;
	while ((ttim++)->name)
		total++;
	while ((tval++)->name)
		total++;
	while ((tref++)->name)
		total++;
	while ((tbool++)->name)
		total++;

	return total;
}


void
tune_display_parms(dbref player, char *name)
{
	char buf[BUFFER_LEN + 50];
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;

	while (tstr->name) {
		strcpyn(buf, sizeof(buf), tstr->name);
		if (!*name || equalstr(name, buf)) {
			snprintf(buf, sizeof(buf), "(str)  %-20s = %.4096s", tstr->name, *tstr->str);
			notify(player, buf);
		}
		tstr++;
	}

	while (ttim->name) {
		strcpyn(buf, sizeof(buf), ttim->name);
		if (!*name || equalstr(name, buf)) {
			snprintf(buf, sizeof(buf), "(time) %-20s = %s", ttim->name, timestr_full(*ttim->tim));
			notify(player, buf);
		}
		ttim++;
	}

	while (tval->name) {
		strcpyn(buf, sizeof(buf), tval->name);
		if (!*name || equalstr(name, buf)) {
			snprintf(buf, sizeof(buf), "(int)  %-20s = %d", tval->name, *tval->val);
			notify(player, buf);
		}
		tval++;
	}

	while (tref->name) {
		strcpyn(buf, sizeof(buf), tref->name);
		if (!*name || equalstr(name, buf)) {
			snprintf(buf, sizeof(buf), "(ref)  %-20s = %s", tref->name, unparse_object(player, *tref->ref));
			notify(player, buf);
		}
		tref++;
	}

	while (tbool->name) {
		strcpyn(buf, sizeof(buf), tbool->name);
		if (!*name || equalstr(name, buf)) {
			snprintf(buf, sizeof(buf), "(bool) %-20s = %s", tbool->name, ((*tbool->boolval) ? "yes" : "no"));
			notify(player, buf);
		}
		tbool++;
	}
	notify(player, "*done*");
}


void
tune_save_parms_to_file(FILE * f)
{
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;

	while (tstr->name) {
		fprintf(f, "%s=%.4096s\n", tstr->name, *tstr->str);
		tstr++;
	}

	while (ttim->name) {
		fprintf(f, "%s=%s\n", ttim->name, timestr_full(*ttim->tim));
		ttim++;
	}

	while (tval->name) {
		fprintf(f, "%s=%d\n", tval->name, *tval->val);
		tval++;
	}

	while (tref->name) {
		fprintf(f, "%s=#%d\n", tref->name, *tref->ref);
		tref++;
	}

	while (tbool->name) {
		fprintf(f, "%s=%s\n", tbool->name, (*tbool->boolval) ? "yes" : "no");
		tbool++;
	}
}


stk_array*
tune_parms_array(const char* pattern, int mlev)
{
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;
	stk_array *nu = new_array_packed(0);
	struct inst temp1;
	char pat[BUFFER_LEN];
	char buf[BUFFER_LEN];
	int i = 0;

	strcpyn(pat, sizeof(pat), pattern);
	while (tbool->name) {
		if (tbool->security <= mlev) {
			strcpyn(buf, sizeof(buf), tbool->name);
			if (!*pattern || equalstr(pat, buf)) {
				stk_array *item = new_array_dictionary();
				array_set_strkey_strval(&item, "type", "boolean");
				array_set_strkey_strval(&item, "group", tbool->group);
				array_set_strkey_strval(&item, "name",  tbool->name);
				array_set_strkey_intval(&item, "value", *tbool->boolval? 1 : 0);
				array_set_strkey_intval(&item, "mlev",  tbool->security);
				array_set_strkey_strval(&item, "label", tbool->label);

				temp1.type = PROG_ARRAY;
				temp1.data.array = item;
				array_set_intkey(&nu, i++, &temp1);
				CLEAR(&temp1);
			}
		}
		tbool++;
	}

	while (ttim->name) {
		if (ttim->security <= mlev) {
			strcpyn(buf, sizeof(buf), ttim->name);
			if (!*pattern || equalstr(pat, buf)) {
				stk_array *item = new_array_dictionary();
				array_set_strkey_strval(&item, "type", "timespan");
				array_set_strkey_strval(&item, "group", ttim->group);
				array_set_strkey_strval(&item, "name",  ttim->name);
				array_set_strkey_intval(&item, "value", *ttim->tim);
				array_set_strkey_intval(&item, "mlev",  ttim->security);
				array_set_strkey_strval(&item, "label", ttim->label);

				temp1.type = PROG_ARRAY;
				temp1.data.array = item;
				array_set_intkey(&nu, i++, &temp1);
				CLEAR(&temp1);
			}
		}
		ttim++;
	}

	while (tval->name) {
		if (tval->security <= mlev) {
			strcpyn(buf, sizeof(buf), tval->name);
			if (!*pattern || equalstr(pat, buf)) {
				stk_array *item = new_array_dictionary();
				array_set_strkey_strval(&item, "type", "integer");
				array_set_strkey_strval(&item, "group", tval->group);
				array_set_strkey_strval(&item, "name",  tval->name);
				array_set_strkey_intval(&item, "value", *tval->val);
				array_set_strkey_intval(&item, "mlev",  tval->security);
				array_set_strkey_strval(&item, "label", tval->label);

				temp1.type = PROG_ARRAY;
				temp1.data.array = item;
				array_set_intkey(&nu, i++, &temp1);
				CLEAR(&temp1);
			}
		}
		tval++;
	}

	while (tref->name) {
		if (tref->security <= mlev) {
			strcpyn(buf, sizeof(buf), tref->name);
			if (!*pattern || equalstr(pat, buf)) {
				stk_array *item = new_array_dictionary();
				array_set_strkey_strval(&item, "type", "dbref");
				array_set_strkey_strval(&item, "group", tref->group);
				array_set_strkey_strval(&item, "name",  tref->name);
				array_set_strkey_refval(&item, "value", *tref->ref);
				array_set_strkey_intval(&item, "mlev",  tref->security);
				array_set_strkey_strval(&item, "label", tref->label);
				switch (tref->typ) {
					case NOTYPE:
						array_set_strkey_strval(&item, "objtype",  "any");
						break;
					case TYPE_PLAYER:
						array_set_strkey_strval(&item, "objtype",  "player");
						break;
					case TYPE_THING:
						array_set_strkey_strval(&item, "objtype",  "thing");
						break;
					case TYPE_ROOM:
						array_set_strkey_strval(&item, "objtype",  "room");
						break;
					case TYPE_EXIT:
						array_set_strkey_strval(&item, "objtype",  "exit");
						break;
					case TYPE_PROGRAM:
						array_set_strkey_strval(&item, "objtype",  "program");
						break;
					case TYPE_GARBAGE:
						array_set_strkey_strval(&item, "objtype",  "garbage");
						break;
					default:
						array_set_strkey_strval(&item, "objtype",  "unknown");
						break;
				}

				temp1.type = PROG_ARRAY;
				temp1.data.array = item;
				array_set_intkey(&nu, i++, &temp1);
				CLEAR(&temp1);
			}
		}
		tref++;
	}

	while (tstr->name) {
		if (tstr->security <= mlev) {
			strcpyn(buf, sizeof(buf), tstr->name);
			if (!*pattern || equalstr(pat, buf)) {
				stk_array *item = new_array_dictionary();
				array_set_strkey_strval(&item, "type", "string");
				array_set_strkey_strval(&item, "group", tstr->group);
				array_set_strkey_strval(&item, "name",  tstr->name);
				array_set_strkey_strval(&item, "value", *tstr->str);
				array_set_strkey_intval(&item, "mlev",  tstr->security);
				array_set_strkey_strval(&item, "label", tstr->label);

				temp1.type = PROG_ARRAY;
				temp1.data.array = item;
				array_set_intkey(&nu, i++, &temp1);
				CLEAR(&temp1);
			}
		}
		tstr++;
	}

	return nu;
}


void
tune_save_parmsfile(void)
{
	FILE *f;

	f = fopen(PARMFILE_NAME, "w");
	if (!f) {
		log_status("Couldn't open file %s!", PARMFILE_NAME);
		return;
	}

	tune_save_parms_to_file(f);

	fclose(f);
}



const char *
tune_get_parmstring(const char *name, int mlev)
{
	static char buf[BUFFER_LEN + 50];
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;

	while (tstr->name) {
		if (!string_compare(name, tstr->name)) {
			if (tstr->security > mlev)
				return "";
			return (*tstr->str);
		}
		tstr++;
	}

	while (ttim->name) {
		if (!string_compare(name, ttim->name)) {
			if (ttim->security > mlev)
				return "";
			snprintf(buf, sizeof(buf), "%d", *ttim->tim);
			return (buf);
		}
		ttim++;
	}

	while (tval->name) {
		if (!string_compare(name, tval->name)) {
			if (tval->security > mlev)
				return "";
			snprintf(buf, sizeof(buf), "%d", *tval->val);
			return (buf);
		}
		tval++;
	}

	while (tref->name) {
		if (!string_compare(name, tref->name)) {
			if (tref->security > mlev)
				return "";
			snprintf(buf, sizeof(buf), "#%d", *tref->ref);
			return (buf);
		}
		tref++;
	}

	while (tbool->name) {
		if (!string_compare(name, tbool->name)) {
			if (tbool->security > mlev)
				return "";
			snprintf(buf, sizeof(buf), "%s", ((*tbool->boolval) ? "yes" : "no"));
			return (buf);
		}
		tbool++;
	}

	/* Parameter was not found.  Return null string. */
	strcpyn(buf, sizeof(buf), "");
	return (buf);
}



/* return values:
 *  0 for success.
 *  1 for unrecognized parameter name.
 *  2 for bad parameter value syntax.
 *  3 for bad parameter value.
 */

#define TUNESET_SUCCESS 0
#define TUNESET_UNKNOWN 1
#define TUNESET_SYNTAX 2
#define TUNESET_BADVAL 3

void
tune_freeparms()
{
	struct tune_str_entry *tstr = tune_str_list;
	while (tstr->name) {
		if (!tstr->isdefault) {
			free((char *) *tstr->str);
		}
		tstr++;
	}
}

int
tune_setparm(const char *parmname, const char *val)
{
	struct tune_str_entry *tstr = tune_str_list;
	struct tune_time_entry *ttim = tune_time_list;
	struct tune_val_entry *tval = tune_val_list;
	struct tune_ref_entry *tref = tune_ref_list;
	struct tune_bool_entry *tbool = tune_bool_list;
	char buf[BUFFER_LEN];
	char *parmval;

	strcpyn(buf, sizeof(buf), val);
	parmval = buf;

	while (tstr->name) {
		if (!string_compare(parmname, tstr->name)) {
			if (!tstr->isdefault)
				free((char *) *tstr->str);
			if (*parmval == '-')
				parmval++;
			*tstr->str = string_dup(parmval);
			tstr->isdefault = 0;

			return 0;
		}
		tstr++;
	}

	while (ttim->name) {
		if (!string_compare(parmname, ttim->name)) {
			int days, hrs, mins, secs, result;
			char *end;

			days = hrs = mins = secs = 0;
			end = parmval + strlen(parmval) - 1;
			switch (*end) {
			case 's':
			case 'S':
				*end = '\0';
				if (!number(parmval))
					return 2;
				secs = atoi(parmval);
				break;
			case 'm':
			case 'M':
				*end = '\0';
				if (!number(parmval))
					return 2;
				mins = atoi(parmval);
				break;
			case 'h':
			case 'H':
				*end = '\0';
				if (!number(parmval))
					return 2;
				hrs = atoi(parmval);
				break;
			case 'd':
			case 'D':
				*end = '\0';
				if (!number(parmval))
					return 2;
				days = atoi(parmval);
				break;
			default:
				result = sscanf(parmval, "%dd %2d:%2d:%2d", &days, &hrs, &mins, &secs);
				if (result != 4)
					return 2;
				break;
			}
			*ttim->tim = (days * 86400) + (3600 * hrs) + (60 * mins) + secs;
			return 0;
		}
		ttim++;
	}

	while (tval->name) {
		if (!string_compare(parmname, tval->name)) {
			if (!number(parmval))
				return 2;
			*tval->val = atoi(parmval);
			return 0;
		}
		tval++;
	}

	while (tref->name) {
		if (!string_compare(parmname, tref->name)) {
			dbref obj;

			if (*parmval != NUMBER_TOKEN)
				return 2;
			if (!number(parmval + 1))
				return 2;
			obj = (dbref) atoi(parmval + 1);
			if (obj < 0 || obj >= db_top)
				return 2;
			if (tref->typ != NOTYPE && Typeof(obj) != tref->typ)
				return 3;
			*tref->ref = obj;
			return 0;
		}
		tref++;
	}

	while (tbool->name) {
		if (!string_compare(parmname, tbool->name)) {
			if (*parmval == 'y' || *parmval == 'Y') {
				*tbool->boolval = 1;
			} else if (*parmval == 'n' || *parmval == 'N') {
				*tbool->boolval = 0;
			} else {
				return 2;
			}
			return 0;
		}
		tbool++;
	}

	return 1;
}

void
tune_load_parms_from_file(FILE * f, dbref player, int cnt)
{
	char buf[BUFFER_LEN];
	char *c, *p;
	int result=0;

	while (!feof(f) && (cnt < 0 || cnt--)) {
		fgets(buf, sizeof(buf), f);
		if (*buf != '#') {
			p = c = index(buf, '=');
			if (c) {
				*c++ = '\0';
				while (p > buf && isspace(*(--p)))
					*p = '\0';
				while (isspace(*c))
					c++;
				for (p = c; *p && *p != '\n' && *p != '\r'; p++) ;
				*p = '\0';
				for (p = buf; isspace(*p); p++) ;
				if (*p) {
					result = tune_setparm(p, c);
				}
				switch (result) {
				case TUNESET_SUCCESS:
					strcatn(buf, sizeof(buf), ": Parameter set.");
					break;
				case TUNESET_UNKNOWN:
					strcatn(buf, sizeof(buf), ": Unknown parameter.");
					break;
				case TUNESET_SYNTAX:
					strcatn(buf, sizeof(buf), ": Bad parameter syntax.");
					break;
				case TUNESET_BADVAL:
					strcatn(buf, sizeof(buf), ": Bad parameter value.");
					break;
				}
				if (result && player != NOTHING)
					notify(player, p);
			}
		}
	}
}

void
tune_load_parmsfile(dbref player)
{
	FILE *f;

	f = fopen(PARMFILE_NAME, "r");
	if (!f) {
		log_status("Couldn't open file %s!", PARMFILE_NAME);
		return;
	}

	tune_load_parms_from_file(f, player, -1);

	fclose(f);
}


void
do_tune(dbref player, char *parmname, char *parmval)
{
	int result;

	if (!Wizard(player)) {
		notify(player, "You pull out a harmonica and play a short tune.");
		return;
	}

	if (*parmname && *parmval) {
		result = tune_setparm(parmname, parmval);
		switch (result) {
		case TUNESET_SUCCESS:
			log_status("TUNED: %s(%d) tuned %s to %s",
					   NAME(player), player, parmname, parmval);
			notify(player, "Parameter set.");
			tune_display_parms(player, parmname);
			break;
		case TUNESET_UNKNOWN:
			notify(player, "Unknown parameter.");
			break;
		case TUNESET_SYNTAX:
			notify(player, "Bad parameter syntax.");
			break;
		case TUNESET_BADVAL:
			notify(player, "Bad parameter value.");
			break;
		}
		return;
	} else if (*parmname) {
		if (!string_compare(parmname, "save")) {
			tune_save_parmsfile();
			notify(player, "Saved parameters to configuration file.");
		} else if (!string_compare(parmname, "load")) {
			tune_load_parmsfile(player);
			notify(player, "Restored parameters from configuration file.");
		} else {
			tune_display_parms(player, parmname);
		}
		return;
	} else if (!*parmval && !*parmname) {
		tune_display_parms(player, parmname);
	} else {
		notify(player, "But what do you want to tune?");
		return;
	}
}
static const char *tune_c_version = "$RCSfile$ $Revision: 1.45 $";
const char *get_tune_c_version(void) { return tune_c_version; }

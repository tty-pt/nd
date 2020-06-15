/* $Header$ */

/* Copyright 1992-2001 by Fuzzball Software */
/* Consider this code protected under the GNU public license, with explicit
 * permission to distribute when linked against openSSL. */

#define DEFINE_HEADER_VERSIONS
#include "fb.h"
#undef DEFINE_HEADER_VERSIONS
#include "copyright.h"
#include "config.h"
#include "match.h"
#include "mpi.h"
#include "web.h"

#include <sys/types.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

# define NEED_SOCKLEN_T
//"do not include netinet6/in6.h directly, include netinet/in.h.  see RFC2553"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <openssl/ssl.h>

#include "db.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "props.h"
#include "mcp.h"
#include "externs.h"
#include "interp.h"
#include "kill.h"
#include "search.h"
#include "view.h"
#include "geography.h"
#include "item.h"
#include "mob.h"
#undef NDEBUG
#include "debug.h"

typedef enum {
	TELNET_STATE_NORMAL,
	TELNET_STATE_IAC,
	TELNET_STATE_WONT,
	TELNET_STATE_DONT,
	TELNET_STATE_SB
} telnet_states_t;

#define TELNET_IAC        255
#define TELNET_DONT       254
#define TELNET_DO         253
#define TELNET_WONT       252
#define TELNET_WILL       251
#define TELNET_SB         250
#define TELNET_EL         248
#define TELNET_EC         247
#define TELNET_AYT        246
#define TELNET_IP         244
#define TELNET_BRK        243
#define TELNET_NOP        241

struct text_block {
	int nchars;
	struct text_block *nxt;
	char *start;
	char *buf;
};

struct text_queue {
	int lines;
	struct text_block *head;
	struct text_block **tail;
};

struct descriptor_data {
	int descriptor;
	int connected;
	int con_number;
	int booted;
	dbref player;
	int output_size;
	struct text_queue output;
	struct text_queue input;
	char *raw_input;
	char *raw_input_at;
	int telnet_enabled;
	telnet_states_t telnet_state;
	int telnet_sb_opt;
	long last_time;
	long connected_at;
	long last_pinged_at;
	const char *username;
	int quota;
        struct {
		unsigned ip;
		unsigned old;
	} web;
	struct descriptor_data *next;
	struct descriptor_data **prev;
	McpFrame mcpframe;
};

struct descriptor_data *descriptor_list = NULL;

#define MAX_LISTEN_SOCKS 16

int shutdown_flag = 0;

static const char *create_fail =
		"Either there is already a player with that name, or that name is illegal.\r\n";

static const char *flushed_message = "<Output Flushed>\r\n";
static const char *shutdown_message = "\r\nGoing down - Bye\r\n";

int resolver_sock[2];

static int numsocks = 0;
static int listener_port[MAX_LISTEN_SOCKS];
static int sock[MAX_LISTEN_SOCKS];
static int ndescriptors = 0;
struct descriptor_data *descr_lookup_table[FD_SETSIZE];
struct descriptor_data *descr_count_table[FD_SETSIZE];
int current_descr_count = 0;

extern void fork_and_dump(void);
void process_commands(void);
int shovechars();
void shutdownsock(struct descriptor_data *d);
struct descriptor_data *initializesock(int s);
void freeqs(struct descriptor_data *d);
void welcome_user(struct descriptor_data *d);
void close_sockets(const char *msg);
int boot_off(dbref player);
void boot_player_off(dbref player);
const char *addrout(int, long, unsigned short);
int make_socket(int);
struct descriptor_data *new_connection(int port, int sock);
void dump_users(struct descriptor_data *d, char *user);
void parse_connect(const char *msg, char *command, char *user, char *pass);
int do_command(struct descriptor_data *d, char *command);
int is_interface_command(const char* cmd);
int queue_string(struct descriptor_data *, const char *);
int queue_write(struct descriptor_data *, const char *, int);
int process_output(struct descriptor_data *d);
int process_input(struct descriptor_data *d);
void announce_connect(struct descriptor_data *, dbref);
void announce_disconnect(struct descriptor_data *);
char *time_format_1(long);
char *time_format_2(long);
void    remember_player_descr(dbref player, int);
void    update_desc_count_table();
int*    get_player_descrs(dbref player, int* count);
void    forget_player_descr(dbref player, int);
struct descriptor_data* descrdata_by_descr(int i);
int online_init(void);
dbref online_next(int *ptr);
# define socket_write(d, buf, count) write(d->descriptor, buf, count)
# define socket_read(d, buf, count) read(d->descriptor, buf, count)

extern FILE *input_file;
extern FILE *delta_infile;
extern FILE *delta_outfile;

short optflags = 0;
pid_t global_resolver_pid=0;
pid_t global_dumper_pid=0;
short global_dumpdone=0;

time_t sel_prof_start_time;
long sel_prof_idle_sec;
long sel_prof_idle_usec;
unsigned long sel_prof_idle_use;

void
show_program_usage(char *prog)
{
	fprintf(stderr, "Usage: %s [-sSyWv?] [-C [path]]", prog);
	fprintf(stderr, "    Options:\n");
	fprintf(stderr, "        -C PATH   changes directory to PATH before starting up.\n");
	fprintf(stderr, "        -S        don't do db sanity checks at startup time.\n");
	fprintf(stderr, "        -s        load db, then enter the interactive sanity editor.\n");
	fprintf(stderr, "        -y        attempt to auto-fix a corrupt db after loading.\n");
	fprintf(stderr, "        -W        only allow wizards to login.\n");
	fprintf(stderr, "        -v        display this server's version.\n");
	fprintf(stderr, "        -?        display this message.\n");
	exit(1);
}

/* NOTE: Will need to think about this more for unicode */
#define isinput( q ) isprint( (q) & 127 )

extern int sanity_violated;
int time_since_combat = 0;

int
main(int argc, char **argv)
{
	register char c;

	listener_port[0] = TINYPORT;

	int i;
	for (i = 0; i < FD_SETSIZE; i++)
		descr_lookup_table[i] = NULL;
	for (i = 0; i < FD_SETSIZE; i++)
		descr_count_table[i] = NULL;

	while ((c = getopt(argc, argv, "dsyvSC:")) != -1) {
		switch (c) {
		case 'd':
			optflags |= OPT_DETACH;
			break;
		case 's':
			optflags |= OPT_SANITY_INTERACTIVE;
			break;
		case 'S':
			optflags |= OPT_NOSANITY;
			break;
		case 'y':
			optflags |= OPT_SANITY_AUTOFIX;
			break;
		case 'v':
			printf("%s\n", VERSION);
			break;
			
		case 'C':
			if (chdir(optarg)) {
				perror("chdir");
				return 4;
			}
			break;
		default:
			show_program_usage(*argv);
			return 1;

		case '?':
			show_program_usage(*argv);
			return 0;
		}
	}

	warn("INIT: TinyMUCK %s starting.", "version");
	warn("%s PID is: %d", argv[0], getpid());

	mcp_initialize();

	sel_prof_start_time = time(NULL); /* Set useful starting time */
	sel_prof_idle_sec = 0;
	sel_prof_idle_usec = 0;
	sel_prof_idle_use = 0;

	if (init_game() < 0) {
		warn("Couldn't load " STD_DB "!");
		return 2;
	}

	CBUG(map_init());

	set_signals();

	sanity(AMBIGUOUS);
	if (sanity_violated) {
		optflags |= OPT_WIZONLY;
		if (optflags & OPT_SANITY_AUTOFIX)
			sanfix(AMBIGUOUS);
	}

	if (shovechars())
		return 1;

	map_close();
	mob_save();

	close_sockets("\r\nServer shutting down.\r\n");

	dump_database();

	return 0;
}

int
queue_ansi(struct descriptor_data *d, const char *msg)
{
	char buf[BUFFER_LEN + 8];

	if (d->connected) {
		if (FLAGS(d->player) & CHOWN_OK) {
			strip_bad_ansi(buf, msg);
		} else {
			strip_ansi(buf, msg);
		}
	} else {
		strip_ansi(buf, msg);
	}
	mcp_frame_output_inband(&d->mcpframe, buf);
	return strlen(buf);
	/* return queue_string(d, buf); */
}

int notify_nolisten_level = 0;

int
notify_nolisten(dbref player, const char *msg, int isprivate)
{
	int retval = 0;
	char buf[BUFFER_LEN + 2];
	char buf2[BUFFER_LEN + 2];
	int firstpass = 1;
	char *ptr1;
	const char *ptr2;
	dbref ref;
	int di;
	int* darr;
	int dcount;

	ptr2 = msg;
	while (ptr2 && *ptr2) {
		ptr1 = buf;
		while (ptr2 && *ptr2 && *ptr2 != '\r')
			*(ptr1++) = *(ptr2++);
		*(ptr1++) = '\r';
		*(ptr1++) = '\n';
		*(ptr1++) = '\0';
		if (*ptr2 == '\r')
			ptr2++;

		darr = get_player_descrs(player, &dcount);
		for (di = 0; di < dcount; di++) {
			queue_ansi(descrdata_by_descr(darr[di]), buf);
			if (firstpass) retval++;
		}

#if ZOMBIES
		if ((Typeof(player) == TYPE_THING) && (FLAGS(player) & ZOMBIE) &&
		    !(FLAGS(OWNER(player)) & ZOMBIE) &&
		    (!(FLAGS(player) & DARK) || Wizard(OWNER(player)))) {
			ref = getloc(player);
			if (Wizard(OWNER(player)) || ref == NOTHING ||
			    Typeof(ref) != TYPE_ROOM || !(FLAGS(ref) & ZOMBIE)) {
				if (isprivate || getloc(player) != getloc(OWNER(player))) {
					char pbuf[BUFFER_LEN];
					const char *prefix;
					char ch = *match_args;

					*match_args = '\0';

					if (notify_nolisten_level <= 0)
					{
						notify_nolisten_level++;

						prefix = do_parse_prop(-1, player, player, MESGPROP_PECHO, "(@Pecho)", pbuf, sizeof(pbuf), MPI_ISPRIVATE);

						notify_nolisten_level--;
					}
					else
						prefix = 0;

					*match_args = ch;

					if (!prefix || !*prefix) {
						prefix = NAME(player);
						snprintf(buf2, sizeof(buf2), "%s> %.*s", prefix,
							 (int)(BUFFER_LEN - (strlen(prefix) + 3)), buf);
					} else {
						snprintf(buf2, sizeof(buf2), "%s %.*s", prefix,
							 (int)(BUFFER_LEN - (strlen(prefix) + 2)), buf);
					}

					darr = get_player_descrs(OWNER(player), &dcount);
					for (di = 0; di < dcount; di++) {
						queue_ansi(descrdata_by_descr(darr[di]), buf2);
						if (firstpass) retval++;
					}
				}
			}
		}
#endif
		firstpass = 0;
	}
	return retval;
}

int
notify_filtered(dbref from, dbref player, const char *msg, int isprivate)
{
	if (msg == 0)
		return 0;
	return notify_nolisten(player, msg, isprivate);
}

int
notify_from_echo(dbref from, dbref player, const char *msg, int isprivate)
{

#if LISTENERS
	const char *ptr=msg;
#if !LISTENERS_OBJ
	if (Typeof(player) == TYPE_ROOM)
#endif
		listenqueue(-1, from, getloc(from), player, player, NOTHING,
			    "_listen", ptr, LISTEN_MLEV, 1, 0);
	listenqueue(-1, from, getloc(from), player, player, NOTHING,
		    "~listen", ptr, LISTEN_MLEV, 1, 1);
	listenqueue(-1, from, getloc(from), player, player, NOTHING,
		    "~olisten", ptr, LISTEN_MLEV, 0, 1);
#endif

	if (Typeof(player) == TYPE_THING && (FLAGS(player) & VEHICLE) &&
		(!(FLAGS(player) & DARK) || Wizard(OWNER(player))))
	{
		dbref ref;

		ref = getloc(player);
		if (Wizard(OWNER(player)) || ref == NOTHING ||
			Typeof(ref) != TYPE_ROOM || !(FLAGS(ref) & VEHICLE)
				) {
			if (!isprivate && getloc(from) == getloc(player)) {
				char buf[BUFFER_LEN];
				char pbuf[BUFFER_LEN];
				const char *prefix;
				char ch = *match_args;

				*match_args = '\0';
				prefix = do_parse_prop(-1, from, player, MESGPROP_OECHO, "(@Oecho)", pbuf, sizeof(pbuf), MPI_ISPRIVATE);
				*match_args = ch;

				if (!prefix || !*prefix)
					prefix = "Outside>";
				snprintf(buf, sizeof(buf), "%s %.*s", prefix, (int)(BUFFER_LEN - (strlen(prefix) + 2)), msg);
				ref = DBFETCH(player)->contents;
				while (ref != NOTHING) {
					notify_filtered(from, ref, buf, isprivate);
					ref = DBFETCH(ref)->next;
				}
			}
		}
	}

	return notify_filtered(from, player, msg, isprivate);
}

int
notify_from(dbref from, dbref player, const char *msg)
{
	return notify_from_echo(from, player, msg, 1);
}

int
notify(dbref player, const char *msg)
{
	return notify_from_echo(player, player, msg, 1);
}

void
notify_fmt(dbref player, char *format, ...)
{
	va_list args;
	char bufr[BUFFER_LEN];

	va_start(args, format);
	vsnprintf(bufr, sizeof(bufr), format, args);
	bufr[sizeof(bufr)-1] = '\0';
	notify(player, bufr);
	va_end(args);
}

struct timeval
timeval_sub(struct timeval now, struct timeval then)
{
	now.tv_sec -= then.tv_sec;
	now.tv_usec -= then.tv_usec;
	if (now.tv_usec < 0) {
		now.tv_usec += 1000000;
		now.tv_sec--;
	}
	return now;
}

int
msec_diff(struct timeval now, struct timeval then)
{
	return ((now.tv_sec - then.tv_sec) * 1000 + (now.tv_usec - then.tv_usec) / 1000);
}

struct timeval
msec_add(struct timeval t, int x)
{
	t.tv_sec += x / 1000;
	t.tv_usec += (x % 1000) * 1000;
	if (t.tv_usec >= 1000000) {
		t.tv_sec += t.tv_usec / 1000000;
		t.tv_usec = t.tv_usec % 1000000;
	}
	return t;
}

struct timeval
update_quotas(struct timeval last, struct timeval current)
{
	int nslices;
	int cmds_per_time;
	struct descriptor_data *d;
	int td = msec_diff(current, last);
	time_since_combat += td;

	nslices = td / COMMAND_TIME_MSEC;

	if (nslices > 0) {
		for (d = descriptor_list; d; d = d->next) {
			if (d->connected) {
				cmds_per_time = ((FLAGS(d->player) & INTERACTIVE)
								 ? (COMMANDS_PER_TIME * 8) : COMMANDS_PER_TIME);
			} else {
				cmds_per_time = COMMANDS_PER_TIME;
			}
			d->quota += cmds_per_time * nslices;
			if (d->quota > COMMAND_BURST_SIZE)
				d->quota = COMMAND_BURST_SIZE;
		}
	}
	return msec_add(last, nslices * COMMAND_TIME_MSEC);
}

int
queue_immediate(struct descriptor_data *d, const char *msg)
{
	char buf[BUFFER_LEN + 8];
	int quote_len = 0;

	if (d->connected) {
		if (FLAGS(d->player) & CHOWN_OK) {
			strip_bad_ansi(buf, msg);
		} else {
			strip_ansi(buf, msg);
		}
	} else {
		strip_ansi(buf, msg);
	}

	if (d->mcpframe.enabled && !(strncmp(buf, MCP_MESG_PREFIX, 3) && strncmp(buf, MCP_QUOTE_PREFIX, 3)))
	{
		quote_len = strlen(MCP_QUOTE_PREFIX);
		socket_write(d, MCP_QUOTE_PREFIX, quote_len);
	}

	return socket_write(d, buf, strlen(buf)) + quote_len;
}

void
goodbye_user(struct descriptor_data *d)
{
	queue_immediate(d, "\r\n");
	queue_immediate(d, LEAVE_MESSAGE);
	queue_immediate(d, "\r\n\r\n");
}

#if IDLEBOOT
static inline void
idleboot_user(struct descriptor_data *d)
{
	queue_immediate(d, "\r\n");
	queue_immediate(d, IDLEBOOT_MESSAGE);
	queue_immediate(d, "\r\n\r\n");
	d->booted = 1;
}
#endif

int send_keepalive(struct descriptor_data *d);
static int con_players_max = 0;	/* one of Cynbe's good ideas. */
static int con_players_curr = 0;	/* for playermax checks. */
extern void purge_free_frames(void);

static void
do_tick()
{
	if (time_since_combat < 1000)
		return;

	time_since_combat = 0;
	mob_update();
	geo_update();
}

int
shovechars()
{
	fd_set input_set, output_set;
	time_t now;
	struct timeval last_slice, current_time;
	struct timeval next_slice;
	struct timeval timeout, slice_timeout;
	int maxd = 0, cnt;
	struct descriptor_data *d, *dnext;
	struct descriptor_data *newd;
	struct timeval sel_in, sel_out;
	int avail_descriptors;
	int i;

	sock[0] = make_socket(listener_port[0]);
	maxd = sock[0] + 1;
	numsocks++;

	gettimeofday(&last_slice, (struct timezone *) 0);

	avail_descriptors = sysconf(_SC_OPEN_MAX) - 5;

	(void) time(&now);

	mob_init();

	/* Daemonize */
	if ((optflags & OPT_DETACH) && daemon(1, 1) != 0)
		_exit(0);

/* And here, we do the actual player-interaction loop */

	while (shutdown_flag == 0) {
		gettimeofday(&current_time, (struct timezone *) 0);
		last_slice = update_quotas(last_slice, current_time);

		/* next_muckevent(); */
		process_commands();
		do_tick();

		for (d = descriptor_list; d; d = dnext) {
			dnext = d->next;
			if (d->booted) {
				process_output(d);
				if (d->booted == 2) {
					goodbye_user(d);
				}
				d->booted = 0;
				process_output(d);
				shutdownsock(d);
			}
		}
		if (global_dumpdone != 0) {
			DUMPDONE_WARN();
			global_dumpdone = 0;
		}
		purge_free_frames();
		untouchprops_incremental(1);

		if (shutdown_flag)
			break;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		next_slice = msec_add(last_slice, COMMAND_TIME_MSEC);
		slice_timeout = timeval_sub(next_slice, current_time);

		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		if (ndescriptors < avail_descriptors) {
			for (i = 0; i < numsocks; i++) {
				FD_SET(sock[i], &input_set);
			}
		}
		for (d = descriptor_list; d; d = d->next) {
			if (d->input.lines > 100)
				timeout = slice_timeout;
			else
				FD_SET(d->descriptor, &input_set);

			if (d->output.head)
				FD_SET(d->descriptor, &output_set);
		}

		gettimeofday(&sel_in,NULL);
		if (select(maxd, &input_set, &output_set, (fd_set *) 0, &timeout) < 0) {
			if (errno != EINTR) {
				perror("select");
				return 1;
			}
		} else {
			gettimeofday(&sel_out,NULL);
			if (sel_out.tv_usec < sel_in.tv_usec) {
				sel_out.tv_usec += 1000000;
				sel_out.tv_sec -= 1;
			}
			sel_out.tv_usec -= sel_in.tv_usec;
			sel_out.tv_sec -= sel_in.tv_sec;
			sel_prof_idle_sec += sel_out.tv_sec;
			sel_prof_idle_usec += sel_out.tv_usec;
			if (sel_prof_idle_usec >= 1000000) {
				sel_prof_idle_usec -= 1000000;
				sel_prof_idle_sec += 1;
			}
			sel_prof_idle_use++;
			(void) time(&now);
			for (i = 0; i < numsocks; i++) {
				if (FD_ISSET(sock[i], &input_set)) {
					if (!(newd = new_connection(listener_port[i], sock[i]))) {
						if (errno && errno != EINTR && errno != EMFILE && errno != ENFILE) {
							perror("new_connection");
							/* return; */
						}
					} else {
						if (newd->descriptor >= maxd)
							maxd = newd->descriptor + 1;
					}
				}
			}
			for (cnt = 0, d = descriptor_list; d; d = dnext) {
				dnext = d->next;
				if (FD_ISSET(d->descriptor, &input_set)) {
					if (!process_input(d)) {
						d->booted = 1;
					}
				}
				if (FD_ISSET(d->descriptor, &output_set)) {
					if (!process_output(d)) {
						d->booted = 1;
					}
				}
				if (d->connected) {
					cnt++;
#if IDLEBOOT
					if (((now - d->last_time) > MAXIDLE) &&
					    !Wizard(d->player))
						idleboot_user(d);
#endif
				} else {
					/* Hardcode 300 secs -- 5 mins -- at the login screen */
					if ((now - d->connected_at) > 300) {
						warn("connection screen: connection timeout 300 secs");
						d->booted = 1;
					}
				}
#if IDLE_PING_TIME > 0
				if ( d->connected && ((now - d->last_pinged_at) > IDLE_PING_TIME) ) {
					const char *tmpptr = get_property_class( d->player, "_/sys/no_idle_ping" );
					if( !tmpptr && !send_keepalive(d)) {
						d->booted = 1;
					}
				}
#endif
			}
			if (cnt > con_players_max) {
				add_property((dbref) 0, "_sys/max_connects", NULL, cnt);
				con_players_max = cnt;
			}
			con_players_curr = cnt;
		}
	}

	/* End of the player processing loop */

	(void) time(&now);
	add_property((dbref) 0, "_sys/lastdumptime", NULL, (int) now);
	add_property((dbref) 0, "_sys/shutdowntime", NULL, (int) now);
	return 0;
}

void
wall_and_flush(const char *msg)
{
	struct descriptor_data *d, *dnext;
	char buf[BUFFER_LEN + 2];

	if (!msg || !*msg)
		return;
	strlcpy(buf, msg, sizeof(buf));
	strlcat(buf, "\r\n", sizeof(buf));

	for (d = descriptor_list; d; d = dnext) {
		dnext = d->next;
		queue_ansi(d, buf);
		/* queue_write(d, "\r\n", 2); */
		if (!process_output(d)) {
			d->booted = 1;
		}
	}
}

void
flush_user_output(dbref player)
{
    int di;
    int* darr;
    int dcount;
	struct descriptor_data *d;

	darr = get_player_descrs(OWNER(player), &dcount);
    for (di = 0; di < dcount; di++) {
        d = descrdata_by_descr(darr[di]);
        if (d && !process_output(d)) {
            d->booted = 1;
        }
    }
}

void
wall_wizards(const char *msg)
{
	struct descriptor_data *d, *dnext;
	char buf[BUFFER_LEN + 2];

	strlcpy(buf, msg, sizeof(buf));
	strlcat(buf, "\r\n", sizeof(buf));

	for (d = descriptor_list; d; d = dnext) {
		dnext = d->next;
		if (d->connected && Wizard(d->player)) {
			queue_ansi(d, buf);
			if (!process_output(d)) {
				d->booted = 1;
			}
		}
	}
}

struct descriptor_data *
new_connection(int port, int sock)
{
	int newsock;
	struct sockaddr_in addr;
	socklen_t addr_len;

	addr_len = (socklen_t)sizeof(addr);
	newsock = accept(sock, (struct sockaddr *) &addr, &addr_len);
	if (newsock < 0) {
		return 0;
	} else {
#ifdef F_SETFD
		fcntl(newsock, F_SETFD, 1);
#endif
		warn("ACCEPT: %d on descriptor %d", ntohs(addr.sin_port), newsock);
		warn("CONCOUNT: There are now %d open connections.", ++ndescriptors);
		return initializesock(newsock);
	}
}

/*  addrout -- Translate address 'a' from addr struct to text.		*/

const char *
addrout(int lport, long a, unsigned short prt)
{
	static char buf[128];
	struct in_addr addr;

	addr.s_addr = a;

	prt = ntohs(prt);

	a = ntohl(a);

	snprintf(buf, sizeof(buf), "%ld.%ld.%ld.%ld(%u)",
			(a >> 24) & 0xff, (a >> 16) & 0xff, (a >> 8) & 0xff, a & 0xff, prt);
	return buf;
}

void
shutdownsock(struct descriptor_data *d)
{
	if (d->connected) {
		warn("DISCONNECT: descriptor %d player %s(%d) from %s",
				   d->descriptor, NAME(d->player), d->player, d->username);
		announce_disconnect(d);
	} else {
		warn("DISCONNECT: descriptor %d from %s never connected.",
				   d->descriptor, d->username);
	}
	shutdown(d->descriptor, 2);
	close(d->descriptor);
	if (d)
		descr_lookup_table[d->descriptor] = NULL;
	freeqs(d);
	*d->prev = d->next;
	if (d->next)
		d->next->prev = d->prev;
	if (d->username)
		free((void *) d->username);
	mcp_frame_clear(&d->mcpframe);
	free(d);
	ndescriptors--;
	warn("CONCOUNT: There are now %d open connections.", ndescriptors);
}

void
SendText(McpFrame * mfr, const char *text)
{
	queue_string((struct descriptor_data *) mfr->descriptor, text);
}

void
FlushText(McpFrame * mfr)
{
	struct descriptor_data *d = (struct descriptor_data *)mfr->descriptor;
	if (d && !process_output(d)) {
		d->booted = 1;
	}
}

int
mcpframe_to_descr(McpFrame * ptr)
{
	return ((struct descriptor_data *) ptr->descriptor)->descriptor;
}

int
mcpframe_to_user(McpFrame * ptr)
{
	return ((struct descriptor_data *) ptr->descriptor)->player;
}

struct descriptor_data *
initializesock(int s)
{
	struct descriptor_data *d;

	d = malloc(sizeof(struct descriptor_data));

	memset(d, 0, sizeof(struct descriptor_data));
	d->descriptor = s;
	d->connected = 0;
	d->booted = 0;
	d->player = -1;
	d->con_number = 0;
	d->connected_at = time(NULL);
	if (fcntl(s, F_SETFL, O_NONBLOCK) == -1) {
		perror("make_nonblocking: fcntl");
		panic("O_NONBLOCK fcntl failed");
	}
	d->output_size = 0;
	d->output.lines = 0;
	d->output.head = 0;
	d->output.tail = &d->output.head;
	d->input.lines = 0;
	d->input.head = 0;
	d->input.tail = &d->input.head;
	d->raw_input = 0;
	d->raw_input_at = 0;
	d->telnet_enabled = 0;
	d->telnet_state = TELNET_STATE_NORMAL;
	d->telnet_sb_opt = 0;
	d->quota = COMMAND_BURST_SIZE;
	d->last_time = d->connected_at;
	d->last_pinged_at = d->connected_at;
	mcp_frame_init(&d->mcpframe, d);
	d->username = alloc_string("");
	if (descriptor_list)
		descriptor_list->prev = &d->next;
	d->next = descriptor_list;
	d->prev = &descriptor_list;
	descriptor_list = d;
	if (d)
		descr_lookup_table[d->descriptor] = d;
	mcp_negotiation_start(&d->mcpframe);
	return d;
}

int
make_socket(int port)
{
	int s;
	int opt;
	struct sockaddr_in server;

	s = socket(AF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		perror("creating stream socket");
		exit(3);
	}

	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	opt = 1;
	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	/*
	opt = 240;
	if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(1);
	}
	*/

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(s, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding stream socket");
		close(s);
		exit(4);
	}
	listen(s, 5);
	return s;
}

struct text_block *
make_text_block(const char *s, int n)
{
	struct text_block *p;

	p = malloc(sizeof(struct text_block));
	p->buf = malloc(n * sizeof(char));
	bcopy(s, p->buf, n);
	p->nchars = n;
	p->start = p->buf;
	p->nxt = 0;
	return p;
}

void
free_text_block(struct text_block *t)
{
	free(t->buf);
	free((char *) t);
}

void
add_to_queue(struct text_queue *q, const char *b, int n)
{
	struct text_block *p;

	if (n == 0)
		return;

	p = make_text_block(b, n);
	p->nxt = 0;
	*q->tail = p;
	q->tail = &p->nxt;
	q->lines++;
}

int
flush_queue(struct text_queue *q, int n)
{
	struct text_block *p;
	int really_flushed = 0;

	n += strlen(flushed_message);

	while (n > 0 && (p = q->head)) {
		n -= p->nchars;
		really_flushed += p->nchars;
		q->head = p->nxt;
		q->lines--;
		free_text_block(p);
	}
	p = make_text_block(flushed_message, strlen(flushed_message));
	p->nxt = q->head;
	q->head = p;
	q->lines++;
	if (!p->nxt)
		q->tail = &p->nxt;
	really_flushed -= p->nchars;
	return really_flushed;
}

int
queue_write(struct descriptor_data *d, const char *b, int n)
{
	int space;

	space = MAX_OUTPUT - d->output_size - n;
	if (space < 0)
		d->output_size -= flush_queue(&d->output, -space);
	add_to_queue(&d->output, b, n);
	d->output_size += n;
	return n;
}

int
queue_string(struct descriptor_data *d, const char *s)
{
	return queue_write(d, s, strlen(s));
}

int
send_keepalive(struct descriptor_data *d)
{
	int cnt;
	char telnet_nop[] = {
		TELNET_IAC, TELNET_NOP, '\0'
	};

	/* drastic, but this may give us crash test data */
	if (!d || !d->descriptor) {
		fprintf(stderr, "process_output: bad descriptor or connect struct!\n");
		abort();
	}

	if (d->telnet_enabled) {
		cnt = socket_write(d, telnet_nop, strlen(telnet_nop));
	} else {
		cnt = socket_write(d, "", 0);
	}
	/* We expect a 0 return */
	if (cnt < 0) {
		if (errno == EWOULDBLOCK)
			return 1;
		if (errno == 0)
			return 1;
		warn("keepalive socket write descr=%i, errno=%i", d->descriptor, errno);
		return 0;
	}
	return 1;
}

int
process_output(struct descriptor_data *d)
{
	struct text_block **qp, *cur;
	int cnt;

	/* drastic, but this may give us crash test data */
	if (!d || !d->descriptor) {
		fprintf(stderr, "process_output: bad descriptor or connect struct!\n");
		abort();
	}

	if (d->output.lines == 0) {
		return 1;
	}

	for (qp = &d->output.head; (cur = *qp);) {
		cnt = socket_write(d, cur->start, cur->nchars);

		if (cnt <= 0) {
			if (errno == EWOULDBLOCK)
				return 1;
			return 0;
		}
		d->output_size -= cnt;
		if (cnt == cur->nchars) {
			d->output.lines--;
			if (!cur->nxt) {
				d->output.tail = qp;
				d->output.lines = 0;
			}
			*qp = cur->nxt;
			free_text_block(cur);
			continue;			/* do not adv ptr */
		}
		cur->nchars -= cnt;
		cur->start += cnt;
		break;
	}
	return 1;
}

void
freeqs(struct descriptor_data *d)
{
	struct text_block *cur, *next;

	cur = d->output.head;
	while (cur) {
		next = cur->nxt;
		free_text_block(cur);
		cur = next;
	}
	d->output.lines = 0;
	d->output.head = 0;
	d->output.tail = &d->output.head;

	cur = d->input.head;
	while (cur) {
		next = cur->nxt;
		free_text_block(cur);
		cur = next;
	}
	d->input.lines = 0;
	d->input.head = 0;
	d->input.tail = &d->input.head;

	if (d->raw_input)
		free(d->raw_input);
	d->raw_input = 0;
	d->raw_input_at = 0;
}

void
save_command(struct descriptor_data *d, const char *command)
{
	add_to_queue(&d->input, command, strlen(command) + 1);
}

int
process_input(struct descriptor_data *d)
{
	char buf[MAX_COMMAND_LEN * 2];
	int got;
	char *p, *pend, *q, *qend;

	got = socket_read(d, buf, sizeof(buf));

	if (got <= 0)
		return 0;

	if (!d->raw_input) {
		d->raw_input = malloc(MAX_COMMAND_LEN * sizeof(char));
		d->raw_input_at = d->raw_input;
	}
	p = d->raw_input_at;
	pend = d->raw_input + MAX_COMMAND_LEN - 1;
	for (q = buf, qend = buf + got; q < qend; q++) {
		if (*q == '\n') {
			d->last_time = time(NULL);
			*p = '\0';
			if (p >= d->raw_input)
				save_command(d, d->raw_input);
			p = d->raw_input;
		} else if (d->telnet_state == TELNET_STATE_IAC) {
			switch (*((unsigned char *)q)) {
				case TELNET_BRK: /* Break */
				case TELNET_IP: /* Interrupt Process */
					save_command(d, BREAK_COMMAND);
					d->telnet_state = TELNET_STATE_NORMAL;
					break;
				case TELNET_AYT: /* AYT */
					{
						char sendbuf[] = "[Yes]\r\n";
						socket_write(d, sendbuf, strlen(sendbuf));
						d->telnet_state = TELNET_STATE_NORMAL;
						break;
					}
				case TELNET_EC: /* Erase character */
					if (p > d->raw_input)
						p--;
					d->telnet_state = TELNET_STATE_NORMAL;
					break;
				case TELNET_EL: /* Erase line */
					p = d->raw_input;
					d->telnet_state = TELNET_STATE_NORMAL;
					break;
				case TELNET_WONT:
					d->telnet_state = TELNET_STATE_WONT;
					break;
				case TELNET_WILL:
				case TELNET_DO:
				case TELNET_DONT:
					d->telnet_state = TELNET_STATE_DONT;
					break;
				case TELNET_SB: /* SB (option subnegotiation) */
					d->telnet_state = TELNET_STATE_SB;
					break;
				case TELNET_IAC: /* IAC a second time */
#if 0
					/* If we were 8 bit clean, we'd pass this along */
					*p++ = *q;
#endif
				default:
					/* just ignore */
					d->telnet_state = TELNET_STATE_NORMAL;
					break;
			}
		} else if (d->telnet_state == TELNET_STATE_DONT) {
			/* We don't negotiate: send back DONT option */
			unsigned char sendbuf[4];
			sendbuf[0] = TELNET_IAC;
			sendbuf[1] = TELNET_DONT;
			sendbuf[2] = *q;
			sendbuf[3] = '\0';
			socket_write(d, sendbuf, 3);
			d->telnet_state = TELNET_STATE_NORMAL;
			d->telnet_enabled = 1;
		} else if (d->telnet_state == TELNET_STATE_WONT) {
			/* Ignore WONT option. */
			d->telnet_state = TELNET_STATE_NORMAL;
			d->telnet_enabled = 1;
		} else if (d->telnet_state == TELNET_STATE_SB) {
			d->telnet_sb_opt = *((unsigned char*)q);
			/* TODO: Start remembering subnegotiation data. */
			d->telnet_state = TELNET_STATE_NORMAL;
		} else if (*((unsigned char *)q) == TELNET_IAC) {
			/* Got TELNET IAC, store for next byte */	
			d->telnet_state = TELNET_STATE_IAC;
		} else if (p < pend) {
			/* NOTE: This will need rethinking for unicode */
			if ( isinput( *q ) ) {
				*p++ = *q;
			} else if (*q == '\t') {
				*p++ = ' ';
			} else if (*q == 8 || *q == 127) {
				/* if BS or DEL, delete last character */
				if (p > d->raw_input)
					p--;
			}
			d->telnet_state = TELNET_STATE_NORMAL;
		}
	}
	if (p > d->raw_input) {
		d->raw_input_at = p;
	} else {
		free(d->raw_input);
		d->raw_input = 0;
		d->raw_input_at = 0;
	}
	return 1;
}

void
process_commands(void)
{
	int nprocessed;
	struct descriptor_data *d, *dnext;
	struct text_block *t;

	do {
		nprocessed = 0;
		for (d = descriptor_list; d; d = dnext) {
			dnext = d->next;
			if (d->quota > 0 && (t = d->input.head)) {
				if (d->connected && PLAYER_BLOCK(d->player) && !is_interface_command(t->start)) {
					char *tmp = t->start;
					if (!strncmp(tmp, "#$\"", 3)) {
						/* Un-escape MCP escaped lines */
						tmp += 3;
					}

					/* Didn't send blank line.  Eat it.  */
					nprocessed++;
					d->input.head = t->nxt;
					d->input.lines--;
					if (!d->input.head) {
						d->input.tail = &d->input.head;
						d->input.lines = 0;
					}
					free_text_block(t);
				} else {
					if (strncmp(t->start, "#$#", 3)) {
						/* Not an MCP mesg, so count this against quota. */
						d->quota--;
					}
					nprocessed++;
					if (!do_command(d, t->start)) {
						d->booted = 2;
						/* Disconnect player next pass through main event loop. */
					}
					d->input.head = t->nxt;
					d->input.lines--;
					if (!d->input.head) {
						d->input.tail = &d->input.head;
						d->input.lines = 0;
					}
					free_text_block(t);
				}
			}
		}
	} while (nprocessed > 0);
}

int
is_interface_command(const char* cmd)
{
	const char* tmp = cmd;
	if (!strncmp(tmp, "#$\"", 3)) {
		/* dequote MCP quoting. */
		tmp += 3;
	}
	if (!strncmp(cmd, "#$#", 3)) /* MCP mesg. */
		return 1;
	if (!strcmp(tmp, BREAK_COMMAND))
		return 1;
	if (!strcmp(tmp, QUIT_COMMAND))
		return 1;
	if (!strncmp(tmp, WHO_COMMAND, strlen(WHO_COMMAND)))
		return 1;
	return 0;
}

int
do_command(struct descriptor_data *d, char *command)
{
	char buf[BUFFER_LEN];
	char cmdbuf[BUFFER_LEN];

	if (!mcp_frame_process_input(&d->mcpframe, command, cmdbuf, sizeof(cmdbuf))) {
		d->quota++;
		return 1;
	}
	command = cmdbuf;
	if (d->connected)
		ts_lastuseobject(d->player);

	if (!strcmp(command, QUIT_COMMAND)) {
		return 0;
	} else if ((!strncmp(command, WHO_COMMAND, sizeof(WHO_COMMAND) - 1)) ||
                   (*command == OVERIDE_TOKEN &&
                    (!strncmp(command+1, WHO_COMMAND, sizeof(WHO_COMMAND) - 1))
                   )) {
		strlcpy(buf, "@", sizeof(buf));
		strlcat(buf, WHO_COMMAND, sizeof(buf));
		strlcat(buf, " ", sizeof(buf));
		strlcat(buf, command + sizeof(WHO_COMMAND) - 1, sizeof(buf));
		if (!d->connected || (FLAGS(d->player) & INTERACTIVE)) {
#if SECURE_WHO
			queue_ansi(d, "Sorry, WHO is unavailable at this point.\r\n");
#else
			dump_users(d, command + sizeof(WHO_COMMAND) - 1);
#endif
		} else {
			if ((!(TrueWizard(OWNER(d->player)) &&
                              (*command == OVERIDE_TOKEN))) &&
                            can_move(d->descriptor, d->player, buf, 2)) {
				do_move(d->descriptor, d->player, buf, 2);
			} else {
				dump_users(d, command + sizeof(WHO_COMMAND) - 
                                           ((*command == OVERIDE_TOKEN)?0:1));
			}
		}
	} else {
		if (d->connected) {
			process_command(d->descriptor, d->player, command);
		} else {
			char commandb[MAX_COMMAND_LEN];
			char user[MAX_COMMAND_LEN];
			char password[MAX_COMMAND_LEN];

			parse_connect(command, commandb, user, password);

			if (*commandb == 'c')
				auth(d->descriptor, user, password);
			else
				welcome_user(d);
		}
	}
	return 1;
}

void
interact_warn(dbref player)
{
	if (FLAGS(player) & INTERACTIVE) {
		char buf[BUFFER_LEN];

		snprintf(buf, sizeof(buf), "***  %s  ***",
				(FLAGS(player) & READMODE) ?
				"You are currently using a program.  Use \"@Q\" to return to a more reasonable state of control."
				: (PLAYER_INSERT_MODE(player) ?
				   "You are currently inserting MUF program text.  Use \".\" to return to the editor, then \"quit\" if you wish to return to your regularly scheduled Muck universe."
				   : "You are currently using the MUF program editor."));
		notify(player, buf);
	}
}

int
auth(int descr, char *user, char *password)
{
        int created = 0;
        dbref player = connect_player(user, password);
	struct descriptor_data *d = descrdata_by_descr(descr);

        if (((optflags & OPT_WIZONLY) || (PLAYERMAX && con_players_curr >= PLAYERMAX_LIMIT)) && !TrueWizard(player)) {
                queue_ansi(d, (optflags & OPT_WIZONLY)
                           ? "Sorry, but the game is in maintenance mode currently, and "
                           "only wizards are allowed to connect.  Try again later."
                           : PLAYERMAX_BOOTMESG);
                queue_string(d, "\r\n");
                d->booted = 1;
                return 1;
        }

        if (player == NOTHING) {
                player = create_player(user, password);

                if (player == NOTHING) {
                        queue_ansi(d, create_fail);
                        /* if (d->web.ip) */
                        /*         web_logout(d->descriptor); */

                        warn("FAILED CREATE %s on descriptor %d", user, d->descriptor);
                        return 1;
                }

                warn("CREATED %s(%d) on descriptor %d",
                           NAME(player), player, d->descriptor);
                created = 1;
        } else
                warn("CONNECTED: %s(%d) on descriptor %d",
                           NAME(player), player, d->descriptor);
        d->connected = 1;
        d->connected_at = time(NULL);
        d->player = player;
        update_desc_count_table();
        remember_player_descr(player, d->descriptor);
        /* cks: someone has to initialize this somewhere. */
        PLAYER_SET_BLOCK(d->player, 0);
        welcome_user(d);
        spit_file(player, MOTD_FILE);
        announce_connect(d, player);
        if (created) {
                do_help(player, "begin", "");
                mob_put(player);
        } else {
                interact_warn(player);
                if (sanity_violated && Wizard(player))
                        notify(player,
                               "#########################################################################\n"
                               "## WARNING!  The DB appears to be corrupt!  Please repair immediately! ##\n"
                               "#########################################################################");
        }
        if (!(web_support(d->descriptor) && d->web.old))
                do_view(d->descriptor, player);

        look_room(d->descriptor, player, getloc(player), 0);

        con_players_curr++;
        return 0;
}

void
identify(int descr, unsigned ip, unsigned old)
{
	struct descriptor_data *d = descrdata_by_descr(descr);
        d->web.ip = ip;
        d->web.old = old;
}

void
parse_connect(const char *msg, char *command, char *user, char *pass)
{
	char *p;

	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = command;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = tolower(*msg++);
	*p = '\0';
	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = user;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = *msg++;
	*p = '\0';
	while (*msg && isinput(*msg) && isspace(*msg))
		msg++;
	p = pass;
	while (*msg && isinput(*msg) && !isspace(*msg))
		*p++ = *msg++;
	*p = '\0';
}

int
boot_off(dbref player)
{
    int* darr;
    int dcount;
	struct descriptor_data *last = NULL;

	darr = get_player_descrs(player, &dcount);
	if (darr) {
        last = descrdata_by_descr(darr[0]);
	}

	if (last) {
		process_output(last);
		last->booted = 1;
		/* shutdownsock(last); */
		return 1;
	}
	return 0;
}

void
boot_player_off(dbref player)
{
    int di;
    int* darr;
    int dcount;
    struct descriptor_data *d;
 
	darr = get_player_descrs(player, &dcount);
    for (di = 0; di < dcount; di++) {
        d = descrdata_by_descr(darr[di]);
        if (d) {
            d->booted = 1;
        }
    }
}

void
close_sockets(const char *msg) {
	struct descriptor_data *d, *dnext;
	int i;

	for (d = descriptor_list; d; d = dnext) {
		dnext = d->next;
		if (d->connected) {
			forget_player_descr(d->player, d->descriptor);
		}
		if (!d->web.ip) {
			socket_write(d, msg, strlen(msg));
			socket_write(d, shutdown_message, strlen(shutdown_message));
		}
		if (shutdown(d->descriptor, 2) < 0)
			perror("shutdown");
		close(d->descriptor);
		freeqs(d);
		*d->prev = d->next;
		if (d->next)
			d->next->prev = d->prev;
		if (d->username)
			free((void *) d->username);
		mcp_frame_clear(&d->mcpframe);
		free(d);
		ndescriptors--;
	}
	update_desc_count_table();
	for (i = 0; i < numsocks; i++) {
		close(sock[i]);
	}
}

void
do_armageddon(dbref player, const char *msg)
{
	char buf[BUFFER_LEN];

	if (!Wizard(player)) {
		notify(player, "Sorry, but you don't look like the god of War to me.");
		warn("ILLEGAL ARMAGEDDON: tried by %s", unparse_object(player, player));
		return;
	}
	snprintf(buf, sizeof(buf), "\r\nImmediate shutdown initiated by %s.\r\n", NAME(player));
	if (msg || *msg)
		strlcat(buf, msg, sizeof(buf));
	warn("ARMAGEDDON initiated by %s(%d): %s", NAME(player), player, msg);
	fprintf(stderr, "ARMAGEDDON initiated by %s(%d): %s\n", NAME(player), player, msg);
	close_sockets(buf);

	exit(1);
}

void
emergency_shutdown(void)
{
	close_sockets("\r\nEmergency shutdown due to server crash.");
}

void
dump_users(struct descriptor_data *e, char *user)
{
	struct descriptor_data *d;
	int wizard, players;
	time_t now;
	char buf[2048];
	char pbuf[64];
	char secchar = ' ';

/* #ifdef GOD_PRIV */
/* -- Wizard should always override WHO_DOING JES
	if (WHO_DOING) {
		wizard = e->connected && God(e->player);
	} else {
		wizard = e->connected && Wizard(e->player);
	}
*/
/* #else */
	wizard = e->connected && Wizard(e->player) && !WHO_DOING;
/* #endif */

	while (*user && (isspace(*user) || *user == '*')) {
#if WHO_DOING
		if (*user == '*' && e->connected && Wizard(e->player))
			wizard = 1;
#endif
		user++;
	}

	if (wizard)
		/* S/he is connected and not quelled. Okay; log it. */
		warn("WIZ: %s(%d) in %s(%d):  %s", NAME(e->player),
					(int) e->player, NAME(DBFETCH(e->player)->location),
					(int) DBFETCH(e->player)->location, "WHO");

	if (!*user)
		user = NULL;

	(void) time(&now);
	if (wizard) {
		queue_ansi(e, "Player Name                Location     On For Idle   Host\r\n");
	} else {
#if WHO_DOING
		queue_ansi(e, "Player Name           On For Idle   Doing...\r\n");
#else
		queue_ansi(e, "Player Name           On For Idle\r\n");
#endif
	}

	d = descriptor_list;
	players = 0;
	while (d) {
		if (d->connected &&
			(!WHO_HIDES_DARK ||
			 (wizard || !(FLAGS(d->player) & DARK))) &&
			++players && (!user || string_prefix(NAME(d->player), user))
				) {

			secchar = ' ';

			if (wizard) {
				/* don't print flags, to save space */
				snprintf(pbuf, sizeof(pbuf), "%.*s(#%d)", PLAYER_NAME_LIMIT + 1,
						NAME(d->player), (int) d->player);
#ifdef GOD_PRIV
				if (!God(e->player))
					snprintf(buf, sizeof(buf),
							"%-*s [%6d] %10s %4s%c%c\r\n",
							PLAYER_NAME_LIMIT + 10, pbuf,
							(int) DBFETCH(d->player)->location,
							time_format_1(now - d->connected_at),
							time_format_2(now - d->last_time),
							((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
							secchar);
				else
#endif
					snprintf(buf, sizeof(buf),
							"%-*s [%6d] %10s %4s%c%c %s\r\n",
							PLAYER_NAME_LIMIT + 10, pbuf,
							(int) DBFETCH(d->player)->location,
							time_format_1(now - d->connected_at),
							time_format_2(now - d->last_time),
							((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
							secchar, d->username);
			} else {
#if WHO_DOING
				/* Modified to take into account PLAYER_NAME_LIMIT changes */
				snprintf(buf, sizeof(buf), "%-*s %10s %4s%c%c %.*s\r\n",
					 PLAYER_NAME_LIMIT + 1,
					 NAME(d->player),
					 time_format_1(now - d->connected_at),
					 time_format_2(now - d->last_time),
					 ((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
					 secchar,
					 /* Things must end on column 79. The required cols
					  * (not counting player name, but counting forced
					  * space after it) use up 20 columns.
					  *
					  * !! Don't forget to update this if that changes
					  */
					 (int) (79 - (PLAYER_NAME_LIMIT + 20)),
					 GETDOING(d->player) ?
					 GETDOING(d->player) : ""
					);
#else
				snprintf(buf, sizeof(buf), "%-*s %10s %4s%c%c\r\n",
					 (int)(PLAYER_NAME_LIMIT + 1),
					 NAME(d->player),
					 time_format_1(now - d->connected_at),
					 time_format_2(now - d->last_time),
					 ((FLAGS(d->player) & INTERACTIVE) ? '*' : ' '),
					 secchar);
#endif
			}
			queue_ansi(e, buf);
		}
		d = d->next;
	}
	if (players > con_players_max)
		con_players_max = players;
	snprintf(buf, sizeof(buf), "%d player%s %s connected.  (Max was %d)\r\n", players,
			(players == 1) ? "" : "s", (players == 1) ? "is" : "are", con_players_max);
	queue_ansi(e, buf);
}

char *
time_format_1(long dt)
{
	register struct tm *delta;
	static char buf[64];

	delta = gmtime((time_t *) &dt);
	if (delta->tm_yday > 0)
		snprintf(buf, sizeof(buf), "%dd %02d:%02d", delta->tm_yday, delta->tm_hour, delta->tm_min);
	else
		snprintf(buf, sizeof(buf), "%02d:%02d", delta->tm_hour, delta->tm_min);
	return buf;
}

char *
time_format_2(long dt)
{
	register struct tm *delta;
	static char buf[64];

	delta = gmtime((time_t *) &dt);
	if (delta->tm_yday > 0)
		snprintf(buf, sizeof(buf), "%dd", delta->tm_yday);
	else if (delta->tm_hour > 0)
		snprintf(buf, sizeof(buf), "%dh", delta->tm_hour);
	else if (delta->tm_min > 0)
		snprintf(buf, sizeof(buf), "%dm", delta->tm_min);
	else
		snprintf(buf, sizeof(buf), "%ds", delta->tm_sec);
	return buf;
}

void
announce_puppets(dbref player, const char *msg, const char *prop)
{
	dbref what, where;
	const char *ptr, *msg2;
	char buf[BUFFER_LEN];

	for (what = 0; what < db_top; what++) {
		if (Typeof(what) == TYPE_THING && (FLAGS(what) & ZOMBIE)) {
			if (OWNER(what) == player) {
				where = getloc(what);
				if ((!Dark(where)) && (!Dark(player)) && (!Dark(what))) {
					msg2 = msg;
					if ((ptr = (char *) get_property_class(what, prop)) && *ptr)
						msg2 = ptr;
					snprintf(buf, sizeof(buf), "%.512s %.3000s", NAME(what), msg2);
					notify_except(DBFETCH(where)->contents, what, buf, what);
				}
			}
		}
	}
}

void
announce_connect(struct descriptor_data *d, dbref player)
{
	dbref loc;
	char buf[BUFFER_LEN];
	struct match_data md;
	dbref exit;
	int descr = d->descriptor;

	if ((loc = getloc(player)) == NOTHING)
		return;

	if ((!Dark(player)) && (!Dark(loc))) {
		snprintf(buf, sizeof(buf), "%s has connected.", NAME(player));
		notify_except(DBFETCH(loc)->contents, player, buf, player);
	}

	exit = NOTHING;
	if (PLAYER_DESCRCOUNT(player)) {
		init_match(descr, player, "connect", TYPE_EXIT, &md);	/* match for connect */
		md.match_level = 1;
		match_all_exits(&md);
		exit = match_result(&md);
		if (exit == AMBIGUOUS)
			exit = NOTHING;
	}

	if (!d->web.ip && (exit == NOTHING || !(FLAGS(exit) & STICKY))) {
		if (can_move(descr, player, AUTOLOOK_CMD, 1)) {
			do_move(descr, player, AUTOLOOK_CMD, 1);
		} else {
			do_look_around(descr, player);
		}
	}

	/*
	 * See if there's a connect action.  If so, and the player is the first to
	 * connect, send the player through it.  If the connect action is set
	 * sticky, then suppress the normal look-around.
	 */

	if (exit != NOTHING)
		do_move(descr, player, "connect", 1);

	if (PLAYER_DESCRCOUNT(player) == 1) {
		announce_puppets(player, "wakes up.", "_/pcon");
	}

	/* queue up all _connect programs referred to by properties */
	envpropqueue(descr, player, getloc(player), NOTHING, player, NOTHING,
				 "_connect", "Connect", 1, 1);
	envpropqueue(descr, player, getloc(player), NOTHING, player, NOTHING,
				 "_oconnect", "Oconnect", 1, 0);

	ts_useobject(player);
	return;
}

void
announce_disconnect(struct descriptor_data *d)
{
	dbref player = d->player;
	dbref loc;
	char buf[BUFFER_LEN];

	if ((loc = getloc(player)) == NOTHING)
		return;

	if ((!Dark(player)) && (!Dark(loc))) {
		snprintf(buf, sizeof(buf), "%s has disconnected.", NAME(player));
		notify_except(DBFETCH(loc)->contents, player, buf, player);
	}

	/* trigger local disconnect action */
	if (PLAYER_DESCRCOUNT(player) == 1) {
		if (can_move(d->descriptor, player, "disconnect", 1)) {
			do_move(d->descriptor, player, "disconnect", 1);
		}
		announce_puppets(player, "falls asleep.", "_/pdcon");
	}

	d->connected = 0;
	d->player = NOTHING;

    forget_player_descr(player, d->descriptor);
    update_desc_count_table();

	/* queue up all _connect programs referred to by properties */
	envpropqueue(d->descriptor, player, getloc(player), NOTHING, player, NOTHING,
				 "_disconnect", "Disconnect", 1, 1);
	envpropqueue(d->descriptor, player, getloc(player), NOTHING, player, NOTHING,
				 "_odisconnect", "Odisconnect", 1, 0);

	ts_lastuseobject(player);
	DBDIRTY(player);
}

/***** O(1) Connection Optimizations *****/

void
update_desc_count_table()
{
	int c;
	struct descriptor_data *d;

	current_descr_count = 0;
	for (c = 0, d = descriptor_list; d; d = d->next)
	{
		if (d->connected)
		{
			d->con_number = c + 1;
			descr_count_table[c++] = d;
			current_descr_count++;
		}
	}
}

struct descriptor_data *
descrdata_by_count(int c)
{
	c--;
	if (c >= current_descr_count || c < 0) {
		return NULL;
	}
	return descr_count_table[c];
}

int
index_descr(int index)
{
    if((index < 0) || (index >= FD_SETSIZE))
		return -1;
	if(descr_lookup_table[index] == NULL)
		return -1;
	return descr_lookup_table[index]->descriptor;
}

int*
get_player_descrs(dbref player, int* count)
{
	int* darr;

	if (Typeof(player) == TYPE_PLAYER) {
		*count = PLAYER_DESCRCOUNT(player);
	    darr = PLAYER_DESCRS(player);
		if (!darr) {
			*count = 0;
		}
		return darr;
	} else {
		*count = 0;
		return NULL;
	}
}

void
remember_player_descr(dbref player, int descr)
{
	int  count = 0;
	int* arr   = NULL;

	if (Typeof(player) != TYPE_PLAYER)
		return;

	count = PLAYER_DESCRCOUNT(player);
	arr = PLAYER_DESCRS(player);

	if (!arr) {
		arr = (int*)malloc(sizeof(int));
		arr[0] = descr;
		count = 1;
	} else {
		arr = (int*)realloc(arr,sizeof(int) * (count+1));
		arr[count] = descr;
		count++;
	}
	PLAYER_SET_DESCRCOUNT(player, count);
	PLAYER_SET_DESCRS(player, arr);
}

void
forget_player_descr(dbref player, int descr)
{
	int  count = 0;
	int* arr   = NULL;

	if (Typeof(player) != TYPE_PLAYER)
		return;

	count = PLAYER_DESCRCOUNT(player);
	arr = PLAYER_DESCRS(player);

	if (!arr) {
		count = 0;
	} else if (count > 1) {
		int src, dest;
		for (src = dest = 0; src < count; src++) {
			if (arr[src] != descr) {
				if (src != dest) {
					arr[dest] = arr[src];
				}
				dest++;
			}
		}
		if (dest != count) {
			count = dest;
			arr = (int*)realloc(arr,sizeof(int) * count);
		}
	} else {
		free((void*)arr);
		arr = NULL;
		count = 0;
	}
	PLAYER_SET_DESCRCOUNT(player, count);
	PLAYER_SET_DESCRS(player, arr);
}

struct descriptor_data *
descrdata_by_descr(int c)
{
	if (c >= FD_SETSIZE || c < 0)
		return NULL;
	else
		return descr_lookup_table[c];
}

/*** JME ***/

/*** Foxen ***/
int
least_idle_player_descr(dbref who)
{
	struct descriptor_data *d;
	struct descriptor_data *best_d = NULL;
	int dcount, di;
	int* darr;
	long best_time = 0;

	darr = get_player_descrs(who, &dcount);
	for (di = 0; di < dcount; di++) {
		d = descrdata_by_descr(darr[di]);
		if (d && (!best_time || d->last_time > best_time)) {
			best_d = d;
			best_time = d->last_time;
		}
	}
	if (best_d) {
		return best_d->con_number;
	}
	return 0;
}

int
most_idle_player_descr(dbref who)
{
	struct descriptor_data *d;
	struct descriptor_data *best_d = NULL;
	int dcount, di;
	int* darr;
	long best_time = 0;

	darr = get_player_descrs(who, &dcount);
	for (di = 0; di < dcount; di++) {
		d = descrdata_by_descr(darr[di]);
		if (d && (!best_time || d->last_time < best_time)) {
			best_d = d;
			best_time = d->last_time;
		}
	}
	if (best_d) {
		return best_d->con_number;
	}
	return 0;
}

void
pboot(int c)
{
	struct descriptor_data *d;

	d = descrdata_by_count(c);

	if (d) {
		process_output(d);
		d->booted = 1;
		/* shutdownsock(d); */
	}
}

int 
pdescrboot(int c)
{
    struct descriptor_data *d;

    d = descrdata_by_descr(c);

    if (d) {
		process_output(d);
		d->booted = 1;
		/* shutdownsock(d); */
		return 1;
    }
	return 0;
}

void
pnotify(int c, char *outstr)
{
	struct descriptor_data *d;

	d = descrdata_by_count(c);

	if (d) {
		queue_ansi(d, outstr);
		queue_write(d, "\r\n", 2);
	}
}

int
pdescrnotify(int c, char *outstr)
{
	struct descriptor_data *d;

	d = descrdata_by_descr(c);

	if (d) {
		queue_ansi(d, outstr);
		queue_write(d, "\r\n", 2);
		return 1;
	}
	return 0;
}

int
pdescr(int c)
{
	struct descriptor_data *d;

	d = descrdata_by_count(c);

	if (d) {
		return (d->descriptor);
	}

	return -1;
}

int 
pdescrcount(void)
{
    return current_descr_count;
}

int 
pfirstdescr(void)
{
    struct descriptor_data *d;

	d = descrdata_by_count(1);
    if (d) {
		return d->descriptor;
	}

	return 0;
}

int 
plastdescr(void)
{
    struct descriptor_data *d;

	d = descrdata_by_count(current_descr_count);
	if (d) {
		return d->descriptor;
	}
	return 0;
}

int
pnextdescr(int c)
{
	struct descriptor_data *d;

    d = descrdata_by_descr(c);
	if (d) {
		d = d->next;
	}
	while (d && (!d->connected))
		d = d->next;
	if (d) {
		return (d->descriptor);
	}
	return (0);
}

int
pdescrcon(int c)
{
	struct descriptor_data *d;

    d = descrdata_by_descr(c);
	if (d) {
		return d->con_number;
	} else {
		return 0;
	}
}

int
pset_user(int c, dbref who)
{
	struct descriptor_data *d;
	static int setuser_depth = 0;

	if (++setuser_depth > 8) {
		/* Prevent infinite loops */
		setuser_depth--;
		return 0;
	}

    d = descrdata_by_descr(c);
	if (d && d->connected) {
		announce_disconnect(d);
		if (who != NOTHING) {
			d->player = who;
			d->connected = 1;
			update_desc_count_table();
            remember_player_descr(who, d->descriptor);
			announce_connect(d, who);
		}
		setuser_depth--;
		return 1;
	}
	setuser_depth--;
	return 0;
}

int
dbref_first_descr(dbref c)
{
	int dcount;
	int* darr;

	darr = get_player_descrs(c, &dcount);
	if (dcount > 0) {
		return darr[dcount - 1];
	} else {
		return -1;
	}
}

McpFrame *
descr_mcpframe(int c)
{
	struct descriptor_data *d;

    d = descrdata_by_descr(c);
	if (d) {
		return &d->mcpframe;
	}
	return NULL;
}

int
pdescrflush(int c)
{
	struct descriptor_data *d;
	int i = 0;

	if (c != -1) {
		d = descrdata_by_descr(c);
		if (d) {
			if (!process_output(d)) {
				d->booted = 1;
			}
			i++;
		}
	} else {
		for (d = descriptor_list; d; d = d->next) {
			if (!process_output(d)) {
				d->booted = 1;
			}
			i++;
		}
	}
	return i;
}

int
pdescrsecure(int c)
{
	return 0;
}

int
pdescrbufsize(int c)
{
	struct descriptor_data *d;

	d = descrdata_by_descr(c);

	if (d) {
		return (MAX_OUTPUT - d->output_size);
	}

	return -1;
}

dbref
partial_pmatch(const char *name)
{
	struct descriptor_data *d;
	dbref last = NOTHING;

	d = descriptor_list;
	while (d) {
		if (d->connected && (last != d->player) && string_prefix(NAME(d->player), name)) {
			if (last != NOTHING) {
				last = AMBIGUOUS;
				break;
			}
			last = d->player;
		}
		d = d->next;
	}
	return (last);
}

void
art(int descr, const char *art)
{
	FILE *f;
	char *ptr;
	char buf[BUFFER_LEN];
	struct descriptor_data *d;

	if (*art == '/' || strstr(art, "..")
	    || (!(string_prefix(art, "bird/")
		|| string_prefix(art, "fish/")
                || !strchr(art, '/'))))

		return;
	
        d = descrdata_by_descr(descr);

	/* queue_string(d, "\r\n"); */

        if (!web_art(descr, art, buf, sizeof(buf)))
                return;

	snprintf(buf, sizeof(buf), "../art/%s.txt", art);

	if ((f = fopen(buf, "rb")) == NULL) 
		return;

	while (fgets(buf, sizeof(buf) - 3, f)) {
		ptr = index(buf, '\n');
		if (ptr && ptr > buf && *(ptr - 1) != '\r') {
			*ptr++ = '\r';
			*ptr++ = '\n';
			*ptr++ = '\0';
		}
		queue_ansi(d, buf);
	}

	fclose(f);
	queue_string(d, "\r\n");
}

void
mob_welcome(struct descriptor_data *d)
{
	struct obj const *o = mob_obj_random();
	if (o) {
		CBUG(*o->name == '\0');
		queue_string(d, o->name);
		queue_string(d, "\r\n\r\n");
		art(d->descriptor, o->art);
                if (*o->description) {
                        if (*o->description != '\0')
                                queue_string(d, o->description);
                        queue_string(d, "\r\n\r\n");
                }
	}
}

void
welcome_user(struct descriptor_data *d)
{
	FILE *f;
	char *ptr;
	char buf[BUFFER_LEN];

        if (!web_support(d->descriptor)) {
                if ((f = fopen(WELC_FILE, "rb")) == NULL) {
                        queue_ansi(d, DEFAULT_WELCOME_MESSAGE);
                        perror("spit_file: welcome.txt");
                } else {
                        while (fgets(buf, sizeof(buf) - 3, f)) {
                                ptr = index(buf, '\n');
                                if (ptr && ptr > buf && *(ptr - 1) != '\r') {
                                        *ptr++ = '\r';
                                        *ptr++ = '\n';
                                        *ptr++ = '\0';
                                }
                                queue_ansi(d, buf);
                        }
                        fclose(f);
                }
        }

        mob_welcome(d);

	if (optflags & OPT_WIZONLY) {
		queue_ansi(d, "## The game is currently in maintenance mode, and only wizards will be able to connect.\r\n");
	}
#if PLAYERMAX
	else if (con_players_curr >= PLAYERMAX_LIMIT) {
		if (PLAYERMAX_WARNMESG && *PLAYERMAX_WARNMESG) {
			queue_ansi(d, PLAYERMAX_WARNMESG);
			queue_string(d, "\r\n");
		}
	}
#endif
}

void
dump_status(void)
{
	struct descriptor_data *d;
	time_t now;
	char buf[BUFFER_LEN];

	(void) time(&now);
	warn("STATUS REPORT:");
	for (d = descriptor_list; d; d = d->next) {
		if (d->connected) {
			snprintf(buf, sizeof(buf), "PLAYING descriptor %d player %s(%d) from user %s, %s.\n",
					d->descriptor, NAME(d->player), d->player, d->username,
					(d->last_time) ? "idle %d seconds" : "never used");
		} else {
			snprintf(buf, sizeof(buf), "CONNECTING descriptor %d from user %s, %s.\n",
					d->descriptor, d->username,
					(d->last_time) ? "idle %d seconds" : "never used");
		}
		warn(buf, now - d->last_time);
	}
}

static const char *interface_c_version = "$RCSfile$ $Revision: 1.127 $";
const char *get_interface_c_version(void) { return interface_c_version; }

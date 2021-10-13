/* $Header$ */


#include "config.h"

/* commands for giving help */

#include "mdb.h"
#include "props.h"
#include "params.h"
#include "interface.h"
#include "externs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>

/*
 * Ok, directory stuff IS a bit ugly.
 */
#if defined(HAVE_DIRENT_H) || defined(_POSIX_VERSION)
# include <dirent.h>
# define NLENGTH(dirent) (strlen((dirent)->d_name))
#else							/* not (HAVE_DIRENT_H or _POSIX_VERSION) */
# define dirent direct
# define NLENGTH(dirent) ((dirent)->d_namlen)
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif							/* HAVE_SYS_NDIR_H */
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif							/* HAVE_SYS_DIR_H */
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif							/* HAVE_NDIR_H */
#endif							/* not (HAVE_DIRENT_H or _POSIX_VERSION) */

#if defined(HAVE_DIRENT_H) || defined(_POSIX_VERSION) || defined(HAVE_SYS_NDIR_H) || defined(HAVE_SYS_DIR_H) || defined(HAVE_NDIR_H)
# define DIR_AVALIBLE
#endif

#if defined(STANDALONE_HELP)
# define dbref int

int
notify(dbref player, const char *msg)
{
	return printf("%s\n", msg);
}


int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
				string++, prefix++;
		return *prefix == '\0';
}

#endif

void
spit_file_segment(dbref player, const char *filename, const char *seg)
{
	FILE *f;
	char buf[BUFFER_LEN];
	char segbuf[BUFFER_LEN];
	char *p;
	int startline, endline, currline;

	startline = endline = currline = 0;
	if (seg && *seg) {
		strlcpy(segbuf, seg, sizeof(segbuf));
		for (p = segbuf; isdigit(*p); p++) ;
		if (*p) {
			*p++ = '\0';
			startline = atoi(segbuf);
			while (*p && !isdigit(*p))
				p++;
			if (*p)
				endline = atoi(p);
		} else {
			endline = startline = atoi(segbuf);
		}
	}
	if ((f = fopen(filename, "rb")) == NULL) {
		snprintf(buf, sizeof(buf), "Sorry, %s is missing.  Management has been notified.", filename);
		notify(player, buf);
		fputs("spit_file:", stderr);
		perror(filename);
	} else {
		while (fgets(buf, sizeof buf, f)) {
			for (p = buf; *p; p++) {
				if (*p == '\n' || *p == '\r') {
					*p = '\0';
					break;
				}
			}
			currline++;
			if ((!startline || (currline >= startline)) && (!endline || (currline <= endline))) {
				if (*buf) {
					notify(player, buf);
				} else {
					notify(player, "  ");
				}
			}
		}
		fclose(f);
	}
}

void
spit_file(dbref player, const char *filename)
{
	spit_file_segment(player, filename, "");
}


void
index_file(dbref player, const char *onwhat, const char *file)
{
	FILE *f;
	char buf[BUFFER_LEN];
	char topic[BUFFER_LEN];
	char *p;
	int arglen, found;

	*topic = '\0';
	strlcpy(topic, onwhat, sizeof(topic));
	if (*onwhat) {
		strlcat(topic, "|", sizeof(topic));
	}

	if ((f = fopen(file, "rb")) == NULL) {
		snprintf(buf, sizeof(buf), "Sorry, %s is missing.  Management has been notified.", file);
		notify(player, buf);
		fprintf(stderr, "help: No file %s!\n", file);
	} else {
		if (*topic) {
			arglen = strlen(topic);
			do {
				do {
					if (!(fgets(buf, sizeof buf, f))) {
						snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
						notify(player, buf);
						fclose(f);
						return;
					}
				} while (*buf != '~');
				do {
					if (!(fgets(buf, sizeof buf, f))) {
						snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
						notify(player, buf);
						fclose(f);
						return;
					}
				} while (*buf == '~');
				p = buf;
				found = 0;
				buf[strlen(buf) - 1] = '|';
				while (*p && !found) {
					if (strncmp(p, topic, arglen)) {
						while (*p && (*p != '|'))
							p++;
						if (*p)
							p++;
					} else {
						found = 1;
					}
				}
			} while (!found);
		}
		while (fgets(buf, sizeof buf, f)) {
			if (*buf == '~')
				break;
			for (p = buf; *p; p++) {
				if (*p == '\n' || *p == '\r') {
					*p = '\0';
					break;
				}
			}
			if (*buf) {
				notify(player, buf);
			} else {
				notify(player, "  ");
			}
		}
		fclose(f);
	}
}


#if !defined(STANDALONE_HELP)
void
mcppkg_help_request(McpFrame * mfr, McpMesg * msg, McpVer ver, void *context)
{
	FILE *f;
	const char* file;
	char buf[BUFFER_LEN];
	char topic[BUFFER_LEN];
	char *p;
	int arglen, found;
	McpVer supp = mcp_frame_package_supported(mfr, "org-fuzzball-help");
	McpMesg omsg;

	if (supp.verminor == 0 && supp.vermajor == 0) {
		notify(mcpframe_to_user(mfr), "MCP: org-fuzzball-help not supported.");
		return;
	}

	if (!strcmp(msg->mesgname, "request")) {
		char *onwhat;
		char *valtype;

		onwhat = mcp_mesg_arg_getline(msg, "topic", 0);
		valtype = mcp_mesg_arg_getline(msg, "type", 0);

		*topic = '\0';
		strlcpy(topic, onwhat, sizeof(topic));
		if (*onwhat) {
			strlcat(topic, "|", sizeof(topic));
		}

		if (!strcmp(valtype, "man")) {
			file = MAN_FILE;
		} else if (!strcmp(valtype, "help")) {
			file = HELP_FILE;
		} else if (!strcmp(valtype, "news")) {
			file = NEWS_FILE;
		} else {
			snprintf(buf, sizeof(buf), "Sorry, %s is not a valid help type.", valtype);
			mcp_mesg_init(&omsg, "org-fuzzball-help", "error");
			mcp_mesg_arg_append(&omsg, "text", buf);
			mcp_mesg_arg_append(&omsg, "topic", onwhat);
			mcp_frame_output_mesg(mfr, &omsg);
			mcp_mesg_clear(&omsg);
			return;
		}

		if ((f = fopen(file, "rb")) == NULL) {
			snprintf(buf, sizeof(buf), "Sorry, %s is missing.  Management has been notified.", file);
			fprintf(stderr, "help: No file %s!\n", file);
			mcp_mesg_init(&omsg, "org-fuzzball-help", "error");
			mcp_mesg_arg_append(&omsg, "text", buf);
			mcp_mesg_arg_append(&omsg, "topic", onwhat);
			mcp_frame_output_mesg(mfr, &omsg);
			mcp_mesg_clear(&omsg);
		} else {
			if (*topic) {
				arglen = strlen(topic);
				do {
					do {
						if (!(fgets(buf, sizeof buf, f))) {
							snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
							fclose(f);
							mcp_mesg_init(&omsg, "org-fuzzball-help", "error");
							mcp_mesg_arg_append(&omsg, "text", buf);
							mcp_mesg_arg_append(&omsg, "topic", onwhat);
							mcp_frame_output_mesg(mfr, &omsg);
							mcp_mesg_clear(&omsg);
							return;
						}
					} while (*buf != '~');
					do {
						if (!(fgets(buf, sizeof buf, f))) {
							snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
							fclose(f);
							mcp_mesg_init(&omsg, "org-fuzzball-help", "error");
							mcp_mesg_arg_append(&omsg, "text", buf);
							mcp_mesg_arg_append(&omsg, "topic", onwhat);
							mcp_frame_output_mesg(mfr, &omsg);
							mcp_mesg_clear(&omsg);
							return;
						}
					} while (*buf == '~');
					p = buf;
					found = 0;
					buf[strlen(buf) - 1] = '|';
					while (*p && !found) {
						if (strncasecmp(p, topic, arglen)) {
							while (*p && (*p != '|'))
								p++;
							if (*p)
								p++;
						} else {
							found = 1;
						}
					}
				} while (!found);
			}
			mcp_mesg_init(&omsg, "org-fuzzball-help", "entry");
			mcp_mesg_arg_append(&omsg, "topic", onwhat);
			while (fgets(buf, sizeof buf, f)) {
				if (*buf == '~')
					break;
				for (p = buf; *p; p++) {
					if (*p == '\n' || *p == '\r') {
						*p = '\0';
						break;
					}
				}
				if (!*buf) {
					strlcpy(buf, "  ", sizeof(buf));
				}
				mcp_mesg_arg_append(&omsg, "text", buf);
			}
			fclose(f);
			mcp_frame_output_mesg(mfr, &omsg);
			mcp_mesg_clear(&omsg);
		}
	}
}
#endif


int
show_subfile(dbref player, const char *dir, const char *topic, const char *seg, int partial)
{
	char buf[256];
	struct stat st;

#ifdef DIR_AVALIBLE
	DIR *df;
	struct dirent *dp;
#endif

	if (!topic || !*topic)
		return 0;

	if ((*topic == '.') || (*topic == '~') || (strchr(topic, '/'))) {
		return 0;
	}
	if (strlen(topic) > 63)
		return 0;


#ifdef DIR_AVALIBLE
	/* TO DO: (1) exact match, or (2) partial match, but unique */
	*buf = 0;

	if ((df = (DIR *) opendir(dir))) {
		while ((dp = readdir(df))) {
			if ((partial && string_prefix(dp->d_name, topic)) ||
				(!partial && !strcmp(dp->d_name, topic))
					) {
				snprintf(buf, sizeof(buf), "%s/%s", dir, dp->d_name);
				break;
			}
		}
		closedir(df);
	}

	if (!*buf) {
		return 0;				/* no such file or directory */
	}

#else                           /* !DIR_AVAILABLE */
	snprintf(buf, sizeof(buf), "%s/%s", dir, topic);
#endif 

	if (stat(buf, &st)) {
		return 0;
	} else {
		spit_file_segment(player, buf, seg);
		return 1;
	}
}


#if !defined(STANDALONE_HELP)
void
do_man(command_t *cmd)
{
	dbref player = cmd->player;
	char *topic = cmd->argv[1];
	char *seg = cmd->argv[2];
	if (show_subfile(player, MAN_DIR, topic, seg, FALSE))
		return;
	index_file(player, topic, MAN_FILE);
}

void
do_help(command_t *cmd)
{
	dbref player = cmd->player;
	char *topic = cmd->argv[1];
	char *seg = cmd->argv[2];
	if (show_subfile(player, HELP_DIR, topic, seg, FALSE))
		return;
	index_file(player, topic, HELP_FILE);
}


void
do_news(command_t *cmd)
{
	dbref player = cmd->player;
	char *topic = cmd->argv[1];
	char *seg = cmd->argv[2];
	if (show_subfile(player, NEWS_DIR, topic, seg, FALSE))
		return;
	index_file(player, topic, NEWS_FILE);
}

void
do_info(command_t *cmd)
{
	dbref player = cmd->player;
	const char *topic = cmd->argv[1];
	const char *seg = cmd->argv[2];
	char *buf;
	int f;
	int cols;
	int buflen = 80;

#ifdef DIR_AVALIBLE
	DIR *df;
	struct dirent *dp;
#endif

	if (*topic) {
		if (!show_subfile(player, INFO_DIR, topic, seg, TRUE)) {
			notify(player, NO_INFO_MSG);
		}
	} else {
#ifdef DIR_AVALIBLE
		buf = (char *) calloc(1, buflen);
		(void) strlcpy(buf, "    ", buflen);
		f = 0;
		cols = 0;
		if ((df = (DIR *) opendir(INFO_DIR))) {
			while ((dp = readdir(df))) {

				if (*(dp->d_name) != '.') {
					if (!f)
						notify(player, "Available information files are:");
					if ((cols++ > 2) || ((strlen(buf) + strlen(dp->d_name)) > 63)) {
						notify(player, buf);
						strlcpy(buf, "    ", buflen);
						cols = 0;
					}
					strlcat(buf, dp->d_name, buflen);
					strlcat(buf, " ", buflen);
					f = strlen(buf);
					while ((f % 20) != 4)
						buf[f++] = ' ';
					buf[f] = '\0';
				}
			}
			closedir(df);
		}
		if (f)
			notify(player, buf);
		else
			notify(player, "No information files are available.");
		free(buf);
		notify(player, "Index not available on this system.");
#endif							/* !DIR_AVALIBLE */
	}
}
#else /* STANDALONE_HELP */

int
main(int argc, char**argv)
{
	char* helpfile = NULL;
	char* topic = NULL;
	char buf[BUFFER_LEN];
	if (argc < 2) {
			fprintf(stderr, "Usage: %s muf|help [topic]\n", argv[0]);
		exit(-1);
	} else if (argc == 2 || argc == 3) {
		if (argc == 2) {
			topic = "";
		} else {
			topic = argv[2];
		}
		if (!strcmp(argv[1], "man")) {
			helpfile = MAN_FILE;
		} else if (!strcmp(argv[1], "muf")) {
			helpfile = MAN_FILE;
		} else if (!strcmp(argv[1], "help")) {
			helpfile = HELP_FILE;
		} else {
			fprintf(stderr, "Usage: %s muf|help [topic]\n", argv[0]);
			exit(-2);
		}

		helpfile = strrstr(helpfile, '/');
		helpfile++;
#ifdef HELPFILE_DIR
		snprintf(buf, sizeof(buf), "%s/%s", HELPFILE_DIR, helpfile);
#else
		snprintf(buf, sizeof(buf), "%s/%s", "/usr/local/fbmuck/help", helpfile);
#endif

		index_file(1, topic, buf);
		exit(0);
	} else if (argc > 3) {
		fprintf(stderr, "Usage: %s muf|help [topic]\n", argv[0]);
		exit(-1);
	}
	return 0;
}

#endif /* STANDALONE_HELP */

static const char *help_c_version = "$RCSfile$ $Revision: 1.15 $";
const char *get_help_c_version(void) { return help_c_version; }

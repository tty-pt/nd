#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#ifndef __OpenBSD__
#include <bsd/string.h>
#endif

#include "io.h"
#include "entity.h"

/* commands for giving help */

#include "params.h"
#include "utils.h"

#include "command.h"

#define NLENGTH(dirent) (strlen((dirent)->d_name))

#define HELP_FILE "data/muckhelp.txt"	/* For the 'help' command      */
#define HELP_DIR  "data/help"	/* For 'help' subtopic files   */
#define MAN_FILE  "data/man.txt"	/* For the 'man' command       */
#define MAN_DIR   "data/man"	/* For 'man' subtopic files    */
#define INFO_DIR  "data/info/"

#define NO_INFO_MSG "That file does not exist.  Type 'info' to get a list of the info files available."

void
spit_file_segment(OBJ *player, const char *filename, const char *seg)
{
	ENT *eplayer = &player->sp.entity;
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
		notify(eplayer, buf);
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
					notify(eplayer, buf);
				} else {
					notify(eplayer, "  ");
				}
			}
		}
		fclose(f);
	}
}

void
spit_file(OBJ *player, const char *filename)
{
	spit_file_segment(player, filename, "");
}


void
index_file(OBJ *player, const char *onwhat, const char *file)
{
	ENT *eplayer = &player->sp.entity;
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
		notify(eplayer, buf);
		fprintf(stderr, "help: No file %s!\n", file);
	} else {
		if (*topic) {
			arglen = strlen(topic);
			do {
				do {
					if (!(fgets(buf, sizeof buf, f))) {
						snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
						notify(eplayer, buf);
						fclose(f);
						return;
					}
				} while (*buf != '~');
				do {
					if (!(fgets(buf, sizeof buf, f))) {
						snprintf(buf, sizeof(buf), "Sorry, no help available on topic \"%s\"", onwhat);
						notify(eplayer, buf);
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
				notify(eplayer, buf);
			} else {
				notify(eplayer, "  ");
			}
		}
		fclose(f);
	}
}

int
show_subfile(OBJ *player, const char *dir, const char *topic, const char *seg, int partial)
{
	char buf[512];
	struct stat st;
	DIR *df;
	struct dirent *dp;

	if (!topic || !*topic)
		return 0;

	if ((*topic == '.') || (*topic == '~') || (strchr(topic, '/'))) {
		return 0;
	}
	if (strlen(topic) > 63)
		return 0;


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
	OBJ *player = cmd->player;
	char *topic = cmd->argv[1];
	char *seg = cmd->argv[2];
	if (show_subfile(player, MAN_DIR, topic, seg, 0))
		return;
	index_file(player, topic, MAN_FILE);
}

void
do_help(command_t *cmd)
{
	OBJ *player = cmd->player;
	char *topic = cmd->argv[1];
	char *seg = cmd->argv[2];
	if (show_subfile(player, HELP_DIR, topic, seg, 0))
		return;
	index_file(player, topic, HELP_FILE);
}

void
do_info(command_t *cmd)
{
	OBJ *player = cmd->player;
	ENT *eplayer = &player->sp.entity;
	const char *topic = cmd->argv[1];
	const char *seg = cmd->argv[2];
	char *buf;
	int f;
	int cols;
	int buflen = 80;
	DIR *df;
	struct dirent *dp;

	if (*topic) {
		if (!show_subfile(player, INFO_DIR, topic, seg, 1)) {
			notify(eplayer, NO_INFO_MSG);
		}
	} else {
		buf = (char *) calloc(1, buflen);
		(void) strlcpy(buf, "    ", buflen);
		f = 0;
		cols = 0;
		if ((df = (DIR *) opendir(INFO_DIR))) {
			while ((dp = readdir(df))) {

				if (*(dp->d_name) != '.') {
					if (!f)
						notify(eplayer, "Available information files are:");
					if ((cols++ > 2) || ((strlen(buf) + strlen(dp->d_name)) > 63)) {
						notify(eplayer, buf);
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
			notify(eplayer, buf);
		else
			notify(eplayer, "No information files are available.");
		free(buf);
		notify(eplayer, "Index not available on this system.");
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

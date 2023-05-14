#ifndef UTILS_H
#define UTILS_H

struct popen2 {
	int in, out, pid;
};

int ok_name(const char *name);
int string_prefix(const char *string, const char *prefix);
int popen2(struct popen2 *child, const char *cmdline);
/* char *gpt(char *prompt); */

#endif

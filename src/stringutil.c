/* $Header$ */


#include "copyright.h"
#include "config.h"
#include "mdb.h"
#include "props.h"
#include "params.h"
#include "interface.h"

/* String utilities */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "externs.h"

const char *
exit_prefix(register const char *string, register const char *prefix)
{
	const char *p;
	const char *s = string;

	while (*s) {
		p = prefix;
		string = s;
		while (*s && *p && tolower(*s) == tolower(*p)) {
			s++;
			p++;
		}
		while (*s && isspace(*s))
			s++;
		if (!*p && (!*s || *s == EXIT_DELIMITER)) {
			return string;
		}
		while (*s && (*s != EXIT_DELIMITER))
			s++;
		if (*s)
			s++;
		while (*s && isspace(*s))
			s++;
	}
	return 0;
}

int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
		string++, prefix++;
	return *prefix == '\0';
}

/* accepts only nonempty matches starting at the beginning of a word */
const char *
string_match(register const char *src, register const char *sub)
{
	if (*sub != '\0') {
		while (*src) {
			if (string_prefix(src, sub))
				return src;
			/* else scan to beginning of next word */
			while (*src && isalnum(*src))
				src++;
			while (*src && !isalnum(*src))
				src++;
		}
	}
	return 0;
}

char *
alloc_string(const char *string)
{
	char *s;

	/* NULL, "" -> NULL */
	if (!string || !*string)
		return 0;

	if ((s = (char *) malloc(strlen(string) + 1)) == 0) {
		abort();
	}
	strcpy(s, string);  /* Guaranteed enough space. */
	return s;
}

char *
intostr(int i)
{
	static char num[16];
	int j, k;
	char *ptr2;

	k = i;
	ptr2 = num + 14;
	num[15] = '\0';
	if (i < 0)
		i = -i;
	while (i) {
		j = i % 10;
		*ptr2-- = '0' + j;
		i /= 10;
	}
	if (!k)
		*ptr2-- = '0';
	if (k < 0)
		*ptr2-- = '-';
	return (++ptr2);
}

int
is_valid_pose_separator(char ch)
{
	return (ch == '\'') || (ch == ' ') || (ch == ',') || (ch == '-');
}

int
is_prop_prefix(const char* Property, const char* Prefix)
{
	while(*Property == PROPDIR_DELIMITER)
		Property++;

	while(*Prefix == PROPDIR_DELIMITER)
		Prefix++;

	while(*Prefix)
	{
		if (*Property == '\0')
			return 0;

		if (*Property++ != *Prefix++)
			return 0;
	}

	return (*Property == '\0') || (*Property == PROPDIR_DELIMITER);
}

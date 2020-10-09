/* $Header$ */


#include "copyright.h"
#include "config.h"
#include "db.h"
#include "props.h"
#include "params.h"
#include "interface.h"
#include "mpi.h"

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

/*
 * Encrypt one string with another one.
 */

#define CHARCOUNT 97

static char enarr[256];
static int charset_count[] = { 96, 97, 0 };
static int initialized_crypt = 0;

void
init_crypt(void)
{
	int i;

	for (i = 0; i <= 255; i++)
		enarr[i] = (char) i;
	for (i = 'A'; i <= 'M'; i++)
		enarr[i] = (char) enarr[i] + 13;
	for (i = 'N'; i <= 'Z'; i++)
		enarr[i] = (char) enarr[i] - 13;
	enarr['\r'] = 127;
	enarr[127] = '\r';
	enarr[ESCAPE_CHAR] = 31;
	enarr[31] = ESCAPE_CHAR;
	initialized_crypt = 1;
}


const char *
strencrypt(const char *data, const char *key)
{
	static char linebuf[BUFFER_LEN];
	char buf[BUFFER_LEN + 8];
	const char *cp;
	char *ptr;
	char *ups;
	const char *upt;
	int linelen;
	int count;
	int seed, seed2, seed3;
	int limit = BUFFER_LEN;
	int result;

	if (!initialized_crypt)
		init_crypt();

	seed = 0;
	for (cp = key; *cp; cp++)
		seed = ((((*cp) ^ seed) + 170) % 192);

	seed2 = 0;
	for (cp = data; *cp; cp++)
		seed2 = ((((*cp) ^ seed2) + 21) & 0xff);
	seed3 = seed2 = ((seed2 ^ (seed ^ (RANDOM() >> 24))) & 0x3f);

	count = seed + 11;
	for (upt = data, ups = buf, cp = key; *upt; upt++) {
		count = (((*cp) ^ count) + (seed ^ seed2)) & 0xff;
		seed2 = ((seed2 + 1) & 0x3f);
		if (!*(++cp))
			cp = key;
		result = (enarr[(unsigned char)*upt] - (32 - (CHARCOUNT - 96))) + count + seed;
		*ups = enarr[(result % CHARCOUNT) + (32 - (CHARCOUNT - 96))];
		count = (((*upt) ^ count) + seed) & 0xff;
		ups++;
	}
	*ups++ = '\0';

	ptr = linebuf;

	linelen = strlen(data);
	*ptr++ = (' ' + 2);
	*ptr++ = (' ' + seed3);
	limit--;
	limit--;

	for (cp = buf; cp < &buf[linelen]; cp++) {
		limit--;
		if (limit < 0)
			break;
		*ptr++ = *cp;
	}
	*ptr++ = '\0';
	return linebuf;
}



const char *
strdecrypt(const char *data, const char *key)
{
	char linebuf[BUFFER_LEN];
	static char buf[BUFFER_LEN];
	const char *cp;
	char *ptr;
	char *ups;
	const char *upt;
	int linelen;
	int outlen;
	int count;
	int seed, seed2;
	int result;
	int chrcnt;

	if (!initialized_crypt)
		init_crypt();

	ptr = linebuf;

	if ((data[0] - ' ') < 1 || (data[0] - ' ') > 2) {
		return "";
	}

	linelen = strlen(data);
	chrcnt = charset_count[(data[0] - ' ') - 1];
	seed2 = (data[1] - ' ');

	strlcpy(linebuf, data + 2, sizeof(linebuf));

	seed = 0;
	for (cp = key; *cp; cp++) {
		seed = (((*cp) ^ seed) + 170) % 192;
	}

	count = seed + 11;
	outlen = strlen(linebuf);
	upt = linebuf;
	ups = buf;
	cp = key;
	while ((const char *) upt < &linebuf[outlen]) {
		count = (((*cp) ^ count) + (seed ^ seed2)) & 0xff;
		if (!*(++cp))
			cp = key;
		seed2 = ((seed2 + 1) & 0x3f);

		result = (enarr[(unsigned char)*upt] - (32 - (chrcnt - 96))) - (count + seed);
		while (result < 0)
			result += chrcnt;
		*ups = enarr[result + (32 - (chrcnt - 96))];

		count = (((*ups) ^ count) + seed) & 0xff;
		ups++;
		upt++;
	}
	*ups++ = '\0';

	return buf;
}


char *
strip_ansi(char *buf, const char *input)
{
	const char *is;
	char *os;

	buf[0] = '\0';
	os = buf;

	is = input;

	while (*is) {
		if (*is == ESCAPE_CHAR) {
			is++;
			if (*is == '[') {
				is++;
				while (isdigit(*is) || *is == ';')
					is++;
				if (*is == 'm')
					is++;
			} else {
				is++;
			}
		} else {
			*os++ = *is++;
		}
	}
	*os = '\0';

	return buf;
}


char *
strip_bad_ansi(char *buf, const char *input)
{
	const char *is;
	char *os;
	int aflag = 0;
	int limit = BUFFER_LEN - 5;

	buf[0] = '\0';
	os = buf;

	is = input;

	while (*is && limit-->0) {
		if (*is == ESCAPE_CHAR) {
			if (is[1] == '\0') {
				is++;
			} else if (is[1] != '[') {
				is++;
				is++;
			} else {
				aflag = 1;
				*os++ = *is++;	/* esc */
				*os++ = *is++;	/*  [  */
				while (isdigit(*is) || *is == ';') {
					*os++ = *is++;
				}
				if (*is != 'm') {
					*os++ = 'm';
				}
				*os++ = *is++;
			}
		} else {
			*os++ = *is++;
		}
	}
	if (aflag) {
		int termrn = 0;
		if (*(os - 2) == '\r' && *(os - 1) == '\n') {
			termrn = 1;
			os -= 2;
		}
		*os++ = '\033';
		*os++ = '[';
		*os++ = '0';
		*os++ = 'm';
		if (termrn) {
			*os++ = '\r';
			*os++ = '\n';
		}
	}
	*os = '\0';

	return buf;
}

/* Prepends what before before, granted it doesn't come
 * before start in which case it returns 0.
 * Otherwise it modifies *before to point to that new location,
 * and it returns the number of chars prepended.
 */
int
prepend_string(char** before, char* start, const char* what)
{
   char* ptr;
   size_t len;
   len = strlen(what);
   ptr = *before - len;
   if (ptr < start)
       return 0;
   memcpy((void*) ptr, (const void*) what, len);
   *before = ptr;
   return len;
}

int
is_valid_pose_separator(char ch)
{
	return (ch == '\'') || (ch == ' ') || (ch == ',') || (ch == '-');
}

void
prefix_message(char* Dest, const char* Src, const char* Prefix, int BufferLength, int SuppressIfPresent)
{
	int PrefixLength			= strlen(Prefix);
	int CheckForHangingEnter	= 0;

	while((BufferLength > PrefixLength) && (*Src != '\0'))
	{
		if (*Src == '\r')
		{
			Src++;
			continue;
		}

		if (!SuppressIfPresent || strncmp(Src, Prefix, PrefixLength) || (
				!is_valid_pose_separator(Src[PrefixLength]) &&	
				(Src[PrefixLength] != '\r') &&
				(Src[PrefixLength] != '\0')
			))
		{
			strlcpy(Dest, Prefix, BufferLength);

			Dest			+= PrefixLength;
			BufferLength	-= PrefixLength;

			if (BufferLength > 1)
			{
				if (!is_valid_pose_separator(*Src))
				{
					*Dest++ = ' ';
					BufferLength--;
				}
			}
		}

		while((BufferLength > 1) && (*Src != '\0'))
		{
				*Dest++ = *Src;
				BufferLength--;

				if (*Src++ == '\r')
				{
					CheckForHangingEnter = 1;
					break;
				}
		}
	}

	if (CheckForHangingEnter && (Dest[-1] == '\r'))
		Dest--;

	*Dest = '\0';
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

int
has_suffix(const char* text, const char* suffix)
{
	int tlen = text ? strlen(text) : 0;
	int slen = suffix ? strlen(suffix) : 0;

	if (!tlen || !slen || (tlen < slen))
		return 0;

	return !strcmp(text + tlen - slen, suffix);
}

int
has_suffix_char(const char* text, char suffix)
{
	int tlen = text ? strlen(text) : 0;

	if (tlen < 1)
		return 0;

	return text[tlen - 1] == suffix;
}

static const char *stringutil_c_version = "$RCSfile$ $Revision: 1.29 $";
const char *get_stringutil_c_version(void) { return stringutil_c_version; }

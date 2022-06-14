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

int
string_prefix(register const char *string, register const char *prefix)
{
	while (*string && *prefix && tolower(*string) == tolower(*prefix))
		string++, prefix++;
	return *prefix == '\0';
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

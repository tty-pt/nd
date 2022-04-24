/* $Header$ */

#include "copyright.h"
#include "config.h"
#include "params.h"

#include "mdb.h"
#include "defaults.h"
#include "props.h"
#include "externs.h"
#include "interface.h"
#include <string.h>
#include <math.h>

/* property.c
   A whole new lachesis mod.
   Adds property manipulation routines to TinyMUCK.   */

/* Completely rewritten by darkfox and Foxen, for propdirs and other things */



void
set_property_nofetch(dbref player, const char *pname, PData * dat)
{
	PropPtr p;
	char buf[BUFFER_LEN];
	char *n;

	/* Make sure that we are passed a valid property name */
	if (!pname)
		return;

	while (*pname == PROPDIR_DELIMITER)
		pname++;

	strlcpy(buf, pname, sizeof(buf));

	/* truncate propnames with a ':' in them at the ':' */
	n = index(buf, PROP_DELIMITER);
	if (n)
		*n = '\0';
	if (!*buf)
		return;

	p = propdir_new_elem(&(DBFETCH(player)->properties), buf);

	/* free up any old values */
	clear_propnode(p);

	SetPFlagsRaw(p, dat->flags);
	if (PropFlags(p) & PROP_ISUNLOADED) {
		SetPDataUnion(p, dat->data);
		return;
	}
	switch (PropType(p)) {
	case PROP_STRTYP:
		if (!dat->data.str || !*(dat->data.str)) {
			SetPType(p, PROP_DIRTYP);
			SetPDataStr(p, NULL);
			if (!PropDir(p)) {
				remove_property_nofetch(player, pname);
			}
		} else {
			SetPDataStr(p, alloc_string(dat->data.str));
		}
		break;
	case PROP_INTTYP:
		SetPDataVal(p, dat->data.val);
		if (!dat->data.val) {
			SetPType(p, PROP_DIRTYP);
			if (!PropDir(p)) {
				remove_property_nofetch(player, pname);
			}
		}
		break;
	case PROP_FLTTYP:
		SetPDataFVal(p, dat->data.fval);
		if (dat->data.fval == 0.0) {
			SetPType(p, PROP_DIRTYP);
			if (!PropDir(p)) {
				remove_property_nofetch(player, pname);
			}
		}
		break;
	case PROP_REFTYP:
		SetPDataRef(p, dat->data.ref);
		if (dat->data.ref == NOTHING) {
			SetPType(p, PROP_DIRTYP);
			SetPDataRef(p, 0);
			if (!PropDir(p)) {
				remove_property_nofetch(player, pname);
			}
		}
		break;
	case PROP_LOKTYP:
		SetPDataLok(p, dat->data.lok);
		break;
	case PROP_DIRTYP:
		SetPDataVal(p, 0);
		if (!PropDir(p)) {
			remove_property_nofetch(player, pname);
		}
		break;
	}
}


void
set_property(dbref player, const char *name, PData * dat)
{
	set_property_nofetch(player, name, dat);
}

void
set_property_value(dbref obj, const char *propstr, int val)
{
	PData mydat;
	mydat.flags = PROP_INTTYP;
	mydat.data.val = val;
	set_property_nofetch(obj, propstr, &mydat);
}

int // FIXME
addc_property_value(dbref obj, const char *pname, int val, int cap)
{
	PropPtr p = get_property(obj, pname);
	int cur;
	if (p) {
		cur = PropDataVal(p);
		cur += val;
		if (cur > cap)
			cur = cap;
		PropDataVal(p) = cur;
	} else {
		cur = val > cap ? cap : val;
		add_prop_nofetch(obj, pname, "", cur);
	}
	return cur;
}

void
set_property_hash(dbref obj, const char *propstr, int idx, int val)
{
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), "%s/%d", propstr, idx);
	set_property_value(obj, buf, val);
}

void
set_property_mark(dbref obj, const char *propstr, char mark, char *value)
{
	char buf[BUFSIZ];
	PData dat;
	snprintf(buf, sizeof(buf), "%s/%c", propstr, mark);
	dat.data.str = value;
	dat.flags = PROP_STRTYP;
	remove_property_nofetch(obj, buf);
	set_property_nofetch(obj, buf, &dat);
}

void
set_property_dbref(dbref obj, const char *propstr, dbref val)
{
	PData mydat;
	mydat.flags = PROP_REFTYP;
	mydat.data.ref = val;
	set_property_nofetch(obj, propstr, &mydat);
}

void
set_lock_property(dbref player, const char *pname, const char *lok)
{
	PData mydat;

	mydat.flags = PROP_LOKTYP;
	if (!lok || !*lok) {
		mydat.data.lok = TRUE_BOOLEXP;
	} else {
		mydat.data.lok = parse_boolexp(1, lok, 1);
	}
	set_property(player, pname, &mydat);
}



/* adds a new property to an object */
void
add_prop_nofetch(dbref player, const char *pname, const char *strval, int value)
{
	PData mydat;

	if (strval && *strval) {
		mydat.flags = PROP_STRTYP;
		mydat.data.str = (char *) strval;
	} else if (value) {
		mydat.flags = PROP_INTTYP;
		mydat.data.val = value;
	} else {
		mydat.flags = PROP_DIRTYP;
		mydat.data.str = NULL;
	}
	set_property_nofetch(player, pname, &mydat);
}



void
remove_proplist_item(dbref player, PropPtr p, int allp)
{
	const char *ptr;

	if (!p)
		return;
	ptr = PropName(p);

	if (Prop_System(ptr))
		return;

	if (!allp) {
		if (*ptr == PROP_HIDDEN)
			return;
		if (*ptr == PROP_SEEONLY)
			return;
		if (ptr[0] == '_' && ptr[1] == '\0')
			return;
		if (PropFlags(p) & PROP_SYSPERMS)
			return;
	}
	remove_property(player, ptr);
}


/* removes property list --- if it's not there then ignore */
void
remove_property_list(dbref player, int all)
{
	PropPtr l;
	PropPtr p;
	PropPtr n;

	if ((l = DBFETCH(player)->properties) != NULL) {
		p = first_node(l);
		while (p) {
			n = next_node(l, PropName(p));
			remove_proplist_item(player, p, all);
			l = DBFETCH(player)->properties;
			p = n;
		}
	}

}

/* removes property --- if it's not there then ignore */
void
remove_property_nofetch(dbref player, const char *pname)
{
	PropPtr l;
	char buf[BUFFER_LEN];

	strlcpy(buf, pname, sizeof(buf));

	l = DBFETCH(player)->properties;
	l = propdir_delete_elem(l, buf);
	DBFETCH(player)->properties = l;
}


void
remove_property(dbref player, const char *pname)
{
	remove_property_nofetch(player, pname);
}


PropPtr
get_property(dbref player, const char *pname)
{
	PropPtr p;
	char buf[BUFFER_LEN];

	strlcpy(buf, pname, sizeof(buf));

	p = propdir_get_elem(DBFETCH(player)->properties, buf);
	return (p);
}


/* checks if object has property, returning 1 if it or any of its contents has
   the property stated                                                      */
int
has_property(dbref what, const char *pname, const char *strval,
			 int value)
{
	dbref things;

	if (has_property_strict(what, pname, strval, value))
		return 1;
	for (things = DBFETCH(what)->contents; things != NOTHING; things = DBFETCH(things)->next) {
		if (has_property(things, pname, strval, value))
			return 1;
	}
	return 0;
}


static int has_prop_recursion_limit = 2;
/* checks if object has property, returns 1 if it has the property */
int
has_property_strict(dbref what, const char *pname, const char *strval,
					int value)
{
	PropPtr p;
	const char *str;
	char *ptr;
	char buf[BUFFER_LEN];

	p = get_property(what, pname);

	if (p) {
		switch (PropType(p)) {
		    case PROP_STRTYP:
			str = DoNull(PropDataStr(p));

			strlcpy(buf, str, sizeof(buf));
			ptr = buf;

			has_prop_recursion_limit++;
			return (equalstr((char *) strval, ptr));

		    case PROP_INTTYP:
			return (value == PropDataVal(p));
		    case PROP_FLTTYP:
			return (value == (int) PropDataFVal(p));
		    default:
			/* assume other types don't match */
			return 0;
		}
	}
	return 0;
}

/* return string value of property */
const char *
get_property_class(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);
	if (p) {
		if (PropType(p) != PROP_STRTYP)
			return (char *) NULL;
		return (PropDataStr(p));
	} else {
		return (char *) NULL;
	}
}

/* return value of property */
int
get_property_value(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);

	if (p) {
		if (PropType(p) != PROP_INTTYP)
			return 0;
		return (PropDataVal(p));
	} else {
		return 0;
	}
}

int
get_property_hash(dbref player, const char *pname, int idx)
{
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), "%s/%d", pname, idx);
	return get_property_value(player, buf);
}

const char *
get_property_mark(dbref player, const char *pname, char mark)
{
	char buf[BUFSIZ];
	snprintf(buf, sizeof(buf), "%s/%c", pname, mark);
	return get_property_class(player, buf);
}

/* return float value of a property */
double
get_property_fvalue(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);
	if (p) {
		if (PropType(p) != PROP_FLTTYP)
			return 0.0;
		return (PropDataFVal(p));
	} else {
		return 0.0;
	}
}

/* return boolexp lock of property */
dbref
get_property_dbref(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);
	if (!p)
		return NOTHING;
	if (PropType(p) != PROP_REFTYP)
		return NOTHING;
	return PropDataRef(p);
}


/* return boolexp lock of property */
struct boolexp *
get_property_lock(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);
	if (!p)
		return TRUE_BOOLEXP;
	if (PropType(p) != PROP_LOKTYP)
		return TRUE_BOOLEXP;
	return PropDataLok(p);
}


/* return flags of property */
int
get_property_flags(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);

	if (p) {
		return (PropFlags(p));
	} else {
		return 0;
	}
}


/* return flags of property */
void
clear_property_flags(dbref player, const char *pname, int flags)
{
	PropPtr p;

	flags &= ~PROP_TYPMASK;
	p = get_property(player, pname);
	if (p) {
		SetPFlags(p, (PropFlags(p) & ~flags));
	}
}


/* return flags of property */
void
set_property_flags(dbref player, const char *pname, int flags)
{
	PropPtr p;

	flags &= ~PROP_TYPMASK;
	p = get_property(player, pname);
	if (p) {
		SetPFlags(p, (PropFlags(p) | flags));
	}
}


/* return type of property */
int
get_property_type(dbref player, const char *pname)
{
	PropPtr p;

	p = get_property(player, pname);

	if (p) {
		return (PropType(p));
	} else {
		return 0;
	}
}



PropPtr
copy_prop(dbref old)
{
	PropPtr p, n = NULL;

	p = DBFETCH(old)->properties;
	copy_proplist(old, &n, p);
	return (n);
}

/* Return a pointer to the first property in a propdir and duplicates the
   property name into 'name'.  Returns NULL if the property list is empty
   or does not exist. */
PropPtr
first_prop_nofetch(dbref player, const char *dir, PropPtr * list, char *name, int maxlen)
{
	char buf[BUFFER_LEN];
	PropPtr p;

	if (dir) {
		while (*dir && *dir == PROPDIR_DELIMITER) {
			dir++;
		}
	}
	if (!dir || !*dir) {
		*list = DBFETCH(player)->properties;
		p = first_node(*list);
		if (p) {
			strlcpy(name, PropName(p), maxlen);
		} else {
			*name = '\0';
		}
		return (p);
	}

	strlcpy(buf, dir, sizeof(buf));
	*list = p = propdir_get_elem(DBFETCH(player)->properties, buf);
	if (!p) {
		*name = '\0';
		return NULL;
	}
	*list = PropDir(p);
	p = first_node(*list);
	if (p) {
		strlcpy(name, PropName(p), maxlen);
	} else {
		*name = '\0';
	}

	return (p);
}


/* first_prop() returns a pointer to the first property.
 * player    dbref of object that the properties are on.
 * dir       pointer to string name of the propdir
 * list      pointer to a proplist pointer.  Returns the root node.
 * name      printer to a string.  Returns the name of the first node.
 */

PropPtr
first_prop(dbref player, const char *dir, PropPtr * list, char *name, int maxlen)
{

	return (first_prop_nofetch(player, dir, list, name, maxlen));
}



/* next_prop() returns a pointer to the next property node.
 * list    Pointer to the root node of the list.
 * prop    Pointer to the previous prop.
 * name    Pointer to a string.  Returns the name of the next property.
 */

PropPtr
next_prop(PropPtr list, PropPtr prop, char *name, int maxlen)
{
	PropPtr p = prop;

	if (!p || !(p = next_node(list, PropName(p))))
		return ((PropPtr) 0);

	strlcpy(name, PropName(p), maxlen);
	return (p);
}

long
size_properties(dbref player, int load)
{
	return size_proplist(DBFETCH(player)->properties);
}


/* return true if a property contains a propdir */
int
is_propdir_nofetch(dbref player, const char *pname)
{
	PropPtr p;
	char w[BUFFER_LEN];

	strlcpy(w, pname, sizeof(w));
	p = propdir_get_elem(DBFETCH(player)->properties, w);
	if (!p)
		return 0;
	return (PropDir(p) != (PropPtr) NULL);
}


int
is_propdir(dbref player, const char *pname)
{

	return (is_propdir_nofetch(player, pname));
}


PropPtr
envprop(dbref * where, const char *propname, int typ)
{
	PropPtr temp;

	while (*where != NOTHING) {
		temp = get_property(*where, propname);
		if (temp && (!typ || PropType(temp) == typ))
			return temp;
		*where = getparent(*where);
	}
	return NULL;
}


const char *
envpropstr(dbref * where, const char *propname)
{
	PropPtr temp;

	temp = envprop(where, propname, PROP_STRTYP);
	if (!temp)
		return NULL;
	if (PropType(temp) == PROP_STRTYP)
		return (PropDataStr(temp));
	return NULL;
}


char *
displayprop(dbref player, dbref obj, const char *name, char *buf, size_t bufsiz)
{
	char mybuf[BUFFER_LEN];
	int pdflag;
	char blesschar = '-';
	PropPtr p = get_property(obj, name);

	if (!p) {
		snprintf(buf, bufsiz, "%s: No such property.", name);
		return buf;
	}
	if (PropFlags(p) & PROP_BLESSED)
		blesschar = 'B';
	pdflag = (PropDir(p) != NULL);
	snprintf(mybuf, bufsiz, "%.*s%c", (BUFFER_LEN / 4), name, (pdflag) ? PROPDIR_DELIMITER : '\0');
	switch (PropType(p)) {
	case PROP_STRTYP:
		snprintf(buf, bufsiz, "%c str %s:%.*s", blesschar, mybuf, (BUFFER_LEN / 2), PropDataStr(p));
		break;
	case PROP_REFTYP:
		snprintf(buf, bufsiz, "%c ref %s:%s", blesschar, mybuf, unparse_object(player, PropDataRef(p)));
		break;
	case PROP_INTTYP:
		snprintf(buf, bufsiz, "%c int %s:%d", blesschar, mybuf, PropDataVal(p));
		break;
	case PROP_FLTTYP:
		snprintf(buf, bufsiz, "%c flt %s:%.17g", blesschar, mybuf, PropDataFVal(p));
		break;
	case PROP_LOKTYP:
		if (PropFlags(p) & PROP_ISUNLOADED) {
			snprintf(buf, bufsiz, "%c lok %s:*UNLOCKED*", blesschar, mybuf);
		} else {
			snprintf(buf, bufsiz, "%c lok %s:%.*s", blesschar, mybuf, (BUFFER_LEN / 2),
					unparse_boolexp(player, PropDataLok(p), 1));
		}
		break;
	case PROP_DIRTYP:
		snprintf(buf, bufsiz, "%c dir %s:(no value)", blesschar, mybuf);
		break;
	}
	return buf;
}



#define DOWNCASE(x) (tolower(x))

extern short db_conversion_flag;

int
db_get_single_prop(FILE * f, dbref obj, long pos, PropPtr pnode, const char *pdir)
{
	char getprop_buf[BUFFER_LEN * 3];
	char *name, *flags, *value, *p;
	int flg;
	long tpos=0L;
	struct boolexp *lok;
	short do_diskbase_propvals;
	PData mydat;

	do_diskbase_propvals = 0;

	if (pos) {
		fseek(f, pos, 0);
	} else if (do_diskbase_propvals) {
		tpos = ftell(f);
	}
	name = fgets(getprop_buf, sizeof(getprop_buf), f);
	if (!name) {
		wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
		wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
		wall_wizards("##   logfile for technical details.");
		warn("Failed to read property from disk: Failed disk read.  obj = #%d, pos = %ld, pdir = %s", obj, pos, pdir);
		return -1;
	}
	if (*name == '*') {
		if (!strcmp(name, "*End*\n")) {
			return 0;
		}
		if (name[1] == '\n') {
			return 2;
		}
	}

	flags = index(name, PROP_DELIMITER);
	if (!flags) {
		wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
		wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
		wall_wizards("##   logfile for technical details.");
		warn("Failed to read property from disk: Corrupt property, flag delimiter not found.  obj = #%d, pos = %ld, pdir = %s, data = %s", obj, pos, pdir, name);
		return -1;
	}
	*flags++ = '\0';

	value = index(flags, PROP_DELIMITER);
	if (!value) {
		wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
		wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
		wall_wizards("##   logfile for technical details.");
		warn("Failed to read property from disk: Corrupt property, value delimiter not found.  obj = #%d, pos = %ld, pdir = %s, data = %s:%s", obj, pos, pdir, name, flags);
		return -1;
	}
	*value++ = '\0';

	p = index(value, '\n');
	if (p) {
		*p = '\0';
	}

	if (!number(flags)) {
		wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
		wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
		wall_wizards("##   logfile for technical details.");
		warn("Failed to read property from disk: Corrupt property flags.  obj = #%d, pos = %ld, pdir = %s, data = %s:%s:%s", obj, pos, pdir, name, flags, value);
		return -1;
	}
	flg = atoi(flags);

	switch (flg & PROP_TYPMASK) {
	case PROP_STRTYP:
		if (!do_diskbase_propvals || pos) {
			flg &= ~PROP_ISUNLOADED;
			if (pnode) {
				SetPDataStr(pnode, alloc_string(value));
				SetPFlagsRaw(pnode, flg);
			} else {
				mydat.flags = flg;
				mydat.data.str = value;
				set_property_nofetch(obj, name, &mydat);
			}
		} else {
			flg |= PROP_ISUNLOADED;
			mydat.flags = flg;
			mydat.data.val = tpos;
			set_property_nofetch(obj, name, &mydat);
		}
		break;
	case PROP_LOKTYP:
		if (!do_diskbase_propvals || pos) {
			lok = parse_boolexp(1, value, 32767);
			flg &= ~PROP_ISUNLOADED;
			if (pnode) {
				SetPDataLok(pnode, lok);
				SetPFlagsRaw(pnode, flg);
			} else {
				mydat.flags = flg;
				mydat.data.lok = lok;
				set_property_nofetch(obj, name, &mydat);
			}
		} else {
			flg |= PROP_ISUNLOADED;
			mydat.flags = flg;
			mydat.data.val = tpos;
			set_property_nofetch(obj, name, &mydat);
		}
		break;
	case PROP_INTTYP:
		if (!number(value)) {
			wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
			wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
			wall_wizards("##   logfile for technical details.");
			warn("Failed to read property from disk: Corrupt integer value.  obj = #%d, pos = %ld, pdir = %s, data = %s:%s:%s", obj, pos, pdir, name, flags, value);
			return -1;
		}
		mydat.flags = flg;
		mydat.data.val = atoi(value);
		set_property_nofetch(obj, name, &mydat);
		break;
	case PROP_FLTTYP:
		mydat.flags = flg;
		if (!number(value) && !ifloat(value)) {
			char *tpnt = value;
			int dtemp = 0;

			if ((*tpnt == '+') || (*tpnt == '-')) {
				if (*tpnt == '+') {
					dtemp = 0;
				} else {
					dtemp = 1;
				}
				tpnt++;
			}
			tpnt[0] = toupper(tpnt[0]);
			tpnt[1] = toupper(tpnt[1]);
			tpnt[2] = toupper(tpnt[2]);
			if (!strncmp(tpnt, "INF", 3)) {
				if (!dtemp) {
					mydat.data.fval = HUGE_VAL;
				} else {
					mydat.data.fval = -HUGE_VAL;
				}
			} else {
				if (!strncmp(tpnt, "NAN", 3)) {
					/* FIXME: This should be NaN. */
					mydat.data.fval = HUGE_VAL;
				}
			}
		} else {
			sscanf(value, "%lg", &mydat.data.fval);
		}
		set_property_nofetch(obj, name, &mydat);
		break;
	case PROP_REFTYP:
		if (!number(value)) {
			wall_wizards("## WARNING! A corrupt property was found while trying to read it from disk.");
			wall_wizards("##   This property has been skipped and will not be loaded.  See the sanity");
			wall_wizards("##   logfile for technical details.");
			warn("Failed to read property from disk: Corrupt dbref value.  obj = #%d, pos = %ld, pdir = %s, data = %s:%s:%s", obj, pos, pdir, name, flags, value);
			return -1;
		}
		mydat.flags = flg;
		mydat.data.ref = atoi(value);
		set_property_nofetch(obj, name, &mydat);
		break;
	case PROP_DIRTYP:
		break;
	}
	return 1;
}

void
db_getprops(FILE * f, dbref obj, const char *pdir)
{
	while (db_get_single_prop(f, obj, 0L, (PropPtr) NULL, pdir)) ;
}


void
db_putprop(FILE * f, const char *dir, PropPtr p)
{
	char buf[BUFFER_LEN * 2];
	const char *ptr2;
	char tbuf[50];
	int outflags = (PropFlagsRaw(p) & ~(PROP_TOUCHED | PROP_ISUNLOADED | PROP_DIRUNLOADED));

	if (PropType(p) == PROP_DIRTYP)
		return;

	ptr2 = "";
	switch (PropType(p)) {
	case PROP_INTTYP:
		if (!PropDataVal(p))
			return;
		ptr2 = intostr(PropDataVal(p));
		break;
	case PROP_FLTTYP:
		if (!PropDataFVal(p))
			return;
		snprintf(tbuf, sizeof(tbuf), "%.17g", PropDataFVal(p));
		ptr2 = tbuf;
		break;
	case PROP_REFTYP:
		if (PropDataRef(p) == NOTHING)
			return;
		ptr2 = intostr(PropDataRef(p));
		break;
	case PROP_STRTYP:
		if (!*PropDataStr(p))
			return;
		ptr2 = PropDataStr(p);
		break;
	case PROP_LOKTYP:
		if (PropFlags(p) & PROP_ISUNLOADED)
			return;
		if (PropDataLok(p) == TRUE_BOOLEXP)
			return;
		ptr2 = unparse_boolexp((dbref) 1, PropDataLok(p), 0);
		break;
	}

	snprintf(buf, sizeof(buf), "%s%s%c%d%c%s\n",
			dir+1, PropName(p), PROP_DELIMITER,
			outflags, PROP_DELIMITER, ptr2);

	if (fputs(buf, f) == EOF) {
		warn("Failed to write out property db_putprop(dir = %s)", dir);
		abort();
	}
}


int
db_dump_props_rec(dbref obj, FILE * f, const char *dir, PropPtr p)
{
	char buf[BUFFER_LEN];
	int count = 0;
	int pdcount;

	if (!p)
		return 0;

	count += db_dump_props_rec(obj, f, dir, AVL_LF(p));

	db_putprop(f, dir, p);

	if (PropDir(p)) {
		const char *iptr;
		char *optr;

		for (iptr = dir, optr = buf; *iptr;)
			*optr++ = *iptr++;
		for (iptr = PropName(p); *iptr;)
			*optr++ = *iptr++;
		*optr++ = PROPDIR_DELIMITER;
		*optr++ = '\0';

		pdcount = db_dump_props_rec(obj, f, buf, PropDir(p));
		count += pdcount;
	}

	count += db_dump_props_rec(obj, f, dir, AVL_RT(p));

	return count;
}


void
db_dump_props(FILE * f, dbref obj)
{
	db_dump_props_rec(obj, f, "/", DBFETCH(obj)->properties);
}


void
untouchprop_rec(PropPtr p)
{
	if (!p)
		return;
	SetPFlags(p, (PropFlags(p) & ~PROP_TOUCHED));
	untouchprop_rec(AVL_LF(p));
	untouchprop_rec(AVL_RT(p));
	untouchprop_rec(PropDir(p));
}

static dbref untouch_lastdone = 0;
void
untouchprops_incremental(int limit)
{
	PropPtr p;

	while (untouch_lastdone < db_top) {
		/* clear the touch flags */
		p = DBFETCH(untouch_lastdone)->properties;
		if (p) {
			if (!limit--)
				return;
			untouchprop_rec(p);
		}
		untouch_lastdone++;
	}
	untouch_lastdone = 0;
}

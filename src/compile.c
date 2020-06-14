/*
 *  Compile.c   (This is really a tokenizer, not a compiler)
 */

#include "copyright.h"
#include "config.h"

#include "db.h"
#include "props.h"
#include "interface.h"
#include "inst.h"
#include "externs.h"
#include "params.h"
#include "defaults.h"
#include "match.h"
#include "interp.h"
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

/* This file contains code for doing "byte-compilation" of
   mud-forth programs.  As such, it contains many internal
   data structures and other such which are not found in other
   parts of TinyMUCK.                                       */

/* The CONTROL_STACK is a stack for holding previous control statements.
   This is used to resolve forward references for IF/THEN and loops, as well
   as a placeholder for back references for loops. */

#define CTYPE_IF    1
#define CTYPE_ELSE  2
#define CTYPE_BEGIN 3
#define CTYPE_FOR   4			/* Get it?  CTYPE_FOUR!!  HAHAHAHAHA  -Fre'ta */
								/* C-4?  *BOOM!*  -Revar */
#define CTYPE_WHILE 5
#define CTYPE_TRY   6			/* reserved for exception handling */
#define CTYPE_CATCH 7			/* reserved for exception handling */


/* These would be constants, but their value isn't known until runtime. */

struct CONTROL_STACK {
	short type;
	struct INTERMEDIATE *place;
	struct CONTROL_STACK *next;
	struct CONTROL_STACK *extra;
};

/* This structure is an association list that contains both a procedure
   name and the place in the code that it belongs.  A lookup to the procedure
   will see both it's name and it's number and so we can generate a
   reference to it.  Since I want to disallow co-recursion,  I will not allow
   forward referencing.
   */

struct PROC_LIST {
	const char *name;
	int returntype;
	struct INTERMEDIATE *code;
	struct PROC_LIST *next;
};

/* The intermediate code is code generated as a linked list
   when there is no hint or notion of how much code there
   will be, and to help resolve all references.
   There is always a pointer to the current word that is
   being compiled kept.
   */

struct INTERMEDIATE {
	int no;						/* which number instruction this is */
	struct inst in;				/* instruction itself */
	short line;					/* line number of instruction */
	short flags;
	struct INTERMEDIATE *next;	/* next instruction */
};


/* The state structure for a compile. */
typedef struct COMPILE_STATE_T {
	struct CONTROL_STACK *control_stack;
	struct PROC_LIST *procs;

	int nowords;				/* number of words compiled */
	struct INTERMEDIATE *curr_word;	/* word being compiled */
	struct INTERMEDIATE *first_word;	/* first word of the list */
	struct INTERMEDIATE *curr_proc;	/* first word of curr. proc. */
	struct publics *currpubs;
	int nested_fors;
	int nested_trys;

	/* Address resolution data.  Used to relink addresses after compile. */
	struct INTERMEDIATE **addrlist; /* list of addresses to resolve */
	int *addroffsets;               /* list of offsets from instrs */
	int addrmax;                    /* size of current addrlist array */
	int addrcount;                  /* number of allocated addresses */

	/* variable names.  The index into cstat->variables give you what position
	 * the variable holds.
	 */
	const char *variables[MAX_VAR];
	int variabletypes[MAX_VAR];
	const char *localvars[MAX_VAR];
	int localvartypes[MAX_VAR];
	const char *scopedvars[MAX_VAR];
	int scopedvartypes[MAX_VAR];

	struct line *curr_line;		/* current line */
	int lineno;			/* current line number */
	int start_comment;              /* Line last comment started at */
	int force_comment;              /* Only attempt certain compile. */
	const char *next_char;		/* next char * */
	dbref player, program;		/* player and program for this compile */

	int compile_err;			/* 1 if error occured */

	char *line_copy;
	int macrosubs;				/* Safeguard for macro-subst. infinite loops */
	int descr;					/* the descriptor that initiated compiling */
	int force_err_display;		/* If true, always show compiler errors. */
	struct INTERMEDIATE *nextinst;
	hash_tab defhash[DEFHASHSIZE];
} COMPSTATE;

void
do_compile(int descr, dbref player_in, dbref program_in, int force_err_display)
{
}

/* support routines for internal data structures. */

int
scopedvar(COMPSTATE * cstat, const char *token)
{
	int i;

	for (i = 0; i < MAX_VAR && cstat->scopedvars[i]; i++)
		if (!strcmp(token, cstat->scopedvars[i]))
			return 1;

	return 0;
}

int
localvar(COMPSTATE * cstat, const char *token)
{
	int i;

	for (i = 0; i < MAX_VAR && cstat->localvars[i]; i++)
		if (!strcmp(token, cstat->localvars[i]))
			return 1;

	return 0;
}

/* clean up as nicely as we can. */

void
cleanup(COMPSTATE * cstat)
{
}

static const char *compile_c_version = "$RCSfile$ $Revision: 1.88 $";
const char *get_compile_c_version(void) { return compile_c_version; }

#ifndef _INTERP_H
#define _INTERP_H

/* Stuff the interpreter needs. */
/* Some icky machine/compiler #defines. --jim */
#ifdef MIPS
typedef char *voidptr;

#define MIPSCAST (char *)
#else
typedef void *voidptr;

#define MIPSCAST
#endif

#define UPCASE(x) (toupper(x))
#define DOWNCASE(x) (tolower(x))

extern void RCLEAR(struct inst *oper, char *file, int line);

#define CLEAR(oper) RCLEAR(oper, __FILE__, __LINE__)
extern void push(struct inst *stack, int *top, int type, voidptr res);

extern const char* scopedvar_getname(struct frame *fr, int level, int varnum);

extern void copyinst(struct inst *from, struct inst *to);

extern void push(struct inst *stack, int *top, int type, voidptr res);

#define Min(x,y) ((x < y) ? x : y)

#define PRIM_PROTOTYPE dbref player, dbref program, int mlev, \
                       struct inst *pc, struct inst *arg, int *top, \
                       struct frame *fr

#define SORTTYPE_CASEINSENS     0x1
#define SORTTYPE_DESCENDING     0x2

#define SORTTYPE_CASE_ASCEND    0
#define SORTTYPE_NOCASE_ASCEND  (SORTTYPE_CASEINSENS)
#define SORTTYPE_CASE_DESCEND   (SORTTYPE_DESCENDING)
#define SORTTYPE_NOCASE_DESCEND (SORTTYPE_CASEINSENS | SORTTYPE_DESCENDING)
#define SORTTYPE_SHUFFLE        4

extern int nargs;				/* DO NOT TOUCH THIS VARIABLE */

#endif /* _INTERP_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef interph_version
#define interph_version
const char *interp_h_version = "$RCSfile$ $Revision: 1.18 $";
#endif
#else
extern const char *interp_h_version;
#endif


/* Primitives package */

#include "copyright.h"
#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "db.h"
#include "mcp.h"
#include "inst.h"
#include "externs.h"
#include "match.h"
#include "interface.h"
#include "params.h"
#include "defaults.h"
#include "fbstrings.h"
#include "interp.h"
#include "array.h"

static struct inst *oper1, *oper2, *oper3, *oper4;
static int result;


struct mcp_muf_context {
	dbref prog;
};


void
muf_mcp_context_cleanup(void *context)
{
	struct mcp_muf_context* mmc = (struct mcp_muf_context*)context;

	free(mmc);
}


void
muf_mcp_callback(McpFrame * mfr, McpMesg * mesg, McpVer version, void *context)
{
	struct mcp_muf_context* mmc = (struct mcp_muf_context*)context;
	dbref obj = mmc->prog;
	struct mcp_binding *ptr;
	McpArg *arg;
	char *pkgname = mesg->package;
	char *msgname = mesg->mesgname;
	dbref user;
	int descr;

	descr = mcpframe_to_descr(mfr);
	user = mcpframe_to_user(mfr);

	for (ptr = PROGRAM_MCPBINDS(obj); ptr; ptr = ptr->next) {
		if (ptr->pkgname && ptr->msgname) {
			if (!strcmp(ptr->pkgname, pkgname)) {
				if (!strcmp(ptr->msgname, msgname)) {
					break;
				}
			}
		}
	}

	if (ptr) {
		struct frame *tmpfr;
		stk_array *argarr;
		struct inst argname, argval, argpart;

		tmpfr = interp(descr, user, getloc(user), obj, -1, PREEMPT, STD_REGUID, 0);
		if (tmpfr) {
			oper1 = tmpfr->argument.st + --tmpfr->argument.top;
			CLEAR(oper1);
			argarr = new_array_dictionary();
			for (arg = mesg->args; arg; arg = arg->next) {
				if (!arg->value) {
					argval.type = PROG_STRING;
					argval.data.string = NULL;
				} else if (!arg->value->next) {
					argval.type = PROG_STRING;
					argval.data.string = alloc_prog_string(arg->value->value);
				} else {
					McpArgPart *partptr;
					int count = 0;

					argname.type = PROG_INTEGER;
					argval.type = PROG_ARRAY;
					argval.data.array =
							new_array_packed(mcp_mesg_arg_linecount(mesg, arg->name));

					for (partptr = arg->value; partptr; partptr = partptr->next) {
						argname.data.number = count++;
						argpart.type = PROG_STRING;
						argpart.data.string = alloc_prog_string(partptr->value);
						array_setitem(&argval.data.array, &argname, &argpart);
						CLEAR(&argpart);
					}
				}
				argname.type = PROG_STRING;
				argname.data.string = alloc_prog_string(arg->name);
				array_setitem(&argarr, &argname, &argval);
				CLEAR(&argname);
				CLEAR(&argval);
			}
			push(tmpfr->argument.st, &(tmpfr->argument.top), PROG_INTEGER, MIPSCAST & descr);
			push(tmpfr->argument.st, &(tmpfr->argument.top), PROG_ARRAY, MIPSCAST argarr);
			tmpfr->pc = ptr->addr;
			interp_loop(user, obj, tmpfr, 0);
		}
	}
}


struct mcpevent_context {
	int pid;
};


void
mcpevent_context_cleanup(void *context)
{
	struct mcpevent_context* mec = (struct mcpevent_context*)context;

	free(mec);
}


void
muf_mcp_event_callback(McpFrame * mfr, McpMesg * mesg, McpVer version, void *context)
{
	struct mcpevent_context* mec = (struct mcpevent_context*)context;
	int destpid = mec->pid;
	struct frame* destfr = timequeue_pid_frame(destpid);
	int descr = mcpframe_to_descr(mfr);
	char *pkgname = mesg->package;
	char *msgname = mesg->mesgname;
	McpArg *arg;

	if (destfr) {
		char buf[BUFFER_LEN];
		stk_array *argarr;
		stk_array *contarr;
		struct inst argname, argval, argpart;

		argarr = new_array_dictionary();
		for (arg = mesg->args; arg; arg = arg->next) {
			if (!arg->value) {
				argval.type = PROG_ARRAY;
				argval.data.array = NULL;
			} else {
				McpArgPart *partptr;
				int count = 0;

				argname.type = PROG_INTEGER;
				argval.type = PROG_ARRAY;
				argval.data.array =
						new_array_packed(mcp_mesg_arg_linecount(mesg, arg->name));

				for (partptr = arg->value; partptr; partptr = partptr->next) {
					argname.data.number = count++;
					argpart.type = PROG_STRING;
					argpart.data.string = alloc_prog_string(partptr->value);
					array_setitem(&argval.data.array, &argname, &argpart);
					CLEAR(&argpart);
				}
			}
			argname.type = PROG_STRING;
			argname.data.string = alloc_prog_string(arg->name);
			array_setitem(&argarr, &argname, &argval);
			CLEAR(&argname);
			CLEAR(&argval);
		}

		contarr = new_array_dictionary();
		array_set_strkey_intval(&contarr, "descr", descr);
		array_set_strkey_strval(&contarr, "package", pkgname);
		array_set_strkey_strval(&contarr, "message", msgname);

		argname.type = PROG_STRING;
		argname.data.string = alloc_prog_string("args");
		argval.type = PROG_ARRAY;
		argval.data.array = argarr;
		array_setitem(&contarr, &argname, &argval);
		CLEAR(&argname);
		CLEAR(&argval);

		argval.type = PROG_ARRAY;
		argval.data.array = contarr;
		if (msgname && *msgname) {
			snprintf(buf, sizeof(buf), "MCP.%.128s-%.128s", pkgname, msgname);
		} else {
			snprintf(buf, sizeof(buf), "MCP.%.128s", pkgname);
		}
		muf_event_add(destfr, buf, &argval, 0);
		CLEAR(&argval);
	}
}


int
stuff_dict_in_mesg(stk_array* arr, McpMesg* msg)
{
	struct inst argname, *argval;
	char buf[64];
	int result;
	
	result = array_first(arr, &argname);
	while (result) {
		if (argname.type != PROG_STRING) {
			CLEAR(&argname);
			return -1;
		}
		if (!argname.data.string || !*argname.data.string->data) {
			CLEAR(&argname);
			return -2;
		}
		argval = array_getitem(arr, &argname);
		switch (argval->type) {
		case PROG_ARRAY:{
				struct inst subname, *subval;
				int contd = array_first(argval->data.array, &subname);

				mcp_mesg_arg_remove(msg, argname.data.string->data);
				while (contd) {
					subval = array_getitem(argval->data.array, &subname);
					switch (subval->type) {
					case PROG_STRING:
						mcp_mesg_arg_append(msg, argname.data.string->data,
											DoNullInd(subval->data.string));
						break;
					case PROG_INTEGER:
						snprintf(buf, sizeof(buf), "%d", subval->data.number);
						mcp_mesg_arg_append(msg, argname.data.string->data, buf);
						break;
					case PROG_OBJECT:
						snprintf(buf, sizeof(buf), "#%d", subval->data.number);
						mcp_mesg_arg_append(msg, argname.data.string->data, buf);
						break;
					case PROG_FLOAT:
						snprintf(buf, sizeof(buf), "%.15g", subval->data.fnumber);
						mcp_mesg_arg_append(msg, argname.data.string->data, buf);
						break;
					default:
						CLEAR(&argname);
						return -3;
					}
					contd = array_next(argval->data.array, &subname);
				}
				break;
			}
		case PROG_STRING:
			mcp_mesg_arg_remove(msg, argname.data.string->data);
			mcp_mesg_arg_append(msg, argname.data.string->data,
								DoNullInd(argval->data.string));
			break;
		case PROG_INTEGER:
			snprintf(buf, sizeof(buf), "%d", argval->data.number);
			mcp_mesg_arg_remove(msg, argname.data.string->data);
			mcp_mesg_arg_append(msg, argname.data.string->data, buf);
			break;
		case PROG_OBJECT:
			snprintf(buf, sizeof(buf), "#%d", argval->data.number);
			mcp_mesg_arg_remove(msg, argname.data.string->data);
			mcp_mesg_arg_append(msg, argname.data.string->data, buf);
			break;
		case PROG_FLOAT:
			snprintf(buf, sizeof(buf), "%.15g", argval->data.fnumber);
			mcp_mesg_arg_remove(msg, argname.data.string->data);
			mcp_mesg_arg_append(msg, argname.data.string->data, buf);
			break;
		default:
			CLEAR(&argname);
			return -4;
		}
		result = array_next(arr, &argname);
	}
	return 0;
}


void
prim_mcp_register(PRIM_PROTOTYPE)
{
	char *pkgname;
	McpVer vermin, vermax;
	struct mcp_muf_context *mmc;

	CHECKOP(3);
	oper3 = POP();
	oper2 = POP();
	oper1 = POP();

	/* oper1  oper2   oper3  */
	/* pkgstr minver  maxver */

	if (mlev < MCP_MUF_MLEV)
		abort_interp("Permission denied.");
	if (oper1->type != PROG_STRING)
		abort_interp("Package name string expected. (1)");
	if (oper2->type != PROG_FLOAT)
		abort_interp("Floating point minimum version number expected. (2)");
	if (oper3->type != PROG_FLOAT)
		abort_interp("Floating point maximum version number expected. (3)");
	if (!oper1->data.string || !*oper1->data.string->data)
		abort_interp("Package name cannot be a null string. (1)");

	pkgname = oper1->data.string->data;
	vermin.vermajor = (int) oper2->data.fnumber;
	vermin.verminor = (int) (oper2->data.fnumber * 1000) % 1000;
	vermax.vermajor = (int) oper3->data.fnumber;
	vermax.verminor = (int) (oper3->data.fnumber * 1000) % 1000;

	mmc = (struct mcp_muf_context*)malloc(sizeof(struct mcp_muf_context));
	mmc->prog = program;

	mcp_package_register(pkgname, vermin, vermax, muf_mcp_callback, (void *) mmc, muf_mcp_context_cleanup);

	CLEAR(oper1);
	CLEAR(oper2);
	CLEAR(oper3);
}



void
prim_mcp_register_event(PRIM_PROTOTYPE)
{
	char *pkgname;
	McpVer vermin, vermax;
	struct mcpevent_context *mec;


	CHECKOP(3);
	oper3 = POP();
	oper2 = POP();
	oper1 = POP();

	/* oper1  oper2   oper3  */
	/* pkgstr minver  maxver */

	if (mlev < MCP_MUF_MLEV)
		abort_interp("Permission denied.");
	if (oper1->type != PROG_STRING)
		abort_interp("Package name string expected. (1)");
	if (oper2->type != PROG_FLOAT)
		abort_interp("Floating point minimum version number expected. (2)");
	if (oper3->type != PROG_FLOAT)
		abort_interp("Floating point maximum version number expected. (3)");
	if (!oper1->data.string || !*oper1->data.string->data)
		abort_interp("Package name cannot be a null string. (1)");

	pkgname = oper1->data.string->data;
	vermin.vermajor = (int) oper2->data.fnumber;
	vermin.verminor = (int) (oper2->data.fnumber * 1000) % 1000;
	vermax.vermajor = (int) oper3->data.fnumber;
	vermax.verminor = (int) (oper3->data.fnumber * 1000) % 1000;

	mec = (struct mcpevent_context*)malloc(sizeof(struct mcpevent_context));
	mec->pid = fr->pid;

	mcp_package_register(pkgname, vermin, vermax, muf_mcp_event_callback, (void *)mec, mcpevent_context_cleanup);

	CLEAR(oper1);
	CLEAR(oper2);
	CLEAR(oper3);
}



void
prim_mcp_supports(PRIM_PROTOTYPE)
{
	char *pkgname;
	int descr;
	McpVer ver;
	McpFrame *mfr;
	double fver;

	CHECKOP(2);
	oper2 = POP();
	oper1 = POP();

	/* oper1 oper2  */
	/* descr pkgstr */

	if (oper1->type != PROG_INTEGER)
		abort_interp("Integer descriptor number expected. (1)");
	if (oper2->type != PROG_STRING)
		abort_interp("Package name string expected. (2)");
	if (!oper2->data.string || !*oper2->data.string->data)
		abort_interp("Package name cannot be a null string. (2)");

	pkgname = oper2->data.string->data;
	descr = oper1->data.number;

	fver = 0.0;
	mfr = descr_mcpframe(descr);
	if (mfr) {
		ver = mcp_frame_package_supported(mfr, pkgname);
		fver = ver.vermajor + (ver.verminor / 1000.0);
	}

	CLEAR(oper1);
	CLEAR(oper2);

	PushFloat(fver);
}



void
prim_mcp_bind(PRIM_PROTOTYPE)
{
	char *pkgname, *msgname;
	struct mcp_binding *ptr;

	CHECKOP(3);
	oper3 = POP();
	oper2 = POP();
	oper1 = POP();

	/* oper1  oper2  oper3 */
	/* pkgstr msgstr address */

	if (mlev < MCP_MUF_MLEV)
		abort_interp("Permission denied.");
	if (oper1->type != PROG_STRING)
		abort_interp("Package name string expected. (1)");
	if (oper2->type != PROG_STRING)
		abort_interp("Message name string expected. (2)");
	if (oper3->type != PROG_ADD)
		abort_interp("Function address expected. (3)");
	if (!oper1->data.string || !*oper1->data.string->data)
		abort_interp("Package name cannot be a null string. (1)");
	if (oper3->data.addr->progref >= db_top ||
		oper3->data.addr->progref < 0 || (Typeof(oper3->data.addr->progref) != TYPE_PROGRAM)) {
		abort_interp("Invalid address. (3)");
	}
	if (program != oper3->data.addr->progref) {
		abort_interp("Destination address outside current program. (3)");
	}

	pkgname = oper1->data.string->data;
	msgname = oper2->data.string ? oper2->data.string->data : "";

	for (ptr = PROGRAM_MCPBINDS(program); ptr; ptr = ptr->next) {
		if (ptr->pkgname && ptr->msgname) {
			if (!strcmp(ptr->pkgname, pkgname)) {
				if (!strcmp(ptr->msgname, msgname)) {
					break;
				}
			}
		}
	}
	if (!ptr) {
		ptr = (struct mcp_binding *) malloc(sizeof(struct mcp_binding));

		ptr->pkgname = strdup(pkgname);
		ptr->msgname = strdup(msgname);
		ptr->next = PROGRAM_MCPBINDS(program);
		PROGRAM_SET_MCPBINDS(program, ptr);
	}
	ptr->addr = oper3->data.addr->data;

	CLEAR(oper1);
	CLEAR(oper2);
	CLEAR(oper3);
}


void
prim_mcp_send(PRIM_PROTOTYPE)
{
	char *pkgname, *msgname;
	int descr;
	McpFrame *mfr;
	stk_array *arr;

	CHECKOP(4);
	oper3 = POP();
	oper2 = POP();
	oper1 = POP();
	oper4 = POP();

	/* oper4 oper1  oper2  oper3 */
	/* descr pkgstr msgstr argsarray */

	if (mlev < MCP_MUF_MLEV)
		abort_interp("Permission denied.");
	if (oper4->type != PROG_INTEGER)
		abort_interp("Integer descriptor number expected. (1)");
	if (oper1->type != PROG_STRING)
		abort_interp("Package name string expected. (2)");
	if (!oper1->data.string || !*oper1->data.string->data)
		abort_interp("Package name cannot be a null string. (2)");
	if (oper2->type != PROG_STRING)
		abort_interp("Message name string expected. (3)");
	if (oper3->type != PROG_ARRAY)
		abort_interp("Arguments dictionary expected. (4)");

	pkgname = oper1->data.string->data;
	msgname = oper2->data.string ? oper2->data.string->data : "";
	arr = oper3->data.array;
	descr = oper4->data.number;

	mfr = descr_mcpframe(descr);
	if (mfr) {
		McpMesg msg;
		McpVer ver = mcp_frame_package_supported(mfr, pkgname);

		if (ver.verminor == 0 && ver.vermajor == 0)
			abort_interp("MCP package not supported by that descriptor.");

		mcp_mesg_init(&msg, pkgname, msgname);

		result = stuff_dict_in_mesg(arr, &msg);
		if (result) {
			mcp_mesg_clear(&msg);
			switch (result) {
				case -1:
					abort_interp("Args dictionary can only have string keys. (4)");
					break;
				case -2:
					abort_interp("Args dictionary cannot have a null string key. (4)");
					break;
				case -3:
					abort_interp("Unsupported value type in list value. (4)");
					break;
				case -4:
					abort_interp("Unsupported value type in args dictionary. (4)");
					break;
			}
		}
		
		mcp_frame_output_mesg(mfr, &msg);
		mcp_mesg_clear(&msg);
	}

	CLEAR(oper1);
	CLEAR(oper2);
}

static const char *p_mcp_c_version = "$RCSfile$ $Revision: 1.31 $";
const char *get_p_mcp_c_version(void) { return p_mcp_c_version; }

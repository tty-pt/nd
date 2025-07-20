#ifndef UAPI_TYPES_H
#define UAPI_TYPES_H

#include <stddef.h>
#include <string.h>
#include "./object.h"
#include "./skel.h"
#include "./st.h"

typedef struct {
	size_t arg_size;
	size_t ret_size;
	void (*call)(void *, void *, void *);
} sic_adapter_t;

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define PAIR_COUNT(...) \
	         PP_NARG_(__VA_ARGS__, PAIR_RSEQ_N())
#define PP_NARG_(...) \
	PP_ARG_N(__VA_ARGS__)

#define PP_ARG_N( \
		 _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		_21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
		_31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
		_41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
		_51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
		_61, _62, _63, N, ...) N

#define PAIR_RSEQ_N() \
	31,31,30,30,29,29,28,28,27,27,26,26,25,25, \
	24,24,23,23,22,22,21,21,20,20,19,19,18,18, \
	17,17,16,16,15,15,14,14,13,13,12,12,11,11, \
	10,10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, \
	 3, 3, 2, 2, 1, 1, 0, 0

#define FUNC_ARGS(...) CAT(FUNC_ARGS_, PAIR_COUNT(__VA_ARGS__))( __VA_ARGS__)
#define FUNC_ARGS_1(a, b)        a b
#define FUNC_ARGS_2(a, b, ...)   a b, FUNC_ARGS_1(__VA_ARGS__)
#define FUNC_ARGS_3(a, b, ...)   a b, FUNC_ARGS_2(__VA_ARGS__)
#define FUNC_ARGS_4(a, b, ...)   a b, FUNC_ARGS_3(__VA_ARGS__)
#define FUNC_ARGS_5(a, b, ...)   a b, FUNC_ARGS_4(__VA_ARGS__)
#define FUNC_ARGS_6(a, b, ...)   a b, FUNC_ARGS_5(__VA_ARGS__)
#define FUNC_ARGS_7(a, b, ...)   a b, FUNC_ARGS_6(__VA_ARGS__)
#define FUNC_ARGS_8(a, b, ...)   a b, FUNC_ARGS_7(__VA_ARGS__)
#define FUNC_ARGS_9(a, b, ...)   a b, FUNC_ARGS_8(__VA_ARGS__)
#define FUNC_ARGS_10(a, b, ...)  a b, FUNC_ARGS_9(__VA_ARGS__)
#define FUNC_ARGS_11(a, b, ...)  a b, FUNC_ARGS_10(__VA_ARGS__)
#define FUNC_ARGS_12(a, b, ...)  a b, FUNC_ARGS_11(__VA_ARGS__)
#define FUNC_ARGS_13(a, b, ...)  a b, FUNC_ARGS_12(__VA_ARGS__)
#define FUNC_ARGS_14(a, b, ...)  a b, FUNC_ARGS_13(__VA_ARGS__)
#define FUNC_ARGS_15(a, b, ...)  a b, FUNC_ARGS_14(__VA_ARGS__)
#define FUNC_ARGS_16(a, b, ...)  a b, FUNC_ARGS_15(__VA_ARGS__)

#define PAIR_GEN(...) CAT(PAIR_GEN_, PAIR_COUNT(__VA_ARGS__))( __VA_ARGS__)
#define PAIR_GEN_1(a, b)        a b;
#define PAIR_GEN_2(a, b, ...)   a b; PAIR_GEN_1(__VA_ARGS__)
#define PAIR_GEN_3(a, b, ...)   a b; PAIR_GEN_2(__VA_ARGS__)
#define PAIR_GEN_4(a, b, ...)   a b; PAIR_GEN_3(__VA_ARGS__)
#define PAIR_GEN_5(a, b, ...)   a b; PAIR_GEN_4(__VA_ARGS__)
#define PAIR_GEN_6(a, b, ...)   a b; PAIR_GEN_5(__VA_ARGS__)
#define PAIR_GEN_7(a, b, ...)   a b; PAIR_GEN_6(__VA_ARGS__)
#define PAIR_GEN_8(a, b, ...)   a b; PAIR_GEN_7(__VA_ARGS__)
#define PAIR_GEN_9(a, b, ...)   a b; PAIR_GEN_8(__VA_ARGS__)
#define PAIR_GEN_10(a, b, ...)  a b; PAIR_GEN_9(__VA_ARGS__)
#define PAIR_GEN_11(a, b, ...)  a b; PAIR_GEN_10(__VA_ARGS__)
#define PAIR_GEN_12(a, b, ...)  a b; PAIR_GEN_11(__VA_ARGS__)
#define PAIR_GEN_13(a, b, ...)  a b; PAIR_GEN_12(__VA_ARGS__)
#define PAIR_GEN_14(a, b, ...)  a b; PAIR_GEN_13(__VA_ARGS__)
#define PAIR_GEN_15(a, b, ...)  a b; PAIR_GEN_14(__VA_ARGS__)
#define PAIR_GEN_16(a, b, ...)  a b; PAIR_GEN_15(__VA_ARGS__)

#define CALL_NARGS(...) CAT(CALL_NARGS_, PAIR_COUNT(__VA_ARGS__))( __VA_ARGS__)
#define CALL_NARGS_1(a, b)        args.b
#define CALL_NARGS_2(a, b, ...)   args.b, CALL_NARGS_1(__VA_ARGS__)
#define CALL_NARGS_3(a, b, ...)   args.b, CALL_NARGS_2(__VA_ARGS__)
#define CALL_NARGS_4(a, b, ...)   args.b, CALL_NARGS_3(__VA_ARGS__)
#define CALL_NARGS_5(a, b, ...)   args.b, CALL_NARGS_4(__VA_ARGS__)
#define CALL_NARGS_6(a, b, ...)   args.b, CALL_NARGS_5(__VA_ARGS__)
#define CALL_NARGS_7(a, b, ...)   args.b, CALL_NARGS_6(__VA_ARGS__)
#define CALL_NARGS_8(a, b, ...)   args.b, CALL_NARGS_7(__VA_ARGS__)
#define CALL_NARGS_9(a, b, ...)   args.b, CALL_NARGS_8(__VA_ARGS__)
#define CALL_NARGS_10(a, b, ...)  args.b, CALL_NARGS_9(__VA_ARGS__)
#define CALL_NARGS_11(a, b, ...)  args.b, CALL_NARGS_10(__VA_ARGS__)
#define CALL_NARGS_12(a, b, ...)  args.b, CALL_NARGS_11(__VA_ARGS__)
#define CALL_NARGS_13(a, b, ...)  args.b, CALL_NARGS_12(__VA_ARGS__)
#define CALL_NARGS_14(a, b, ...)  args.b, CALL_NARGS_13(__VA_ARGS__)
#define CALL_NARGS_15(a, b, ...)  args.b, CALL_NARGS_14(__VA_ARGS__)
#define CALL_NARGS_16(a, b, ...)  args.b, CALL_NARGS_15(__VA_ARGS__)

/* the callee uses this to be called */
#define SIC_DECL(ftype, fname, ...) \
    struct fname##_args { \
        PAIR_GEN(__VA_ARGS__) \
    }; \
    typedef ftype fname##_t(FUNC_ARGS(__VA_ARGS__)); \
    fname##_t fname __attribute__((weak));

#define SIC_DEF(ftype, fname, ...) \
    void fname##_adapter_call(void *res, void *fname, void *arg) { \
        struct fname##_args args; \
	memcpy(&args, arg, sizeof(args)); \
        ftype result = ((fname##_t *) fname)(CALL_NARGS(__VA_ARGS__)); \
	if (res) memcpy(res, &result, sizeof(ftype)); \
    } \
    sic_adapter_t fname##_adapter  __attribute__((visibility("default"))) = { \
	    .arg_size = sizeof(struct fname##_args), \
	    .ret_size = sizeof(ftype), \
	    .call = &fname##_adapter_call, \
    };

#define STR(x) #x
#define XSTR(x) STR(x)

/* the caller uses this to call */
#define SIC_CALL(retp, fname, ...) { \
	struct fname##_args args = { __VA_ARGS__ }; \
	sic_call(retp, XSTR(fname), &args); \
}

#define SIC_AREG(fname) \
	sic_areg(XSTR(fname), &fname##_adapter)

typedef void sic_areg_t(char *name, sic_adapter_t *adapter);
sic_areg_t sic_areg;

typedef void sic_call_t(void *retp, char *symbol, void *args);
sic_call_t sic_call;

SIC_DECL(int, sic_examine, unsigned, player_ref, unsigned, ref)
SIC_DECL(int, sic_fbcp, char *, p, unsigned, ref)
SIC_DECL(int, sic_add, unsigned, ref, unsigned, skel_id, unsigned, where_id, uint64_t, v)
SIC_DECL(unsigned short, sic_view_flags, unsigned short, flags, unsigned, ref)
SIC_DECL(struct icon, sic_icon, struct icon, i, unsigned, ref)
SIC_DECL(int, sic_del, unsigned, ref)
SIC_DECL(int, sic_clone, unsigned, orig_ref, unsigned, nu_ref)
SIC_DECL(int, sic_update, unsigned, ref, double, dt)

SIC_DECL(int, sic_auth, unsigned, player_ref)
SIC_DECL(int, sic_leave, unsigned, player_ref, unsigned, loc_ref)
SIC_DECL(int, sic_enter, unsigned, player_ref, unsigned, loc_ref)
SIC_DECL(int, sic_spawn, unsigned, player_ref, unsigned, loc_ref, struct bio, bio, uint64_t, v)
SIC_DECL(ENT, sic_fight_start, unsigned, player_ref, ENT, eplayer)
SIC_DECL(ENT, sic_mob_recovered, unsigned, player_ref, ENT, eplayer)
SIC_DECL(ENT, sic_mob_recovering, unsigned, player_ref, ENT, eplayer)

extern unsigned type_hd, action_hd, vtf_hd, vtf_max;

typedef unsigned action_register_t(char *label, char *icon);
action_register_t action_register;

typedef struct {
	struct print_info pi;
	char emp;
} vtf_t;

typedef unsigned vtf_register_t(char emp, enum color fg, unsigned flags);
vtf_register_t vtf_register;

#endif

#include "mcp.h"

#include "metal.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/* #include <stdlib.h> */
#include "tty.h"

#define GET_FLAG(x)	( mcp.flags & x )
#define SET_FLAGS(x)	{ mcp.flags |= x ; }
#define UNSET_FLAGS(x)	( mcp.flags &= ~(x) )

enum mcp_flags {
	MCP_ON = 1,
	MCP_KEY = 2,
	MCP_MULTI = 4,
	MCP_ARG = 8,
	MCP_NOECHO = 16,
	MCP_SKIP = 32,

	MCP_NAME = 128,
	MCP_HASH = 256,
};

enum mcp_state {
	// unknown
        // 1 means "\n" was read from telnet
        // 2 means "\n#"
        // 3 means "\n#$" and confirms mcp
	MCP_CONFIRMED = 4,
};

struct mcp_arg {
        char key[32];
        char value[1024];
};

struct {
	int state, flags;
        struct mcp_arg args[8];
        int args_l;
	char name[32];
	char cache[2048], *cache_p;
} mcp;

static char in_buf[4096];
static char out_buf[8282];
char *out_p, *last_p;

static inline void mcp_set(char *buf) {
	/* *mcp.cache_p = '\0'; */
	memcpy(buf, mcp.cache, sizeof(mcp.cache));
	memset(mcp.cache, 0, sizeof(mcp.cache));
	mcp.cache_p = mcp.cache;
}

static inline void mcp_clear() {
	last_p = out_p - 2;
	memset(mcp.args, 0, sizeof(mcp.args));
	memset(mcp.name, 0, sizeof(mcp.name));
	memset(mcp.cache, 0, sizeof(mcp.cache));
	mcp.args_l = 0;
	mcp.cache_p = mcp.cache;
}

static void mcp_emit() {
	struct mcp_arg *arg;
	out_p += sprintf(out_p, "{ \"key\": \"%s\"", mcp.name);
	for (arg = mcp.args; arg < &mcp.args[mcp.args_l]; arg++) {
		out_p += sprintf(out_p, ", \"%s\": \"", arg->key);
		out_p += tty_proc(out_p, arg->value);
		*out_p ++ = '"';
	}
	out_p += sprintf(out_p, " }, ");
	mcp_clear();
}

static void inband_emit() {
	out_p += sprintf(out_p, "{ \"key\": \"inband\", \"data\": \"");
	out_p += tty_proc(out_p, mcp.cache);
	out_p += sprintf(out_p, "\" }, ");
	mcp_clear();
}

static inline struct mcp_arg *
mcp_arg() {
	return &mcp.args[mcp.args_l];
}

static inline void
mcp_proc_ch(char *p) {
	if (GET_FLAG(MCP_SKIP)) {
		if (*p != '\n')
			return;
		mcp.flags ^= MCP_SKIP;
		mcp.state = 1;
		return;
	}

	switch (*p) {
	case '#':
		switch (mcp.state) {
		case 3:
		case 1:
			mcp.state++;
			return;
		}
		break;
	case '$':
		if (mcp.state == 2) {
			mcp.state++;
			return;
		}
		break;
	case '*':
		if (mcp.state == MCP_CONFIRMED) {
			// only assert MULTI
			mcp.flags = MCP_MULTI | MCP_NOECHO;
			mcp.state = 0;
			return;
		}

		if (!GET_FLAG(MCP_ON) || !GET_FLAG(MCP_KEY))
			break;

		SET_FLAGS(MCP_MULTI | MCP_SKIP);
		mcp_set(mcp_arg()->key);
		return;

	case ':':
		if (GET_FLAG(MCP_MULTI)) {
			if (mcp.state == MCP_CONFIRMED) {
				mcp.state = 0;
				mcp.flags = MCP_SKIP | MCP_MULTI;
				mcp_set(mcp_arg()->value);
				mcp.args_l ++;
				mcp_emit();
			} else
				mcp.flags &= ~MCP_NOECHO;
			return;
		} else if (GET_FLAG(MCP_KEY)) {
			mcp_set(mcp_arg()->key);
			return;
		}
		break;

	case '"':
		if (!GET_FLAG(MCP_ON))
			break;

		mcp.flags ^= MCP_KEY;
		if (GET_FLAG(MCP_KEY)) {
			mcp_set(mcp_arg()->value);
			mcp.args_l++;
		}
		return;

	case ' ':
		if (!GET_FLAG(MCP_ON))
			break;
		else if (GET_FLAG(MCP_NAME)) {
			mcp.flags ^= MCP_NAME | MCP_HASH | MCP_NOECHO;
			mcp_set(mcp.name);
			mcp.args_l = 0;
			return;
		} else if (GET_FLAG(MCP_HASH)) {
			mcp.flags ^= MCP_HASH | MCP_KEY | MCP_NOECHO;
			return;
		} else if (GET_FLAG(MCP_KEY))
			return;
		break;
	case '\n':
		mcp.state = 1;
		if (GET_FLAG(MCP_MULTI))
			*mcp.cache_p++ = '\n';
		return;
	}

	if (mcp.state == MCP_CONFIRMED) {
		// new mcp
		if (mcp.name[0])
			mcp_emit();
		else
			inband_emit();
		mcp.flags = MCP_ON | MCP_NAME;
		mcp.state = 0;
	} else if (mcp.state) {
		// mcp turned out impossible
		if (GET_FLAG(MCP_ON))
			mcp_emit();
		strncpy(mcp.cache_p, "\n#$#", mcp.state);
		mcp.cache_p += mcp.state;
		mcp.state = mcp.flags = 0;
	}

	if (!GET_FLAG(MCP_NOECHO))
		*mcp.cache_p++ = *p;
}

export char *
mcp_proc() {
	char *in;

        for (in = in_buf; *in != '\0'; in++)
		mcp_proc_ch(in);

	if (!GET_FLAG(MCP_ON) && !GET_FLAG(MCP_MULTI))
		inband_emit();

	if (last_p != out_buf) {
		*last_p++ = ']';
		*last_p++ = '\0';
		return out_buf;
	}

	return NULL;
}

export char *
mcp_input() {
	return in_buf;
}

export void
mcp_reset() {
	last_p = out_p = out_buf;
	*out_p++ = '[';
	*out_p = '\0';
}

export void
mcp_init() {
        mcp.state = 1;
	mcp_reset();
}

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <metal.h>

#define GET_FLAG(x)	( mcp.flags & x )
#define SET_FLAGS(x)	{ mcp.flags |= x ; }
#define UNSET_FLAGS(x)	( mcp.flags &= ~(x) )

struct attr {
        int fg, bg, x;
};

struct tty {
        int esc_state, csi_changed;
        struct attr c_attr, csi;
        char *end_tag;
} ttys[2];

enum mcp_flags {
	MCP_ON = 1,
	MCP_KEY = 2,
	MCP_MULTI = 4,
	MCP_ARG = 8,
	MCP_NOECHO = 16,
	MCP_SKIP = 32,
	MCP_SECOND = 64,

	MCP_NAME = 128,
	MCP_HASH = 256,
	JSON_CLOSED = 512,
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
        char data[1024];
};

struct mcp {
	int state, flags;
        struct mcp_arg mcp_args[3];
	char *mcpk, *mcpv;
        int mcp_args_l;
} mcp;

/* exporting pointers stopped working
 * on transition to asm.js */
static char in_buf[4096];
static char out_buf[8282];

static inline void
params_push(struct tty *tty, int x)
{
	int     fg = tty->c_attr.fg,
		bg = tty->c_attr.bg;

	switch (x) {
	case 0: fg = 7; bg = 0; break;
	case 1: fg += 8; break;
	default: if (x >= 40)
			 bg = x - 40;
		 else if (x >= 30)
			 fg = (fg >= 8 ? 8 : 0) + x - 30;
	}

	tty->csi.fg = fg;
	tty->csi.bg = bg;
	tty->csi.x = x;
}

static inline size_t
end_tag_reset(char *out, struct tty *tty)
{
	size_t ret = sprintf(out, "%s", tty->end_tag);
	tty->end_tag = "";
	return ret;
}

static inline size_t
csi_change(char *out, struct tty *tty)
{
	char *fout = out;
	int a = tty->csi.fg != 7, b = tty->csi.bg != 0;
        out += sprintf(out, "%s", tty->end_tag);

	if (a || b) {
                out += sprintf(out, "<span class=\"");
		if (a)
			out += sprintf(out, "fg%d ", tty->csi.fg);
		if (b)
			out += sprintf(out, "bg%d", tty->csi.bg);

		out += sprintf(out, "\">");
		tty->end_tag = "</span>";
	} else
		tty->end_tag = "";

        return out - fout;
}

void tty_init(struct tty *tty) {
	tty->csi.fg = tty->c_attr.fg = 7;
	tty->csi.bg = tty->c_attr.bg = 0;
	tty->csi.x = tty->c_attr.x = 0;
        tty->end_tag = "";
	/* UNSET_FLAGS(MCP_MULTILINE); */
}

static inline ssize_t
_json_close(char *out, struct tty *tty) {
	char *fout = out;
	SET_FLAGS(JSON_CLOSED);
	out += sprintf(out, "\" },");
	return out - fout;
}

static inline size_t
_json_open(char *out, struct tty *tty, char *str)
{
	char *fout = out;
	if (!GET_FLAG(JSON_CLOSED)) {
		out += _json_close(out, tty);
		UNSET_FLAGS(JSON_CLOSED);
	}
        out += sprintf(out, "{ \"key\": \"%s", str);
	return out - fout;
}

static inline size_t
json_inband_open(char *out, struct tty *tty)
{
	char *fout = out;
	out += _json_open(out, tty, "inband\", value: \"");
	mcp.flags = 0;
	return out - fout;
}

/* static inline size_t */
/* mcp_handler(char *out, struct tty *tty, char *in) { */
/* 	/1* console_log(in, 17); *1/ */
/* 	size_t ret = json_close(out, tty); */
/* 	mcp.mcpk = NULL; */
/* 	return ret; */
/* } */

static inline unsigned
esc_state_0(char *out, struct tty *tty, char *p) {
	char *fout = out;
	unsigned ret = 1;

	if (GET_FLAG(MCP_SKIP)) {
		if (*p != '\n')
			return 0;

		mcp.state = 1;
		mcp.flags ^= MCP_SKIP;
	}

	switch (*p) {
	case '#':
		switch (mcp.state) {
		case 3:
		case 1:
			mcp.state++;
			return 0;
		}
		break;
	case '$':
		if (mcp.state == 2) {
			mcp.state++;
			return 0;
		}
		break;
	case '*':
		if (mcp.state == MCP_CONFIRMED) {
			// only assert MULTI
			mcp.flags = MCP_MULTI | MCP_NOECHO;
			mcp.state = 0;
			return 0;
		}

		if (!GET_FLAG(MCP_ON) || !GET_FLAG(MCP_KEY))
			break;

		SET_FLAGS(MCP_MULTI | MCP_SKIP);
		return sprintf(out, "\": \"");

	case ':':
		if (GET_FLAG(MCP_MULTI)) {
			if (mcp.state == MCP_CONFIRMED) {
				mcp.state = 0;
				mcp.flags = MCP_SKIP;
			} else
				mcp.flags &= ~MCP_NOECHO;
		} else if (GET_FLAG(MCP_KEY))
			return sprintf(out, "\":");
		break;

	case '"':
		if (!GET_FLAG(MCP_ON))
			break;

		mcp.flags ^= MCP_KEY;
		if (GET_FLAG(MCP_KEY))
			return sprintf(out, "\", \"");
		break;

	case ' ':
		if (!GET_FLAG(MCP_ON))
			break;
		else if (GET_FLAG(MCP_NAME)) {
			mcp.flags ^= MCP_NAME | MCP_HASH | MCP_NOECHO;
			return sprintf(out, "\", \"");
		} else if (GET_FLAG(MCP_HASH)) {
			mcp.flags ^= MCP_HASH | MCP_KEY | MCP_NOECHO;
			return 0;
		} else if (GET_FLAG(MCP_KEY))
			return 0;
		break;
	case '\n':
		mcp.flags &= ~MCP_NOECHO;
		mcp.state = 1;
		return 0;
	}

	if (mcp.state == MCP_CONFIRMED) {
		// new mcp
		out += _json_open(out, tty, "");
		mcp.mcpk = out;
		mcp.flags = MCP_ON | MCP_NAME;
		mcp.state = 0;
	} else if (mcp.state) {
		// mcp turned out impossible
		if (GET_FLAG(MCP_ON))
			out += json_inband_open(out, tty);
		mcp.state--;
		strncpy(out, "#$#", mcp.state);
		out += mcp.state;
		mcp.flags = 0;
	}

	/* if (tty->csi_changed) { */
	/* 	out += csi_change(out, tty); */
	/* 	tty->csi_changed = 0; */
	/* } */

	if (!GET_FLAG(MCP_NOECHO))
		*out++ = *p;

	return out - fout;
}

static inline size_t
esc_state_any(char *out, struct tty *tty, char *p) {
	size_t ret = 0;
	register char ch = *p;

	switch (ch) {
	case '\x18':
	case '\x1a':
		tty->esc_state = 0;
		return 0;
	case '\x1b':
		tty->esc_state = 1;
		return 0;
	case '\x9b':
		tty->esc_state = 2;
		return 0;
	case '\x07': 
	case '\x00':
	case '\x7f':
	case '\v':
	case '\r':
	case '\f':
		return 0;
	}

	switch (tty->esc_state) {
	case 0:
		/* *out = *p; */
		/* return 1; */
		return esc_state_0(out, tty, p);
	case 1:
		switch (ch) {
		case '[':
			tty->esc_state = 2;
			break;
		case '=':
		case '>':
		case 'H':
			tty->esc_state = 0; /* IGNORED */
		}
		break;
	case 2: // just saw CSI
		switch (ch) {
		case 'K':
		case 'H':
		case 'J':
			tty->esc_state = 0;
			return 0;
		case '?':
			tty->esc_state = 5;
			return 0;
		}
		params_push(tty, 0);
		tty->esc_state = 3;
	case 3: // saw CSI and parameters
		switch (ch) {
		case 'm':
			if (tty->c_attr.bg != tty->csi.bg
			    || tty->c_attr.fg != tty->csi.fg)
			{
				tty->c_attr.fg = tty->csi.fg;
				tty->c_attr.bg = tty->csi.bg;
				tty->c_attr.x = 0;
				tty->csi.x = 0;
				tty->csi_changed = 1;
			}
			tty->esc_state = 0;
			break;
		case '[':
			tty->esc_state = 4;
			break;
		case ';':
			params_push(tty, 0);
			break;
		default:
			if (ch >= '0' && ch <= '9')
				params_push(tty, tty->csi.x * 10 + (ch - '0'));
			else
				tty->esc_state = 0;
		}
		break;

	case 5: params_push(tty, ch);
		tty->esc_state = 6;
		break;
	case 4:
	case 6: tty->esc_state = 0;
		break;

	/* default: CBUG(1); */
	default:
		abort();
	}

	return ret;
}

void tty_parse(char *out, struct tty *tty) {
	char *fout = out, *in;

        for (in = in_buf; *in != '\0'; in++)
		out += esc_state_any(out, tty, in);

	*out = '\0';
}

export
void
metal_init() {
	int i;
	for (i = 0; i < sizeof(ttys) / sizeof(struct tty); i++)
		tty_init(&ttys[i]);
	mcp.state = 1;
	mcp.flags = JSON_CLOSED;
}

export
char *
metal_write() {
        tty_parse(out_buf, &ttys[0]);
	return out_buf;
}

export
char *
metal_input() {
        return in_buf;
}

int
main(int argc, char *argv[], char *envp[])
{
	return 0;
}

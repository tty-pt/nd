#include <stdio.h>
#include <metal.h>
#include "mcp.h"

struct attr {
        int fg, bg, x;
};

struct tty {
        int esc_state, csi_changed;
        struct attr c_attr, csi;
        char *end_tag;
} ttys[2];

/* exporting pointers stopped working
 * on transition to asm.js */
static char in_buf[4096];
static char out_buf[8282];
char *out_p;

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

#if 0
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
		return esc_state_0(tty, p);
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
#endif

export
void
metal_init() {
	int i;
	for (i = 0; i < sizeof(ttys) / sizeof(struct tty); i++)
		tty_init(&ttys[i]);
	out_p = out_buf;
	mcp_init();
}

export
char *
metal_write() {
	mcp_proc(in_buf);
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

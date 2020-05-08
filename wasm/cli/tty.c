#include "tty.h"

#include <stdio.h>

struct attr {
        int fg, bg, x;
};

struct tty {
        int esc_state, csi_changed;
        struct attr c_attr, csi;
        char *end_tag;
};

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
csi_change(char *out, struct tty *tty)
{
	char *fout = out;
	int a = tty->csi.fg != 7, b = tty->csi.bg != 0;
        out += sprintf(out, "%s", tty->end_tag);

	if (a || b) {
                out += sprintf(out, "<span class=\"");
		if (a)
			out += sprintf(out, "cf%d", tty->csi.fg);
		if (b)
			out += sprintf(out, " c%d", tty->csi.bg);

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
}

static inline size_t
tty_proc_ch(char *out, struct tty *tty, char *p) {
        char *fout = out;
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
                if (tty->csi_changed) {
                        out += csi_change(out, tty);
                        tty->csi_changed = 0;
                }

		*out++ = *p;
		return out - fout;
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
	}

	return 0;
}

size_t
tty_proc(char *out, char *input) {
        struct tty tty;
	char *in, *fout = out;

        tty_init(&tty);
        for (in = input; *in != '\0'; in++)
		out += tty_proc_ch(out, &tty, in);
        out += sprintf(out, "%s", tty.end_tag);

        return out - fout;
}

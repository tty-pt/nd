#include "tty.h"

#include <stdio.h>
#include <ctype.h>

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

static inline void
csi_change(struct tty *tty)
{
	int a = tty->csi.fg != 7, b = tty->csi.bg != 0;
	tty->driver.csic_pre(tty);

	if (a || b) {
		tty->driver.csic_start(tty);
		if (a)
			tty->driver.csic_fg(tty);
		if (b)
			tty->driver.csic_bg(tty);

		tty->driver.csic_end(tty);
	} else
		tty->driver.csic_nil(tty);
}

void tty_init(struct tty *tty, struct tty_driver driver) {
	tty->csi.fg = tty->c_attr.fg = 7;
	tty->csi.bg = tty->c_attr.bg = 0;
	tty->csi.x = tty->c_attr.x = 0;
        tty->end_tag = "";
	tty->esc_state = 0;
	tty->driver = driver;
	driver.init(tty);
}

char *escape_map[256] = {
	[ 0 ... 255 ] = "",
	/* [(unsigned) '\n'] = "\\n", */
	[(unsigned) '\t'] = "\\t",
	[(unsigned) '"'] = "\\\"",
	[(unsigned) '\\'] = "\\\\",
	[(unsigned) '/'] = "\\/",
};

static inline void
esc_state_0(struct tty *tty, unsigned char ch) {
	if (tty->csi_changed) {
		csi_change(tty);
		tty->csi_changed = 0;
	}

	tty->driver.echo(tty, ch);
	/* if (*escape_map[(unsigned) ch] != '\0') */
	/* 	out += sprintf(out, "%s", escape_map[(int) ch]); */
	/* else */
	/* 	*out++ = ch; */

	/* return out - fout; */
}

static inline void
tty_proc_ch(struct tty *tty, char *p) {
	register char ch = *p;

	switch (ch) {
	case '\x18':
	case '\x1a':
		tty->esc_state = 0;
		return;
	case '\x1b':
		tty->esc_state = 1;
		return;
	case '\x9b':
		tty->esc_state = 2;
		return;
	case '\x07': 
	case '\x00':
	case '\x7f':
	case '\v':
	case '\r':
	case '\f':
		return;
	}

	switch (tty->esc_state) {
	case 0:
		esc_state_0(tty, ch);
		return;
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
			return;
		case '?':
			tty->esc_state = 5;
			return;
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
}

void
tty_proc(struct tty *tty, char *input) {
	char *in;

	tty->driver.reinit(tty);
        for (in = input; *in != '\0'; in++)
		tty_proc_ch(tty, in);

	tty->driver.csic_pre(tty);
	tty->driver.flush(tty);
}

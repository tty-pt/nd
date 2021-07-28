#include <stdio.h>
#include "tty.h"

void
tee_csic_fg(struct tty *tty) {
	struct buf_tty_payload *pl
		= (struct buf_tty_payload *) tty->driver.payload;

	int fg = tty->csi.fg;

	if (fg >= 8) {
		pl->out += sprintf(pl->out, "\x1b[1m");
		fg -= 8;
	}

	pl->out += sprintf(pl->out, "\x1b[%dm", fg + 30);
}

void
tee_csic_bg(struct tty *tty) {
	struct buf_tty_payload *pl
		= (struct buf_tty_payload *) tty->driver.payload;

	pl->out += sprintf(pl->out, "\x1b[%dm", tty->csi.bg + 40);
}

void
tee_csic_end(struct tty *tty) {
	tty->end_tag = "\x1b[0m";
}

void
tee_echo(struct tty *tty, char ch) {
	struct buf_tty_payload *pl
		= (struct buf_tty_payload *) tty->driver.payload;

	*pl->out++ = ch;
}

struct tty_driver tee_tty_driver = {
	.csic_pre = &buf_csic_pre,
	.csic_start = &csic_nothing,
	.csic_fg = &tee_csic_fg,
	.csic_bg = &tee_csic_bg,
	.csic_end = &tee_csic_end,
	.csic_nil = &buf_csic_nil,
	.flush = &buf_flush,
	.echo = &tee_echo,
	.init = &buf_init,
	.reinit = &buf_init,
};


#include "tty.h"

void
web_csic_start(struct tty *tty) {
	web_tty_payload *pl = (web_tty_payload *) tty->driver.payload;
	pl->out += sprintf(pl->out, "<span class=\\\"");
}

void
web_csic_fg(struct tty *tty) {
	web_tty_payload *pl = (web_tty_payload *) tty->driver.payload;
	pl->out += sprintf(pl->out, "cf%d", tty->csi.fg);
}

void
web_csic_bg(struct tty *tty) {
	web_tty_payload *pl = (web_tty_payload *) tty->driver.payload;
	pl->out += sprintf(pl->out, " c%d", tty->csi.bg);
}

void
web_csic_end(struct tty *tty) {
	web_tty_payload *pl = (web_tty_payload *) tty->driver.payload;
	pl->out += sprintf(pl->out, "\\\">");
	tty->end_tag = "</span>";
}

struct tty_driver {
	.csic_pre = &buf_csic_pre,
	.csic_start = &web_csic_start,
	.csic_fg = &web_csic_fg,
	.csic_bg = &web_csic_bg,
	.csic_end = &web_csic_end,
	.csic_nil = &buf_csic_nil,
} web_tty_driver;

#ifndef TTY_H
#define TTY_H

#include <stdlib.h>
#include <stdio.h>

struct tty;

typedef void tty_csi_cb(struct tty *);
typedef void tty_echo_cb(struct tty *, char ch);

struct tty_driver {
	tty_csi_cb *csic_pre,
		   *csic_start,
		   *csic_fg,
		   *csic_bg,
		   *csic_end,
		   *csic_nil,
		   *flush,
		   *init,
		   *reinit;

	tty_echo_cb *echo;

	void *payload;
	ssize_t payload_len;
} tee_tty_driver, gl_tty_driver;

struct attr {
        int fg, bg, x;
};

struct tty {
	int flags;
        int esc_state, csi_changed;
        struct attr c_attr, csi;
	struct tty_driver driver;
	char *end_tag;
};

struct buf_tty_payload {
	char outbuf[4 * BUFSIZ];
	char *out;
};

void tty_init(struct tty *tty, struct tty_driver driver);
void tty_proc(struct tty *tty, char *input);

static void csic_nothing(struct tty *tty) {}

static void
buf_csic_pre(struct tty *tty) {
	struct buf_tty_payload *pl =
		(struct buf_tty_payload *) tty->driver.payload;
	pl->out += sprintf(pl->out, "%s", tty->end_tag);
}

static void
buf_csic_nil(struct tty *tty) {
	tty->end_tag = "";
}

static void
buf_flush(struct tty *tty) {
	struct buf_tty_payload *pl =
		(struct buf_tty_payload *) tty->driver.payload;

	*pl->out++ = '\0';
	printf("%s", pl->outbuf);
}

static void
buf_init(struct tty *tty) {
	struct buf_tty_payload *pl = malloc(sizeof(struct buf_tty_payload));
	pl->out = pl->outbuf;
	tty->driver.payload = pl;
}

#endif

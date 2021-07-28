#include "tty.h"
#include "global.h"

#include <GL/glut.h>

struct glcolor {
	float r, g, b;
} glCol[] = {
	{ .23, .25, .23 },
	{ .75, .0, .0 },
	{ .23, .37, .28 },
	{ .63, .59, .35 },
	{ .0, .0, .75 },
	{ .75, .0, .75 },
	{ .25, .36, .37 },
	{ .75, .76, .85 },

	{ .55, .52, .65 },
	{ 1., .0, .0 },
	{ .32, .82, .54 },
	{ 1., 1., .0 },
	{ .45, .61, .74 },
	{ 1., .0, 1. },
	{ .0, 1., 1. },
	{ 1., 1., 1. },
};

enum gl_tty_item_type {
	GLIT_CHAR,
	GLIT_FG,
	GLIT_BG,
};

union gl_tty_item {
	struct glcolor *fg, *bg;
	char ch;
};

struct gl_tty_fifo {
	enum gl_tty_item_type type;
	union gl_tty_item item;
	struct gl_tty_fifo *next;
};

struct gl_tty_payload {
	struct gl_tty_fifo *first, *last;
	unsigned nlines, nchars;
};

void
gl_tty_append(struct tty *tty,
	      enum gl_tty_item_type type,
	      union gl_tty_item item)
{
	struct gl_tty_payload *pl
		= (struct gl_tty_payload *) tty->driver.payload;

	struct gl_tty_fifo *fifo = malloc(sizeof(struct gl_tty_fifo));

	fifo->next = NULL;
	fifo->type = type;
	fifo->item = item;

	if (pl->last != NULL)
		pl->last->next = fifo;
	else
		pl->first = fifo;

	pl->last = fifo;
}

void
gl_csic_fg(struct tty *tty) {
	union gl_tty_item item = { .fg = &glCol[tty->csi.fg] };
	gl_tty_append(tty, GLIT_FG, item);
}

void
gl_csic_bg(struct tty *tty) {
	union gl_tty_item item = { .bg = &glCol[tty->csi.bg] };
	gl_tty_append(tty, GLIT_BG, item);
}

void
gl_echo(struct tty *tty, char ch) {
	union gl_tty_item item = { .ch = ch };

	struct gl_tty_payload *pl
		= (struct gl_tty_payload *) tty->driver.payload;

	gl_tty_append(tty, GLIT_CHAR, item);

	if (ch != '\n') {
		pl->nchars++;
		if (pl->nchars < maxchars)
			return;

		item.ch = '\n';
		gl_tty_append(tty, GLIT_CHAR, item);
	}

	{
		struct gl_tty_fifo *fifo = pl->first;

		int wasnl = 0;

		pl->nlines++;
		pl->nchars = 0;

		if (pl->nlines < maxlines)
			return;

		for (; fifo && !wasnl; )
		{
			wasnl = fifo->type == GLIT_CHAR && fifo->item.ch == '\n';
			pl->first = fifo->next;
			free(fifo);

			fifo = pl->first;

			if (!fifo) {
				pl->last = NULL;
				break;
			}

		}

		if (wasnl)
			pl->nlines--;
	}
}

static void
gl_init(struct tty *tty) {
	struct gl_tty_payload *pl = malloc(sizeof(struct gl_tty_payload));
	pl->first = NULL;
	pl->last = NULL;
	pl->nlines = 0;
	tty->driver.payload = pl;
}

static void
gl_csic_pre(struct tty *tty) {
	union gl_tty_item fgi = { .fg = &glCol[7] };
	union gl_tty_item bgi = { .bg = &glCol[0] };
	gl_tty_append(tty, GLIT_FG, fgi);
	gl_tty_append(tty, GLIT_BG, bgi);
}

struct tty_driver gl_tty_driver = {
	.csic_pre = &gl_csic_pre,
	.csic_start = &csic_nothing,
	.csic_fg = &gl_csic_fg,
	.csic_bg = &gl_csic_bg,
	.csic_end = &csic_nothing,
	.csic_nil = &buf_csic_nil,
	.flush = &csic_nothing,
	.echo = &gl_echo,
	.init = &gl_init,
	.reinit = &csic_nothing,
};

void
gl_tty_render(struct tty *tty) {
	struct gl_tty_payload *pl
		= (struct gl_tty_payload *) tty->driver.payload;

	struct gl_tty_fifo *fifo = pl->first;
	struct glcolor fg = glCol[7], bg = glCol[0];
	char ch;
	float y = ttyMinY;
	float x = -1.;

	glClearColor(bg.r, bg.g, bg.b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	for (fifo = pl->first; fifo; fifo = fifo->next) {
		switch (fifo->type) {
		case GLIT_FG:
			fg = *fifo->item.fg;
			break;
		case GLIT_BG:
			bg = *fifo->item.bg;
			break;
		case GLIT_CHAR:
			ch = fifo->item.ch;
			switch (ch) {
			case '\n':
				y -= ttyLineY;
				x = -1.;
				glRasterPos3f(x, y, 0.);
				break;
			default:
				/* if (x >= 1. - ttyCharX) { */
				/* 	y -= ttyLineY; */
				/* 	x = -1.; */
				/* } */

				glColor3f(bg.r, bg.g, bg.b);
				glBegin(GL_QUADS);
				glVertex3f(x - ttyCharXL, y - ttyLineYB, -0.001);
				glVertex3f(x + ttyCharXR, y - ttyLineYB, -0.001);
				glVertex3f(x + ttyCharXR, y + ttyLineYT, -0.001);
				glVertex3f(x - ttyCharXL, y + ttyLineYT, -0.001);
				glEnd();
				glColor3f(fg.r, fg.g, fg.b);

				glRasterPos3f(x, y, 0.);
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ch);
				x += ttyCharX;

			}
			continue;
		}
	}
}

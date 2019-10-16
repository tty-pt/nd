#include <stdio.h>

#define MEXPORT __attribute__((used)) __attribute__ ((visibility ("default")))
#define WRITEF(...) output_l += snprintf(output, sizeof(output) - output_l, \
                                         __VA_ARGS__);

MEXPORT
const char *argc = "";

struct attr {
        int fg, bg, x;
};
static int esc_state = 0;
static struct attr c_attr, csi;
static char output[2048];
static char *end_tag = "";
int output_l;

static inline void
params_push(int x)
{
	int     fg = c_attr.fg,
		bg = c_attr.bg;

	switch (x) {
	case 0: fg = 7; bg = 0; break;
	case 1: fg += 8; break;
	default: if (x >= 40)
			 bg = x - 40;
		 else if (x >= 30)
			 fg = (fg >= 8 ? 8 : 0) + x - 30;
	}

	csi.fg = fg;
	csi.bg = bg;
	csi.x = x;
}

/* static inline void */
/* end_tag_reset(struct ws *ws) */
/* { */
/* 	WRITEF("%s", ws->end_tag); */
/* 	ws->end_tag = ""; */
/* } */

static inline void
csi_change()
{
	int a = csi.fg != 7, b = csi.bg != 0;
        WRITEF("%s", end_tag);

	if (a || b) {
                WRITEF("<span class=\\\"");
		if (a)
			WRITEF("fg%d ", csi.fg);
		if (b)
			WRITEF("bg%d", csi.bg);

		WRITEF("\\\">");
		end_tag = "</span>";
	} else
		end_tag = "";
}

static inline void
esc_state_any(char *p) {
	register char ch = *p;

	switch (ch) {
	case '\x18':
	case '\x1a':
		esc_state = 0;
		return;
	case '\x1b':
		esc_state = 1;
		return;
	case '\x9b':
		esc_state = 2;
		return;
	case '\x07': 
	case '\x00':
	case '\x7f':
	case '\v':
	case '\r':
	case '\f':
		return;
	}

	switch (esc_state) {
	case 0:
                output[output_l++] = *p;
		break;
	case 1:
		switch (ch) {
		case '[':
			esc_state = 2;
			break;
		case '=':
		case '>':
		case 'H':
			esc_state = 0; /* IGNORED */
		}
		break;
	case 2: // just saw CSI
		switch (ch) {
		case 'K':
		case 'H':
		case 'J':
			esc_state = 0;
			return;
		case '?':
			esc_state = 5;
			return;
		}
		params_push(0);
		esc_state = 3;
	case 3: // saw CSI and parameters
		switch (ch) {
		case 'm':
			if (c_attr.bg != csi.bg
			    || c_attr.fg != csi.fg)
			{
				c_attr.fg = csi.fg;
				c_attr.bg = csi.bg;
				c_attr.x = 0;
				csi.x = 0;
                                csi_change();

			}
			esc_state = 0;
			break;
		case '[':
			esc_state = 4;
			break;
		case ';':
			params_push(0);
			break;
		default:
			if (ch >= '0' && ch <= '9')
				params_push(csi.x * 10 + (ch - '0'));
			else
				esc_state = 0;
		}
		break;

	case 5: params_push(ch);
		esc_state = 6;
		break;
	case 4:
	case 6: esc_state = 0;
		break;

	/* default: CBUG(1); */
	}
}

MEXPORT void
input_parse(char *input, int input_l)
{
        char *p;
	for (p = input;
             output_l < sizeof(output) && *p && p < input + input_l;
             p++)
                esc_state_any(p);
}

int
main(int argc, char *argv[], char *envp[]) {
	csi.fg = c_attr.fg = 7;
	csi.bg = c_attr.bg = 0;
	csi.x = c_attr.x = 0;
        end_tag = "";
        output_l = 0;
        printf("hello world\n");
        getchar();
        return 1;
}

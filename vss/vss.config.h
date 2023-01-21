#define CONFIG_IPH 12px
#define CONFIG_ROUND
#include "vss/vss.h"

ROUND
ALL_FLEX
CALL(TEXT_SIZE, ALL_TEXT_SIZES)
CALL(BACKGROUND_COLOR, ALL_COLORS)
CALL(COLOR, ALL_COLORS)
CALL(BORDER, ALL_COLORS)
CALL(SIZE, ALL_SIZES)
CALL(PADDING, ALL_SIZES)
CALL(AXIS_horizontal, ALL_SIZES)
CALL(AXIS_vertical, ALL_SIZES)
AXIS_0
FULL_SIZE
/* CALL(FLEX_VERTICAL, SS) */
CALL(ROUND_T, ALL_TEXT_SIZES)
CALL(ROUND_EDGE, ALL_SIZES)
ROUND_PADDING( 4, 14)
ROUND_PADDING( 8, 17)
ROUND_PADDING( 8, 20)
ROUND_PADDING( 8, 26)
TEXT_ALIGN(right)
FONT_WEIGHT(bold)
DIRE(right)

#define C #3c403c
#define CF #c1c3da
#define CM VAL(COLOR, 13)
#define CBM rgba(0.05, 0.05, 0.05, 0.1)

.oa { overflow: auto; }
.dn { display: none; }
.cf { background: C; }
.cb { color: CF; }
body { caret-color: CM; margin: 0; }
h1,h2,h3,h4,h5,h6 { color: VAL(COLOR, 15); }
img { color: CF; };
.modal a { color: CM; }
input { border: solid thin VAL(COLOR, 0); color: CF; }
input:focus {
        border: solid thin CM;
        outline: CM;
}
.abs { position: absolute; }
.rel { position: relative; }
a { color: CF; }
html { height: 100%; }
.sr2 {
	width: 512px;
	height: 256px;
}
.sr1 {
	width: 256px;
	height: 256px;
}

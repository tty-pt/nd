#define CONFIG_IPH 12px
#define CONFIG_ROUND
#include "vss/vss.h"

CALL(TEXT_SIZE, TS)
CALL(BG_COLOR, CS)
CALL(COLOR, CS)
/* CALL(BO_COLOR, CS) */
CALL(SIZE, SS)
CALL(PADDING, SS)
CALL(HORIZONTAL, SS)
CALL(VERTICAL, SS)
/* CALL(FLEX_VERTICAL, SS) */
CALL(ROUND_T, TS)
CALL(ROUND_EDGE, SS)
ROUND_PADDING( xs, )
ROUND_PADDING( s, m)

#define C #3c403c
#define CF #c1c3da
#define CM C_13
#define CBM rgba(0.05, 0.05, 0.05, 0.1)

.cf { background: C; }
.cb { color: CF; }
body { caret-color: CM; }
h1,h2,h3,h4,h5,h6 { color: C_15; }
img { color: CF; };
.modal a { color: CM; }
input { border: solid thin C_0; color: CF; }
input:focus {
        border: solid thin CM;
        outline: CM;
}
a { color: CF; }

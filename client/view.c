#include "view.h"
#include "compiler.h"

static char view_buf[128];
/* static view_t view; */

export char *
view_input() {
	return view_buf;
}

export int
view_build() {
	return 0;
}

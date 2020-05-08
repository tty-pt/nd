#include <metal.h>
#include "mcp.h"

/* exporting pointers stopped working
 * on transition to asm.js */
static char in_buf[4096];
static char out_buf[8282];
char *out_p;

export
void
metal_init() {
	out_p = out_buf;
	mcp_init();
}

export
char *
metal_write() {
	mcp_proc(in_buf);
	return out_buf;
}

export
char *
metal_input() {
        return in_buf;
}

int
main(int argc, char *argv[], char *envp[])
{
	return 0;
}

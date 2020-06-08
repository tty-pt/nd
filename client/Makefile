include /usr/local/mk/wasm.mk
# LDLIBS = ${PROG_LDLIBS}
obj-y := view.o tty.o mcp.o main.o
CFLAGS += -DCLIENT
exe := client.wasm
all: client.js

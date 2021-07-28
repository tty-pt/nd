include ${METAL_PATH}/mk/wasm.mk
# LDLIBS = ${PROG_LDLIBS}
# LDFLAGS = --allow-undefined
obj-y := view.o tty.o mcp.o main.o
CFLAGS += -DCLIENT
exe := client.wasm
all: client.js

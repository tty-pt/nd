ARCH=wasm32
TARGET=${ARCH}-unknown-unknown
sysroot:=${PWD}/..
LLVM_ROOT:=/usr/local
CC:=${LLVM_ROOT}/bin/clang
CFLAGS += --sysroot ${sysroot} --target=${TARGET} -I${sysroot}/include
LDFLAGS += -L${sysroot}/lib

WLDFLAGS := --allow-undefined-file=${sysroot}/lib/wasm.syms --export-dynamic
LDFLAGS += ${WLDFLAGS}
RTLIB=clang_rt.builtins-wasm32
ldlibs-y := ${sysroot}/lib/crt1.o -lc -l${RTLIB}
LD := ${LLVM_ROOT}/bin/wasm-ld
LINK.c := ${LD} ${LDFLAGS}

obj-y := wasm-cli.o
exe := wasm-cli

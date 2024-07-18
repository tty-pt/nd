#!/bin/make

include .config

all: node_modules/ nd htdocs/

htdocs/: index.html index.js
	pnpm build

nd:
	${MAKE} -C src ${MFLAGS} all

deps:
	${MAKE} -C src ${MFLAGS} deps

node_modules/:
	pnpm i

clean:
	${Q}${MAKE} -C src ${MFLAGS} clean
	rm -rf htdocs/ node_modules/

run: all
	./nd

FORCE:

.PHONY: backup run clean nd deps

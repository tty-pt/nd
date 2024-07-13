#!/bin/make -f

include .config

subdirs := src/

all: ${subdirs} htdocs/

htdocs/: node_modules/ index.html index.js
	pnpm build

node_modules/:
	pnpm i

$(subdirs): FORCE
	${MAKE} -C $@ ${MFLAGS} all

subdirs-clean != test -z "${subdirs}" || echo ${subdirs:%=%-clean}

$(subdirs-clean): FORCE
	${Q}${MAKE} -C ${@:%-clean=%} ${MFLAGS} clean

clean: ${subdirs-clean}
	rm -rf htdocs/ node_modules/

DESTDIR ?= /
PREFIX ?= ${DESTDIR}usr

install:
	install -d ${DESTDIR}var/nd
	install -m 644 game/std.db.ok ${DESTDIR}var/nd/std.db.ok

run: all
	./nd

FORCE:

.PHONY: install backup run clean

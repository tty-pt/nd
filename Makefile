#!/bin/make

DESTDIR ?= ../../

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
	rm -rf htdocs/ node_modules/ ${DESTDIR}var/nd

DESTDIR ?= /
PREFIX ?= ${DESTDIR}usr

install: nd htdocs/ ${DESTDIR}var/nd/std.db.ok \
	${DESTDIR}etc/group ${DESTDIR}etc/passwd \
	${DESTDIR}etc/vim/vimrc.local

$(DESTDIR)var/nd/std.db.ok: game/std.db.ok ${DESTDIR}var/nd
	install -m 644 game/std.db.ok $@

$(DESTDIR)var/nd:
	install -d ${DESTDIR}var/nd

$(DESTDIR)etc/vim:
	install -d ${DESTDIR}etc/vim

$(DESTDIR)etc/group:
	echo "root:X:0:" > $@

$(DESTDIR)etc/passwd:
	echo "root:X:0:0:root:/root:/bin/bash" > $@

$(DESTDIR)etc/vim/vimrc.local: ${DESTDIR}etc/vim
	echo "colorscheme pablo" > $@

run: all
	./nd

FORCE:

.PHONY: install backup run clean nd deps

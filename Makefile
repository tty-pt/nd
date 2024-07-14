#!/bin/make -f

include .config

all: nd htdocs/

htdocs/: node_modules/ index.html index.js
	pnpm build

node_modules/:
	pnpm i

nd:
	${MAKE} -C src ${MFLAGS} all

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

$(DESTDIR)etc/group:
	echo "root:X:0:" > $@

$(DESTDIR)etc/passwd:
	echo "root:X:0:0:root:/root:/bin/bash" > $@

$(DESTDIR)etc/vim/vimrc.local:
	echo "colorscheme pablo" > $@

run: all
	./nd

FORCE:

.PHONY: install backup run clean

#!/bin/make -f

include .config

subdirs := src/

js-src != find js -type f
js-$(production) := ./build/static/js/main.js

all: ${subdirs} index.html ${js-y}

$(subdirs): FORCE
	${MAKE} -C $@ ${MFLAGS} all

subdirs-clean != test -z "${subdirs}" || echo ${subdirs:%=%-clean}

$(subdirs-clean): FORCE
	${Q}${MAKE} -C ${@:%-clean=%} ${MFLAGS} clean

clean: ${subdirs-clean}

art-y != find art -type f | tr ' ' '\\ '
art-y += nd256.png

js-$(production): ${js-src}
	npm run build

./node_modules/:
	npm install

backup-date != date +%s
backup := neverdark-${backup-date}.tar.gz
backup: ${backup}
$(backup):
	tar czf $@ game/geo.db game/data/std-db.db

DESTDIR ?= /
PREFIX ?= ${DESTDIR}usr/local

datadir := ${PREFIX}/share/neverdark
artdir := ${datadir}/art

$(artdir):
	mkdir -p $@
art-install := ${art-y:%=${datadir}/%}
$(art-install):
	install -m 644 ${@:${datadir}/%=%} $@

install: ${artdir} ${art-install}
	${INSTALL} -m 644 index.html styles.css ${datadir}/
	${INSTALL} nd ${PREFIX}/bin/neverdark

run: all
	./nd

FORCE:

.PHONY: install backup run clean

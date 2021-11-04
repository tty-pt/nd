#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ game/data/

include scripts/Makefile.common

GCC_JS := ${CC} -E -P -nostdinc -undef -x c
.SUFFIXES: .js .hjs

.hjs.js:
	${GCC_JS} -o $@ $<

all: index.html main.js vim.css

main.js: main.hjs canvas.hjs

inline-js := main.js
index.html: pre-index.html ${inline-js}
	${scripts}/html_tool.sh pre-index.html ${inline-js} > $@

vim.css: vss/

subdirs-cleaner := ${subdirs:%=%-cleaner}
$(subdirs-cleaner):
	${MAKE} -C ${@:%-cleaner=%} cleaner
cleaner: ${subdirs-cleaner}
	rm config.status config.cache config.log

web: src
	${srcdir}/src/ws-server

game/data/: src/ vss/

backup-date != date +%s
backup := neverdark-${backup-date}.tar.gz
backup: ${backup}
$(backup):
	tar czf $@ game/geo.db game/data/std-db.db

DESTDIR ?= /
PREFIX ?= ${DESTDIR}usr/local
INSTALL_SCRIPT ?= install
INSTALL_DATA ?= install -m 644

art-y := tilemap.png
art-y := ${art-y:%=art/%}
datadir := ${PREFIX}/share/neverdark
artdir := ${datadir}/art

$(artdir):
	mkdir -p $@
art-install := ${art-y:%=${datadir}/%}
$(art-install):
	install -m 644 ${@:${datadir}/%=%} $@

install: ${artdir} ${art-install}
	${INSTALL_SCRIPT} ${srcdir}/src/fbmuck ${PREFIX}/bin/neverdark
	${INSTALL_DATA} index.html ${datadir}/
	${INSTALL_DATA} vim.css ${datadir}/
	${INSTALL_DATA} styles.css ${datadir}/
	${INSTALL_SCRIPT} ${srcdir}/src/fbmuck ${PREFIX}/bin/neverdark

run: all
	./src/fbmuck -C ./game

.PHONY: cleaner ${subdirs-cleaner} web backup run

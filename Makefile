#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ game/data/

include scripts/Makefile.common

GCC_JS := ${CC} -E -P -nostdinc -undef -x c

all: index.html main.js vim.css

js-src != find js -type f
art-y != find art -type f
lib-y != find lib -type f

pre.js: ${js-src}
	${GCC_JS} -o $@ ./js/main.js

./node_modules/:
	npm install

main.js: ./node_modules/ pre.js
	./node_modules/.bin/babel pre.js > $@

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

datadir := ${PREFIX}/share/neverdark
artdir := ${datadir}/art
libdir := ${datadir}/lib

$(artdir):
	mkdir -p $@
art-install := ${art-y:%=${datadir}/%}
$(art-install):
	install -m 644 ${@:${datadir}/%=%} $@

$(libdir):
	mkdir -p $@
lib-install := ${lib-y:%=${datadir}/%}
$(lib-install):
	install -m 644 ${@:${datadir}/%=%} $@

install: ${artdir} ${art-install} ${libdir} ${lib-install}
	${INSTALL_SCRIPT} ${srcdir}/src/fbmuck ${PREFIX}/bin/neverdark
	${INSTALL_DATA} index.html ${datadir}/
	${INSTALL_DATA} vim.css ${datadir}/
	${INSTALL_DATA} styles.css ${datadir}/
	${INSTALL_SCRIPT} ${srcdir}/src/fbmuck ${PREFIX}/bin/neverdark

run: all
	./src/fbmuck -C ./game

.PHONY: cleaner ${subdirs-cleaner} web backup run

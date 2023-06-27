include .config
#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ game/data/

include scripts/Makefile.common

GCC_JS := ${CC} -E -P -nostdinc -undef -x c

js-src != find js -type f
js-$(production) := ./build/static/js/main.js

all: index.html vim.css ${js-y}

art-y != find art -type f | tr ' ' '\\ '
art-y += nd256.png

js-$(production): ${js-src}
	npm run build

./node_modules/:
	npm install

index.html: pre-index.html
	${scripts}/html_tool.sh pre-index.html > $@

vim.css: vss/

subdirs-cleaner := ${subdirs:%=%-cleaner}
$(subdirs-cleaner):
	${MAKE} -C ${@:%-cleaner=%} cleaner
cleaner: ${subdirs-cleaner}
	rm config.status config.cache config.log

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

.PHONY: cleaner ${subdirs-cleaner} backup run

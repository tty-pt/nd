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

# art-y != find art -type f | sed 's/ /\\ /g' | sed 's/\(.*\)/"\1"/'
# art-y += nd256.png

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
PREFIX ?= ${DESTDIR}usr

# artdir := ${DESTDIR}items/nd/art

# $(artdir):
# 	mkdir -p $@
# art-install := ${art-y:"%="${artdir}/%}
# $(art-install):
	# artdir
	# install -m 644 ${@:${artdir}/%=%} $@

install: ${artdir} ${art-install}
	# install -m 644 index.html styles.css ${datadir}/
	# install nd ${PREFIX}/bin/nd
	install -d ${DESTDIR}var/nd
	install -m 644 game/std.db.ok ${DESTDIR}var/nd/std.db.ok

run: all
	./nd

FORCE:

.PHONY: install backup run clean

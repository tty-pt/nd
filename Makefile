#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ pre/ game/data/ wasm/src/

include scripts/Makefile.common

subdirs-install := ${subdirs:%=%-install}
$(subdirs-install):
	${MAKE} -C ${@:%-install=%} install
install: ${subdirs-install}

subdirs-cleaner := ${subdirs:%=%-cleaner}
$(subdirs-cleaner):
	${MAKE} -C ${@:%-cleaner=%} cleaner
cleaner: ${subdirs-cleaner}
	rm config.status config.cache config.log

web: src
	${srcdir}/src/ws-server

game/data/: ${subdirs}

.PHONY: cleaner ${subdirs-cleaner} web

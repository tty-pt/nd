#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ pre/ game/data/ wasm/cli

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

wasm/cli: wasm

wasm: metal.tar.gz
	cd wasm && tar xzf ../metal.tar.gz

metal.tar.gz:
	curl -O https://github.com/quirinpa/metal/raw/master/metal.tar.gz

.PHONY: cleaner ${subdirs-cleaner} web

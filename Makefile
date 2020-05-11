#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ game/data/ wasm/cli/

include scripts/Makefile.common

all: index.html main.js

.SUFFIXES: .js .hjs

js-flags-y := -E -P -nostdinc -undef -x c

.hjs.js:
	gcc ${js-flags-y} -o $@ $<

index.html: pre-index.html vim.css
	${scripts}/html_tool.sh pre-index.html ${inline-js} > $@

vim.css: FORCE
	${MAKE} -C vss/

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

wasm/cli/: wasm/lib/

mt := git
metal-dir-git := ${HOME}/metal/
METAL-DIR := ${metal-dir-${mt}}
metal-tar := $(METAL-DIR)metal.tar.gz
CONFIG-MT-NEED := y
mt-need-$(CONFIG-MT-NEED) := ${metal-tar}
# mt-phony-git := ${metal-tar}

metal-tar: $(metal-tar)

$(metal-tar):
	${MAKE} -C ${METAL-DIR} tar

wasm/lib/: ${mt-need-y}
	cd wasm && tar xzf ${metal-tar}

metal.tar.gz:
	curl -LO https://github.com/quirinpa/metal/raw/master/metal.tar.gz

.PHONY: cleaner ${subdirs-cleaner} web \
	${mt-phony-${mt}} metal-tar

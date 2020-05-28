#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ game/data/ wasm/cli/

include scripts/Makefile.common
-include wasm/hjs.mk

all: index.html main.js vim.css

main.js: wasm/cli/

inline-js := main.js
index.html: pre-index.html ${inline-js}
	${scripts}/html_tool.sh pre-index.html ${inline-js} > $@

vim.css: vss/

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
	${srcdir}/src/ws-server &

game/data/: ${subdirs}

wasm/cli/: wasm/lib/

mt := pkg
metal-dir := ${HOME}/metal
metal-tar := ${metal-dir}/metal.tar.gz
mt-need-y := ${metal-tar}
# mt-phony-git := ${metal-tar}

metal-tar: ${metal-tar}

$(metal-tar):
	${MAKE} -C ${metal-dir} tar

wasm/lib/ wasm/metal.hjs: ${mt-need-y}
	cd wasm && tar xzf ${metal-tar}

metal.tar.gz:
	curl -LO https://github.com/quirinpa/metal/raw/master/metal.tar.gz

.PHONY: cleaner ${subdirs-cleaner} web \
	${mt-phony-${mt}} metal-tar

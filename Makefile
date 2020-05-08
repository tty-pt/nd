#!/bin/make -f

srcdir := ${PWD}
subdirs := src/ vss/ pre/ game/data/ wasm/cli/

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

wasm/cli/: wasm/lib/

mt := git
metal-dir-git := ${HOME}/metal/
METAL-DIR := ${metal-dir-${mt}}
metal-tar := $(METAL-DIR)metal.tar.gz
# mt-phony-git := ${metal-tar}

metal-tar: $(metal-tar)

$(metal-tar):
	${MAKE} -C ${METAL-DIR} tar

wasm/lib/: ${metal-tar}
	cd wasm && tar xzf ${metal-tar}

metal.tar.gz:
	curl -LO https://github.com/quirinpa/metal/raw/master/metal.tar.gz

.PHONY: cleaner ${subdirs-cleaner} web \
	${mt-phony-${mt}} metal-tar

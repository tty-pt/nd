#!/bin/make

DESTDIR ?= /var/www
PREFIX ?= /usr
SHARE := ${DESTDIR}${PREFIX}/share
uapi != ls include/uapi | while read file; do echo ${DESTDIR}${PREFIX}/include/nd/$$file; done

include .config

all: node_modules/ nd htdocs/index.html

htdocs/index.html htdocs/index.js: index.html index.js
	pnpm build

nd:
	@${MAKE} -C src ${MFLAGS} all

libnd.a: nd

deps:
	${MAKE} -C src ${MFLAGS} deps

node_modules/:
	pnpm i

clean:
	${Q}${MAKE} -C src ${MFLAGS} clean
	rm -rf htdocs/ node_modules/

run: all
	./nd

FORCE:

install: src/libnd.a ${uapi}
	install -d ${SHARE}
	install -d ${SHARE}/mk/
	install -d ${SHARE}/nd
	install -d ${SHARE}/nd/lib
	install -d ${SHARE}/nd/include
	install -m 0644 module.mk ${SHARE}/mk/module.mk
	install -m 0755 ndcc ${DESTDIR}${PREFIX}/bin/
	install -m 0644 src/libnd.a ${DESTDIR}${PREFIX}/lib/
	install -m 0644 module.ld ${SHARE}/nd/

$(uapi): ${uapi:${DESTDIR}${PREFIX}/include/nd/%=include/uapi/%}
	@install -d ${DESTDIR}${PREFIX}/include
	install -m 0644 ${@:${DESTDIR}${PREFIX}/include/nd/%=include/uapi/%} $@

.PHONY: backup run clean nd deps install

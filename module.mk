prefix := ${DESTDIR}/usr /usr/local /usr
CFLAGS := ${prefix:%=-I%/include}
LDFLAGS := -lnd ${prefix:%=-L%/lib}
MODDIR := ${DESTDIR}${PREFIX}/share/nd
pwd != pwd
bname != basename ${pwd}
PREFIX ?= /usr
inc != ls ${DESTDIR}${PREFIX}/include/nd
inc := ${inc:%=${DESTDIR}/usr/include/nd/%}
uapi != test -d include/uapi && ls include/uapi/ || echo -n ""
uapi := ${uapi:%=${DESTDIR}${PREFIX}/include/nd/%}
CC := ndcc
.SUFFIXES: .so .d .c

.c.so: ${DESTDIR}/usr/lib/libnd.a ${inc}
	${CC} -name ${bname} -shared -g -o $@ -fPIC \
		-Wno-initializer-overrides \
		${@:%.so=%.c} ${CFLAGS} ${LDFLAGS}

.c.d:
	@echo MKDEP $@
	@${CC} -MM -o $@ ${CFLAGS} $<

install: main.so install-head
	@install -m 755 main.so ${MODDIR}/lib/${bname}.so

$(uapi): ${uapi:${DESTDIR}${PREFIX}/include/nd/%=include/uapi/%}
	@test -z "$@" || install -m 644 \
		${@:${DESTDIR}${PREFIX}/include/nd/%=include/uapi/%} \
		${DESTDIR}${PREFIX}/include/nd/

install-head: ${uapi}

.PHONY: all install install-head

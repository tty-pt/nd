prefix := ${DESTDIR}/usr /usr/local /usr
CFLAGS := ${prefix:%=-I%/include}
LDFLAGS := -lnd ${prefix:%=-L%/lib}
MODDIR := ${DESTDIR}/usr/share/nd
pwd != pwd
bname != basename ${pwd}
inc != ls ${DESTDIR}/usr/include/nd
inc := ${inc:%=${DESTDIR}/usr/include/nd/%}
uapi != test -d include/uapi && ls include/uapi/ || echo -n ""
CC := ndcc
.SUFFIXES: .so .d .c

.c.so: ${DESTDIR}/usr/lib/libnd.a ${inc}
	${CC} -name ${bname} -shared -g -o $@ -fPIC ${@:%.so=%.c} ${CFLAGS} ${LDFLAGS}

.c.d:
	@echo MKDEP $@
	@${CC} -MM -o $@ ${CFLAGS} $<

install: main.so ${uapi:%=${MODDIR}/include/%}
	@install -m 755 main.so ${MODDIR}/lib/${bname}.so

$(uapi:%=$(MODDIR)/include/%): ${uapi:%=include/uapi/%}
	@test -z "$@" || install -m 644 ${@:${MODDIR}/include/%=include/uapi/%} ${MODDIR}/include/

.PHONY: all install

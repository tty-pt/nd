prefix := /var/www/usr /usr/local /usr
CFLAGS := ${prefix:%=-I%/include}
LDFLAGS := -lnd ${prefix:%=-L%/lib}
MODDIR := /var/www/usr/share/nd
pwd != pwd
bname != basename ${pwd}
inc != ls /var/www/usr/include/nd
inc := ${inc:%=/var/www/usr/include/nd/%}
.SUFFIXES: .so .d .c

.c.so: /var/www/usr/lib/libnd.a ${inc}
	${CC} -shared -g -o $@ -fPIC ${@:%.so=%.c} ${CFLAGS} ${LDFLAGS}

.c.d:
	@echo MKDEP $@
	@${CC} -MM -o $@ ${CFLAGS} -I../../include $<

install: main.so
	@install -d ${MODDIR}
	@install -m 755 main.so ${MODDIR}/${bname}.so

all: main.so

.PHONY: all install

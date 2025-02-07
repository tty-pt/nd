prefix := ../.. /var/www/usr /usr/local
CFLAGS := ${prefix:%=-I%/include}
LDFLAGS := -lnd -L../../src ${prefix:%=-L%/lib}
.SUFFIXES: .so .d .c

.c.so:
	${CC} -shared -g -o $@ -fPIC ${@:%.so=%.c} ${CFLAGS} ${LDFLAGS}

.c.d:
	@echo MKDEP $@
	@${CC} -MM -o $@ ${CFLAGS} -I../../include $<

all: main.so

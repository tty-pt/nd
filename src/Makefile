srcdir := ..
RELDIR := ${srcdir}/src

prefix ?= /usr/local
INSTALL_BINDIR := ${prefix}/bin

uname != uname
config	!= cat ${srcdir}/.config | sed -n '/^.*=y$$/p' | sed 's/^\(.*\)=y$$/-D\1/' | tr '\n' ' '
cflags-Darwin	+= -I/usr/local/opt/berkeley-db@4/include
node_modules != npm root
npm-lib := @tty-pt/qdb @tty-pt/ndc
prefix := ${srcdir} \
	  /usr/local \
	  ${npm-lib:%=${node_modules}/%}
modpath := /usr/share/nd
CFLAGS := -g -O0 -Wall -Wpedantic -Wextra \
	  -Wno-initializer-overrides ${cflags-${uname}} \
	  ${config} ${prefix:%=-I%/include} \
	  -D_GNU_SOURCE -std=c99 -Wno-gnu-designator
ldlibs-Linux := -lbsd -lrt
LDLIBS := -lxxhash -lpcre -lm -lc -lqdb -ldb -lndc -lcrypto -lssl ${ldlibs-${uname}}
ldflags-Darwin += -L/usr/local/opt/berkeley-db@4/lib

LDFLAGS := ${ldflags-${uname}} ${prefix:%=-L%/lib} \
	   ${prefix:%=-Wl,-rpath,%/lib} \
	   ${modpath:%=-Wl,-rpath,%}

exe := ../nd
obj-y := object.o entity.o spacetime.o mcp.o map.o \
	noise.o view.o item.o spell.o drink.o \
	look.o match.o speech.o wiz.o interface.o

src = ${obj-y:%.o=%.c}
CC ?= clang
LD := ${CC}

all: ${exe} libnd.a ../compile_commands.json

$(exe): ${obj-y}
	@echo LD $@
	${LINK.c} ${LDFLAGS} -o $@ ${obj-y} ${LDLIBS} -lutil

libnd.so: libnd.c
	${CC} -shared -g -o $@ -fPIC ${CFLAGS} libnd.c

libnd.o: libnd.c
	${CC} -g -c -o $@ -fPIC ${CFLAGS} libnd.c

libnd.a: libnd.o
	ar rcs libnd.a libnd.o

.c.o:
	@echo CC $@
	${COMPILE.c} -MJ ${@:%.o=%.json} -o $@ -fPIC ${<:%=${RELDIR}/%}

../compile_commands.json: ${obj-y}
	@echo '[' > $@
	@cat *.json >> $@
	@echo ']' >> $@

CTAGS ?= ctags

tags: ${src}
	${CTAGS} --c-types=-m ${src} ../include/*.h

.SUFFIXES: .d .o .c
clean:
	@echo rm ${RELDIR}/{ ${exe} ${obj-y} }
	@rm ${exe} ${obj-y} >/dev/null 2>&1 || true

deps := ${src:%.c=%.d}
deps: ${deps}

.c.d:
	@echo MKDEP $@
	@${CC} -MM -o $@ ${CFLAGS} -I../include $<

.PHONY: all clean deps

-include $(deps)

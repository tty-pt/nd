srcdir := ..
RELDIR := ${srcdir}/src

prefix ?= /usr/local
INSTALL_BINDIR := ${prefix}/bin

uname != uname
config	!= cat ${srcdir}/.config | sed -n '/^.*=y$$/p' | sed 's/^\(.*\)=y$$/-D\1/' | tr '\n' ' '
cflags-Darwin	+= -I/usr/local/opt/berkeley-db@4/include
node_modules != npm root
npm-lib := @tty-pt/qhash @tty-pt/ndc
CFLAGS := -g -O0 -Wall -Wpedantic -Wextra -Wno-initializer-overrides ${cflags-${uname}} ${config} \
	-I/usr/local/include -I${srcdir}/include \
	${npm-lib:%=-I${node_modules}/%/include} \
	-D_GNU_SOURCE -std=c99 \
	-Wno-gnu-designator
ldlibs-Linux := -lbsd -lrt
LDLIBS := -lxxhash -lpcre -lm -lc -lqhash -ldb -lndc -lcrypto -lssl ${ldlibs-${uname}}
ldflags-Darwin += -L/usr/local/opt/berkeley-db@4/lib
LDFLAGS	:= ${ldflags-${uname}} -L/usr/local/lib -Wl,-rpath,/usr/local/lib ${npm-lib:%=-L${node_modules}/%} ${npm-lib:%=-Wl,-rpath,${node_modules}/%}

exe := ../nd
obj-y := skel.o object.o entity.o spacetime.o mcp.o map.o \
	plant.o biome.o noise.o view.o item.o spell.o drink.o \
	mob.o kill.o shop.o create.o look.o match.o \
	move.o utils.o rob.o set.o speech.o wiz.o \
	interface.o wts.o

src = ${obj-y:%.o=%.c}
LD := ${CC}

all: ${exe} libnd.a

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
	@${COMPILE.c} -o $@ -fPIC ${<:%=${RELDIR}/%}

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

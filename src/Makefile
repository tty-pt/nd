srcdir ?= ..
reldir-OpenBSD := ${PWD:${srcdir}/%=%}
reldir-Darwin := ${reldir-OpenBSD}
reldir-Linux := ${reldir-OpenBSD}/src
RELDIR=${reldir-${uname}}

prefix ?= /usr/local
INSTALL_BINDIR := ${prefix}/bin

uname != uname
config	!= cat ${srcdir}/.config | sed -n '/^.*=y$$/p' | sed 's/^\(.*\)=y$$/-D\1/' | tr '\n' ' '
cflags-Darwin	+= -I/usr/local/opt/berkeley-db@4/include
node_modules != realpath ${srcdir}/node_modules
npm-lib := @tty-pt/qhash @tty-pt/ndc
npm-lib := ${npm-lib:%=${node_modules}/%}
CFLAGS := -g -O0 -Wall -Wno-initializer-overrides ${cflags-${uname}} ${config} \
	${npm-lib:%=-I%/include} -I/usr/local/include -I${srcdir}/include \
	-D_GNU_SOURCE -std=c99 \
	-Wno-gnu-zero-variadic-macro-arguments -Wno-gnu-designator
ldlibs-Linux := -lbsd
LDLIBS := -lxxhash -lpcre -lm -lc -lqhash -ldb -lndc ${ldlibs-${uname}}
ldflags-Darwin += -L/usr/local/opt/berkeley-db@4/lib
LDFLAGS	:= ${ldflags-${uname}} ${npm-lib:%=-L%} ${npm-lib:%=-Wl,-rpath,%}

exe := ../nd
obj-y := object.o entity.o spacetime.o mcp.o map.o \
	plant.o biome.o noise.o view.o item.o spell.o drink.o \
	mob.o kill.o shop.o create.o look.o match.o \
	move.o player.o utils.o rob.o set.o speech.o wiz.o \
	copyright.o interface.o

src = ${obj-y:%.o=%.c}
LD := gcc

all: ${exe}

$(exe): ${obj-y}
	@echo LD $@
	@${LINK.c} -o $@ $^ ${LDLIBS} -lrt -lutil

.c.o:
	@echo CC $@
	@${COMPILE.c} -o $@ ${<:%=${RELDIR}/%}

CTAGS ?= ctags

tags: ${src}
	${CTAGS} --c-types=-m ${src} ../include/*.h

.c.d:
	@echo MKDEP $@
	@gcc -MM -o $@ ${CFLAGS} -I../include $<

clean:
	@echo rm ${RELDIR}/{ ${exe} ${obj-y} }
	@rm ${exe} ${obj-y} >/dev/null 2>&1 || true

.PHONY: all clean
.SUFFIXES: .d .o .c

-include ${src:%.c=%.d}

srcdir= ..

prefix?=/usr/local
INCLUDE= ${srcdir}/include
INSTALL_BINDIR=${prefix}/bin
INSTALL_HELPDIR=${prefix}/share/fbmuck
CONFIG_SSL=y

libs-y := -lpcre -lm -lxxhash -ldb
libs-$(CONFIG_SSL) += -lssl -lcrypto
LDFLAGS = -L/usr/local/lib -L/usr/lib ${libs-y}

CFLAGS := -g -Wall -Wno-initializer-overrides \
	-I${INCLUDE} -I/usr/include -I/usr/local/include

# CC = gcc
# CFLAGS += -O1 -fdump-tree-optimized

obj-y := crt_malloc.o geometry.o noise.o geography.o item.o kill.o shop.o \
        array.o boolexp.o compile.o create.o db.o db_header.o debugger.o \
	disassem.o diskprop.o edit.o events.o game.o hashtab.o help.o inst.o \
	interp.o log.o look.o match.o mcp.o mcpgui.o mcppkgs.o mfuns2.o \
	mfuns.o move.o msgparse.o mufevent.o oldcompress.o p_array.o \
	p_connects.o p_db.o p_error.o p_float.o player.o p_math.o p_mcp.o \
	p_misc.o p_props.o p_regex.o predicates.o propdirs.o property.o \
	props.o p_stack.o p_strings.o random.o rob.o sanity.o set.o \
	signal.o smatch.o snprintf.o speech.o strftime.o stringutil.o \
	timequeue.o timestamp.o unparse.o utils.o wiz.o \
	version.o interface.o copyright.o

TARGETS := fbmuck
src := ChangeLog COPYING ${obj-y:%.o=%.c} compress.c ${TARGETS:%=%.c}

HELPFILES := man.txt help.txt mpihelp.txt

all: ${TARGETS}

version.o: version.h version.c

version.h:
	{ \
		DATE=`date` ; git=${srcdir}/.git ; \
		HASH=`git branch -v | head -n1 | awk '{ print $$3 }'` ; \
		printf "#define HASH \"$$HASH\"\n#define DATE \"$$DATE\"\n" ; \
	} > $@

fbmuck: ${obj-y}
	${LINK.c} -o $@ ${obj-y}

tags: ${src}
	${CTAGS} --c-types=-m ${src} ../include/*.h

.depend: ${obj-y:%.o=%.c}
	mkdep ${CFLAGS} -I../include ${obj-y:%.o=%.c}

clean:
	-rm *.o core ${TARGETS} || true

cleaner: clean
	-rm config.status config.cache config.log ${INCLUDE}/autoconf.h ${TARGETS} prochelp

install: all
	${INSTALL} -d ${INSTALL_BINDIR}
	${INSTALL} ${TARGETS} ${INSTALL_BINDIR}
	${INSTALL} -d ${INSTALL_HELPDIR}
	cd ../game/data && ${INSTALL} -m 644 ${HELPFILES} ${INSTALL_HELPDIR}
	${INSTALL} ../game/restart ${INSTALL_HELPDIR}/restart-script

.PHONY: version.h clean cleaner install

# FIXME: Check base and minimal dbs into CVS as unpacked file trees under dbs/

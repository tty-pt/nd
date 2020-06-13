# srcdir= ..

prefix?=/usr/local
INCLUDE= ${srcdir}/include
INSTALL_BINDIR=${prefix}/bin
INSTALL_HELPDIR=${prefix}/share/fbmuck
CONFIG_SSL=y

ldlibs-y := -lpcre -lm -lxxhash -ldb
ldlibs-$(CONFIG_SSL) += -lssl -lcrypto
cflags-y := -g -Wall -Wno-initializer-overrides

exe := fbmuck prochelp ws-server
src = ChangeLog COPYING ${real-obj-y:%.o=%.c}

prochelp-obj-y := prochelp.o
ws-server-obj-y := ws-server.o
fbmuck-obj-y := geometry.o plant.o biome.o noise.o search.o \
	view.o item.o spell.o drink.o mob.o kill.o geography.o \
	shop.o
fbmuck-obj-y += array.o boolexp.o compile.o create.o db.o db_header.o debugger.o \
	disassem.o edit.o events.o game.o hashtab.o help.o inst.o \
	interp.o look.o match.o mcp.o mcppkgs.o mfuns2.o \
	mfuns.o move.o msgparse.o mufevent.o p_array.o \
	p_connects.o p_db.o p_error.o p_float.o player.o p_math.o \
	p_props.o p_regex.o predicates.o propdirs.o property.o \
	props.o p_stack.o p_strings.o random.o rob.o sanity.o set.o \
	signal.o smatch.o snprintf.o speech.o stringutil.o \
	timequeue.o timestamp.o unparse.o utils.o wiz.o \
	version.o copyright.o interface.o web.o

exe-obj-y := ${fbmuck-obj-y} ${prochelp-obj-y} ${ws-server-obj-y}

HELPFILES := man.txt help.txt mpihelp.txt

version.o: version.h version.c

version.h:
	{ \
		DATE=`date` ; git=${srcdir}/.git ; \
		HASH=`git branch -v | head -n1 | awk '{ print $$3 }'` ; \
		printf "#define HASH \"$$HASH\"\n#define DATE \"$$DATE\"\n" ; \
	} > $@

tags: ${src}
	${CTAGS} --c-types=-m ${src} ../include/*.h

.depend: ${obj-y:%.o=%.c}
	mkdep ${CFLAGS} -I../include ${obj-y:%.o=%.c}

cleaner: clean
	-rm config.status config.cache config.log ${INCLUDE}/autoconf.h ${exe} prochelp

install: all
	${INSTALL} -d ${INSTALL_BINDIR}
	${INSTALL} ${exe} ${INSTALL_BINDIR}
	${INSTALL} -d ${INSTALL_HELPDIR}
	cd ../game/data && ${INSTALL} -m 644 ${HELPFILES} ${INSTALL_HELPDIR}

.PHONY: cleaner install

# FIXME: Check base and minimal dbs into CVS as unpacked file trees under dbs/

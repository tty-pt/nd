# srcdir= ..

prefix?=/usr/local
INCLUDE= ${srcdir}/include
INSTALL_BINDIR=${prefix}/bin
INSTALL_HELPDIR=${prefix}/share/fbmuck
CONFIG_SSL=y

ldlibs-y := -lpcre -lm -lxxhash -ldb
ldlibs-$(CONFIG_SSL) += -lssl -lcrypto
cflags-y := -g -Wall -Wno-initializer-overrides

exe := fbmuck prochelp
src = ChangeLog COPYING ${real-obj-y:%.o=%.c}

prochelp-obj-y := prochelp.o
fbmuck-obj-y := object.o entity.o spacetime.o mcp.o map.o \
	plant.o biome.o noise.o \
	view.o item.o spell.o drink.o mob.o kill.o \
	shop.o ws.o
fbmuck-obj-y += create.o \
	hashtab.o help.o \
	look.o match.o \
	move.o \
	player.o \
	predicates.o propdirs.o property.o \
	props.o rob.o set.o \
	speech.o stringutil.o \
	unparse.o wiz.o \
	copyright.o interface.o

exe-obj-y := ${fbmuck-obj-y} ${prochelp-obj-y}

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

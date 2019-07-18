#!/bin/make -f

subdirs=src
RM=rm -f

SSLTOP=/usr

$(subdirs):
	${MAKE} -C $@ all
	@echo cd ..

all:	${subdirs}

subdirs-install := ${subdirs:%=%-install}
$(subdirs-install):
	${MAKE} -C ${@:%-install=%} install

install: ${subdirs-install}
	if [ ! -f game/data/server.pem ]; then \
	echo "You can use 'make cert' to generate a self-signed server certificate"; \
	echo "for use in allowing secure encrypted SSL connections to your Muck."; \
	echo " "; \
	fi

cert game/server.pem:
	if [ -r game/data/server.pem ]; then \
		echo "Will not overwrite game/data/server.pem"; \
		echo "Remove that file and do 'make cert' again";\
		echo "to create a new certificate."; \
		echo; \
	else \
		if [ ! -r ${HOME}/.rnd ]; then \
			$(SSLTOP)/bin/openssl rand -rand /etc/hosts:/etc/passwd 0; \
		fi; \
		echo "Creating secure certificate. Please answer all the questions."; \
		echo "The key and certificate will be stored in the game/data/server.pem file."; \
		echo; \
		openssl req -x509 -nodes -out game/data/server.pem -keyout game/data/server.pem -config fuzzball.cnf -new -days 730; \
	fi; \

subdirs-clean := ${subdirs:%=%-clean}
$(subdirs-clean):
	${MAKE} -C ${@:%-clean=%} clean

clean: ${subdirs-clean}

subdirs-cleaner := ${subdirs:%=%-cleaner}
$(subdirs-cleaner):
	${MAKE} -C ${@:%-cleaner=%} cleaner

cleaner: ${subdirs-cleaner}
	${RM} config.status config.cache config.log

help:
	cd src && make prochelp
	cd game/data && ../../src/prochelp mpihelp.raw mpihelp.txt ../../docs/mpihelp.html
	cd game/data && ../../src/prochelp mufman.raw man.txt ../../docs/mufman.html
	cd game/data && ../../src/prochelp muckhelp.raw help.txt ../../docs/muckhelp.html

.PHONY: ${subdirs} clean ${subdirs-clean} ${subdirs-cleaner} help

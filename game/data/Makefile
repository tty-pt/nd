srcdir := ../..

prochelp: ${srcdir}/src/prochelp.c
	cc ${srcdir}/src/prochelp.c -o ${@}

help: prochelp muckhelp.raw
	./prochelp muckhelp.raw help.txt ${srcdir}/docs/muckhelp.html

.PHONY: help

.SUFFIXES : .txt .raw

obj-y := mpihelp.txt mufman.txt muckhelp.txt

all: ${obj-y}

.raw.txt:
	${srcdir}/src/prochelp $< $@ ${@:%.txt=${srcdir}/docs/%.html}

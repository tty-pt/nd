mod != ls | while read line; do test ! -d $$line || echo $$line; done
all: ${mod:%=%/main.so}

$(mod:%=%/main.so): ${mod} ../src/libnd.a

$(mod):
	${MAKE} -C $@ install

deps: ${mod:%=%/main.d}

$(mod:%=%/main.d):
	${MAKE} -C ${@:%/main.d=%} main.d

.PHONY: all deps ${mod}

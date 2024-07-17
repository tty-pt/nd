bin/nd: items/nd/nd
	cp items/nd/nd $@

items/nd/nd: /usr/local/lib/libndc.so /usr/local/lib/libqhash.so
	${MAKE} -C items/nd PWD=${PWD:%=%/items/nd}

/usr/local/lib/libndc.so:
	${sudo} ${MAKE} -C items/nd/node_modules/@tty-pt/ndc \
		install

/usr/local/lib/libqhash.so:
	${sudo} ${MAKE} -C items/nd/node_modules/@tty-pt/qhash \
		install

mod-bin += nd


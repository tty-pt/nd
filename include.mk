bin/nd: items/nd/nd
	cp items/nd/nd $@

items/nd/nd: /usr/local/lib/libndc.so /usr/local/lib/libqhash.so
	${MAKE} -C items/nd PWD=${PWD:%=%/items/nd}

/usr/local/lib/libndc.so: items/nd/node_modules/
	${sudo} ${MAKE} -C items/nd/node_modules/@tty-pt/ndc \
		install

/usr/local/lib/libqhash.so: items/nd/node_modules/
	${sudo} ${MAKE} -C items/nd/node_modules/@tty-pt/qhash \
		install

items/nd/node_modules/:
	${MAKE} -C items/nd/ node_modules/

all: var/nd/std.db

var/nd/std.db: var/nd
	cp items/nd/game/std.db.ok $@

chroot_mkdir += var/nd

mod-bin += nd


bin/nd: items/nd/nd etc/group etc/passwd etc/vim/vimrc.local
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

etc/group: etc
	echo "root:X:0:" > $@

etc/passwd: etc
	echo "root:X:0:0:root:/root:/bin/bash" > $@

etc/vim/vimrc.local: etc/vim
	echo "colorscheme pablo" > $@

chroot_mkdir += var/nd

mod-bin += nd


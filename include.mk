nd-chroot_mkdir_OpenBSD += dev
chroot_mkdir += ${nd-chroot_mkdir_${uname}} etc etc/vim usr/lib usr/include var/env
ttys := 0 1 2 3 4 5 6 7 8 9
ptys := ${ttys:%=ptyp%}
ttys := ${ttys:%=ttyp%}
nods-OpenBSD := ptm ${ptys} ${ttys} tty
nods-OpenBSD := ${nods-OpenBSD:%=dev/%}
nods-Linux := dev/ptmx dev/pts dev/urandom dev/null dev/zero
nods := ${nods-${uname}}
mounts-Linux := dev sys proc
mounts := ${mounts-${uname}}
mkdir := var/nd/st var/nd/env
uapi != ls items/nd/include/uapi
egdb != which egdb || echo gdb
urandom-major-OpenBSD := 0
urandom-minor-OpenBSD := 45
urandom-major-Linux := 1
urandom-minor-Linux := 9
null-major-OpenBSD := 2
null-minor-OpenBSD := 2
null-major-Linux := 1
null-minor-Linux := 3
zero-major-OpenBSD := 2
zero-minor-OpenBSD := 12
zero-major-Linux := 1
zero-minor-Linux := 5

items/nd/nd: FORCE
	@${MAKE} -C items/nd PWD=${PWD:%=%/items/nd}

usr/lib/libnd.a: usr/lib FORCE
	@${MAKE} -C items/nd/src PWD=${PWD:%=%/items/nd} libnd.a
	cp items/nd/src/libnd.a usr/lib/

-include items/nd/man/include.mk

all: items/nd/nd var/nd/std.db.ok mounts dev/ nods etc/vim/vimrc.local \
	${pages:%=usr/share/man/man10/%} usr/share/man/mandoc.db \
	${uapi:%=usr/include/%} usr/lib/libnd.a var/nd/env certs.txt

var/nd/std.db.ok:
	mkdir -p var/nd || true
	cp items/nd/game/std.db.ok $@

etc/vim: etc

etc/vim/vimrc.local: etc/vim
	echo "colorscheme pablo" > $@

dev/pts:
	@if ! mount | grep -q "on ${PWD}/$@ type"; then \
		mkdir -p $@ 2>/dev/null || true ; \
		echo ${sudo} mount -t devpts devpts $@ ; \
		${sudo} mount -t devpts devpts $@ ; \
		fi

dev/ptmx:
	@if test ! -c $@; then \
		cmd="mknod $@ c 5 2 && chmod 666 $@" ; \
		echo "${sudo} sh -c \"$$cmd\"" ; \
		${sudo} sh -c "$$cmd" ; \
		fi

dev/urandom dev/zero dev/null:
	${sudo} mknod $@ c ${${@:dev/%=%}-major-${uname}} ${${@:dev/%=%}-minor-${uname}}
	${sudo} chmod 666 $@

dev/ptm:
	${sudo} mknod $@ c 81 0

$(ptys:%=dev/%):
	${sudo} mknod $@ c 6 ${@:dev/ptyp%=%}

$(ttys:%=dev/%):
	${sudo} mknod $@ c 5 ${@:dev/ttyp%=%}

$(mkdir):
	mkdir -p $@

mounts: ${mounts}
nods: ${nods}

nods-clean:
	rm -rf ${nods}

clean: nods-clean

dev/tty:
	${sudo} mknod $@ c 1 0 || true
	${sudo} chmod 666 $@
	${sudo} chown root:wheel $@

run: all bin/qhash
	${sudo-${USER}} ./items/nd/nd -C ${PWD} -p 8000

srun: all ss_key.pem ss_cert.pem bin/qhash
	${sudo} ./items/nd/nd -C ${PWD} -K ${PWD}/certs.txt

certs.txt:
	touch certs.txt

osrun: all
	${sudo} ./items/nd/nd -C ${PWD} \
		-K ${PWD}/certs.txt

ostrace: all
	${sudo} ktrace -di ./items/nd/nd -C ${PWD} \
		-K ${PWD}/certs.txt

osdbg: all
	${sudo} ${egdb} -ex "handle SIGPIPE nostop noprint pass" -ex "set pagination off" -ex "run" --args ./items/nd/nd -C ${PWD} \
		-K ${PWD}/certs.txt -d

ss_key.pem:
	openssl genpkey -algorithm RSA -out ss_key.pem -aes256

ss_cert.pem: ss_key.pem
	openssl req -new -key ss_key.pem -out ss_csr.pem
	openssl req -x509 -key ss_key.pem -in ss_csr.pem -out ss_cert.pem -days 365

${uapi:%=usr/include/%}: usr/include ${uapi:%=items/nd/include/uapi/%}
	cp ${@:usr/include/%=items/nd/include/uapi/%} $@

$(chroot_mkdir):
	mkdir -p $@

FORCE:

mod-install += items/nd/install

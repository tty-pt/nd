nd-chroot_mkdir_OpenBSD += dev
chroot_mkdir += ${nd-chroot_mkdir_${uname}} etc
ttys := 0 1 2 3 4 5 6 7 8 9
ptys := ${ttys:%=ptyp%}
ttys := ${ttys:%=ttyp%}
nods-OpenBSD := ptm ${ptys} ${ttys} tty urandom null zero
nods-OpenBSD := ${nods-OpenBSD:%=dev/%}
nods-Linux := dev/ptmx dev/pts
nods := ${nods-${uname}}
mounts-Linux := dev sys proc
mounts := ${mounts-${uname}}
mkdir- := var/nd/st
mkdir-OpenBSD := dev ${mkdir-}
uapi != ls items/nd/include/uapi

items/nd/nd: FORCE
	@${MAKE} -C items/nd PWD=${PWD:%=%/items/nd}

usr/lib/libnd.a: FORCE
	@${MAKE} -C items/nd/src PWD=${PWD:%=%/items/nd} libnd.a
	cp items/nd/src/libnd.a usr/lib/

-include items/nd/man/include.mk

all: items/nd/nd var/nd/std.db.ok mounts dev/ nods etc/ etc/vim/vimrc.local \
	${pages:%=usr/share/man/man10/%} usr/share/man/mandoc.db \
	${uapi:%=usr/include/%} usr/lib/libnd.a

var/nd/std.db.ok:
	mkdir -p var/nd || true
	cp items/nd/game/std.db.ok $@

etc/vim/vimrc.local: etc/vim/
	echo "colorscheme pablo" > $@

dev/pts:
	@if ! mount | grep -q "on ${PWD}/$@ type"; then \
		mkdir -p $@ 2>/dev/null || true ; \
		echo ${sudo} mount -t devpts devpts $@ ; \
		${sudo} mount -t devpts devpts $@ ; \
		fi

dev/ptmx:
	@if test ! -c $@; then \
		cmd="mknod $@ c 5 2 $@ && chmod 666 $@" ; \
		echo ${sudo} sh -c \"$$cmd\" ; \
		${sudo} sh -c \"$$cmd\" ; \
		fi

dev/urandom:
	${sudo} mknod $@ c 45 0

dev/null:
	${sudo} mknod $@ c 2 2
	${sudo} chmod 666 $@

dev/zero:
	${sudo} mknod $@ c 2 12
	${sudo} chmod 666 $@

dev/ptm:
	${sudo} mknod $@ c 81 0

$(ptys:%=dev/%):
	${sudo} mknod $@ c 6 ${@:dev/ptyp%=%}

$(ttys:%=dev/%):
	${sudo} mknod $@ c 5 ${@:dev/ttyp%=%}

${mkdir-${uname}}:
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

run: all
	${sudo-${USER}} ./items/nd/nd -C ${PWD} -p 8000

srun: all ss_key.pem ss_cert.pem
	${sudo} ./items/nd/nd -C ${PWD} -c ss_cert.pem -k ss_key.pem

osrun: all
	${sudo} ./items/nd/nd -C ${PWD} -c /etc/ssl/tty.pt.crt -k /etc/ssl/private/tty.pt.key

ostrace: all
	${sudo} ktrace -di ./items/nd/nd -C ${PWD} -c /etc/ssl/tty.pt.pem -k /etc/ssl/private/tty.pt.key

osdbg: all
	${sudo} egdb -ex "handle SIGPIPE nostop noprint pass" -ex "set pagination off" -ex "run" --args ./items/nd/nd -C ${PWD} -c /etc/ssl/tty.pt.pem -k /etc/ssl/private/tty.pt.key

ss_key.pem:
	openssl genpkey -algorithm RSA -out ss_key.pem -aes256

ss_cert.pem: ss_key.pem
	openssl req -new -key ss_key.pem -out ss_csr.pem
	openssl req -x509 -key ss_key.pem -in ss_csr.pem -out ss_cert.pem -days 365

${uapi:%=usr/include/%}: ${uapi:%=items/nd/include/uapi/%}
	cp ${@:usr/include/%=items/nd/include/uapi/%} $@

FORCE:

mod-install += items/nd/install

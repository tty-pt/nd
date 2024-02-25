#!/bin/sh

ROOT=$1

install_extra() {
        line=$1
        target_path="`dirname $line`"
        test -d "$ROOT$target_path" || mkdir -p $ROOT$target_path
	echo install_extra $line $ROOT$target_path $link >&2
	link="`readlink "$line"`"
	if test ! -z "$link"; then
		echo "$link" | grep -q "^\/" \
			&& install_extra "$link" \
			|| install_extra "$target_path/$link"
	fi
        if test -d "$ROOT$line" || test -f "$ROOT$line"; then
		return
	fi
	cp -r $line $ROOT$line
	echo cp -r $line $ROOT$line
}                                                                                                        

install_bin() {                                                                                          
        path=$1
	echo install_bin $1 >&2
        ldd $path | tail -n +1 | while read filename arrow ipath rest; do
		test -z "$ipath" && continue
                install_extra "$ipath"
        done
	install_extra $1
}

test $# -ge 1 || exit 1
mount --bind /dev $1/dev
mount --bind /sys $1/sys
mount --bind /proc $1/proc
mount -t devpts devpts $1/dev/pts
mknod $1/dev/ptmx c 5 2
chmod 666 $1/dev/ptmx
cat .chroot | while read line; do install_bin "`which $line`"; done
cat .chroot-extra | while read line; do install_extra "$line"; done

mkdir chroot_node_modules
cd node_modules
ls | while read path; do
	link="`readlink $path`"
	if test -z "$link"; then
		ls $path | while read next; do echo $path/$next; done
	else
		echo $path
	fi
done | while read path; do
	# path=$line
	echo cp -r `readlink -f $path` ../chroot_node_modules/$path >&2
	mkdir -p `dirname ../chroot_node_modules/$path` || true
	cp -r `readlink -f $path` ../chroot_node_modules/$path
done
cd - >/dev/null

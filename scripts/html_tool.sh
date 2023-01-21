#/bin/sh

parse() {
        while read st cmd arg nd; do
		if test "$st" = "<!--"; then
			case $cmd in
			CAT) cat $arg ;;
			STYLE) echo "<style>" ; cat $arg ; echo "</style>" ;;
			DEPS)
				if test $# -ge 1; then
					echo '<script>'
					cat $@
					echo '</script>'
				fi
				;;
			esac
		else
			echo $st $cmd $arg $nd
                fi
        done
}

file=$1
shift

cat $file | envsubst | parse $@

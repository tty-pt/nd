parse() {
        while read st cmd arg nd; do
		if [[ "$st" == "<!--" ]]; then
			case $cmd in
			CAT) cat $arg ;;
			STYLE) echo "<style>" ; cat $arg ; echo "</style>" ;;
			DEPS) echo '<script>' ; cat $@ ; echo '</script>' ;;
			esac
		else
			echo $st $cmd $arg $nd
                fi
        done
}

file=$1
shift

cat $file | parse $@

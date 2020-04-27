scripts := ${srcdir}/scripts

# COMPILE=ADVANCED

srcs := main
open-js := ${libs:%=../lib/%.js} ${srcs:%=%.js}
js != [[ -z "${COMPILE}" ]] && echo ${open-js} || echo app.min.js

all: ../index.html

../index.html: pre-index.html ${js} ../vim.css ../styles.css
	${scripts}/html_tool.sh pre-index.html ${js} > $@

app.min.js: ${open-js}
	jsc --js ${open-js} --entry_point main --js_output_file $@ \
		-O ${COMPILE} --module_resolution BROWSER

clean:
	rm ../index.html app.min.js || true

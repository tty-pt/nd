pages != ls items/nd/man/*.10 | xargs -I X basename X
makewhatis != which makewhatis || echo mandb -p

$(pages:%=usr/share/man/man10/%): ${pages:%=items/nd/man/%}
	@mkdir -p `dirname $@` 2>/dev/null || true
	cp ${@:usr/share/man/man10/%=items/nd/man/%} $@

usr/share/man/mandoc.db: $(pages:%=usr/share/man/man10/%)
	${makewhatis} usr/share/man

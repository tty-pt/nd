pages := status equip unequip cast combo select help walk \
	 claim unclaim door undoor wall unwall position \
	 set-position say avatar reroll look inventory get \
	 put drop give fight train sh advitam bio boot chown \
	 create name toad examine consume fill buy sell shop \
	 sit stand clone heal diff recycle pose begin stchown

$(pages:%=usr/share/man/man10/%.10): ${pages:%=items/nd/man/%.10}
	@mkdir -p `dirname $@` 2>/dev/null || true
	cp ${@:usr/share/man/man10/%=items/nd/man/%} $@

usr/share/man/mandoc.db: $(pages:%=usr/share/man/man10/%.10)
	makewhatis usr/share/man

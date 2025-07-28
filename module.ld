SECTIONS
{
    . = ALIGN(8);

    .sic_auto_init :
    {
        __start_sic_auto_init = .;
        KEEP(*(.sic_auto_init))
        __stop_sic_auto_init = .;
    }

    /DISCARD/ : {
	    *(.init)
    }
}
INSERT AFTER .text;

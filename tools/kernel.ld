ENTRY(_start)

SECTIONS
{
    . = 1M;
    __code_start__ = .;
	.text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.init)
        *(.fini)
		*(.text)
		*(.gnu.linkonce.t*)
	}
    __code_end__   = .;
    __data_start__ = .;
	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
        *(.ctors)
        *(.dtors)
        *(.gnu.linkonce.d*)
        *(.jcr)
	}
    __data_end__ = .;
    __rodata_start__ = .;
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata*)
		*(.gnu.linkonce.r*)
	}
    __rodata_end__ = .;
	.eh_frame BLOCK(4K) : ALIGN(4K) {
	    *(.eh_frame)
	    *(.gnu.linkonce.e*)
	}
    __bss_start__ = .;
	.bss BLOCK(4K) : ALIGN(4K) {
		*(.bss)
		*(.gnu.linkonce.b*)
        *(COMMON)
		*(.common)
		*(.gnu.linkonce.c*)
	}
    __bss_end__ = .;
	__end__     = .;
	.debug : {
	    *(.debug*)
	    *(.stab*)
	    *(.line*)
	    *(.dwarf*)
	    *(.gnu.linkonce.d*)
	    *(.gnu_debuglink)
        *(.debug_str)
        *(.comment)
	}
}

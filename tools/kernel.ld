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
        *(.gnu.linkonce.d*)
        *(.ctors)
        *(.dtors)
        *(.jcr)
	}
    __data_end__ = .;
    __rodata_start__ = .;
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata*)
		*(.gnu.linkonce.r*)
		*(.common)
		*(.gnu.linkonce.c*)
	}
    __rodata_end__ = .;
	.eh_frame BLOCK(4K) : ALIGN(4K) {
	    *(.eh_frame)
	    *(.gnu.linkonce.e*)
	}
    __bss_start__ = .;
	.bss BLOCK(4K) : ALIGN(4K) {
        *(COMMON)
		*(.bss)
		*(.gnu.linkonce.b*)
	}
    __bss_end__ = .;
	__end__     = .;
	.debug BLOCK(4K) : ALIGN(4K) {
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

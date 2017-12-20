ENTRY(_start)

SECTIONS
{
    . = 1M;
	.text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        *(.init)
        *(.fini)
		*(.text)
		*(.gnu.linkonce.t*)
	}
	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
        *(.gnu.linkonce.d*)
        *(.ctors)
        *(.dtors)
        *(.jcr)
	}
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata*)
		*(.gnu.linkonce.r*)
		*(.common)
		*(.gnu.linkonce.c*)
	}
	.eh_frame BLOCK(4K) : ALIGN(4K) {
	    *(.eh_frame)
	    *(.gnu.linkonce.e*)
	}
	.bss BLOCK(4K) : ALIGN(4K) {
		*(.bss)
		*(.gnu.linkonce.b*)
	}
	__end__ = .;
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

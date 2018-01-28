ENTRY(_start)

SECTIONS
{
    . = 1M;
    __kernel_start__ = .;
	.text BLOCK(4K) : ALIGN(4K) {
        *(.multiboot)
        __code_start__ = .;
        *(.init)
        *(.fini)
		*(.text*)
		*(.gnu.linkonce.t*)
        __code_end__   = .;
	}
	.data BLOCK(4K) : ALIGN(4K) {
        __data_start__ = .;
		*(.data)
        *(.ctors*)
        *(.dtors*)
        *(.gnu.linkonce.d*)
        *(.jcr)
        __data_end__ = .;
	}
	.rodata BLOCK(4K) : ALIGN(4K) {
        __rodata_start__ = .;
		*(.rodata*)
		*(.gnu.linkonce.r*)
        __rodata_end__ = .;
	}
	.eh_frame BLOCK(4K) : ALIGN(4K) {
	    *(.eh_frame)
	    *(.gnu.linkonce.e*)
	}
	.bss BLOCK(4K) : ALIGN(4K) {
        __bss_start__ = .;
		*(.bss)
		*(.gnu.linkonce.b*)
        *(COMMON)
		*(.common)
		*(.gnu.linkonce.c*)
        __bss_end__ = .;
	}
    __kernel_end__ = .;
    /DISCARD/ : {
        *(.comment)
    }
}

ENTRY(_start)

SECTIONS
{
    . = 1M;
    __kernel_start__ = .;
    .text BLOCK(4K) : ALIGN(4K) {
        __code_start__ = .;
        *(.multiboot2)
        __code_start__ = .;
        *(.init)
        *(.fini)
		*(.text*)
        __code_end__   = .;
    }
    .data BLOCK(4K) : ALIGN(4K) {
        __data_start__ = .;
		*(.data*)
        *(.jcr)
        __data_end__ = .;
    }
    .rodata BLOCK(4K) : ALIGN(4K) {
        __rodata_start__ = .;
        *(.ctors*)
        *(.init_array)
        *(.dtors*)
        *(.fini_array)
        *(.rodata*)
        __rodata_end__ = .;
    }
    .eh_frame BLOCK(4K) : ALIGN(4K) {
        *(.eh_frame*)
    }
    .bss BLOCK(4K) : ALIGN(4K) {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        *(.common)
        __bss_end__ = .;
	}
    __kernel_end__ = .;
    .debug : {
        *(.debug*)
    }
    .note : {
        *(.note*)
        *(.comment)
    }
}

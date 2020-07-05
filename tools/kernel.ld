ENTRY(_start)
SECTIONS
{
    . = 1M;
    __kernel_start__ = .;
    .text : ALIGN(16) {
        __code_start__ = .;
        KEEP(*(.text.boot))
        *(.init)
        *(.fini)
		*(.text*)
        __code_end__ = .;
    }
    .ctors : ALIGN(16) {
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
    }
    .dtors : ALIGN(16) {
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
    }
    .data : ALIGN(16) {
        __data_start__ = .;
		*(.data*)
        *(.jcr)
        __data_end__ = .;
    }
    .rodata : ALIGN(16) {
        __rodata_start__ = .;
        *(.rodata*)
        __rodata_end__ = .;
    }
    .eh_frame : ALIGN(16) {
        start_eh_frame = .;
        __eh_frame_start__ = .;
        *(.eh_frame*)
        QUAD(0)
        __eh_frame_end__ = .;
    }
    .bss : ALIGN(16) {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        *(.common)
        __bss_end__ = .;
	}
    __kernel_end__ = .;
    .note : {
        *(.note*)
        *(.comment)
    }
}

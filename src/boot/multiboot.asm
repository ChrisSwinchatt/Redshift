MAGIC  equ 0xE85250D6
ARCH   equ 0
LENGTH equ multiboot2.end - multiboot2
[section .multiboot]
align 8
multiboot2:
    ; Header.
    dd MAGIC                    ; Magic number.
    dd ARCH                     ; Architecture.
    dd LENGTH                   ; Header size.
    dd -(MAGIC + ARCH + LENGTH) ; Header checksum.
    ; Tags.
    .info:                      ; Multiboot information.
        dw 1, 0
        dd .align - .info
        dd 1, 2, 3, 5, 6
        dd 0                    ; Align next tag to 8 byte boundary.
    .align:                     ; Module alignment.
        dw 6, 0
        dd .end_tag - .align
    .end_tag:                   ; End of tags.
        dw 0, 0
        dd .end - .end_tag
    ; End of header.
    .end:

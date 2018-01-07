; Copyright (c) 2012-2018 Chris Swinchatt.
;
; Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
; documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
; rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
; persons to whom the Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
; Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
; WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
; COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
; OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
[bits 32]
align 4

STACK_SIZE equ 0x8000 ; 32 kiB stack.

[section .text]
; Entry point.
[global _start]
_start:
        ; Set up the stack. NB: Stack must be 16-bytes aligned when we call _init and _boot.
        cli
        mov     esp,                              __stack_top__
        xor     ebp,                              ebp
        ; Save bootloader magic number and tags pointer.
        mov     [__multiboot_bootloader_magic__], eax
        mov     [__multiboot_bootloader_tags__],  ebx
        ; Call global constructors.
        [extern _init]
        call    _init
        [extern boot]
        call    boot
        ; Fall through.
[global _exit]
_exit:
        ; Call global destructors.
        [extern _fini]
        call    _fini
        [extern hang]
        call    hang

[section .data]
[global __multiboot_bootloader_magic__]
__multiboot_bootloader_magic__: dd 0
[global __multiboot_bootloader_tags__]
__multiboot_bootloader_tags__:  dd 0

[section .bss]
align 16
[global __stack_bottom__]
__stack_bottom__: resb STACK_SIZE
[global __stack_top__]
__stack_top__:

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

STACK_SIZE equ 0x8000 ; 32 kiB stack.

[section .text]
; Entry point.
[global _start]
_start:
        cli
        mov        esp, __stack_top__              ; Set stack pointer.
        xor        ebp,    ebp
        push dword 0                               ; } Push dummy values to keep stack pointer 16-bytes aligned.
        push dword 0                               ; } walk_stack also needs 0 values to find the top of the stack.
        push       ebx                             ; Save multiboot info.
        push       eax                             ; Save multiboot magic number.
        [extern    __stack_chk_guard_setup]
        call       __stack_chk_guard_setup         ; Set up the stack guard.
        [extern    _init]
        call       _init                           ; Call constructors.
        [extern    boot]
        call       boot                            ; Call boot. EAX and EBX are still on the stack.
        [extern    _fini]
        call       _fini                           ; Call destructors.
[global hang]
hang:
        cli
        hlt
        jmp hang
[section .bss]
align 16
[global __stack_bottom__]
__stack_bottom__: resb STACK_SIZE
[global __stack_top__]
__stack_top__:

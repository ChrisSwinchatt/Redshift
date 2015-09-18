; Copyright (c) 2012 Chris Swinchatt.
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
STACK_SIZE equ 0x10000
[section .text]
; Entry point.
[global _start]
_start:             mov     esp, __stack_top__              ; Create the stack.
                    push    ebx                             ; Save multiboot info.
                    push    eax                             ; Save multiboot magic number.
[extern __stack_chk_guard_setup]
                    call    __stack_chk_guard_setup         ; Set up the stack guard.
[extern boot]
                    call    boot                            ; Call boot. EAX and EBX are still on the stack.
                    add     esp, 8                          ; Clean up stack. This isn't strictly necessary but 
[global hang]
hang:               cli
                    hlt
                    jmp     hang
[section .bss]
[global __stack__]
__stack__:          resb    STACK_SIZE
[global __stack_top__]
__stack_top__:

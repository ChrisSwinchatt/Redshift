;; Copyright (c) 2012-2018 Chris Swinchatt.
 ;
 ; Permission is hereby granted, free of charge, to any person obtaining a copy
 ; of this software and associated documentation files (the "Software"), to deal
 ; in the Software without restriction, including without limitation the rights
 ; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 ; copies of the Software, and to permit persons to whom the Software is
 ; furnished to do so, subject to the following conditions:
 ;
 ; The above copyright notice and this permission notice shall be included in
 ; all copies or substantial portions of the Software.
 ;
 ; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 ; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 ; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 ; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 ; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 ; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 ; SOFTWARE.
;;
[bits 32]
[section .text]

[global set_state_and_jump]
set_state_and_jump:
    cli
    ; Pop registers from the stack. We have to pop them in reverse order of struct cpu_state (hal/cpu/state.h).
    sub   esp,       8    ; Skip interrupt and error_code.
    pop   eax
    mov   cr4,       eax
    pop   eax
    mov   cr3,       eax
    pop   eax
    mov   cr2,       eax
    pop   eax
    mov   cr0,       eax
    popfd
    cli
    pop   eax
    mov   [__eip__], eax  ; Save EIP for later.
    pop   eax
    mov   ax,        cs
    pop   eax
    mov   ax,        ds
    pop   eax
    mov   ax,        es
    pop   eax
    mov   ax,        fs
    pop   eax
    mov   ax,        gs
    pop   eax
    mov   ax,        ss
    pop   edi             ; Can't use POPAD here because cpu_state has a different ordering for GP registers.
    pop   esi
    pop   ebp
    pop   eax
    mov   [__esp__], eax  ; Save ESP for later.
    pop   edx
    pop   ecx
    pop   ebx
    pop   eax
    ; Set stack pointer & interrupt flag and jump!
    mov   esp,       [__esp__]
    sti
    jmp  [__eip__]

[section .data]
__eip__: dd 0
__esp__: dd 0

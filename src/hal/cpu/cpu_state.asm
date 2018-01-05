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
; cpu_state (hal/cpu/state.h) structure analogue. Members must match struct cpu_state in order, alignment and size.
[section .data]
align 4
[global cpu_state]
cpu_state:
    [global cpu_state_eax]
    cpu_state_eax:        dd 0
    [global cpu_state_ebx]
    cpu_state_ebx:        dd 0
    [global cpu_state_ecx]
    cpu_state_ecx:        dd 0
    [global cpu_state_edx]
    cpu_state_edx:        dd 0
    [global cpu_state_esp]
    cpu_state_esp:        dd 0
    [global cpu_state_ebp]
    cpu_state_ebp:        dd 0
    [global cpu_state_esi]
    cpu_state_esi:        dd 0
    [global cpu_state_edi]
    cpu_state_edi:        dd 0
    [global cpu_state_cs]
    cpu_state_cs:         dd 0
    [global cpu_state_ds]
    cpu_state_ds:         dd 0
    [global cpu_state_es]
    cpu_state_es:         dd 0
    [global cpu_state_fs]
    cpu_state_fs:         dd 0
    [global cpu_state_gs]
    cpu_state_gs:         dd 0
    [global cpu_state_ss]
    cpu_state_ss:         dd 0
    [global cpu_state_eip]
    cpu_state_eip:        dd 0
    [global cpu_state_eflags]
    cpu_state_eflags:     dd 0
    [global cpu_state_cr0]
    cpu_state_cr0:        dd 0
    [global cpu_state_cr2]
    cpu_state_cr2:        dd 0
    [global cpu_state_cr3]
    cpu_state_cr3:        dd 0
    [global cpu_state_cr4]
    cpu_state_cr4:        dd 0
    [global cpu_state_interrupt]
    cpu_state_interrupt:  dd 0
    [global cpu_state_error_code]
    cpu_state_error_code: dd 0

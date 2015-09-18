;; Copyright (c) 2012 Chris Swinchatt.
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
%macro ISR 1
    [global isr%1]
    isr%1:
        ; Push dummy error code. CPU has pushed SS, ESP, CS, EFLAGS and EIP.
        push byte 0
        ; Push ISR number.
        push byte %1
        ; Create stack frame.
        push  ebp
        mov   ebp, esp
        ; Jump to pre-handler.
        jmp  isr_pre_handler
%endmacro

%macro ISR_E 1
    [global isr%1]
    isr%1:
        ; Push ISR number. CPU has pushed error code, SS, ESP, CS, EFLAGS and EIP.
        push byte %1
        ; Create stack frame. We have to do this after pushing the ISR number, otherwise the pre-handler will mess up
        ; the stack.
        push  ebp
        mov   ebp, esp
        ; Jump to pre-handler.
        jmp  isr_pre_handler
%endmacro

ISR    0
ISR    1
ISR    2
ISR    3
ISR    4
ISR    5
ISR    6
ISR    7
ISR_E  8
ISR    9
ISR_E 10
ISR_E 11
ISR_E 12
ISR_E 13
ISR_E 14
ISR   15
ISR   16
ISR   17
ISR   18
ISR   19
ISR   20
ISR   21
ISR   22
ISR   23
ISR   24
ISR   25
ISR   26
ISR   27
ISR   28
ISR   29
ISR   30
ISR   31

[global isr_pre_handler]
isr_pre_handler:
    ; Note: See ISR macros for function prologue.
    ; Get all the registers on the stack, excepting those that were pushed automatically by the CPU.
    ; The order here *must exactly* match (in reverse) the order of struct cpu_state in cpu_state.h.
    pushad
    xor   eax, eax
    mov   eax, ds
    push  eax
    mov   eax, es
    push  eax
    mov   eax, fs
    push  eax
    mov   eax, gs
    push  eax
    mov   eax, cr0
    push  eax
    mov   eax, cr2
    push  eax
    mov   eax, cr3
    push  eax
    mov   eax, cr4
    push  eax
    ; Set the kernel's segment selector.
    mov   ax,  ds
    push  eax
    mov   ax,  0x10
    mov   ds,  ax
    mov   es,  ax
    mov   fs,  ax
    mov   gs,  ax
    ; Pass isr_handler a pointer to the data we created on the stack.
    push  ebp
    ; Call the proper handler.
    [extern isr_handler]
    call  isr_handler
    add   esp, 4        ; Remove EBP.
    ; Restore original segment selectors.
    pop   eax           ; Restore EAX.
    mov   ds,  ax
    mov   es,  ax
    mov   fs,  ax
    mov   gs,  ax
    ; Restore original GP registers.
    add   esp, 32       ; Remove D/E/F/GS and CR0/2/3/4.
    popad               ; Remove and restore GP registers.
    ; Restore original EBP and ESP.
    leave
    add esp,   8        ; Remove the error code and the ISR number.
    ; Pop SS, ESP, EFLAGS and EIP, then return.
    iret

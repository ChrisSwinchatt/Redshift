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
%macro IRQ 2
    [global irq%1]
    irq%1:
        ; Push dummy error code.
        push byte 0
        ; Push ISR number.
        push byte %2
        ; Create stack frame.
        push  ebp
        mov   ebp, esp
        ; Jump to pre-handler.
        jmp irq_pre_handler
%endmacro

IRQ  0, 32
IRQ  1, 33
IRQ  2, 34
IRQ  3, 35
IRQ  4, 36
IRQ  5, 37
IRQ  6, 38
IRQ  7, 39
IRQ  8, 40
IRQ  9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

[global irq_pre_handler]
irq_pre_handler:
    ; Note: See IRQ macro for function prologue.
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
    [extern irq_handler]
    call  irq_handler
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
    add esp,   8        ; Remove the dummy error code and the IRQ number.
    ; Pop SS, ESP, EFLAGS and EIP, then return.
    iret

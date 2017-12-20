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
%macro call_handler 1
    ; Save all the registers on the stack, excepting those that were pushed automatically by the CPU
    ; and the stubs above. The order here must *exactly* match, in reverse, the order in struct
    ; cpu_state in cpu_state.h.
    pushad                          ; Push EAX, ECX, EBX, ESP, EBP, ESI, EDI
    push  ds                        ; Push DS
    push  es                        ; Push ES
    push  fs                        ; Push FS
    push  gs                        ; Push GS
    mov   eax, cr0                  ; Push CR0
    push  eax
    mov   eax, cr2                  ; Push CR2
    push  eax
    mov   eax, cr3                  ; Push CR3
    push  eax
    mov   eax, cr4                  ; Push CR4
    push  eax
    ; Set the kernel's segment selector.
    mov   ax,  0x10
    mov   ds,  ax
    mov   es,  ax
    mov   fs,  ax
    mov   gs,  ax
    ; Call handler.
    push  esp                       ; Pass pointer to registers struct.
    cld
    [extern %1]
    call %1
    pop  esp
    ; Restore registers.
    add   esp, 16                   ; Remove CR4/3/2/0.
    pop   gs                        ; Restore GS.
    pop   fs                        ; Restore FS.
    pop   es                        ; Restore ES.
    pop   ds                        ; Restore DS.
    popad                           ; Restore GP registers.
    add   esp, 8                    ; Remove the error code and the ISR number.
    ; Return.
    iret                            ; Restore SS, ESP, EFLAGS, CS and EIP.
%endmacro

; ISR stub (no error code).
%macro ISR 1
    [global isr%1]
    isr%1:
        push byte    0              ; Push dummy error code. CPU has pushed SS, ESP, EFLAGS, CS and EIP.
        push byte    %1             ; Push ISR number.
        call_handler isr_handler    ; Jump to pre-handler.
%endmacro

; ISR stub (error code).
%macro ISR_E 1
    [global isr%1]
    isr%1:
        push byte    %1
        call_handler isr_handler
%endmacro

; IRQ stub.
%macro IRQ 2
    [global irq%1]
    irq%1:
        push byte    0              ; Push dummy error code.
        push byte    %2             ; Push IRQ's ISR number.
        call_handler irq_handler
%endmacro

; ISR and IRQ stubs.
ISR      0
ISR      1
ISR      2
ISR      3
ISR      4
ISR      5
ISR      6
ISR      7
ISR_E    8
ISR      9
ISR_E   10
ISR_E   11
ISR_E   12
ISR_E   13
ISR_E   14
ISR     15
ISR     16
ISR_E   17
ISR     18
ISR     19
ISR     20
ISR     21
ISR     22
ISR     23
ISR     24
ISR     25
ISR     26
ISR     27
ISR     28
ISR     29
ISR_E   30
ISR     31
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

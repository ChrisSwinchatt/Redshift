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

; ISR stub (no error code).
%macro ISR 1
    [global isr%1]
    isr%1:
        push dword 0                ; Push dummy error code. CPU has pushed SS, ESP, EFLAGS, CS and EIP.
        push dword %1               ; Push ISR number.
        jmp        isr_stub         ; Jump to ISR stub.
%endmacro

; ISR stub (error code).
%macro ISR_E 1
    [global isr%1]
    isr%1:
        push dword %1
        jmp        isr_stub
%endmacro

; IRQ stub.
%macro IRQ 2
    [global irq%1]
    irq%1:
        push dword 0                ; Push dummy error code.
        push dword %2               ; Push IRQ number.
        jmp        irq_stub         ; Jump to IRQ stub.
%endmacro

[extern cpu_state]
[extern cpu_state_eax]
[extern cpu_state_ebx]
[extern cpu_state_ecx]
[extern cpu_state_edx]
[extern cpu_state_esp]
[extern cpu_state_ebp]
[extern cpu_state_esi]
[extern cpu_state_edi]
[extern cpu_state_cs]
[extern cpu_state_ds]
[extern cpu_state_es]
[extern cpu_state_fs]
[extern cpu_state_gs]
[extern cpu_state_ss]
[extern cpu_state_eip]
[extern cpu_state_eflags]
[extern cpu_state_cr0]
[extern cpu_state_cr2]
[extern cpu_state_cr3]
[extern cpu_state_cr4]
[extern cpu_state_interrupt]
[extern cpu_state_error_code]

%macro prologue 0
    ; Save GP registers into cpu_state (except esp).
    mov   [cpu_state_eax],        eax
    mov   [cpu_state_ebx],        ebx
    mov   [cpu_state_ecx],        ecx
    mov   [cpu_state_edx],        edx
    mov   [cpu_state_ebp],        ebp
    mov   [cpu_state_esi],        esi
    mov   [cpu_state_edi],        edi
    ; Retrieve & save the error code and ISR number.
    pop   eax
    mov   [cpu_state_interrupt],  eax
    pop   eax
    mov   [cpu_state_error_code], eax
    ; Retrieve & save values pushed by CPU.
    pop   eax
    mov   [cpu_state_eip],        eax
    pop   eax
    mov   [cpu_state_cs],         eax
    pop   eax
    mov   [cpu_state_eflags],     eax
    pop   eax
    mov   [cpu_state_esp],        eax
    pop   eax
    mov   [cpu_state_ss],         eax
    ; Save data segment selectors.
    xor   eax,                    eax
    mov   eax,                    ds
    mov   [cpu_state_ds],         eax
    mov   eax,                    es
    mov   [cpu_state_es],         eax
    mov   eax,                    fs
    mov   [cpu_state_fs],         eax
    mov   eax,                    gs
    mov   [cpu_state_gs],         eax
    ; Save control registers.
    mov   eax,                    cr0
    mov   [cpu_state_cr0],        eax
    mov   eax,                    cr2
    mov   [cpu_state_cr2],        eax
    mov   eax,                    cr3
    mov   [cpu_state_cr3],        eax
    mov   eax,                    cr4
    mov   [cpu_state_cr4],        eax
%endmacro

%macro call_handler 1
    ; Set data segment selector for kernel.
    mov   ax,  0x10
    mov   ds,  ax
    mov   es,  ax
    mov   fs,  ax
    mov   gs,  ax
    mov   ss,  ax
    ; Call handler.
    push    cpu_state     ; Push pointer to cpu_state.
    cld
    [extern %1]
    call    %1
    add     esp, 4        ; Remove pointer.
%endmacro

%macro epilogue 0
    ; Restore GP registers from cpu_state (except esp).
    mov  eax,  [cpu_state_eax]
    mov  ebx,  [cpu_state_ebx]
    mov  ecx,  [cpu_state_ecx]
    mov  edx,  [cpu_state_edx]
    mov  ebp,  [cpu_state_ebp]
    mov  esi,  [cpu_state_esi]
    mov  edi,  [cpu_state_edi]
    ; Restore data segment selectors.
    xor  eax,  eax
    mov  eax,  [cpu_state_ds]
    mov  ds,   eax,
    mov  eax,  [cpu_state_es]
    mov  es,   eax
    mov  eax,  [cpu_state_fs]
    mov  fs,   eax
    mov  eax,  [cpu_state_gs]
    mov  gs,   eax
    ; Put the values the CPU pushes back onto the stack.
    push dword [cpu_state_ss]
    push dword [cpu_state_esp]
    push dword [cpu_state_eflags]
    push dword [cpu_state_cs]
    push dword [cpu_state_eip]
    ; Return.
    iret                                ; Restore SS, ESP, EFLAGS, CS and EIP.
%endmacro

isr_stub:
    prologue
    call_handler isr_handler
    epilogue

irq_stub:
    prologue
    call_handler irq_handler
    epilogue

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
ISR     30
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

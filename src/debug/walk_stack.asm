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
align 4

[section .text]
; Walks backwards through the call stack and builds a list of return addresses.
; Args:
;  * Array of 32-bit addresses.
;  * Maximum number of elements in array.
; Return value: The number of addresses stored in the array.
; Calling convention: cdecl
[global walk_stack]
walk_stack:
    ; Create stack frame & save caller's EDI and EBX.
    push ebp
    mov  ebp,       esp
    mov  [ebp - 4], edi
    mov  [ebp - 8], ebx
    ; Set up local registers.
    xor  eax,       eax         ; EAX = return value (number of stack frames found).
    mov  ebx,       [esp +  0]  ; EBX = old EBP.
    mov  edi,       [esp +  8]  ; Destination array pointer in EDI.
    mov  ecx,       [esp + 12]  ; Maximum array size in ECX.
.walk:
    ; Walk backwards through EBP linked list, storing return addresses in EDI array.
    mov  edx,       [ebx +  4]  ; EDX = previous stack frame's IP.
    test edx,       edx
    jz   .done                  ; Leave if it is 0.
    mov  ebx,       [ebx +  0]  ; EBX = previous stack frame's BP.
    mov  [edi],     edx         ; Copy IP.
    add  edi,       4
    inc  eax
    loop .walk
.done:
    ; Restore caller's EDI and EBX, leave stack frame & return EAX.
    mov  edi,       [ebp - 4]
    mov  ebx,       [ebp - 8]
    mov  esp,       ebp
    pop  ebp
    ret

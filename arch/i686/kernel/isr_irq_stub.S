/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
.intel_syntax noprefix

.section .text

isr_stub:
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    ebp
    push    esi
    push    edi
    push    ds
    push    es
    push    fs
    push    gs
    mov     eax,    cr0
    push    eax
    mov     eax,    cr2
    push    eax
    mov     eax,    cr3
    push    eax
    mov     eax,    cr4
    push    eax

    mov     ebx,    esp
    sub     ebx,    60      /* Address of where CPU pushed SS. */

    mov     ax,     0x10
    mov     ds,     ax
    mov     es,     ax
    mov     fs,     ax
    mov     gs,     ax
    mov     ss,     ax
    cld
    push    ebx             /* Pass pointer to the stacked register state. */
    call    isr_handler

    add     esp,    20      /* Remove the register state pointer and segment selectors */
    pop     edi
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    add     esp,    4       /* Remove the error code. */
    iret

irq_stub:
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    ebp
    push    esi
    push    edi
    push    ds
    push    es
    push    fs
    push    gs
    mov     eax,    cr0
    push    eax
    mov     eax,    cr2
    push    eax
    mov     eax,    cr3
    push    eax
    mov     eax,    cr4
    push    eax

    mov     ebx,    esp
    sub     ebx,    60      /* Address of where CPU pushed SS. */

    mov     ax,     0x10
    mov     ds,     ax
    mov     es,     ax
    mov     fs,     ax
    mov     gs,     ax
    mov     ss,     ax
    cld
    push    ebx             /* Pass pointer to the stacked register state. */
    call    isr_handler

    add     esp,    20      /* Remove the register state pointer and segment selectors */
    pop     edi
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    add     esp,    4       /* Remove the error code. */
    iret

/* CPU has pushed SS and ESP (if we changed PL), EFLAGS, CS and EIP. */
#define DEFINE_ISR(ISR)          \
    .global isr##ISR            ;\
    .type   isr##ISR, @function ;\
    isr##ISR:                   ;\
        push 0                  ;\
        push ISR                ;\
        jmp  isr_stub

/* CPU has pushed SS and ESP (if we changed PL), EFLAGS, CS, EIP, *and error code*. */
#define DEFINE_ISR_E(ISR)        \
    .global isr##ISR            ;\
    .type   isr##ISR, @function ;\
    isr##ISR:                   ;\
        push ISR                ;\
        jmp  isr_stub

#define DEFINE_IRQ(IRQ, ISR)     \
    .global irq##IRQ            ;\
    .type   irq##IRQ, @function ;\
    irq##IRQ:                   ;\
        push 0                  ;\
        push ISR                ;\
        jmp  irq_stub

/* ISR and IRQ stubs.
 */
DEFINE_ISR(0)
DEFINE_ISR(1)
DEFINE_ISR(2)
DEFINE_ISR(3)
DEFINE_ISR(4)
DEFINE_ISR(5)
DEFINE_ISR(6)
DEFINE_ISR(7)
DEFINE_ISR_E(8)
DEFINE_ISR(9)
DEFINE_ISR_E(10)
DEFINE_ISR_E(11)
DEFINE_ISR_E(12)
DEFINE_ISR_E(13)
DEFINE_ISR_E(14)
DEFINE_ISR(15)
DEFINE_ISR(16)
DEFINE_ISR_E(17)
DEFINE_ISR(18)
DEFINE_ISR(19)
DEFINE_ISR(20)
DEFINE_ISR(21)
DEFINE_ISR(22)
DEFINE_ISR(23)
DEFINE_ISR(24)
DEFINE_ISR(25)
DEFINE_ISR(26)
DEFINE_ISR(27)
DEFINE_ISR(28)
DEFINE_ISR(29)
DEFINE_ISR(30)
DEFINE_ISR(31)
DEFINE_IRQ(0,  32)
DEFINE_IRQ(1,  33)
DEFINE_IRQ(2,  34)
DEFINE_IRQ(3,  35)
DEFINE_IRQ(4,  36)
DEFINE_IRQ(5,  37)
DEFINE_IRQ(6,  38)
DEFINE_IRQ(7,  39)
DEFINE_IRQ(8,  40)
DEFINE_IRQ(9,  41)
DEFINE_IRQ(10, 42)
DEFINE_IRQ(11, 43)
DEFINE_IRQ(12, 44)
DEFINE_IRQ(13, 45)
DEFINE_IRQ(14, 46)
DEFINE_IRQ(15, 47)

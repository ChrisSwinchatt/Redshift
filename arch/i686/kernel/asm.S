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

.global disable_interrupts
.type   disable_interrupts, @function
disable_interrupts:
    cli
    ret

.global enable_interrupts
.type   enable_interrupts, @function
enable_interrupts:
    sti
    ret

.global wait_for_interrupt
.type   wait_for_interrupt, @function
wait_for_interrupt:
    /* Wait for an interrupt to occur. */
    sti
    hlt
    jmp wait_for_interrupt

.set IF_BIT,    9
.set IF_MASK,   1 << IF_BIT

.global get_interrupt_state
.type   get_interrupt_state, @function
get_interrupt_state:
    push    ebp
    mov     ebp,    esp
    pushf
    pop     eax
    mov     ecx,    IF_MASK
    and     eax,    ecx
    leave
    ret


.global hang
.type   hang, @function
hang:
   call disable_interrupts
   jmp  wait_for_interrupt

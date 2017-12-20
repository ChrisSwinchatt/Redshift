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
#ifndef REDSHIFT_KERNEL_CPU_STATE_H
#define REDSHIFT_KERNEL_CPU_STATE_H

#include <kernel/redshift.h>
#include <string.h>

/**
 * CPU registers state.
 *
 * NB: This ordering is significant. It has to *exactly match* (in reverse) the order that registers are pushed in
 * the ISR and IRQ handlers (isr_irq_stub.asm). Namely, the CPU first pushes SS, usermode ESP,
 * EFLAGS, CS and EIP, including an error code for some interrupts. For IRQs and ISRs without an error code, the ISR
 * and IRQ handlers push a dummy error code. The handlers for all ISRs and IRQs then push their own interrupt number
 * (e.g. 32 for IRQ 0), followed by EAX, ECX, EDX, EBX, kernel mode ESP (which is called 'reserved' because it's
 * useless), EBP, ESI, EDI, DS, ES, FS, GS, CR0, CR2, CR3 and CR4.
 *
 * The reverse ordering is because the x86 stack is LIFO.
 */
struct cpu_state {
    uint32_t cr4;       /**< CR4 register.                                */
    uint32_t cr3;       /**< CR3 register.                                */
    uint32_t cr2;       /**< Faulting address when handling a page fault. */
    uint32_t cr0;       /**< CR0 register.                                */
    uint32_t gs;        /**< Extra segment 2.                             */
    uint32_t fs;        /**< Extra segment 1.                             */
    uint32_t es;        /**< Extra segment 0.                             */
    uint32_t ds;        /**< Data segment.                                */
    uint32_t edi;       /**< Destination index.                           */
    uint32_t esi;       /**< Source index.                                */
    uint32_t ebp;       /**< Base pointer.                                */
    uint32_t reserved;  /**< Kernel mode ESP.                             */
    uint32_t ebx;       /**< Base register.                               */
    uint32_t edx;       /**< Divisor register.                            */
    uint32_t ecx;       /**< Counter register.                            */
    uint32_t eax;       /**< Accumulator register.                        */
    uint32_t interrupt; /**< Interrupt number when handling an interrupt. */
    uint32_t errorcode; /**< Error code when handling exceptions.         */
    uint32_t eip;       /**< Return address when handling an interrupt.   */
    uint32_t cs;        /**< Code segment.                                */
    uint32_t eflags;    /**< Flags.                                       */
    uint32_t esp;       /**< Stack pointer.                               */
    uint32_t ss;        /**< Stack segment.                               */
} __packed;

/**
 * Get the CPU state.
 * \param state The CPU state.
 */
static inline void __always_inline get_cpu_state(struct cpu_state* state)
{
    memset(state, 0, sizeof(*state));
    asm volatile(
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        "mov %%esi, %4\n"
        "mov %%edi, %5\n"
        :"=r"(state->eax),
         "=r"(state->ebx),
         "=r"(state->ecx),
         "=r"(state->edx),
         "=r"(state->esi),
         "=r"(state->edi)
        ::"memory"
    );
    asm volatile(
        "mov %%ebp, %0\n"
        "mov %%esp, %1\n"
        "mov %%cs,  %%eax\n"
        "mov %%eax, %2\n"
        "mov %%ds,  %%eax\n"
        "mov %%eax, %3\n"
        "mov %%es,  %%eax\n"
        "mov %%eax, %4\n"
        :"=r"(state->ebp),
         "=r"(state->esp),
         "=r"(state->cs),
         "=r"(state->ds),
         "=r"(state->es)
        ::"eax", "memory"
    );
    asm volatile(
        "mov %%fs,  %%eax\n"
        "mov %%eax, %0\n"
        "xor %%eax, %%eax\n"
        "mov %%eax, %1\n"
        "mov %%ss,  %%eax\n"
        "mov %%eax, %2\n"
        "mov %%cr0, %%eax\n"
        "mov %%eax, %3\n"
        "mov %%cr2, %%eax\n"
        "mov %%eax, %4\n"
        :"=r"(state->fs),
         "=r"(state->gs),
         "=r"(state->ss),
         "=r"(state->cr0),
         "=r"(state->cr2)
        ::"eax", "memory"
    );
    asm volatile(
        "mov %%cr3, %%eax\n"
        "mov %%eax, %0\n"
        "mov %%cr4, %%eax\n"
        "mov %%eax, %1\n"
        "pushfl\n"
        "pop %%eax\n"
        "mov %%eax, %2\n"
        "call __get_cpu_state_eip__\n"
        "__get_cpu_state_eip__:\n"
        "pop %%eax\n"
        "mov %%eax, %3\n"
        :"=r"(state->cr3),
         "=r"(state->cr4),
         "=r"(state->eflags),
         "=r"(state->eip)
        ::"eax", "memory"
    );
}

#endif // ! REDSHIFT_KERNEL_CPU_STATE_H

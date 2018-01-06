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
#ifndef REDSHIFT_HAL_CPU_STATE_H
#define REDSHIFT_HAL_CPU_STATE_H

#include <redshift/kernel.h>
#include <string.h>

/**
 * CPU registers state.
 *
 * NB: The members here must have the same order, size and alignment as in isr_irq_stub.asm:cpu_state.
 */
struct cpu_state {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t cs;
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
    uint32_t ss;
    uint32_t eip;
    uint32_t eflags;
    uint32_t cr0;
    uint32_t cr2;
    uint32_t cr3;
    uint32_t cr4;
    uint32_t interrupt;
    uint32_t error_code;
} __packed;

/**
 * Get the CPU state.
 * \param state The CPU state.
 */
static inline void __always_inline get_cpu_state(struct cpu_state* regs)
{
    memsetw(regs, 0xBAAD, sizeof(*regs));
    asm(
        "call __get_cpu_state_inner_label_%=\n" /* Push current EIP. */
        "__get_cpu_state_inner_label_%=:\n"
        "pushfl\n"
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        "mov %%esi, %4\n"
        "mov %%edi, %5\n"
        :"=m"(regs->eax),
         "=m"(regs->ebx),
         "=m"(regs->ecx),
         "=m"(regs->edx),
         "=m"(regs->esi),
         "=m"(regs->edi)
        ::"esp", "memory"
    );
    asm(
        "mov %%ebp, %0\n"
        "xor %%eax, %%eax\n"
        "mov %%cs,  %%eax\n"
        "mov %%eax, %1\n"
        "mov %%ds,  %%eax\n"
        "mov %%eax, %2\n"
        "mov %%es,  %%eax\n"
        "mov %%eax, %3\n"
        "mov %%fs,  %%eax\n"
        "mov %%eax, %4\n"
        "mov %%gs, %%eax\n"
        "mov %%eax, %5\n"
        :"=m"(regs->ebp),
         "=m"(regs->cs),
         "=m"(regs->ds),
         "=m"(regs->fs),
         "=m"(regs->gs),
         "=m"(regs->es)
        ::"eax", "memory"
    );
    asm(
        "mov %%ss,  %%eax\n"
        "mov %%eax, %0\n"
        "mov %%cr0, %%eax\n"
        "mov %%eax, %1\n"
        "mov %%cr2, %%eax\n"
        "mov %%eax, %2\n"
        "mov %%cr3, %%eax\n"
        "mov %%eax, %3\n"
        "mov %%cr4, %%eax\n"
        :"=m"(regs->ss),
         "=m"(regs->cr0),
         "=m"(regs->cr2),
         "=m"(regs->cr3),
         "=m"(regs->cr4)
        ::"eax", "memory"
    );
    asm(
        "pop %%eax\n"     /* Retrieve EFLAGS. */
        "mov %%eax, %0\n"
        "pop %%eax\n"     /* Retrieve EIP. */
        "mov %%eax, %1\n"
        "mov %%esp, %2\n"
        :"=m"(regs->eflags),
         "=m"(regs->eip),
         "=m"(regs->esp)
        ::"eax", "esp", "memory"
    );
}

#endif /* ! REDSHIFT_HAL_CPU_STATE_H */

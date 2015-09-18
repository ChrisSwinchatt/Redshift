/* Copyright (c) 2012 Chris Swinchatt.
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
#include <debug/dump_registers.h>
#include <kernel/redshift.h>

static void dump_cr0(uint32_t cr0)
{
    printk("CR0:   [");
    if (test_bit(cr0, 0)) {
        printk("P");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 1)) {
        printk("M");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 2)) {
        printk("E");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 3)) {
        printk("T");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 4)) {
        printk("e");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 5)) {
        printk("N");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 16)) {
        printk("W");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 18)) {
        printk("A");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 29)) {
        printk("n");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 30)) {
        printk("C");
    } else {
        printk("-");
    }
    if (test_bit(cr0, 31)) {
        printk("p");
    } else {
        printk("-");
    }
    printk("]\n");
}

static void dump_eflags(uint32_t eflags)
{
    printk("FLAGS: [");
    if (test_bit(eflags, 0)) {
        printk("C");
    } else {
        printk("-");
    }
    printk("-");
    if (test_bit(eflags, 2)) {
        printk("P");
    } else {
        printk("-");
    }
    printk("-");
    if (test_bit(eflags, 4)) {
        printk("A");
    } else {
        printk("-");
    }
    printk("-");
    if (test_bit(eflags, 6)) {
        printk("Z");
    } else {
        printk("-");
    }
    if (test_bit(eflags, 7)) {
        printk("S");
    } else {
        printk("-");
    }
    if (test_bit(eflags, 8)) {
        printk("T");
    } else {
        printk("-");
    }
    if (test_bit(eflags, 9)) {
        printk("I");
    } else {
        printk("-");
    }
    if (test_bit(eflags, 10)) {
        printk("D");
    } else {
        printk("-");
    }
    if (test_bit(eflags, 11)) {
        printk("O");
    } else {
        printk("-");
    }
    printk("%0lud]\n", (test_bit(eflags, 12) << 1) | test_bit(eflags, 13));
}

void dump_registers(const struct cpu_state* state)
{
    printk("EAX:   0x%08lX EBX: 0x%08lX ECX: 0x%08lX EDX: 0x%08lX\n"
           "ESP:   0x%08lX EBP: 0x%08lX EDI: 0x%08lX ESI: 0x%08lX\n"
           "CS:    0x%04lX     DS:  0x%04lX     ES:  0x%04lX     FS:  0x%04lX\n"
           "GS:    0x%04lX     SS:  0x%04lX     CR2: 0x%08lX CR3: 0x%08lX\n"
           "EIP:   0x%08lX\n",
           state->eax, state->ebx, state->ecx, state->edx, state->esp,
           state->ebp, state->edi, state->esi, state->cs, state->ds, state->es,
           state->fs, state->gs, state->ss, state->cr2, state->cr3, state->eip);
    dump_cr0(state->cr0);
    dump_eflags(state->eflags);
}

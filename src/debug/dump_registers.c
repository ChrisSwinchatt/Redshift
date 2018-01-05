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
#include <redshift/debug/dump_registers.h>
#include <redshift/kernel.h>

static void dump_cr0(uint32_t cr0)
{
    printk(PRINTK_INFO "CR0:   [");
    if (TEST_BIT(cr0, 31)) {
        printk(PRINTK_INFO "p");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 30)) {
        printk(PRINTK_INFO "C");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 29)) {
        printk(PRINTK_INFO "n");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 18)) {
        printk(PRINTK_INFO "A");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 16)) {
        printk(PRINTK_INFO "W");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 5)) {
        printk(PRINTK_INFO "N");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 4)) {
        printk(PRINTK_INFO "e");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 3)) {
        printk(PRINTK_INFO "T");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 2)) {
        printk(PRINTK_INFO "E");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 1)) {
        printk(PRINTK_INFO "M");
    } else {
        printk("-");
    }
    if (TEST_BIT(cr0, 0)) {
        printk(PRINTK_INFO "P");
    } else {
        printk("-");
    }
    printk("]\n");
}

static void dump_eflags(uint32_t eflags)
{
    printk(PRINTK_INFO "FLAGS: [%0lud", (TEST_BIT(eflags, 12) << 1) | TEST_BIT(eflags, 13));
    if (TEST_BIT(eflags, 11)) {
        printk(PRINTK_INFO "O");
    } else {
        printk("-");
    }
    if (TEST_BIT(eflags, 10)) {
        printk(PRINTK_INFO "D");
    } else {
        printk("-");
    }
    if (TEST_BIT(eflags, 9)) {
        printk(PRINTK_INFO "I");
    } else {
        printk("-");
    }
    if (TEST_BIT(eflags, 8)) {
        printk(PRINTK_INFO "T");
    } else {
        printk("-");
    }
    if (TEST_BIT(eflags, 7)) {
        printk(PRINTK_INFO "S");
    } else {
        printk("-");
    }
    if (TEST_BIT(eflags, 6)) {
        printk(PRINTK_INFO "Z");
    } else {
        printk("-");
    }
    printk("-");
    if (TEST_BIT(eflags, 4)) {
        printk(PRINTK_INFO "A");
    } else {
        printk("-");
    }
    printk("-");
    if (TEST_BIT(eflags, 2)) {
        printk(PRINTK_INFO "P");
    } else {
        printk("-");
    }
    printk("-");
    if (TEST_BIT(eflags, 0)) {
        printk(PRINTK_INFO "C");
    } else {
        printk("-");
    }
    printk("]\n");
}

void dump_registers(const struct cpu_state* state)
{
    printk(
        "EAX: %08lX EBX: %08lX ECX: %08lX EDX: %08lX\n"
        "ESI: %08lX EDI: %08lX EBP: %08lX ESP: %08lX\n"
        "CS:  %04lX  EIP:    %08lX\n"
        "DS:  %04lX  CR0:    %08lX\n"
        "ES:  %04lX  CR2:    %08lX\n"
        "FS:  %04lX  CR3:    %08lX\n"
        "GS:  %04lX  CR4:    %08lX\n"
        "SS:  %04lX  EFLAGS: %08lX\n",
        state->eax,          state->ebx, state->ecx, state->edx,
        state->esi,          state->edi, state->ebp, state->esp,
        state->cs & 0xFFFF,  state->eip,
        state->ds & 0xFFFF,  state->cr0,
        state->es & 0xFFFF,  state->cr2,
        state->fs & 0xFFFF,  state->cr3,
        state->gs & 0xFFFF,  state->cr4,
        state->ss & 0xFFFF,  state->eflags
    );
    dump_cr0(state->cr0);
    dump_eflags(state->eflags);
}

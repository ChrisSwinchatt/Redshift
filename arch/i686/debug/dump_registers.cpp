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
    printk(PRINTK_INFO "CR0:    %08lX [ ", cr0);
    if (TEST_BIT(cr0, 0)) {
        printk(PRINTK_INFO "PE ");
    }
    if (TEST_BIT(cr0, 1)) {
        printk(PRINTK_INFO "MP ");
    }
    if (TEST_BIT(cr0, 2)) {
        printk(PRINTK_INFO "EM ");
    }
    if (TEST_BIT(cr0, 3)) {
        printk(PRINTK_INFO "TS ");
    }
    if (TEST_BIT(cr0, 4)) {
        printk(PRINTK_INFO "ET ");
    }
    if (TEST_BIT(cr0, 5)) {
        printk(PRINTK_INFO "NE ");
    }
    if (TEST_BIT(cr0, 16)) {
        printk(PRINTK_INFO "WP ");
    }
    if (TEST_BIT(cr0, 18)) {
        printk(PRINTK_INFO "AM ");
    }
    if (TEST_BIT(cr0, 29)) {
        printk(PRINTK_INFO "NW ");
    }
    if (TEST_BIT(cr0, 30)) {
        printk(PRINTK_INFO "CD ");
    }
    if (TEST_BIT(cr0, 31)) {
        printk(PRINTK_INFO "PG ");
    }
    printk("] %b\n", cr0);
}

static void dump_eflags(uint32_t eflags)
{
    printk(PRINTK_INFO "EFLAGS: %08lX [ ", eflags);
    if (TEST_BIT(eflags, 0)) {
        printk(PRINTK_INFO "CF ");
    }
    if (TEST_BIT(eflags, 2)) {
        printk(PRINTK_INFO "PF ");
    }
    if (TEST_BIT(eflags, 4)) {
        printk(PRINTK_INFO "AF ");
    }
    if (TEST_BIT(eflags, 6)) {
        printk(PRINTK_INFO "ZF ");
    }
    if (TEST_BIT(eflags, 7)) {
        printk(PRINTK_INFO "SF ");
    }
    if (TEST_BIT(eflags, 8)) {
        printk(PRINTK_INFO "TF ");
    }
    if (TEST_BIT(eflags, 9)) {
        printk(PRINTK_INFO "IF ");
    }
    if (TEST_BIT(eflags, 10)) {
        printk(PRINTK_INFO "DF ");
    }
    if (TEST_BIT(eflags, 11)) {
        printk(PRINTK_INFO "OF ");
    }
    if (TEST_BIT(eflags, 14)) {
        printk(PRINTK_INFO "NT ");
    }
    if (TEST_BIT(eflags, 16)) {
        printk(PRINTK_INFO "RF ");
    }
    if (TEST_BIT(eflags, 17)) {
        printk(PRINTK_INFO "VM ");
    }
    if (TEST_BIT(eflags, 18)) {
        printk(PRINTK_INFO "AC ");
    }
    if (TEST_BIT(eflags, 19)) {
        printk(PRINTK_INFO "VIF ");
    }
    if (TEST_BIT(eflags, 20)) {
        printk(PRINTK_INFO "VIP ");
    }
    if (TEST_BIT(eflags, 21)) {
        printk(PRINTK_INFO "ID ");
    }
    printk("] PL=%0lu %b\n", (TEST_BIT(eflags, 12) << 1) | TEST_BIT(eflags, 13), eflags);
}

void dump_registers(const struct cpu_state* state)
{
    SAVE_INTERRUPT_STATE;
    printk(
        "EAX: %08lX EBX: %08lX ECX: %08lX EDX: %08lX\n"
        "ESI: %08lX EDI: %08lX EBP: %08lX ESP: %08lX\n"
        "CS:  %04lX     DS:  %04lX     SS:  %04lX\n"
        "ES:  %04lX     FS:  %04lX     GS:  %04lX\n"
        "EIP: %08lX CR2: %08lX CR3: %08lX CR4: %08lX\n",
        state->eax,
        state->ebx,
        state->ecx,
        state->edx,
        state->esi,
        state->edi,
        state->ebp,
        state->esp,
        state->cs & 0xFFFF,
        state->ds & 0xFFFF,
        state->ss & 0xFFFF,
        state->es & 0xFFFF,
        state->fs & 0xFFFF,
        state->gs & 0xFFFF,
        state->eip,
        state->cr2,
        state->cr3,
        state->cr4
    );
    dump_cr0(state->cr0);
    dump_eflags(state->eflags);
    RESTORE_INTERRUPT_STATE;
}

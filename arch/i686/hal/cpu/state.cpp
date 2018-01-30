/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include <libk/memory.hpp>
#include <libk/asciz.hpp>
#include <redshift/kernel.hpp>

namespace redshift { namespace hal { namespace cpu_detail {
    void state::print()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        printk(
            "EAX: %08lX EBX: %08lX ECX: %08lX EDX: %08lX\n"
            "ESI: %08lX EDI: %08lX EBP: %08lX ESP: %08lX\n"
            "CS:  %04lX     DS:  %04lX     SS:  %04lX\n"
            "ES:  %04lX     FS:  %04lX     GS:  %04lX\n"
            "EIP: %08lX CR2: %08lX CR3: %08lX CR4: %08lX\n",
            eax,
            ebx,
            ecx,
            edx,
            esi,
            edi,
            ebp,
            esp,
            cs & 0xFFFF,
            ds & 0xFFFF,
            ss & 0xFFFF,
            es & 0xFFFF,
            fs & 0xFFFF,
            gs & 0xFFFF,
            eip,
            cr2,
            cr3,
            cr4
        );
        dump_cr0();
        dump_eflags();
    }

    void state::dump_cr0()
    {
        printk(PRINTK_INFO "CR0:    %08lX [ ", cr0);
        if (TEST_BIT(cr0, 0UL)) {
            printk(PRINTK_INFO "PE ");
        }
        if (TEST_BIT(cr0, 1UL)) {
            printk(PRINTK_INFO "MP ");
        }
        if (TEST_BIT(cr0, 2UL)) {
            printk(PRINTK_INFO "EM ");
        }
        if (TEST_BIT(cr0, 3UL)) {
            printk(PRINTK_INFO "TS ");
        }
        if (TEST_BIT(cr0, 4UL)) {
            printk(PRINTK_INFO "ET ");
        }
        if (TEST_BIT(cr0, 5UL)) {
            printk(PRINTK_INFO "NE ");
        }
        if (TEST_BIT(cr0, 16UL)) {
            printk(PRINTK_INFO "WP ");
        }
        if (TEST_BIT(cr0, 18UL)) {
            printk(PRINTK_INFO "AM ");
        }
        if (TEST_BIT(cr0, 29UL)) {
            printk(PRINTK_INFO "NW ");
        }
        if (TEST_BIT(cr0, 30UL)) {
            printk(PRINTK_INFO "CD ");
        }
        if (TEST_BIT(cr0, 31UL)) {
            printk(PRINTK_INFO "PG ");
        }
        printk("] %b\n", cr0);
    }

    void dump_eflags()
    {
        printk(PRINTK_INFO "EFLAGS: %08lX [ ", eflags);
        if (TEST_BIT(eflags, 0UL)) {
            printk(PRINTK_INFO "CF ");
        }
        if (TEST_BIT(eflags, 2UL)) {
            printk(PRINTK_INFO "PF ");
        }
        if (TEST_BIT(eflags, 4UL)) {
            printk(PRINTK_INFO "AF ");
        }
        if (TEST_BIT(eflags, 6UL)) {
            printk(PRINTK_INFO "ZF ");
        }
        if (TEST_BIT(eflags, 7UL)) {
            printk(PRINTK_INFO "SF ");
        }
        if (TEST_BIT(eflags, 8UL)) {
            printk(PRINTK_INFO "TF ");
        }
        if (TEST_BIT(eflags, 9UL)) {
            printk(PRINTK_INFO "IF ");
        }
        if (TEST_BIT(eflags, 10UL)) {
            printk(PRINTK_INFO "DF ");
        }
        if (TEST_BIT(eflags, 11UL)) {
            printk(PRINTK_INFO "OF ");
        }
        if (TEST_BIT(eflags, 14UL)) {
            printk(PRINTK_INFO "NT ");
        }
        if (TEST_BIT(eflags, 16UL)) {
            printk(PRINTK_INFO "RF ");
        }
        if (TEST_BIT(eflags, 17UL)) {
            printk(PRINTK_INFO "VM ");
        }
        if (TEST_BIT(eflags, 18UL)) {
            printk(PRINTK_INFO "AC ");
        }
        if (TEST_BIT(eflags, 19UL)) {
            printk(PRINTK_INFO "VIF ");
        }
        if (TEST_BIT(eflags, 20UL)) {
            printk(PRINTK_INFO "VIP ");
        }
        if (TEST_BIT(eflags, 21UL)) {
            printk(PRINTK_INFO "ID ");
        }
        printk("] PL=%0lu %b\n", (TEST_BIT(eflags, 12) << 1) | TEST_BIT(eflags, 13), eflags);
    }
}}} // redshift::hal::cpu_detail

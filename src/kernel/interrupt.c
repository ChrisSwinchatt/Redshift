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
#include <debug/dump_hex.h>
#include <kernel/asm.h>
#include <kernel/console.h>
#include <kernel/cpu_state.h>
#include <kernel/interrupt.h>
#include <kernel/redshift.h>

#define ISR_HANDLERS_SIZE 256

isr_handler_t isr_handlers[ISR_HANDLERS_SIZE];

void set_interrupt_handler(uint8_t n, isr_handler_t fp)
{
    isr_handlers[n] = fp;
}

static int call_interrupt_handler(const struct cpu_state* regs)
{
    isr_handler_t handler = isr_handlers[regs->interrupt];
    if (handler) {
        printk(PRINTK_DEBUG "Calling handler at %p for interrupt 0x%02lX\n", handler, regs->interrupt);
        handler(regs);
        return 1;
    } else if (regs->interrupt < 32) { /* Exception. */
        printk(PRINTK_WARNING "Warning: No interrupt handler for exception 0x%02lX\n", regs->interrupt);
    }
    printk(PRINTK_DEBUG "No handler for interrupt 0x%02lX\n", regs->interrupt);
    return 0;
}

typedef enum {
    ISR_TYPE_ABORT,
    ISR_TYPE_FAULT,
    ISR_TYPE_INTERRUPT,
    ISR_TYPE_RESERVED,
    ISR_TYPE_TRAP
} isr_type_t;

static struct isr_info {
    isr_type_t type;
    bool has_error_code;
    const char* str;
} isr_info[] = {
    { ISR_TYPE_FAULT,     false,                "divide-by-zero" },
    { ISR_TYPE_FAULT,     false,                         "debug" },
    { ISR_TYPE_INTERRUPT, false,        "non-maskable interrupt" },
    { ISR_TYPE_TRAP,      false,                    "breakpoint" },
    { ISR_TYPE_TRAP,      false,                      "overflow" },
    { ISR_TYPE_FAULT,     false,          "bound-range exceeded" },
    { ISR_TYPE_FAULT,     false,                "invalid Opcode" },
    { ISR_TYPE_FAULT,     false,          "device not available" },
    { ISR_TYPE_ABORT,      true,                  "double fault" },
    { ISR_TYPE_FAULT,     false,   "coprocessor segment overrun" },
    { ISR_TYPE_FAULT,      true,                   "invalid TSS" },
    { ISR_TYPE_FAULT,      true,           "segment not present" },
    { ISR_TYPE_FAULT,      true,           "stack-segment fault" },
    { ISR_TYPE_FAULT,      true,      "general protection fault" },
    { ISR_TYPE_FAULT,      true,                    "page fault" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_FAULT,     false,      "floating-point exception" },
    { ISR_TYPE_FAULT,      true,               "alignment check" },
    { ISR_TYPE_ABORT,     false,                 "machine check" },
    { ISR_TYPE_FAULT,     false, "SIMD floating-point exception" },
    { ISR_TYPE_FAULT,     false,      "virtualization exception" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_FAULT,     false,            "security exception" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_FAULT,     false,                  "triple fault" }, /* For completeness - can't actually be reported. */
    { ISR_TYPE_INTERRUPT, false,                     "FPU error" }
};

void isr_handler(const struct cpu_state* regs)
{
    static int counter = 0;
    struct isr_info info;
    if ((regs->interrupt > arraysize(isr_info)) || ((info = isr_info[regs->interrupt]).type == ISR_TYPE_INTERRUPT)) {
        counter = 0;
        call_interrupt_handler(regs);
        return;
    }
    switch (info.type) {
        case ISR_TYPE_ABORT:
            if (info.has_error_code) {
                panic("%s (error code = %bb; eip = 0x%08lX)\n", info.str, regs->errorcode, regs->eip);
            } else {
                panic("%s (eip = 0x%08lX)\n", info.str, regs->eip);
            }
            break;
        case ISR_TYPE_FAULT:
            if (!(call_interrupt_handler(regs))) {
                printk(PRINTK_WARNING "Error: %s ", info.str);
            }
            break;
        case ISR_TYPE_TRAP:
            printk(PRINTK_ERROR "Error: %s ", info.str);
            break;
        default:
          break;
    }
    if (info.has_error_code) {
        printk(PRINTK_DEBUG "(error code = %08bb, ", regs->errorcode);
    } else {
        printk(PRINTK_DEBUG "(");
    }
    printk(PRINTK_DEBUG "eip = 0x%08lX)\n", regs->eip);
    if (counter > 1) {
        printk("Hex dump follows.\n");
        dump_hex(regs->eip - 32, 80);
        panic("too many errors.\n");
    }
    ++counter;
}

void irq_handler(const struct cpu_state* regs)
{
    if (regs->interrupt >= 40) {
        io_outb(PIC_SLAVE_CMND, PIC_RESET);
    }
    io_outb(PIC_MASTER_CMND, PIC_RESET);
    call_interrupt_handler(regs);
}

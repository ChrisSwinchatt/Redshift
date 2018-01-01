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
#include <redshift/debug/dump_hex.h>
#include <redshift/debug/dump_registers.h>
#include <redshift/debug/dump_stack.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/console.h>
#include <redshift/hal/cpu.h>
#include <redshift/kernel/interrupt.h>
#include <redshift/kernel.h>

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
        handler(regs);
        return 1;
    } else {
        printk(PRINTK_DEBUG "No handler for interrupt 0x%02lX\n", regs->interrupt);
    }
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
    isr_type_t  type;
    bool        has_error_code;
    const char* message;
} isr_info[] = {
    { ISR_TYPE_FAULT,     false,                "divide-by-zero" },
    { ISR_TYPE_FAULT,     false,                         "debug" },
    { ISR_TYPE_INTERRUPT, false,        "non-maskable interrupt" },
    { ISR_TYPE_TRAP,      false,                    "breakpoint" },
    { ISR_TYPE_TRAP,      false,                      "overflow" },
    { ISR_TYPE_FAULT,     false,          "bound-range exceeded" },
    { ISR_TYPE_FAULT,     false,                "invalid opcode" },
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
    { ISR_TYPE_FAULT,     true,             "security exception" },
    { ISR_TYPE_RESERVED,  false,                              "" },
    { ISR_TYPE_FAULT,     false,                  "triple fault" }, /* For completeness - can't actually be reported. */
    { ISR_TYPE_INTERRUPT, false,                     "FPU error" }
};

void isr_handler(const struct cpu_state* regs)
{
    static int counter = 0;
    if (regs->interrupt >= ARRAY_SIZE(isr_info)) {
        counter = 0;
        call_interrupt_handler(regs);
        return;
    }
    /* Exception.
     */
     struct isr_info info = isr_info[regs->interrupt];
     printk(PRINTK_ERROR "Interrupt 0x%02lX - %s: ", regs->interrupt, info.message);
     if (info.has_error_code) {
         printk(PRINTK_ERROR "(error code = %08bb, ", regs->errorcode);
     } else {
         printk(PRINTK_ERROR "(");
     }
     printk(PRINTK_ERROR "eip = 0x%08lX)\n", regs->eip);
     printk(PRINTK_INFO "CPU state before interrupt occurred:\n");
     dump_registers(regs);
     dump_stack();
     while (true)
        ;
     if (counter > 1) {
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

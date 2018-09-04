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
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/console.hpp>
#include <redshift/hal/cpu.hpp>
#include <redshift/kernel/interrupt.hpp>
#include <redshift/kernel.hpp>

#define ISR_HANDLERS_SIZE 256

isr_handler_t isr_handlers[ISR_HANDLERS_SIZE];

void set_interrupt_handler(uint8_t n, isr_handler_t fp)
{
    isr_handlers[n] = fp;
}

static int call_interrupt_handler(const cpu::state* regs)
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

const struct isr_info isr_info[] = {
    { ISR_TYPE_FAULT,     false, "Divide-by-zero"                },
    { ISR_TYPE_FAULT,     false, "Debug"                         },
    { ISR_TYPE_INTERRUPT, false, "Non-maskable interrupt"        },
    { ISR_TYPE_TRAP,      false, "Breakpoint"                    },
    { ISR_TYPE_TRAP,      false, "Overflow"                      },
    { ISR_TYPE_FAULT,     false, "Bound-range exceeded"          },
    { ISR_TYPE_FAULT,     false, "Invalid opcode"                },
    { ISR_TYPE_FAULT,     false, "Device not available"          },
    { ISR_TYPE_ABORT,      true, "Double fault"                  },
    { ISR_TYPE_FAULT,     false, "Coprocessor segment overrun"   },
    { ISR_TYPE_FAULT,      true, "Invalid TSS"                   },
    { ISR_TYPE_FAULT,      true, "Segment not present"           },
    { ISR_TYPE_FAULT,      true, "Stack-segment fault"           },
    { ISR_TYPE_FAULT,      true, "General protection fault"      },
    { ISR_TYPE_FAULT,      true, "Page fault"                    },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_FAULT,     false, "Floating-point"                },
    { ISR_TYPE_FAULT,      true, "Alignment check"               },
    { ISR_TYPE_ABORT,     false, "Machine check"                 },
    { ISR_TYPE_FAULT,     false, "SIMD floating-point"           },
    { ISR_TYPE_FAULT,     false, "Virtualization"                },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_FAULT,     true, "Security"                       },
    { ISR_TYPE_RESERVED,  false, ""                              },
    { ISR_TYPE_FAULT,     false, ""                              }, // Triple fault - uncatchable so can't be reported.
    { ISR_TYPE_INTERRUPT, false, "FPU error"                     }
};

static void handle_exception(const cpu::state* regs)
{
    struct isr_info info = isr_info[regs->interrupt];
    panic_with_state(
        regs,
        "%s: <interrupt=0x%02lX,error_code=%03bb>",
        info.message,
        regs->interrupt,
        regs->error_code & 0x7 // 3 LSBs contain the EXT (bit 0), IDT (1) and TI (2) flags.
    );
}

void isr_handler(const cpu::state* regs)
{
    if (regs->interrupt < array_size(isr_info)) {
        handle_exception(regs);
    } else {
        call_interrupt_handler(regs);
    }
}

void irq_handler(const cpu::state* regs)
{
    if (regs->interrupt >= 40) {
        io_outb(PIC_SLAVE_CMND, PIC_RESET);
    }
    io_outb(PIC_MASTER_CMND, PIC_RESET);
    call_interrupt_handler(regs);
}

namespace redshift { namespace kernel {
    void interrupt_state_guard::set_interrupt_state(interrupt_state state)
    {
        switch (state) {
            case interrupt_state::enabled:  enable_interrupts();  break;
            case interrupt_state::disabled: disable_interrupts(); break;
            default: UNREACHABLE("No switch case for state=%d", static_cast<int>(state));
        }
    }
}} // redshift::kernel

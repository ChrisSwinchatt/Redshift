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
#include <redshift/boot/pit.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/interrupt.hpp>
#include <redshift/kernel/timer.hpp>
#include <redshift/sched/process.hpp>

namespace redshift { namespace hal {
    int pit::set_frequency(uint32_t freq)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        static int handler_registered = 0;
        if (!(handler_registered)) {
            set_interrupt_handler(IRQ0, &pit_handler);
            handler_registered = 1;
        }
        if (!(freq)) {
            return -1;
        }
        uint32_t divisor = 1193180UL / freq;
        io::outb(io::port::PIT_CMND, 0x36);
        io::outb(io::port::PIT_DATA, static_cast<uint8_t>((divisor >> 0) & 0xFF));
        io::outb(io::port::PIT_DATA, static_cast<uint8_t>((divisor >> 8) & 0xFF));
        return 0;
    }

    void pit::interrupt_handler(const struct cpu_state* regs)
    {
        timer::process_queue(1000 / TICK_RATE);
        UNUSED(regs);
    }
}} // redshift::hal

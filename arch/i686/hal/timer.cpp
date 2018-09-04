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
#include <libk/asciz.hpp>
#include <redshift/hal/cpu.hpp>
#include <redshift/hal/io.hpp>
#include <redshift/hal/timer.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/interrupt.hpp>

namespace redshift { namespace hal {
    timer::event::event(const char* name_, uint64_t period_, event_callback& callback_)
    : name(name_)
    , name_hash(libk::asciz::hash32(name, asciz::length(name)))
    , period(period_)
    , callback(callback_)
    {
        ;
    }

    void timer::init()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t divisor = 1193180UL / TICK_RATE;
        set_interrupt_handler(IRQ0, timer::process_queue);
        io::outb(io::port::PIT_CMND, 0x36);
        io::outb(io::port::PIT_DATA, static_cast<uint8_t>((divisor >> 0) & 0xFF));
        io::outb(io::port::PIT_DATA, static_cast<uint8_t>((divisor >> 8) & 0xFF));
    }

    void timer::add_event(const event& event_)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        m_queue.append(event_);
    }

    void timer::add_event(const char* name, uint64_t period, event_callback& callback)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        add_event(event(
            libk::asciz::duplicate(name),
            period,
            callback
        };
        m_queue.append(e);
    }

    void timer::remove_event(const event& event_)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        m_queue.remove(event_);
    }

    void timer::remove_event(const char* name)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t hash = libk::asciz::hash32(name);
        for (size_t i = 0; i < m_queue.size(); ++i) {
            if (m_queue[i].name_hash == hash) {
                remove_event(m_queue[i]);
                return;
            }
        }
    }

    void timer::process_queue(cpu::state* regs)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        for (size_t i = 0; i < m_queue.size(); ++i) {
            m_queue[i].elapsed_time += TICK_USEC;
            if (m_queue[i].elapsed_time >= m_queue[i].period) {
                // Call event.
                printk(PRINTK_DEBUG "Event \"%s\" (fn=0x%p) raised\n", m_queue[i].name, m_queue[i].callback);
                const bool keep = m_queue[i].callback());
                m_queue[i].elapsed_time = 0;
                if (!(keep)) {
                    remove_event(m_queue[i]);
                }
            }
        }
        UNUSED(regs);
    }
}}

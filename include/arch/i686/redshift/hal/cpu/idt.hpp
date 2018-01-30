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
#ifndef REDSHIFT_HAL_CPU_IDT_HPP
#define REDSHIFT_HAL_CPU_IDT_HPP

#include <redshift/kernel.hpp>

namespace redshift { namespace hal { namespace cpu_ {
    class idt {
    public:
        static constexpr size_t MAX_ENTRIES = 256;

        static void init();

        static void set_gate(uint8_t i, uint32_t base, uint16_t selector, uint8_t flags);

        static void load();
    private:
        struct idt_entry {
            uint16_t base_low;
            uint16_t selector;
            uint8_t  reserved;
            uint8_t  flags;
            uint16_t base_high;
        } __packed;

        struct idt_ptr {
            uint16_t limit;
            uint32_t base;
        } __packed;

        static idt_entry m_entries[MAX_ENTRIES];
        static idt_ptr   m_pointer;
    };
}}} // redshift::hal::idt_

extern "c" void isr0();
extern "c" void isr1();
extern "c" void isr2();
extern "c" void isr3();
extern "c" void isr4();
extern "c" void isr5();
extern "c" void isr6();
extern "c" void isr7();
extern "c" void isr8();
extern "c" void isr9();
extern "c" void isr10();
extern "c" void isr11();
extern "c" void isr12();
extern "c" void isr13();
extern "c" void isr14();
extern "c" void isr15();
extern "c" void isr16();
extern "c" void isr17();
extern "c" void isr18();
extern "c" void isr19();
extern "c" void isr20();
extern "c" void isr21();
extern "c" void isr22();
extern "c" void isr23();
extern "c" void isr24();
extern "c" void isr25();
extern "c" void isr26();
extern "c" void isr27();
extern "c" void isr28();
extern "c" void isr29();
extern "c" void isr30();
extern "c" void isr31();
extern "c" void irq0();
extern "c" void irq1();
extern "c" void irq2();
extern "c" void irq3();
extern "c" void irq4();
extern "c" void irq5();
extern "c" void irq6();
extern "c" void irq7();
extern "c" void irq8();
extern "c" void irq9();
extern "c" void irq10();
extern "c" void irq11();
extern "c" void irq12();
extern "c" void irq13();
extern "c" void irq14();
extern "c" void irq15();

#endif // ! REDSHIFT_HAL_CPU_IDT_HPP

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
#include <redshift/hal/cpu/idt.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel.hpp>

extern "C" void loadidt(uint32_t addr); // loadidt.asm

namespace redshift { namespace hal { namespace cpu_ {
    constexpr size_t MAX_ENTRIES = 256;

    void idt::init()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        m_pointer.limit = (sizeof(m_entries[0]) * IDT_ENTRIES_SIZE) - 1;
        m_pointer.base  = (uint32_t)&idt_entries;
        libk::memory::fill8(&idt_entries, 0, sizeof(m_entries[0]) * idt::MAX_ENTRIES);
        set_gate( 0,  (uint32_t)isr0, 0x08, 0x8E);
        set_gate( 1,  (uint32_t)isr1, 0x08, 0x8E);
        set_gate( 2,  (uint32_t)isr2, 0x08, 0x8E);
        set_gate( 3,  (uint32_t)isr3, 0x08, 0x8E);
        set_gate( 4,  (uint32_t)isr4, 0x08, 0x8E);
        set_gate( 5,  (uint32_t)isr5, 0x08, 0x8E);
        set_gate( 6,  (uint32_t)isr6, 0x08, 0x8E);
        set_gate( 7,  (uint32_t)isr7, 0x08, 0x8E);
        set_gate( 8,  (uint32_t)isr8, 0x08, 0x8E);
        set_gate( 9,  (uint32_t)isr9, 0x08, 0x8E);
        set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
        set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
        set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
        set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
        set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
        set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
        set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
        set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
        set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
        set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
        set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
        set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
        set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
        set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
        set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
        set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
        set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
        set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
        set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
        set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
        set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
        set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
        set_gate(32,  (uint32_t)irq0, 0x08, 0x8E);
        set_gate(33,  (uint32_t)irq1, 0x08, 0x8E);
        set_gate(34,  (uint32_t)irq2, 0x08, 0x8E);
        set_gate(35,  (uint32_t)irq3, 0x08, 0x8E);
        set_gate(36,  (uint32_t)irq4, 0x08, 0x8E);
        set_gate(37,  (uint32_t)irq5, 0x08, 0x8E);
        set_gate(38,  (uint32_t)irq6, 0x08, 0x8E);
        set_gate(39,  (uint32_t)irq7, 0x08, 0x8E);
        set_gate(40,  (uint32_t)irq8, 0x08, 0x8E);
        set_gate(41,  (uint32_t)irq9, 0x08, 0x8E);
        set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
        set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
        set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
        set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
        set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
        set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
        load((uint32_t)&pidt);

    }

    void idt::set_gate(uint8_t i, uint32_t base, uint16_t selector, uint8_t flags)
    {
        DEBUG_ASSERT(i < idt::MAX_ENTRIES);
        idt_entries[i].base_low  = base & 0xFFFF;
        idt_entries[i].base_high = (base >> 16) & 0xFFFF;
        idt_entries[i].selector  = selector;
        idt_entries[i].reserved  = 0;
        idt_entries[i].flags     = flags;
    }

    void idt::load()
    {
        loadidt(reinterpret_cast<uintptr_t>(&m_pointer));
    }
}}} // redshift::hal::idt_

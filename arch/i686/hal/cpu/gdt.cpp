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
#include <redshift/kernel.hpp>
#include <redshift/hal/cpu/gdt.hpp>

extern "C" void loadgdt(uintptr_t addr); // loadgdt.S

namespace redshift { namespace hal { namespace cpu_detail {
    void gdt::init()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        const uint32_t tss_base  = get_tss_base();
        const uint32_t tss_limit = tss_base + get_tss_size();
        DEBUG_ASSERT(tss_limit > tss_base);
        m_pointer.limit = (sizeof(*gdt_entries)*gdt::MAX_ENTRIES) - 1;
        m_pointer.base = (uint32_t)&gdt_entries;
        set_gate(0, 0x00000000, 0x00000000,  0x00, 0x00); // Null descriptor.
        set_gate(1, 0x00000000, 0xFFFFFFFF,  0x9A, 0xCF); // Kernel code segment.
        set_gate(2, 0x00000000, 0xFFFFFFFF,  0x92, 0xCF); // Kernel data segment.
        set_gate(3, 0x00000000, 0xFFFFFFFF,  0xFA, 0xCF); // User-mode code segment.
        set_gate(4, 0x00000000, 0xFFFFFFFF,  0xF2, 0xCF); // User-mode data segment.
        set_gate(5, tss_base,   tss_limit,   0x89, 0x40); // TSS
        load();
    }

    void gdt::set_gate(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
    {
        DEBUG_ASSERT(i < GDT_ENTRIES_MAX);
        m_entries[i].base_low     = (base & 0xFFFF);
        m_entries[i].base_mid     = (base >> 16) & 0xFF;
        m_entries[i].base_high    = (base >> 24) & 0xFF;
        m_entries[i].limit_low    = (limit & 0xFFFF);
        m_entries[i].granularity  = (limit >> 16) & 0x0F;
        m_entries[i].granularity |= gran & 0xF0;
        m_entries[i].access       = access;
    }

    void gdt::load()
    {
        loadgdt(reinterpret_cast<uintptr_t>(&m_pointer));
    }
}}} // redshift::hal::cpu_detail

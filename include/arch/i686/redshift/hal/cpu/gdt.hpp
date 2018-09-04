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
#ifndef REDSHIFT_HAL_CPU_GDT_HPP
#define REDSHIFT_HAL_CPU_GDT_HPP

#include <redshift/kernel.hpp>

namespace redshift { namespace hal { namespace cpu_detail {
    /// GDT.
    class gdt {
    public:
        /// Maximum number of entries.
        static constexpr size_t MAX_ENTRIES = 8;

        gdt(const gdt&) = delete;

        static void init();

        /// Set GDT gate.
        /// \param i Index.
        /// \param base Start address.
        /// \param limit End address.
        /// \param access Access flags.
        /// \param gran Granularity.
        static void set_gate(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

        /// Load GDT.
        static void load();
    private:
        struct gdt_entry {
            uint16_t limit_low;
            uint16_t base_low;
            uint8_t  base_mid;
            uint8_t  access;
            uint8_t  granularity;
            uint8_t  base_high;
        } __packed;

        struct gdt_ptr {
            uint16_t limit;
            uint32_t base;
        } __packed;

        static gdt_entry m_entries[MAX_ENTRIES];
        static gdt_ptr   m_pointer;
    };
}}} // redshift::hal::cpu_detail


#endif // ! REDSHIFT_HAL_CPU_GDT_HPP

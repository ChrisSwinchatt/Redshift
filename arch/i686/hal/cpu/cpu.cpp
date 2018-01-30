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
#include <redshift/hal/cpu.hpp>

namespace redshift { namespace hal {
    void cpu::init()
    {
        gdt::init();
        idt::init();
        pic::init();
        tss::init();
    }

    uint64_t cpu::read_ticks()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t hi = 0, lo = 0;
        asm("xorl %%eax, %%eax\ncpuid":::"%eax", "%ebx", "%ecx", "%edx");
        asm("rdtsc":"=a"(lo), "=d"(hi));
        return (static_cast<uint64_t>(hi) << 32ULL) | static_cast<uint64_t>(lo);
    }

    bool cpu::has_feature(cpu::feature_t feature)
    {
        return TEST_FLAG(m_cpuid.features, (uint32_t)feature);
    }

    bool cpu::has_ext_feature(cpu::feature_ext_t feature)
    {
        return TEST_FLAG(m_cpuid.features_ext, (uint32_t)feature);
    }

    const char* cpu::get_vendor()
    {
        return m_cpuid.vendor;
    }

    const char* cpu::get_vendor_string()
    {
        return m_cpuid.vendor_string;
    }

    const char* cpu::get_brand_string()
    {
        return m_cpuid.brand_string;
    }

    uint32_t cpu::get_features()
    {
        return m_cpuid.features;
    }

    uint32_t cpu::get_ext_features()
    {
        return m_cpuid.features_ext;
    }

    unsigned cpu::get_version_stepping()
    {
        return m_cpuid.stepping;
    }

    unsigned cpu::get_version_model()
    {
        return m_cpuid.model;
    }

    unsigned cpu::get_version_family()
    {
        return m_cpuid.family;
    }

    unsigned cpu::get_version_type()
    {
        return m_cpuid.type;
    }

    uint32_t cpu::get_l1_cache_size()
    {
        return m_cpuid.l1_cache;
    }

    uint32_t cpu::get_l2_cache_size()
    {
        return m_cpuid.l2_cache;
    }

    uint32_t cpu::get_l3_cache_size()
    {
        return m_cpuid.l3_cache;
    }

    uint64_t cpu::get_clock_frequency()
    {
        return m_cpuid.frequency;
    }

    uint32_t cpu::get_logical_cores_count()
    {
        return m_cpuid.logical_cores;
    }

    uint32_t cpu::get_physical_cores_count()
    {
        return m_cpuid.physical_cores;
    }
}} // redshift::hal

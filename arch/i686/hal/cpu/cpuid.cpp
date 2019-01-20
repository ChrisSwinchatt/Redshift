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
#include <redshift/hal/cpu/cpuid.hpp>
#include <redshift/hal/cpu/features.hpp>
#include <redshift/hal/cpu/vendor.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/interrupt.hpp>
#include <redshift/kernel/sleep.hpp>

namespace asciz = libk::asciz;

extern "C" int cpuid_supported(); // cpuid_supported.asm

using redshift::kernel::interrupt_state;
using redshift::kernel::interrupt_state_guard;

namespace redshift { namespace hal { namespace cpu_detail {
    cpuid::cpuid()
    : features(0)
    , features_ext(0)
    , stepping(0)
    , model(0)
    , family(0)
    , type(0)
    , l1_cache(0)
    , l2_cache(0)
    , l3_cache(0)
    , frequency(0)
    , logical_cores(0)
    , physical_cores(0)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        if (!(cpuid_supported()) || !(extended_supported())) {
            panic("necessary CPUID functions not supported by CPU");
        }
        get_vendor_string();
        get_vendor();
        get_features();
        get_brand_string();
        get_cache();
        get_frequency();
        get_cores();
        printk(PRINTK_DEBUG "CPU info: %s %s (%s)\n", brand_string, vendor, vendor_string);
        printk(PRINTK_DEBUG "CPU info: <type=%d,family=%d,model=%d,stepping=%d>\n",
               type, family, model, stepping);
        return 0;
    }

    enum { EAX, EBX, ECX, EDX };

    void cpuid::do_cpuid(uint32_t(* regs)[4])
    {
        __asm__ __volatile__(
            "cpuid"
            :"=a"((*regs)[EAX]), "=b"((*regs)[EBX]),
             "=c"((*regs)[ECX]), "=d"((*regs)[EDX])
            : "a"((*regs)[EAX]),  "b"((*regs)[EBX]),
              "c"((*regs)[ECX]),  "d"((*regs)[EDX])
        );
    }

    bool cpuid::extended_supported()
    {
        uint32_t regs[4] = { cpuid::request::extended_max_function, 0, 0, 0};
        do_cpuid(&regs);
        return regs[EAX] >= cpuid::request::extended_max_function;
    }

    void cpuid::get_vendor_string()
    {
        uint32_t regs[4] = {0};
        do_cpuid(&regs);
        asciz::copy(vendor_string,   reinterpret_cast<char*>(&regs[EBX]), 4);
        asciz::concat(vendor_string, reinterpret_cast<char*>(&regs[EDX]), 4);
        asciz::concat(vendor_string, reinterpret_cast<char*>(&regs[ECX]), 4);
        vendor_string[cpuid::VENDOR_STRING_MAX - 1] = 0;
    }

    void cpuid::get_vendor()
    {
        if ((asciz::compare(vendor_string, cpuid::VENDOR_AMD_NEW, cpuid::VENDOR_STRING_MAX) == 0) ||
                asciz::compare(vendor_string, cpuid::VENDOR_AMD_OLD, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "AMD", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_CENTAUR, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Centaur", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_CYRIX, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Cyrix", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_INTEL, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Intel", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_TRANSMETA1, cpuid::VENDOR_STRING_MAX) == 0 ||
                   asciz::compare(vendor_string, cpuid::VENDOR_TRANSMETA2, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Transmeta", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_NSC, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "NSC", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_NEXGEN, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "NexGen", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_RISE, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Rise", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_SIS, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "SiS", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_UMC, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "UMC", cpuid::VENDOR_MAX);
        } else if (asciz::compare(vendor_string, cpuid::VENDOR_VIA, cpuid::VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "VIA", cpuid::VENDOR_MAX);
        } else {
            asciz::copy(vendor, "Unknown", CPUID_VENDOR_MAX);
        }
    }


    void cpuid::get_features()
    {
        uint32_t regs[4] = {cpuid::request::info_features, 0, 0, 0};
        do_cpuid(&regs);
        stepping     = (regs[EAX] & 0x0f);
        model        = (regs[EAX] >> 4) + (regs[EAX] >> 16);
        family       = (regs[EAX] >> 8) + ((regs[EAX] >> 20) << 4);
        type         = (regs[EAX] >> 12);
        features     = regs[ECX];
        features_ext = regs[EDX];
    }

    void cpuid::get_brand_string()
    {
        uint32_t regs[4] = {cpuid::request::extended_brandstring_1, 0, 0, 0};
        do_cpuid(&regs);
        asciz::copy(brand_string,   (const char*)&regs[EAX], 4);
        asciz::concat(brand_string, (const char*)&regs[EBX], 4);
        asciz::concat(brand_string, (const char*)&regs[ECX], 4);
        asciz::concat(brand_string, (const char*)&regs[EDX], 4);
        regs[EAX] = cpuid::request::extended_brandstring_2;
        do_cpuid(&regs);
        asciz::concat(brand_string, (const char*)&regs[EAX], 4);
        asciz::concat(brand_string, (const char*)&regs[EBX], 4);
        asciz::concat(brand_string, (const char*)&regs[ECX], 4);
        asciz::concat(brand_string, (const char*)&regs[EDX], 4);
        regs[EAX] = cpuid::request::extended_brandstring_3;
        do_cpuid(&regs);
        asciz::concat(brand_string, (const char*)&regs[EAX], 4);
        asciz::concat(brand_string, (const char*)&regs[EBX], 4);
        asciz::concat(brand_string, (const char*)&regs[ECX], 4);
        asciz::concat(brand_string, (const char*)&regs[EDX], 4);
    }

    void cpuid::get_cache()
    {
        // TODO.
        UNUSED(info);
    }

    void cpuid::get_frequency()
    {
        const uint64_t period = 10ULL; // ms
        uint64_t ticks1 = read_ticks();
        msleep(period);
        uint64_t ticks2 = read_ticks();
        frequency = (uint64_t)((ticks2 - ticks1)*1000ULL/period);
        if (frequency == 0) {
            frequency = 1;
        }
    }

    void cpuid::get_cores()
    {
        // TODO.

        UNUSED(info);
    }
}}}

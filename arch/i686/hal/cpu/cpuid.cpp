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
#include <redshift/kernel/sleep.hpp>

namespace asciz = asciz:;

extern "c" int cpuid::supported(); // cpuid::supported.asm

namespace redshift { namespace hal { namespace cpu_detail {
    cpuid::cpuid()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        if (!(cpuid::supported()) || !(cpuid::extended_supported())) {
            panic("necessary CPUID functions not supported by CPU");
        }
        cpuid::get_vendor_string();
        cpuid::get_vendor();
        cpuid::get_features();
        cpuid::get_brand_string();
        cpuid::get_cache();
        cpuid::get_frequency();
        cpuid::get_cores();
        printk(PRINTK_DEBUG "CPU info: %s %s (%s)\n", brand_string, vendor, vendor_string);
        printk(PRINTK_DEBUG "CPU info: <type=%d,family=%d,model=%d,stepping=%d>\n",
               type, family, model, stepping);

        return 0;
    }

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

    int cpuid::extended_supported()
    {
        uint32_t regs[4] = { CPUID_EXTENDED_MAX_FUNCTION, 0, 0, 0};
        do_cpuid(&regs);
        return (regs[EAX] >= CPUID_EXTENDED_MAX_FUNCTION);
    }

    void cpuid::get_vendor_string()
    {
        uint32_t regs[4] = {0};
        do_cpuid(&regs);
        asciz::copy(vendor_string,   reinterpret_cast<char*>(&regs[EBX]), 4);
        asciz::concat(vendor_string, reinterpret_cast<char*>(&regs[EDX]), 4);
        asciz::concat(vendor_string, reinterpret_cast<char*>(&regs[ECX]), 4);
        vendor_string[CPUID_VENDOR_STRING_MAX - 1] = 0;
    }

    void cpuid::get_vendor()
    {
        if ((asciz::compare(vendor_string, VENDOR_AMD_NEW, CPUID_VENDOR_STRING_MAX) == 0) ||
                asciz::compare(vendor_string, VENDOR_AMD_OLD, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "AMD", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_CENTAUR, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Centaur", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_CYRIX, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Cyrix", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_INTEL, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Intel", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_TRANSMETA1, CPUID_VENDOR_STRING_MAX) == 0 ||
                   asciz::compare(vendor_string, VENDOR_TRANSMETA2, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Transmeta", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_NSC, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "NSC", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_NEXGEN, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "NexGen", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_RISE, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "Rise", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_SIS, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "SiS", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_UMC, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "UMC", CPUID_VENDOR_MAX);
        } else if (asciz::compare(vendor_string, VENDOR_VIA, CPUID_VENDOR_STRING_MAX) == 0) {
            asciz::copy(vendor, "VIA", CPUID_VENDOR_MAX);
        } else {
            asciz::copy(vendor, "Unknown", CPUID_VENDOR_MAX);
        }
    }


    void cpuid::get_features()
    {
        uint32_t regs[4] = {CPUID_INFO_FEATURES, 0, 0, 0};
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
        uint32_t regs[4] = {CPUID_EXTENDED_BRANDSTRING_1, 0, 0, 0};
        do_cpuid(&regs);
        asciz::copy(brand_string +  0, (const char*)&regs[EAX], 4);
        asciz::copy(brand_string +  3, (const char*)&regs[EBX], 4);
        asciz::copy(brand_string +  7, (const char*)&regs[ECX], 4);
        asciz::copy(brand_string + 11, (const char*)&regs[EDX], 4);
        regs[EAX] = CPUID_EXTENDED_BRANDSTRING_2;
        do_cpuid(&regs);
        asciz::copy(brand_string + 15, (const char*)&regs[EAX], 4);
        asciz::copy(brand_string + 19, (const char*)&regs[EBX], 4);
        asciz::copy(brand_string + 23, (const char*)&regs[ECX], 4);
        asciz::copy(brand_string + 27, (const char*)&regs[EDX], 4);
        regs[EAX] = CPUID_EXTENDED_BRANDSTRING_3;
        do_cpuid(&regs);
        asciz::copy(brand_string + 31, (const char*)&regs[EAX], 4);
        asciz::copy(brand_string + 35, (const char*)&regs[EBX], 4);
        asciz::copy(brand_string + 39, (const char*)&regs[ECX], 4);
        asciz::copy(brand_string + 43, (const char*)&regs[EDX], 4);
        brand_string[47] = 0;
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

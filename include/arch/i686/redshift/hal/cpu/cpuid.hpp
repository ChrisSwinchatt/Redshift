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
#ifndef REDSHIFT_HAL_CPU_CPUID_H
#define REDSHIFT_HAL_CPU_CPUID_H

#include <redshift/kernel.hpp>
#include <redshift/hal/cpu/features.hpp>
#include <redshift/hal/cpu/vendor.hpp>

namespace redshift { namespace hal { namespace cpu_ {
    /// CPU ID info.
    struct cpuid {
        static constexpr size_t VENDOR_MAX        = 12 + 1;
        static constexpr size_t VENDOR_STRING_MAX = 12 + 1;
        static constexpr size_t BRAND_STRING_MAX  = 48;

        enum class request {
            vendor_string,
            info_features,
            cache_tlb,
            extended_max_function = 0x80000000,
            extended_info_features,
            extended_brandstring_1,
            extended_brandstring_2,
            extended_brandstring_3,
            extended_cachel1_tlb,
            extended_cachel2,
            extended_power_management,
            extended_address_sizes,
        };

        char     vendor[CPUID_VENDOR_MAX];               ///< Vendor.
        char     vendor_string[CPUID_VENDOR_STRING_MAX]; ///< Vendor string.
        char     brand_string[CPUID_BRAND_STRING_MAX];   ///< Brand string.
        uint32_t features;                               ///< Feature flags.
        uint32_t features_ext;                           ///< Feature flags.
        unsigned stepping : 4;                           ///< CPU stepping.
        unsigned model    : 4;                           ///< CPU model.
        unsigned family   : 4;                           ///< CPU family.
        unsigned type     : 1;                           ///< CPU type.
        uint32_t l1_cache;                               ///< L1 cache size.
        uint32_t l2_cache;                               ///< L2 cache size.
        uint32_t l3_cache;                               ///< L3 cache size.
        uint64_t frequency;                              ///< Clock frequency (Hz).
        uint32_t logical_cores;                          ///< Number of logical cores.
        uint32_t physical_cores;                         ///< Number of physical cores.

        cpuid();
    private:
        enum { EAX, EBX, ECX, EDX };

        void cpuid::do_cpuid(uint32_t(* regs)[4])

        int extended_supported();
        void get_vendor_string();
        void get_vendor();
        void get_features();
        void get_brand_string();
        void get_cache();
        void get_frequency();
        void get_cores();
    };
}}} // redshift::hal::cpu_

#endif // ! REDSHIFT_HAL_CPU_CPUID_H

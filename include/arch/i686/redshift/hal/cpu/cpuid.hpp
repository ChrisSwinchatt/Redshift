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
#ifndef REDSHIFT_HAL_CPU_CPUID_HPP
#define REDSHIFT_HAL_CPU_CPUID_HPP

#include <redshift/kernel.hpp>
#include <redshift/hal/cpu/features.hpp>
#include <redshift/hal/cpu/vendor.hpp>

namespace redshift { namespace hal { namespace cpu_detail {
    /// CPU ID info.
    struct cpuid {
        struct vendor_info {
            const char* vendor_strings[];
            const char* name;
        };
        static constexpr vendor_info[] = {
            { { "AMDisbetter!", "AuthenticAMD", nullptr },  },
        };
        static constexpr const char* VENDOR_AMD_OLD    = ;
        static constexpr const char* VENDOR_AMD_NEW    = ;
        static constexpr const char* VENDOR_CENTAUR    = "CentaurHauls";
        static constexpr const char* VENDOR_CYRIX      = "CyrixInstead";
        static constexpr const char* VENDOR_INTEL      = "GenuineIntel";
        static constexpr const char* VENDOR_TRANSMETA1 = "TransmetaCPU";
        static constexpr const char* VENDOR_TRANSMETA2 = "GenuineTMx86";
        static constexpr const char* VENDOR_NSC        = "Geode by NSC";
        static constexpr const char* VENDOR_NEXGEN     = "NexGenDriven";
        static constexpr const char* VENDOR_RISE       = "RiseRiseRise";
        static constexpr const char* VENDOR_SIS        = "SiS SiS SiS ";
        static constexpr const char* VENDOR_UMC        = "UMC UMC UMC ";
        static constexpr const char* VENDOR_VIA        = "VIA VIA VIA ";
        static constexpr size_t      VENDOR_MAX        = 12 + 1;
        static constexpr size_t      VENDOR_STRING_MAX = 12 + 1;
        static constexpr size_t      BRAND_STRING_MAX  = 48;

        enum class request : uint32_t {
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

        /// Vendor.
        char     vendor[VENDOR_MAX];
        /// Vendor string.
        char     vendor_string[VENDOR_STRING_MAX];
        /// Brand string.
        char     brand_string[BRAND_STRING_MAX];
        /// Feature flags.
        uint32_t features;
        /// Feature flags.
        uint32_t features_ext;
        /// CPU stepping.
        unsigned stepping : 4;
        /// CPU model.
        unsigned model    : 4;
        /// CPU family.
        unsigned family   : 4;
        /// CPU type.
        unsigned type     : 1;
        /// L1 cache size.
        uint32_t l1_cache;
        /// L2 cache size.
        uint32_t l2_cache;
        /// L3 cache size.
        uint32_t l3_cache;
        /// Clock frequency (Hz).
        uint64_t frequency;
        /// Number of logical cores.
        uint32_t logical_cores;
        /// Number of physical cores.
        uint32_t physical_cores;

        cpuid();
    private:
        void do_cpuid(uint32_t(* regs)[4]);
        bool extended_supported();
        void get_vendor_string();
        void get_vendor();
        void get_features();
        void get_brand_string();
        void get_cache();
        void get_frequency();
        void get_cores();
    };
}}} // redshift::hal::cpu_detail

#endif // ! REDSHIFT_HAL_CPU_CPUID_HPP

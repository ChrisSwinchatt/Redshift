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
#ifndef REDSHIFT_HAL_CPU_H
#define REDSHIFT_HAL_CPU_H

#include <redshift/kernel.hpp>
#include <redshift/hal/cpu/cpuid.hpp>
#include <redshift/hal/cpu/features.hpp>
#include <redshift/hal/cpu/gdt.hpp>
#include <redshift/hal/cpu/idt.hpp>
#include <redshift/hal/cpu/tss.hpp>
#include <redshift/hal/cpu/state.hpp>

namespace redshift { namespace hal {
    class cpu {
    public:
        using cpuid = cpu_detail::cpuid;
        using gdt   = cpu_detail::gdt;
        using idt   = cpu_detail::idt;
        using pic   = cpu_detail::pic;
        using tss   = cpu_detail::tss;
        using state = cpu_detail::state;

        /// Initialise CPU.
        static void init();

        /// Read the number of ticks since the CPU started.
        /// \return The number of ticks since the CPU started.
        static uint64_t read_ticks();

        /// Test whether a feature is available on the CPU.
        /// \param feature The feature to test.
        /// \return True if the feature is available, otherwise false.
        static bool has_feature(feature_t feature);

        /// Test whether a  extended feature is available on the CPU.
        /// \param feature The extended feature to test.
        /// \return True if the extended feature is available, otherwise false.
        static bool has_ext_feature(feature_ext_t feature);

        /// Get the name of the CPU vendor.
        /// \return The name of the CPU vendor.
        static const char* get_vendor();

        /// Get the CPU vendor string.
        /// \return The CPU vendor string.
        static const char* get_vendor_string();

        /// Get the CPU brand string.
        /// \return The CPU brand string.
        static const char* get_brand_string();

        /// Get the CPU feature flags.
        /// \return The CPU feature flags.
        static uint32_t get_features();

        /// Get the extended CPU feature flags.
        /// \param The extended CPU feature flags.
        static uint32_t get_ext_features();

        /// Get the CPU stepping stepping.
        /// \return The CPU stepping stepping.
        static unsigned get_version_stepping();

        /// Get the CPU model number.
        /// \return The CPU model number.
        static unsigned get_version_model();

        /// Get the CPU family number.
        /// \return The CPU family number.
        static unsigned get_version_family();

        /// Get the CPU type number.
        /// \return The CPU type number.
        static unsigned get_version_type();

        /// Get the size of the L1 cache.
        /// \return The size of the L1 cache.
        static uint32_t get_l1_cache_size();

        /// Get the size of the L2 cache.
        /// \return The size of the L2 cache.
        static uint32_t get_l2_cache_size();

        /// Get the size of the L3 cache.
        /// \return The size of the L3 cache.
        static uint32_t get_l3_cache_size();

        /// Get the CPU clock frequency.
        /// \return The CPU clock frequency.
        static uint64_t get_clock_frequency();

        /// Get the number of logical cores.
        /// \return The number of logical cores.
        static uint32_t get_logical_cores_count();

        /// Get the number of physical cores.
        /// \return The number of physical cores.
        static uint32_t get_physical_cores_count();
    private:
        static cpuid m_cpuid;
    };
}} /// redshift::hal

#endif /// ! REDSHIFT_HAL_CPU_H

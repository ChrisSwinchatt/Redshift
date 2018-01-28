/**
 * \file hal/cpu.h
 * CPU abstraction.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef REDSHIFT_HAL_CPU_H
#define REDSHIFT_HAL_CPU_H

#include <redshift/kernel.h>
#include <redshift/hal/cpu/features.h>
#include <redshift/hal/cpu/state.h>

/**
 * Initialise CPU.
 */
void cpu_init(void);

/**
 * Test whether a feature is available on the CPU.
 * \param feature The feature to test.
 * \return True if the feature is available, otherwise false.
 */
bool cpu_has_feature(cpu_feature_t feature);

/**
 * Test whether a  extended feature is available on the CPU.
 * \param feature The extended feature to test.
 * \return True if the extended feature is available, otherwise false.
 */
bool cpu_has_ext_feature(cpu_feature_ext_t feature);

/**
 * Get the name of the CPU vendor.
 * \return The name of the CPU vendor.
 */
const char* cpu_get_vendor(void);

/**
 * Get the CPU vendor string.
 * \return The CPU vendor string.
 */
const char* cpu_get_vendor_string(void);

/**
 * Get the CPU brand string.
 * \return The CPU brand string.
 */
const char* cpu_get_brand_string(void);

/**
 * Get the CPU feature flags.
 * \return The CPU feature flags.
 */
uint32_t cpu_get_features(void);

/**
 * Get the extended CPU feature flags.
 * \param The extended CPU feature flags.
 */
uint32_t cpu_get_ext_features(void);

/**
 * Get the CPU stepping stepping.
 * \return The CPU stepping stepping.
 */
unsigned cpu_get_version_stepping(void);

/**
 * Get the CPU model number.
 * \return The CPU model number.
 */
unsigned cpu_get_version_model(void);

/**
 * Get the CPU family number.
 * \return The CPU family number.
 */
unsigned cpu_get_version_family(void);

/**
 * Get the CPU type number.
 * \return The CPU type number.
 */
unsigned cpu_get_version_type(void);

/**
 * Get the size of the L1 cache.
 * \return The size of the L1 cache.
 */
uint32_t cpu_get_l1_cache_size(void);

/**
 * Get the size of the L2 cache.
 * \return The size of the L2 cache.
 */
uint32_t cpu_get_l2_cache_size(void);

/**
 * Get the size of the L3 cache.
 * \return The size of the L3 cache.
 */
uint32_t cpu_get_l3_cache_size(void);

/**
 * Get the CPU clock frequency.
 * \return The CPU clock frequency.
 */
uint64_t cpuid_get_clock_frequency(void);

/**
 * Get the number of logical cores.
 * \return The number of logical cores.
 */
uint32_t cpuid_get_logical_cores_count(void);

/**
 * Get the number of physical cores.
 * \return The number of physical cores.
 */
uint32_t cpuid_get_physical_cores_count(void);

#endif /* ! REDSHIFT_HAL_CPU_H */

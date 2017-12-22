/**
 * \file hal/cpu.c
 * \brief CPU abstraction.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
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
#include <kernel/redshift.h>
#include <hal/cpu/cpuid.h>

static struct cpu {
    struct cpuid info;
} __cpu__;

void cpu_init(void)
{
    cpuid_init(&(__cpu__.info));
}

bool cpu_has_feature(cpu_feature_t feature)
{
    return test_flag(__cpu__.info.features, (uint32_t)feature);
}

bool cpu_has_ext_feature(cpu_feature_ext_t feature)
{
    return test_flag(__cpu__.info.features_ext, (uint32_t)feature);
}

const char* cpu_get_vendor(void)
{
    return __cpu__.info.vendor;
}

const char* cpu_get_vendor_string(void)
{
    return __cpu__.info.vendor_string;
}

const char* cpu_get_brand_string(void)
{
    return __cpu__.info.brand_string;
}

uint32_t cpu_get_features(void)
{
    return __cpu__.info.features;
}

uint32_t cpu_get_ext_features(void)
{
    return __cpu__.info.features_ext;
}

unsigned cpu_get_version_stepping(void)
{
    return __cpu__.info.stepping;
}

unsigned cpu_get_version_model(void)
{
    return __cpu__.info.model;
}

unsigned cpu_get_version_family(void)
{
    return __cpu__.info.family;
}

unsigned cpu_get_version_type(void)
{
    return __cpu__.info.type;
}

uint32_t cpu_get_l1_cache_size(void)
{
    return __cpu__.info.l1_cache;
}

uint32_t cpu_get_l2_cache_size(void)
{
    return __cpu__.info.l2_cache;
}

uint32_t cpu_get_l3_cache_size(void)
{
    return __cpu__.info.l3_cache;
}

uint64_t cpuid_get_clock_frequency(void)
{
    return __cpu__.info.frequency;
}

uint32_t cpuid_get_logical_cores_count(void)
{
    return __cpu__.info.logical_cores;
}

uint32_t cpuid_get_physical_cores_count(void)
{
    return __cpu__.info.physical_cores;
}

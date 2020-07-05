/**
 * \file hal/cpu/features.h
 * CPU features.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#ifndef REDSHIFT_HAL_CPU_FEATURES_H
#define REDSHIFT_HAL_CPU_FEATURES_H

#include <redshift/kernel.h>

/**
 * CPU features.
 */
typedef enum {
    CPU_FEATURE_AVX     = 1 << 28,
    CPU_FEATURE_FPU     = 1 << 0,
    CPU_FEATURE_VME     = 1 << 1,
    CPU_FEATURE_DE      = 1 << 2,
    CPU_FEATURE_PSE     = 1 << 3,
    CPU_FEATURE_TSC     = 1 << 4,
    CPU_FEATURE_MSR     = 1 << 5,
    CPU_FEATURE_PAE     = 1 << 6,
    CPU_FEATURE_MCE     = 1 << 7,
    CPU_FEATURE_CX8     = 1 << 8,
    CPU_FEATURE_APIC    = 1 << 9,
    CPU_FEATURE_SEP     = 1 << 11,
    CPU_FEATURE_MTRR    = 1 << 12,
    CPU_FEATURE_PGE     = 1 << 13,
    CPU_FEATURE_MCA     = 1 << 14,
    CPU_FEATURE_CMOV    = 1 << 15,
    CPU_FEATURE_PAT     = 1 << 16,
    CPU_FEATURE_PSE36   = 1 << 17,
    CPU_FEATURE_PSN     = 1 << 18,
    CPU_FEATURE_CLF     = 1 << 19,
    CPU_FEATURE_DTES    = 1 << 21,
    CPU_FEATURE_ACPI    = 1 << 22,
    CPU_FEATURE_MMX     = 1 << 23,
    CPU_FEATURE_FXSR    = 1 << 24,
    CPU_FEATURE_SSE     = 1 << 25,
    CPU_FEATURE_SSE2    = 1 << 26,
    CPU_FEATURE_SS      = 1 << 27,
    CPU_FEATURE_HTT     = 1 << 28,
    CPU_FEATURE_TM1     = 1 << 29,
    CPU_FEATURE_IA64    = 1 << 30,
    CPU_FEATURE_PBE     = 1 << 31
} cpu_feature_t;

/**
 * CPU features (extended set).
 */
typedef enum {
    CPU_FEATURE_EXT_SSE3    = 1 << 0,
    CPU_FEATURE_EXT_PCLMUL  = 1 << 1,
    CPU_FEATURE_EXT_DTES64  = 1 << 2,
    CPU_FEATURE_EXT_MONITOR = 1 << 3,
    CPU_FEATURE_EXT_DS_CPL  = 1 << 4,
    CPU_FEATURE_EXT_VMX     = 1 << 5,
    CPU_FEATURE_EXT_SMX     = 1 << 6,
    CPU_FEATURE_EXT_EST     = 1 << 7,
    CPU_FEATURE_EXT_TM2     = 1 << 8,
    CPU_FEATURE_EXT_SSSE3   = 1 << 9,
    CPU_FEATURE_EXT_CID     = 1 << 10,
    CPU_FEATURE_EXT_FMA     = 1 << 12,
    CPU_FEATURE_EXT_CX16    = 1 << 13,
    CPU_FEATURE_EXT_ETPRD   = 1 << 14,
    CPU_FEATURE_EXT_PDCM    = 1 << 15,
    CPU_FEATURE_EXT_DCA     = 1 << 18,
    CPU_FEATURE_EXT_SSE4_1  = 1 << 19,
    CPU_FEATURE_EXT_SSE4_2  = 1 << 20,
    CPU_FEATURE_EXT_x2APIC  = 1 << 21,
    CPU_FEATURE_EXT_MOVBE   = 1 << 22,
    CPU_FEATURE_EXT_POPCNT  = 1 << 23,
    CPU_FEATURE_EXT_AES     = 1 << 25,
    CPU_FEATURE_EXT_XSAVE   = 1 << 26,
    CPU_FEATURE_EXT_OSXSAVE = 1 << 27
} cpu_feature_ext_t;

#endif /* ! REDSHIFT_HAL_CPU_FEATURES_H */

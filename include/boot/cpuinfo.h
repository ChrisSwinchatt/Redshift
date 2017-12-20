/* Copyright (c) 2012 Chris Swinchatt.
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
#ifndef REDSHIFT_BOOT_CPUINFO_H
#define REDSHIFT_BOOT_CPUINFO_H

#include <kernel/redshift.h>

enum {
    CPU_FEATURE_SSE3    = 1 << 0,
    CPU_FEATURE_PCLMUL  = 1 << 1,
    CPU_FEATURE_DTES64  = 1 << 2,
    CPU_FEATURE_MONITOR = 1 << 3,
    CPU_FEATURE_DS_CPL  = 1 << 4,
    CPU_FEATURE_VMX     = 1 << 5,
    CPU_FEATURE_SMX     = 1 << 6,
    CPU_FEATURE_EST     = 1 << 7,
    CPU_FEATURE_TM2     = 1 << 8,
    CPU_FEATURE_SSSE3   = 1 << 9,
    CPU_FEATURE_CID     = 1 << 10,
    CPU_FEATURE_FMA     = 1 << 12,
    CPU_FEATURE_CX16    = 1 << 13,
    CPU_FEATURE_ETPRD   = 1 << 14,
    CPU_FEATURE_PDCM    = 1 << 15,
    CPU_FEATURE_DCA     = 1 << 18,
    CPU_FEATURE_SSE4_1  = 1 << 19,
    CPU_FEATURE_SSE4_2  = 1 << 20,
    CPU_FEATURE_x2APIC  = 1 << 21,
    CPU_FEATURE_MOVBE   = 1 << 22,
    CPU_FEATURE_POPCNT  = 1 << 23,
    CPU_FEATURE_AES     = 1 << 25,
    CPU_FEATURE_XSAVE   = 1 << 26,
    CPU_FEATURE_OSXSAVE = 1 << 27,
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
};

/**  CPU info structure, stores details about the CPU */
struct cpuinfo {
    char     vendor[24 + 1];        /**< Vendor.                    */
    char     vendor_string[12 + 1]; /**< Vendor string.             */
    char     brand_string[48];      /**< Brand string.              */
    uint32_t features_1;            /**< Feature flags.             */
    uint32_t features_2;            /**< Feature flags.             */
    unsigned stepping : 4;          /**< CPU stepping.              */
    unsigned model    : 4;          /**< CPU model.                 */
    unsigned family   : 4;          /**< CPU family.                */
    unsigned type     : 1;          /**< CPU type.                  */
    uint32_t l1_cache;              /**< L1 cache size.             */
    uint32_t l2_cache;              /**< L2 cache size.             */
    uint32_t l3_cache;              /**< L3 cache size.             */
    uint64_t frequency;             /**< Clock frequency (Hz).      */
    uint32_t logical_cores;         /**< Number of logical cores.   */
    uint32_t physical_cores;        /**< Number of physical cores.  */
};

/**
 * \brief Gather CPU information
 * \param info Pointer to CPU info object
 * \return On success, 0 is returned. On error, -1  is returned.
 */
int cpuinfo_init(struct cpuinfo* info);

#endif /* ! REDSHIFT_BOOT_CPUINFO_H */

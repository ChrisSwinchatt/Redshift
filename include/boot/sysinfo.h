/**
 * \file boot/sysinfo.h
 * \brief Collect system details, primarily from multiboot tags.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef REDSHIFT_BOOT_SYSINFO_H
#define REDSHIFT_BOOT_SYSINFO_H

#include <kernel/redshift.h>
#include <boot/multiboot.h>
#include <hal/cpu.h>
#include <hal/cpu/cpuid.h>

/**
 * System module.
 */
struct module {
    const char* cmdline; /**< Module command-line. */
    uint32_t    start;   /**< Module start.        */
    uint32_t    end;     /**< Module end.          */
};

/**
 * System info.
 */
struct sysinfo {
    struct cpuid   cpuinfo;     /**< CPU info.                 */
    size_t         nb_mod;      /**< Number of module entries. */
    struct module* modules;     /**< Modules.                  */
    size_t         nb_mmap;     /**< Number of memmap entries. */
    struct memmap* memmap;      /**< Memory map.               */
    const char*    bootloader;  /**< Boot loader name.         */
    const char*    cmdline;     /**< Kernel command-line.      */
    uint32_t       device;      /**< BIOS boot device.         */
    uint32_t       slice;       /**< Boot device slice.        */
    uint32_t       partition;   /**< Boot device partition.    */
    uint32_t       mem_lower;   /**< Low memory (kiB).         */
    uint32_t       mem_upper;   /**< High memory (kiB).        */
    uint32_t       modules_end; /**< End of the last module.   */
};

/**
 * \brief Gather system information that can be allocated statically.
 * \param info Pointer to sysinfo object.
 * \param tags Pointer to multiboot tags array.
 */
void sysinfo_init_1(struct sysinfo* info, struct multiboot_tag* tags);

/**
 * \brief Gather system information that must be allocated dynamically.
 * \param info Pointer to sysinfo object.
 * \param tags Pointer to multiboot tags array.
 */
void sysinfo_init_2(struct sysinfo* info, struct multiboot_tag* tags);

#endif /* ! REDSHIFT_SYSINFO_H */

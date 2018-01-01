/* Copyright (c) 2012-2018 Chris Swinchatt.
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
#ifndef REDSHIFT_BOOT_MODULE_H
#define REDSHIFT_BOOT_MODULE_H

#include <redshift/boot/multiboot.h>

/**
 * Boot module linked list.
 */
struct boot_module {
    const char*               cmdline; /**< Module command-line. */
    uintptr_t                 start;   /**< Module start.        */
    uintptr_t                 end;     /**< Module end.          */
    const struct boot_module* next;
};

/**
 * Discover boot modules.
 */
void discover_boot_modules(struct multiboot_tag* mb_tags);

/**
 * Save boot modules.
 */
void save_boot_modules(struct multiboot_tag* mb_tags);

/**
 * Get a readonly pointer to the head of the boot modules list.
 * \return A readonly pointer to the boot modules list.
 */
const struct boot_module* boot_modules_head(void);

/**
 * Get the number of boot modules loaded.
 * \return The number of boot modules loaded.
 */
size_t boot_modules_count(void);

/**
 * Get the address of the end of the last boot module in memory. This is the first usable memory location.
 * \return The address of the end of the last boot module in memory.
 */
uintptr_t boot_modules_end(void);

#endif /* ! REDSHIFT_BOOT_MODULE_H */

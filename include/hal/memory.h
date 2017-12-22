/**
 * \file hal/memory.h
 * \brief Memory abstraction.
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
#ifndef REDSHIFT_HAL_MEMORY_H
#define REDSHIFT_HAL_MEMORY_H

#include <boot/multiboot.h>
#include <kernel/redshift.h>

enum {
    MEMORY_TYPE_INVALID     = MULTIBOOT_MEMORY_INVALID,
    MEMORY_TYPE_AVAILABLE   = MULTIBOOT_MEMORY_AVAILABLE,
    MEMORY_TYPE_RESERVED    = MULTIBOOT_MEMORY_RESERVED,
    MEMORY_TYPE_RECLAIMABLE = MULTIBOOT_MEMORY_RECLAIMABLE,
    MEMORY_TYPE_NVS         = MULTIBOOT_MEMORY_NVS
};

struct memory_map {
    uint32_t type;
    uint64_t start;
    uint64_t end;
    const struct memory_map* next;
};

/**
 * Initialise memory HAL.
 * \param mb_tags The multiboot tags.
 */
void memory_init(struct multiboot_tag* mb_tags);

/**
 * Parse memory map.
 * \param mb_Tags The multiboot tags.
 * \note This has to be called *after* static_init.
 */
void memory_map_init(struct multiboot_tag* mb_tags);

/**
 * Get a readonly pointer to the head of the memory map.
 * \return A readonly pointer to the head of the memory map.
 */
const struct memory_map* memory_map_head(void);

/**
 * Get the number of entries in the memory map.
 * \return The number of entries in the memory map.
 */
size_t memory_map_size(void);

/**
 * Get the size of memory in kiB (lower part).
 */
uint32_t memory_size_lower(void);

/**
 * Get the size of memory in kiB (upper part).
 */
uint32_t memory_size_upper(void);

/**
 * Get the total size of memory in kiB.
 */
uint32_t memory_size_total(void);

#endif /* ! REDSHIFT_HAL_MEMORY_H */

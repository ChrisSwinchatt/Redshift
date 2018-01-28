/**
 * \file hal/memory.h
 * Memory abstraction.
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

#include <redshift/boot/multiboot2.h>
#include <redshift/kernel.h>

/** Memory region type. */
typedef enum {
    MEMORY_TYPE_INVALID     = MULTIBOOT2_MEMORY_INVALID,     /**< Invalid memory.                            */
    MEMORY_TYPE_AVAILABLE   = MULTIBOOT2_MEMORY_AVAILABLE,   /**< Available memory.                          */
    MEMORY_TYPE_RESERVED    = MULTIBOOT2_MEMORY_RESERVED,    /**< Unavailable memory.                        */
    MEMORY_TYPE_RECLAIMABLE = MULTIBOOT2_MEMORY_RECLAIMABLE, /**< Unavailable memory which can be reclaimed. */
    MEMORY_TYPE_NVS         = MULTIBOOT2_MEMORY_NVS          /**< Memory used by ACPI.                       */
} memory_type_t;

/** Memory map. */
struct memory_map {
    memory_type_t            type;  /**< Memory region type.  */
    uint64_t                 start; /**< Memory region start. */
    uint64_t                 end;   /**< Memory region end.   */
    const struct memory_map* next;  /**< Next region in map.  */
};

/**
 * Initialise memory HAL.
 * \param mb_tags The multiboot tags.
 */
void memory_init(struct multiboot2_tag* mb_tags);

/**
 * Parse memory map.
 * \param mb_Tags The multiboot tags.
 * \note This has to be called *after* static_init.
 */
void memory_map_init(struct multiboot2_tag* mb_tags);

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
 * \return The size of memory in kiB (lower part).
 */
size_t memory_size_lower(void);

/**
 * Get the size of memory in kiB (upper part).
 * \return The size of memory in kiB (upper part).
 */
size_t memory_size_upper(void);

/**
 * Get the total size of memory in kiB.
 * \return The total size of memory in kiB.
 */
size_t memory_size_total(void);

#endif /* ! REDSHIFT_HAL_MEMORY_H */

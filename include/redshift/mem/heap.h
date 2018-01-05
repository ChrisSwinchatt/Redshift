/**
 * \file mem/heap.h
 * Heap allocator.
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
#ifndef REDSHIFT_MEM_HEAP_H
#define REDSHIFT_MEM_HEAP_H 1

#include <redshift/kernel.h>
#include <redshift/util/sorted_array.h>

/** Heap flags. */
typedef enum {
    HEAP_FLAGS_SUPERVISOR = 0,      /**< Create supervisor mode heap. */
    HEAP_FLAGS_USER_MODE  = 1 << 0, /**< Create user mode heap.       */
    HEAP_FLAGS_WRITEABLE  = 1 << 2  /**< Create writeable heap. */
} heap_flags_t;

/* Forward declaration for struct heap (mem/heap.c). */
struct heap;

/** Default kernel heap. */
extern struct heap* __kernel_heap__;

/**
 * Initialise the kernel heap and memory manager.
 * \param
 */
void heap_init(void);

/**
 * Create a new heap.
 * \param start The start address.
 * \param end The end address.
 * \param max_size The maximum size of the heap. Must be >= end - start.
 * \param Heap flags.
 * \return The new heap.
 */
struct heap* create_heap(uint32_t start, uint32_t end, size_t max_size, heap_flags_t flags);

/**
 * Allocate a block of memory on a heap.
 * \param heap The heap.
 * \param size The size of the memory to allocate.
 * \param page_align Whether to align the block on a page boundary. Blocks are always aligned to 8-byte boundaries.
 * \return A pointer to the memory block.
 */
void* heap_alloc(struct heap* heap, size_t size, bool page_align);

/**
 * Free a block of memory allocated on a heap.
 * \param The heap.
 * \param The block of memory.
 */
void heap_free(struct heap* heap, void* ptr);

#endif /* ! REDSHIFT_MEM_HEAP_H */

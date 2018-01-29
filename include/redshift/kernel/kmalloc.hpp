/**
 * \file redshift/mem/kmalloc.h
 * \brief Heap allocator.
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
#ifndef REDSHIFT_MEM_KMALLOC_H
#define REDSHIFT_MEM_KMALLOC_H

#include <redshift/kernel.h>

/**
 * Allocate a block of dynamic memory on the kernel heap.
 * \param size The size of the block to allocate.
 * \return A pointer to the allocated block if successful. Panics on failure.
 */
extern void*(* kmalloc)(size_t size);

/**
 * Free a block of memory from the kernel heap.
 * \param ptr The memory block.
 */
extern void(* kfree)(void* ptr);

/**
 * Enable kmalloc/kfree. Called by create_heap (mem/heap.c) after the kernel heap is initialised. Prior to this, kmalloc
 * and kfree will trigger a kernel panic.
 */
void enable_kmalloc(void);

#endif /* ! REDSHIFT_MEM_KMALLOC_H */

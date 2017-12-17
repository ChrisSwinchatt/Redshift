/**
 * \file mem/heap.h
 * \brief Heap allocator.
 * \author Chris Swinchatt <c.swinchatt1@uni.brighton.ac.uk> 
 * \copyright Copyright (c) 2012-2014 Chris Swinchatt.
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

#include <kernel/redshift.h>

struct heap;

int heap_init(void);

struct heap* create_heap(uint32_t start, uint32_t end, size_t max, bool supervisor, bool readonly);

void* heap_alloc(struct heap* heap, size_t size, bool align);

void heap_free(struct heap* heap, void* ptr);

void* kmalloc(size_t size);

void kfree(void* ptr);

#endif /* ! REDSHIFT_MEM_HEAP_H */
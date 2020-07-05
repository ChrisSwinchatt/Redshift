/**
 * \file mem/kmalloc.c
 * \brief Heap allocator.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#include <redshift/kernel.h>
#include <redshift/kernel/kmalloc.h>
#include <redshift/mem/heap.h>

/* Panics if kmalloc is called before the heap is initialised. */
static void* pre_init_kmalloc(size_t ignored)
{
    panic("kmalloc called before heap initialised");
    UNUSED(ignored);
}

/* Panics if kfree is called before the heap is initialised. */
static void pre_init_kfree(void* ignored)
{
    panic("kfree called before heap initialised");
    UNUSED(ignored);
}

/* Real kmalloc implementation. */
static void* real_kmalloc(size_t size)
{
    void* p = heap_alloc(__kernel_heap__, size, true);
    return p;
}

/* Real kfree implementation. */
static void real_kfree(void* ptr)
{
    heap_free(__kernel_heap__, ptr);
}

typedef void* void_ptr;
typedef void_ptr(* kmalloc_fn_t)(size_t);
typedef void(* kfree_fn_t)(void*);

void_ptr(* kmalloc)(size_t) = pre_init_kmalloc;
void    (* kfree)(void*)    = pre_init_kfree;


void enable_kmalloc(void)
{
   kmalloc = real_kmalloc;
   kfree   = real_kfree;
}

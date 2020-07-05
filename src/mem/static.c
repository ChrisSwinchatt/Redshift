/**
 * \file mem/static.c
 * \brief Static memory allocator.
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
#include <redshift/boot/boot_module.h>
#include <redshift/kernel.h>
#include <redshift/mem/common.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/static.h>

uintptr_t heap_addr = 0;

void static_init(void)
{
    PUSH_INTERRUPT_STATE(0);
    heap_addr = MAX((uintptr_t)__kernel_end__, boot_modules_end());
    MAKE_PAGE_ALIGNED(heap_addr);
    POP_INTERRUPT_STATE();
}

uintptr_t static_alloc_base(size_t size, alloc_flags_t flags, uintptr_t* phys)
{
    PUSH_INTERRUPT_STATE(0);
    DEBUG_ASSERT(heap_addr >= (uintptr_t)__kernel_end__);
    DEBUG_ASSERT(size > 0);
    /* Align to page boundary if required. Otherwise, align according to the requirements of the object to be allocated.
     */
    if (TEST_FLAG(flags, ALLOC_PAGE_ALIGN)) {
        MAKE_PAGE_ALIGNED(heap_addr);
    } else {
        MAKE_ALLOC_ALIGNED(heap_addr, size);
     }
    /* Advance heap address. In debug mode, also test that we haven't overwritten the dynamic heap.
     */
    uintptr_t addr = heap_addr;
    heap_addr += size;
    if (phys != NULL) {
        *phys = addr;
    }
    POP_INTERRUPT_STATE();
    return addr;
}

void* static_alloc(size_t size)
{
    return (void*)static_alloc_base(size, ALLOC_SIZE_ALIGN, NULL);
}

/**
 * \file mem/static.c
 * \brief Static memory allocator.
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
#include <redshift/boot/boot_module.h>
#include <redshift/kernel.h>
#include <redshift/mem/common.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/static.h>

uintptr_t heap_addr = 0;

void static_init(void)
{
    uintptr_t end = (uintptr_t)__kernel_end__;
    uintptr_t modules_end = boot_modules_end();
    if (modules_end > end) {
        end = modules_end;
    }
    heap_addr = (end & PAGE_MASK) + PAGE_SIZE;
}

uintptr_t static_alloc_base(size_t size, bool align, uintptr_t* phys)
{
    DEBUG_ASSERT(size > 0);
    /* Align to page boundary if required.
     */
    if (align && (heap_addr & PAGE_MASK)) {
        heap_addr &= PAGE_MASK;
        heap_addr += PAGE_SIZE;
    }
    /* As per ABI, objects must be aligned according to their type, e.g. uint{16,32,64}_t must be aligned on 2, 4 and 8
     * byte boundaries respectively. Since anything aligned to 8 bytes is also aligned to 4 bytes and 2 bytes, we just
     * align everything to 8 bytes.
     */
    if (heap_addr & 0xFFFFFFF8) {
         heap_addr &= 0xFFFFFFF8;
         heap_addr += 8;
     }
    /* Advance heap address. In debug mode, also test that we haven't overwritten the dynamic heap.
     */
    uintptr_t addr = heap_addr;
    heap_addr += size;
    if (phys != NULL) {
        *phys = addr;
    }
    //DEBUG_ASSERT(__kernel_heap__ == NULL || heap_addr < __kernel_heap__->start);
    return addr;
}

void* static_alloc(size_t size)
{
    return (void*)static_alloc_base(size, true, NULL);
}

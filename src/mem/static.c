/**
 * \file mem/static.c
 * \brief Static heap allocator. Allocates memory statically.
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
#include <boot/sysinfo.h>
#include <kernel/redshift.h>
#include <mem/static.h>

extern const symbol_t __end__; /* Defined in linker script. Ends after the .bss section and stack. */
uint32_t heap_addr;

void static_init(void)
{
    uint32_t end = (uint32_t)__end__;
    if (__sysinfo__.modules_end > end) {
        end = __sysinfo__.modules_end;
    }
    heap_addr = (end & 0xFFFFF000) + 0x1000;
}

/* Align heap_addr to a page boundary, where a page is 4 kiB. */
static void page_align(void)
{
    if (heap_addr &  0xFFFFF000) {
        heap_addr &= 0xFFFFF000;
        heap_addr +=     0x1000;
    }
}

/* Align heap_addr to an alignment which depends on an object's size. */
static void size_align(size_t size)
{
    /* As per ABI, objects must be aligned according to their type, e.g. uint{16,32,64}_t must be aligned on 2, 4 or 8
     * byte boundaries respectively. Structures and arrays are aligned based on overall size.
     */
    if (size >= 8 && (heap_addr & 0xFFFFFFF8)) {
         heap_addr &= 0xFFFFFFF8;
         heap_addr += 8;
    } else if (size >= 4 && (heap_addr & 0xFFFFFFF4)) {
         heap_addr &= 0xFFFFFFF4;
         heap_addr += 4;
    } else if (size >= 2 && (heap_addr & 0xFFFFFFF2)) {
         heap_addr &= 0xFFFFFFF2;
         heap_addr += 2;
    }
}

uint32_t static_alloc_base(size_t size, bool align, uint32_t* phys)
{
    DEBUG_ASSERT(size > 0);
    /* Align to page boundary if required, then align according to size.
     */
    if (align && (heap_addr & 0xFFFFF000)) {
        page_align();
    }
    size_align(size);
    /*
     */
    uint32_t addr = heap_addr;
    heap_addr += size;
    if (phys != NULL) {
        *phys = addr;
    }
    return addr;
}

void* static_alloc(size_t size)
{
    return (void*)static_alloc_base(size, true, NULL);
}

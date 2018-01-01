/**
 * \file mem/heap.c
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
#include <redshift/hal/memory.h>
#include <redshift/kernel.h>
#include <redshift/util/sorted_list.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/paging.h>
#include <redshift/mem/static.h>

static bool     kmalloc_allowed = false;
static unsigned alloc_count = 0;
static unsigned free_count  = 0;

enum {
    HEAP_SIZE_MIN  = 0x00070000,
    BLOCKLIST_SIZE = 0x00002000,
    BLOCK_MAGIC    = 0x600DB10C
};

struct blockheader {
    uint32_t magic;
    uint8_t  is_hole;
    size_t   size;
};

struct blockfooter {
    uint32_t magic;
    struct   blockheader* header;
};

struct heap {
    struct   sorted_list* blocklist;
    uint32_t start;
    uint32_t end;
    size_t   max;
    uint8_t  supervisor;
    uint8_t  readonly;
} * __kernel_heap__;

enum {
    HEAP_SIZE_INIT = 0x00100000UL
};

/* Scan the memory map for a region big enough to store the heap. */
static uint32_t get_heap_region(uint64_t size)
{
    DEBUG_ASSERT(size != 0);
    const struct memory_map* memmap = memory_map_head();
    RUNTIME_CHECK(memory_map_size() > 0 && memmap != NULL);
    do {
        const uint64_t region_size = memmap->end - memmap->start;
        if (memmap->type == MEMORY_TYPE_AVAILABLE && region_size >= size) {
            return memmap->start;
        }
    } while ((memmap = memmap->next) != NULL);
    panic("not enough available memory");
}

int heap_init()
{
    const uint32_t start = get_heap_region(HEAP_SIZE_INIT);
    DEBUG_ASSERT(UINT32_MAX - HEAP_SIZE_INIT > start);
    __kernel_heap__ = create_heap(start, start + HEAP_SIZE_INIT, HEAP_SIZE_INIT, true, false);
    DEBUG_ASSERT(__kernel_heap__ != NULL);
    kmalloc_allowed = true;
    return 0;
}

static int32_t get_smallest_hole(struct heap* heap, size_t size, bool aligned)
{
    DEBUG_ASSERT(heap != NULL);
    DEBUG_ASSERT(size != 0);
    uint32_t i, j;
    for (i = 0, j = sorted_list_count(heap->blocklist); i < j; ++i) {
        struct blockheader* header = (struct blockheader*)sorted_list_get(heap->blocklist, i);
        if (aligned) {
            uint32_t address = (uint32_t)header;
            int32_t offset = 0;
            if (((address + sizeof(*header)) & 0xfffff000) != 0) {
                offset = 0x1000 - (address + sizeof(*header)) % 0x1000;
            }
            int32_t hole_size = (int32_t)header->size - offset;
            if (hole_size >= (int32_t)size) {
                return i;
            }
        } else if (header->size >= size) {
            return i;
        }
    }
    return -1;
}

static bool blockheader_ascending_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa);
    DEBUG_ASSERT(pb);
    struct blockheader* a = (struct blockheader*)pa;
    struct blockheader* b = (struct blockheader*)pb;
    return (a->size < b->size);
}

struct heap* create_heap(uint32_t start, uint32_t end, size_t max, bool supervisor, bool readonly)
{
    DEBUG_ASSERT(start % 0x1000 == 0);
    DEBUG_ASSERT(end   % 0x1000 == 0);
    struct heap* heap = static_alloc(sizeof(*heap));
    heap->blocklist = place_sorted_list((void*)start, BLOCKLIST_SIZE, true, blockheader_ascending_predicate);
    if (heap->blocklist == NULL) {
        panic("failed to allocate memory.\n");
    }
    start += sizeof(void*) * BLOCKLIST_SIZE;
    if (start & 0xfffff000) {
        start &= 0xfffff000;
        start += 0x1000;
    }
    printk(PRINTK_DEBUG "Placing kernel heap at 0x%08lX\n", start);
    heap->start      = start;
    heap->end        = end;
    heap->max        = max;
    heap->supervisor = supervisor;
    heap->readonly   = readonly;
    struct blockheader* hole = (struct blockheader*)start;
    hole->size    = end - start;
    hole->magic   = BLOCK_MAGIC;
    hole->is_hole = 1;
    sorted_list_add(heap->blocklist, (void*)hole);
    return heap;
}

static void heap_expand(struct heap* heap, size_t new_size)
{
    DEBUG_ASSERT(new_size > (heap->end - heap->start));
    if (new_size & 0xfffff000) {
        new_size &= 0xfffff000;
        new_size += 0x1000;
    }
    DEBUG_ASSERT(heap->start + new_size < heap->max);
    uint32_t old_size = heap->end - heap->start;
    uint32_t i;
    for (i = old_size; i < new_size; i += 0x1000) {
        frame_alloc(page_get(heap->start + i, kernel_directory, true), heap->supervisor, heap->readonly);
    }
    heap->end = heap->start + new_size;
}

static struct blockheader* place_header(uint32_t addr, bool is_hole, size_t size)
{
    struct blockheader* header = (struct blockheader*)addr;
    header->magic   = BLOCK_MAGIC;
    header->is_hole = (uint8_t)is_hole;
    header->size    = size;
    return header;
}

static struct blockfooter* place_footer(uint32_t addr, struct blockheader* header)
{
    struct blockfooter* footer = (struct blockfooter*)addr;
    footer->magic  = BLOCK_MAGIC;
    footer->header = header;
    return footer;
}

void* heap_alloc(struct heap* heap, size_t size, bool align)
{
    DEBUG_ASSERT(heap != NULL);
    uint32_t new_size = size + sizeof(struct blockheader) + sizeof(struct blockfooter);
    int32_t hole = get_smallest_hole(heap, new_size, align);
    if (hole < 0) {
        /* Expand the heap.
         */
        uint32_t old_len = heap->end - heap->start;
        uint32_t old_end = heap->end;
        heap_expand(heap, old_len + new_size);
        /* Find the header furthest to the end of the heap.
         */
        uint32_t new_len = heap->end - heap->start;
        uint32_t value = 0;
        uint32_t i;
        uint32_t j;
        int32_t index = -1;
        for (i = 0, j = sorted_list_count(heap->blocklist); i < j; ++i) {
            uint32_t tmp = (uint32_t)sorted_list_get(heap->blocklist, i);
            if (tmp > value) {
                value = tmp;
                index = (int32_t)i;
            }
        }
        if (index < 0) {
            /* Create a new header.
             */
            struct blockheader* header = place_header(old_end, true, new_len - old_len);
            place_footer(old_end + header->size - sizeof(struct blockfooter*), header);
            sorted_list_add(heap->blocklist, (void*)header);
        } else {
            /* Modify the existing header.
             */
             struct blockheader* header = sorted_list_get(heap->blocklist, index);
             header->size += new_len - old_len;
             place_footer((uint32_t)header + header->size - sizeof(struct blockfooter*), header);
        }
        /* Try again.
         */
        return heap_alloc(heap, size, align);
    }
    struct blockheader* original_header = (struct blockheader*)sorted_list_get(heap->blocklist, hole);
    uint32_t original_addr = (uint32_t)original_header;
    uint32_t original_size = original_header->size;
    if (original_size - new_size < sizeof(struct blockheader) + sizeof(struct blockfooter)) {
        /* Increase the requested size to match the size of the hole.
         */
        size += original_size - new_size;
        new_size = original_size;
    }
    if (align && (original_addr & 0xfffff000)) {
        /* Page-align the block with a new hole.
         */
        uint32_t new_addr = original_addr + 0x1000 - (original_addr & 0xfff) - sizeof(struct blockheader);
        struct blockheader* hole_header = place_header(original_addr, true,
                                                       0x1000 - (original_addr & 0xfff) - sizeof(*hole_header));
        place_footer(new_addr - sizeof(struct blockfooter), hole_header);
        original_addr  = new_addr;
        original_size -= hole_header->size;
    } else {
        /* Delete the hole.
         */
        sorted_list_remove(heap->blocklist, hole);
    }
    struct blockheader* header = place_header(original_addr, false, new_size);
    place_footer(original_addr + sizeof(*header) + size, header);
    if (size - new_size > 0) {
        /* Create a new hole after the allocated block.
         */
        struct blockheader* hole_header = place_header(original_addr + sizeof(*hole_header) + size +
                                                       sizeof(struct blockfooter*), true, original_size - new_size);
        /* We can't use place_footer here because footer might be at the end of the heap, and then it would be an error
         * to dereference it.
         */
        struct blockfooter* footer = (struct blockfooter*)((uint32_t)hole_header + original_size - new_size -
                                      sizeof(*footer));
        if ((uint32_t)footer < heap->end) {
            footer->magic = BLOCK_MAGIC;
            footer->header = header;
        }
        sorted_list_add(heap->blocklist, (void*)hole_header);
    }
    return (void*)((uint32_t)header + sizeof(*header));
}

static uint32_t heap_contract(struct heap* heap, size_t new_size)
{
    uint32_t old_size = heap->end - heap->start;
    DEBUG_ASSERT(new_size < old_size);
    if (new_size & 0x1000) {
        new_size &= 0x1000;
        new_size += 0x1000;
    }
    if (new_size < HEAP_SIZE_MIN)
        new_size = HEAP_SIZE_MIN;
    uint32_t i;
    for (i = old_size - 0x1000; i > new_size; i -= 0x1000) {
        frame_free(page_get(heap->start + i, kernel_directory, false));
    }
    heap->end = heap->start + new_size;
    return new_size;
}

void heap_free(struct heap* heap, void* ptr)
{
    DEBUG_ASSERT(heap != NULL);
    if (!(ptr)) {
        return;
    }
    /* Make the block into a hole.
     */
    struct blockheader* header = (struct blockheader*)((uint32_t)ptr - sizeof(*header));
    struct blockfooter* footer = (struct blockfooter*)((uint32_t)header + header->size - sizeof(*footer));
    DEBUG_ASSERT(header->magic == BLOCK_MAGIC);
    DEBUG_ASSERT(footer->magic == BLOCK_MAGIC);
    header->is_hole = 1;
    bool add = true;
    /* Attempt to unify the hole with adjacent holes.
     */
    struct blockfooter* test_footer = (struct blockfooter*)((uint32_t)header - sizeof(*test_footer));
    if (test_footer->magic == BLOCK_MAGIC && test_footer->header->is_hole) {
        /* Unify with the hole to the left.
         */
        uint32_t cached_size = header->size;
        header = test_footer->header;
        footer->header = header;
        header->size += cached_size;
        add = false;
    }
    struct blockheader* test_header = (struct blockheader*)((uint32_t)footer + sizeof(*footer));
    if (test_header->magic == BLOCK_MAGIC && test_header->is_hole) {
        /* Unify with the hole to the right.
         */
        header->size += test_header->size;
        test_footer = (struct blockfooter*)((uint32_t)test_header + test_header->size - sizeof(*test_footer));
        footer = test_footer;
        /* Remove the header from the index.
         */
        uint32_t i, j;
        for (i = 0, j = sorted_list_count(heap->blocklist); i < j; ++i) {
            if (sorted_list_get(heap->blocklist, i) == (void*)test_header) {
                break;
            }
        }
        DEBUG_ASSERT(i < j);
        sorted_list_remove(heap->blocklist, i);
    }
    if ((uint32_t)footer + sizeof(*footer) >= heap->end) {
        /* Contract the heap.
         */
        uint32_t old_len = heap->end - heap->start;
        uint32_t new_len = heap_contract(heap, (uint32_t)header - heap->start);
        if (header->size - (old_len - new_len) > 0) {
            /* Resize the current block.
             */
            header->size -= old_len - new_len;
            footer = (struct blockfooter*)((uint32_t)footer + header->size - sizeof(*footer));
            footer->magic = BLOCK_MAGIC;
            footer->header = header;
        } else {
            /* Current block will no longer exist: remove it from the blocklist.
             */
            uint32_t i, j;
            for (i = 0, j = sorted_list_count(heap->blocklist); i < j; ++i) {
                if (sorted_list_get(heap->blocklist, i) == (void*)test_header) {
                    break;
                }
            }
            if (i < j) {
                sorted_list_remove(heap->blocklist, i);
            }
        }
    }
    if (add) {
        sorted_list_add(heap->blocklist, (void*)header);
    }
}

void* kmalloc(size_t size)
{
    if (!(kmalloc_allowed)) {
        panic("%s called before heap initialised", __func__);
    }
    void* p = heap_alloc(__kernel_heap__, size, true);
    if (p) {
        ++alloc_count;
    }
    return p;
}

void kfree(void* ptr)
{
    if (!(kmalloc_allowed)) {
        panic("%s called before heap initialised", __func__);
    }
    heap_free(__kernel_heap__, ptr);
    ++free_count;
    DEBUG_ASSERT(free_count <= alloc_count);
}

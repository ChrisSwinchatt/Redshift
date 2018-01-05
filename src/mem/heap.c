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
#include <redshift/util/sorted_array.h>
#include <redshift/mem/common.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/paging.h>
#include <redshift/mem/static.h>

enum {
    HEAP_SIZE_MIN  = 0x00070000,
    BLOCKLIST_SIZE = 0x00002000,
    BLOCK_MAGIC    = 0x600DB10C
};

typedef enum {
    BLOCK_FLAGS_ALLOCATED = 0,
    BLOCK_FLAGS_IS_HOLE   = 1 << 0
} block_flags_t;

struct heap {
    struct sorted_array* blocklist;   /* List of blocks ordered from smallest to largest. */
    uintptr_t           start;        /* Heap start address.                              */
    uintptr_t           end;          /* Heap end address.                                */
    size_t              max_size;     /* Maximum size of heap (end - start).              */
    int                 alloc_count;  /* Number of times memory was allocated.            */
    int                 free_count;   /* Number of times memory was freed.                */
    uint64_t            bytes_allocd; /* Number of bytes currently allocated.             */
    int                 flags;        /* Heap flags.                                      */
};

struct blockheader {
    uint32_t magic;
    uint32_t flags;
    uint32_t size;
    uint8_t  pad[4]; /* Pad to 16 bytes */
} __packed;

struct blockfooter {
    uint32_t magic;
    struct   blockheader* header;
    uint8_t  pad[8]; /* Pad to 16 bytes */
} __packed;

struct heap* __kernel_heap__ = NULL;

enum {
    HEAP_SIZE_INIT = 0x00100000UL
};

/* Scan the memory map for a region big enough to store the heap. */
static uint32_t get_heap_region(uint64_t size)
{
    DEBUG_ASSERT(size != 0);
    const struct memory_map* memmap = memory_map_head();
    DEBUG_ASSERT(memory_map_size() > 0 && memmap != NULL);
    do {
        const uint64_t region_size = memmap->end - memmap->start;
        if (memmap->type == MEMORY_TYPE_AVAILABLE && region_size >= size) {
            return memmap->start;
        }
    } while ((memmap = memmap->next) != NULL);
    panic("%s: out of memory", __func__);
}

static int32_t get_smallest_hole(struct heap* heap, size_t size, bool aligned)
{
    DEBUG_ASSERT(heap != NULL);
    DEBUG_ASSERT(size != 0);
    for (uint32_t i = 0, j = sorted_array_count(heap->blocklist); i < j; ++i) {
        struct blockheader* header = (struct blockheader*)sorted_array_get(heap->blocklist, i);
        if (aligned) {
            uintptr_t address = (uintptr_t)header;
            int32_t offset = 0;
            if (((address + sizeof(*header)) & PAGE_MASK) != 0) {
                offset = PAGE_SIZE - (address + sizeof(*header)) % PAGE_SIZE;
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

/* Returns true if its left argument comes before its right argument. */
static bool blockheader_ascending_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pb != NULL);
    if (pa == NULL) {
        return false;
    }
    struct blockheader* a = (struct blockheader*)pa;
    struct blockheader* b = (struct blockheader*)pb;
    return a->size < b->size;
}

struct heap* create_heap(uint32_t start, uint32_t end, size_t max_size, heap_flags_t flags)
{
    DEBUG_ASSERT(start % PAGE_SIZE == 0);
    DEBUG_ASSERT(end   % PAGE_SIZE == 0);
    struct heap* heap = static_alloc(sizeof(*heap));
    DEBUG_ASSERT(heap != NULL);
    memset(heap, 0, sizeof(*heap));
    heap->blocklist = place_sorted_array((void*)start, BLOCKLIST_SIZE, true, blockheader_ascending_predicate);
    DEBUG_ASSERT(heap->blocklist != NULL);
    if (heap->blocklist == NULL) {
        panic("failed to allocate memory.\n");
    }
    start += sizeof(void*)*BLOCKLIST_SIZE;
    if (start & PAGE_MASK) {
        start &= PAGE_MASK;
        start += PAGE_SIZE;
    }
    printk(PRINTK_DEBUG "Placing kernel heap at 0x%08lX\n", start);
    heap->start    = start;
    heap->end      = end;
    heap->max_size = max_size;
    heap->flags    = flags;
    struct blockheader* hole = (struct blockheader*)start;
    hole->size  = end - start;
    hole->magic = BLOCK_MAGIC;
    hole->flags = BLOCK_FLAGS_IS_HOLE;
    sorted_array_add(heap->blocklist, (void*)hole);
    return heap;
}

static size_t get_heap_size(struct heap* heap)
{
    return heap->end - heap->start;
}

static page_flags_t heap_flags_to_page_flags(const struct heap* heap, page_flags_t page_flags)
{
    if (TEST_FLAG(heap->flags, HEAP_FLAGS_USER_MODE)) {
        page_flags |= PAGE_FLAGS_USER_MODE;
    }
    if (TEST_FLAG(heap->flags, HEAP_FLAGS_WRITEABLE)) {
        page_flags |= PAGE_FLAGS_WRITEABLE;
    }
    return page_flags;
}

static void heap_expand(struct heap* heap, size_t new_size)
{
    const size_t old_size = get_heap_size(heap);
    DEBUG_ASSERT(new_size > old_size);
    if (new_size & PAGE_MASK) {
        new_size &= PAGE_MASK;
        new_size += PAGE_SIZE;
    }
    printk(
        PRINTK_DEBUG "Expanding heap: <old_size=%lu,new_size=%lu,max_size=%lu>\n",
        old_size,
        new_size,
        heap->max_size
    );
    DEBUG_ASSERT(new_size < heap->max_size);
    uint32_t i;
    const uint32_t page_flags = heap_flags_to_page_flags(heap, PAGE_FLAGS_PRESENT);
    for (i = old_size; i < new_size; i += PAGE_SIZE) {
        frame_alloc(page_get(heap->start + i, kernel_directory, true), page_flags);
    }
    heap->end = heap->start + new_size;
}

static struct blockheader* place_header(uint32_t addr, size_t size, block_flags_t flags)
{
    struct blockheader* header = (struct blockheader*)addr;
    memset(header, 0, sizeof(*header));
    header->magic = BLOCK_MAGIC;
    header->flags = (uint32_t)flags;
    header->size  = size;
    return header;
}

static struct blockfooter* place_footer(uint32_t addr, struct blockheader* header)
{
    struct blockfooter* footer = (struct blockfooter*)addr;
    memset(footer, 0, sizeof(*footer));
    footer->magic  = BLOCK_MAGIC;
    footer->header = header;
    return footer;
}

static int32_t get_last_block(const struct heap* heap)
{
    uint32_t last_value = 0;
    int32_t  last_index = -1;
    for (uint32_t i = 0, j = sorted_array_count(heap->blocklist); i < j; ++i) {
        uint32_t value = (uint32_t)sorted_array_get(heap->blocklist, i);
        if (value > last_value) {
            DEBUG_ASSERT(i < (uint32_t)INT32_MAX);
            last_value = value;
            last_index = (int32_t)i;
        }
    }
    return last_index;
}

/* Create a new block header (index < 0) or update an existing one (index >= 0), then create a new footer. */
static struct blockheader* create_or_update_block(
    struct heap*  heap,
    int32_t       index,
    uintptr_t     address,
    size_t        size,
    block_flags_t flags)
{
    struct blockheader* header = NULL;
    if (index < 0) {
        header = place_header(address, size, flags);
    } else {
        header = sorted_array_get(heap->blocklist, index);
    }
    DEBUG_ASSERT(header != NULL);
    struct blockfooter* footer = place_footer(address + header->size - sizeof(*footer), header);
    DEBUG_ASSERT(footer != NULL);
    sorted_array_add(heap->blocklist, (void*)header);
    return header;
}

/* Create a hole at the end of a  */
static void* create_hole_and_alloc(struct heap* heap, size_t size, bool align, size_t new_size)
{
    /* Expand the heap.
     */
    const uint32_t old_len = heap->end - heap->start;
    const uint32_t old_end = heap->end;
    heap_expand(heap, old_len + new_size);
    const uint32_t new_len = heap->end - heap->start;
    /* Find the header furthest to the end of the heap.
     */
    const int32_t index = get_last_block(heap);
    create_or_update_block(heap, index, old_end, old_len - new_len, BLOCK_FLAGS_IS_HOLE);
    /* Try to allocate again.
     */
    return heap_alloc(heap, size, align);
}

static struct blockheader* create_hole(struct heap* heap, uintptr_t address, size_t size)
{
    struct blockheader* header = place_header(address, size, BLOCK_FLAGS_IS_HOLE);
    DEBUG_ASSERT(header != NULL);
    /* We can't use place_footer here because footer might be at the end of the heap, and then it would be an error
     * to dereference it.
     */
    struct blockfooter* footer = (struct blockfooter*)((uintptr_t)header + size - sizeof(*footer));
    DEBUG_ASSERT(footer != NULL);
    if ((uint32_t)footer < heap->end) {
        footer->magic  = BLOCK_MAGIC;
        footer->header = header;
    }
    sorted_array_add(heap->blocklist, (void*)header);
    return header;
}

static void* alloc_with_hole(struct heap* heap, int32_t hole, size_t old_size, size_t new_size, bool align)
{
    DEBUG_ASSERT(hole >= 0);
    DEBUG_ASSERT(new_size > old_size);
    struct blockheader* original_header = (struct blockheader*)sorted_array_get(heap->blocklist, hole);
    DEBUG_ASSERT(original_header != NULL);
    uintptr_t original_addr = (uintptr_t)original_header;
    size_t    original_size = original_header->size;
    if (original_size - new_size < sizeof(struct blockheader) + sizeof(struct blockfooter)) {
        /* If there is space left over, but not enough to create a hole, expand the allocated memory to include the
         * extra space.
         */
        old_size += original_size - new_size;
        new_size =  original_size;
    }
    if (align && (original_addr & PAGE_MASK)) {
        /* If address needs to be aligned, page-align it and create a new hole in front.
         */
        const size_t              adjusted_size = PAGE_SIZE - (original_addr & 0xFFF) - sizeof(struct blockheader);
        const uintptr_t           new_addr      = original_addr + adjusted_size;
        const struct blockheader* header        = create_hole(heap, original_addr, adjusted_size);
        original_addr  = new_addr;
        original_size -= header->size;
        DEBUG_ASSERT(original_size > 0);
    } else {
        /* Delete the hole.
         */
        sorted_array_remove(heap->blocklist, hole);
    }
    /* Create the block.
     */
    struct blockheader* header = create_or_update_block(heap, -1, original_addr, new_size, BLOCK_FLAGS_ALLOCATED);
    if (old_size - new_size > 0) {
        /* Create a new hole after the allocated block.
         */
        const uintptr_t hole_address = original_addr + sizeof(struct blockheader) + old_size + sizeof(struct blockfooter);
        const size_t    hole_size    = original_size - new_size;
        DEBUG_ASSERT(hole_size > 0);
        create_hole(heap, hole_address, hole_size);
    }
    /* Return a pointer to the usable memory.
     */
    return header;
}

void* heap_alloc(struct heap* heap, size_t size, bool align)
{
    DEBUG_ASSERT(heap != NULL);
    DEBUG_ASSERT(size != 0);
    /* Try to find a hole big enough to contain the memory block.
     */
    uint32_t new_size = size + sizeof(struct blockheader) + sizeof(struct blockfooter);
    int32_t hole      = get_smallest_hole(heap, new_size, align);
    if (hole < 0) {
        /* No hole big enough - try to create a new hole.
         */
        return create_hole_and_alloc(heap, size, new_size, align);
    }
    /* Allocate using the hole we found.
     */
    struct blockheader* header = alloc_with_hole(heap, hole, size, new_size, align);
    /* Update statistics.
     */
    heap->alloc_count++;
    heap->bytes_allocd += header->size;
    return (void*)((uintptr_t)header + sizeof(*header));
}

static uint32_t heap_contract(struct heap* heap, size_t new_size)
{
    uint32_t old_size = heap->end - heap->start;
    DEBUG_ASSERT(new_size < old_size);
    if (new_size & PAGE_SIZE) {
        new_size &= PAGE_SIZE;
        new_size += PAGE_SIZE;
    }
    if (new_size < HEAP_SIZE_MIN)
        new_size = HEAP_SIZE_MIN;
    uint32_t i;
    for (i = old_size - PAGE_SIZE; i > new_size; i -= PAGE_SIZE) {
        frame_free(page_get(heap->start + i, kernel_directory, false));
    }
    heap->end = heap->start + new_size;
    return new_size;
}

/* Try to unify a hole with holes to the left or right. Returns 1 if unified left, 2 if unified right, 3 if both, 0 if neither. */
static int unify_holes(struct heap* heap, struct blockheader** header, struct blockfooter** footer)
{
    DEBUG_ASSERT(header != NULL);
    DEBUG_ASSERT(*header != NULL);
    DEBUG_ASSERT(footer != NULL);
    DEBUG_ASSERT(*footer != NULL);
    int ret = 0;
    struct blockfooter* test_footer = (struct blockfooter*)((uintptr_t)*header - sizeof(*test_footer));
    if (test_footer->magic == BLOCK_MAGIC && TEST_FLAG(test_footer->header->flags, BLOCK_FLAGS_IS_HOLE)) {
       /* Unify with the hole to the left.
        */
        size_t cached_size = (*header)->size;
        *header = test_footer->header;
        (*footer)->header = *header;
        (*header)->size += cached_size;
        ret += 1;
    }
    struct blockheader* test_header = (struct blockheader*)((uintptr_t)(*footer) + sizeof(**footer));
    if (test_header->magic == BLOCK_MAGIC && TEST_FLAG(test_header->flags, BLOCK_FLAGS_IS_HOLE)) {
        /* Unify with the hole to the right.
         */
        (*header)->size += test_header->size;
        test_footer = (struct blockfooter*)((uintptr_t)*header + (*header)->size - sizeof(*test_footer));
        *footer = test_footer;
        (*footer)->magic  = BLOCK_MAGIC;
        (*footer)->header = *header;
        /* Remove the header from the blocklist.
        */
        uint32_t i, j;
        for (i = 0, j = sorted_array_count(heap->blocklist); i < j; ++i) {
            if (sorted_array_get(heap->blocklist, i) == (void*)test_header) {
               break;
            }
        }
        DEBUG_ASSERT(i < j);
        sorted_array_remove(heap->blocklist, i);
        ret += 2;
    }
    return ret;
}

void heap_free(struct heap* heap, void* ptr)
{
    DEBUG_ASSERT(heap != NULL);
    if (ptr == NULL) {
        return;
    }
    /* Find the block's header and footer and mark it as a hole.
     */
    struct blockheader* header = (struct blockheader*)((uintptr_t)ptr - sizeof(*header));
    struct blockfooter* footer = (struct blockfooter*)((uintptr_t)header + header->size - sizeof(*footer));
    DEBUG_ASSERT(header->magic == BLOCK_MAGIC);
    DEBUG_ASSERT(footer->magic == BLOCK_MAGIC);
    DEBUG_ASSERT(footer->header == header);
    const size_t original_size = header->size;
    header->flags |= BLOCK_FLAGS_IS_HOLE;
    /* Attempt to unify the hole with adjacent holes.
     */
    const int  unify_result = unify_holes(heap, &header, &footer);
    const bool unified_left = unify_result == 1 || unify_result == 3;
    const bool add_to_list  = !(unified_left);
    if ((uint32_t)footer + sizeof(*footer) >= heap->end) {
        /* Contract the heap.
         */
        const size_t old_len = heap->end - heap->start;
        const size_t new_len = heap_contract(heap, (uint32_t)header - heap->start);
        if (header->size - (old_len - new_len) > 0) {
            /* Resize the current block.
             */
            header->size -= old_len - new_len;
            footer = (struct blockfooter*)((uint32_t)footer + header->size - sizeof(*footer));
            footer->magic  = BLOCK_MAGIC;
            footer->header = header;
        } else {
            /* Current block will no longer exist: remove it from the blocklist.
             */
            uint32_t i, j;
            for (i = 0, j = sorted_array_count(heap->blocklist); i < j; ++i) {
                if (sorted_array_get(heap->blocklist, i) == (void*)header) {
                    break;
                }
            }
            if (i < j) {
                sorted_array_remove(heap->blocklist, i);
            }
        }
    }
    if (add_to_list) {
        sorted_array_add(heap->blocklist, (void*)header);
    }
    /* Update heap statistics.
     */
    heap->free_count++;
    heap->bytes_allocd -= original_size;
    DEBUG_ASSERT(heap->free_count <= heap->alloc_count);         /* Check for double-free. */
    DEBUG_ASSERT(heap->bytes_allocd <= heap->end - heap->start); /* Check we haven't allocated more bytes than available. */
}

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

kmalloc_fn_t kmalloc_fn = pre_init_kmalloc;
kfree_fn_t   kfree_fn   = pre_init_kfree;

int heap_init(void)
{
    const uint32_t start = get_heap_region(HEAP_SIZE_INIT);
    DEBUG_ASSERT(UINT32_MAX - HEAP_SIZE_INIT > start);
    /* Try to create the heap.
     */
    __kernel_heap__ = create_heap(start, start + HEAP_SIZE_INIT, HEAP_SIZE_INIT, HEAP_FLAGS_WRITEABLE);
    if (__kernel_heap__ == NULL) {
        panic("failed to create heap");
    }
    /* Enable kmalloc/kfree.
     */
    kmalloc_fn = real_kmalloc;
    kfree_fn   = real_kfree;
    return 0;
}

void* kmalloc(size_t size)
{
    return kmalloc_fn(size);
}

void kfree(void* ptr)
{
    kfree_fn(ptr);
}

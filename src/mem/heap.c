/**
 * \file mem/heap.c
 * \brief Heap memory manager.
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

struct heap {
    struct sorted_array* blocklist;   /* List of blocks ordered from smallest to largest. */
    uintptr_t           start;        /* Heap start address.                              */
    uintptr_t           end;          /* Heap end address.                                */
    size_t              max_size;     /* Maximum size of heap (end - start).              */
    unsigned            alloc_count;  /* Number of times memory was allocated.            */
    unsigned            free_count;   /* Number of times memory was freed.                */
    uint64_t            bytes_allocd; /* Number of bytes currently allocated.             */
    int                 flags;        /* Heap flags.                                      */
};

typedef enum {
    BLOCK_FLAGS_ALLOCATED = 0,       /* Marks an allocated (usable/in use) block. */
    BLOCK_FLAGS_AVAILABLE = 1 << 0   /* Marks an available block (hole).          */
} block_flags_t;

struct blockheader {
    uint32_t magic;  /* Magic number. */
    uint32_t flags;  /* Block flags. */
    uint32_t size;   /* Size of the block. Excludes the header and footer. */
    uint8_t  pad[4]; /* Pad to 16 bytes */
} __packed;

struct blockfooter {
    uint32_t magic;
    struct   blockheader* header;
    uint8_t  pad[8]; /* Pad to 16 bytes */
} __packed;

enum {
    MINIMUM_HEAP_SIZE  = 0x00080000UL,                                           /* Minimum size of a heap (512 kiB).   */
    INITIAL_HEAP_SIZE  = 0x00100000UL,                                           /* Initial size of a heap (1 MiB).     */
    BLOCKLIST_SIZE     = 0x00002000UL,                                           /* Maximum number of blocks and holes. */
    BLOCK_MAGIC        = 0x600DB10CUL,                                           /* Block magic number.                 */
    MINIMUM_BLOCK_SIZE = sizeof(struct blockheader) + sizeof(struct blockfooter) /* Size of an empty block.             */
};

struct heap* __kernel_heap__ = NULL;

extern uintptr_t heap_addr; /* mem/static.c */

/* Get the amount of memory owned by a heap. Does not include the heap structure itself. */
static size_t get_heap_size(struct heap* heap)
{
    return heap->end - heap->start;
}

/* Get the address of the usable memory which immediately follows a block.  */
static uintptr_t get_usable_address(struct blockheader* header)
{
    return (uintptr_t)header + sizeof(*header);
}

/* Get the address of a block footer. */
static uintptr_t get_footer_address(struct blockheader* header)
{
    return get_usable_address(header) + header->size;
}

/* Get the address of the end of a block footer. */
static uintptr_t get_footer_end(struct blockfooter* footer)
{
    return (uintptr_t)footer + sizeof(*footer);
}

/* Get the total size of a block (including the header and footer). */
static size_t total_block_size(size_t alloc_size)
{
    return MINIMUM_BLOCK_SIZE + alloc_size;
}

/* Get the size of the usable portion of a block/hole. */
static size_t usable_block_size(size_t total_size)
{
    return total_size - MINIMUM_BLOCK_SIZE;
}

/* Scan the memory map for a region big enough to store the heap. */
static uint32_t get_heap_region(uint64_t size)
{
    DEBUG_ASSERT(size != 0);
    const size_t map_size = memory_map_size();
    DEBUG_ASSERT(memory_map_size() > 0);
    const struct memory_map* region = memory_map_head();
    for (size_t i = 0; i < map_size; ++i, region = region->next) {
        DEBUG_ASSERT(region != NULL);
        const uint64_t region_size = region->end - region->start;
        if (region_size < size) {
            continue;
        } else if (region->type == MEMORY_TYPE_AVAILABLE) {
            uintptr_t address = region->start;
            /* Check we're not about to overwrite the kernel.
            */
            if (region->start < heap_addr && heap_addr <= region->end) {
                address = heap_addr;
            }
            /* Page align the address.
             */
            MAKE_PAGE_ALIGNED(address);
            /* Make sure we can still fit the heap into this region.
             */
            if (address + size <= region->end) {
                /* The static allocator will continue allocating at the end of the kernel heap. This could result in
                 * writing into reserved memory and screwing something up, like ACPI structures or the user's hard drive
                 * This requires redesigning the memory manager somewhat so we'll add it to the to-do list (FIXME).
                 */
                heap_addr = address + size;
                return address;
            }
        } else if (region->type == MEMORY_TYPE_RECLAIMABLE) {
            printk(PRINTK_DEBUG "Reclaimable memory: <start=0x%llX,end=0x%llX>\n", region->start, region->end);
        }
    } while ((region = region->next) != NULL);
    panic("no memory region large enough for kernel heap (%lluK)", size/1024);
}

/* Get the smallest hole at least as big as size. */
static int32_t get_smallest_hole(struct heap* heap, size_t size, bool aligned)
{
    DEBUG_ASSERT(heap != NULL);
    DEBUG_ASSERT(size != 0);
    for (uint32_t i = 0, j = sorted_array_count(heap->blocklist); i < j; ++i) {
        struct blockheader* header = (struct blockheader*)sorted_array_get(heap->blocklist, i);
        if (aligned) {
            uintptr_t address = get_usable_address(header);
            int32_t   offset  = 0;
            if (!(IS_PAGE_ALIGNED(address))) {
                offset = PAGE_SIZE - address%PAGE_SIZE;
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
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    struct blockheader* a = (struct blockheader*)pa;
    struct blockheader* b = (struct blockheader*)pb;
    return a->size < b->size;
}

struct heap* create_heap(uintptr_t start, uintptr_t end, size_t max_size, heap_flags_t flags)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(start % PAGE_SIZE == 0);
    DEBUG_ASSERT(end   % PAGE_SIZE == 0);
    DEBUG_ASSERT(max_size >= end - start);
    printk(
        PRINTK_DEBUG "Placing kernel heap: <start=0x%08lX,end=0x%08lX,size=%luK,max=%luK>\n",
        start,
        end,
        (end - start)/1024,
        max_size/1024
    );
    struct heap* heap = static_alloc(sizeof(*heap));
    DEBUG_ASSERT(heap != NULL);
    memset(heap, 0, sizeof(*heap));
    heap->blocklist = place_sorted_array((void*)start, BLOCKLIST_SIZE, SORTED_ARRAY_STATIC, blockheader_ascending_predicate);
    DEBUG_ASSERT(heap->blocklist != NULL);
    start += BLOCKLIST_SIZE*sizeof(void*);
    heap->start    = start;
    heap->end      = end;
    heap->max_size = max_size;
    heap->flags    = flags;
    struct blockheader* hole = (struct blockheader*)start;
    hole->size  = usable_block_size(end - start);
    hole->magic = BLOCK_MAGIC;
    hole->flags = BLOCK_FLAGS_AVAILABLE;
    sorted_array_add(heap->blocklist, (void*)hole);
    RESTORE_INTERRUPT_STATE;
    return heap;
}

/* Convert heap_flags_t to page_flags_t. */
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

/* Expand the heap. */
static void heap_expand(struct heap* heap, size_t new_size)
{
    const size_t old_size = get_heap_size(heap);
    DEBUG_ASSERT(new_size > old_size);
    MAKE_PAGE_ALIGNED(new_size);
    printk(
        PRINTK_DEBUG "Expanding heap: <from=%luK,by%luK,to=%luK,max_size=%luK>\n",
        old_size/1024UL,
        (new_size - old_size)/1024UL,
        new_size/1024UL,
        heap->max_size/1024UL
    );
    DEBUG_ASSERT(new_size <= heap->max_size);
    uint32_t i;
    const uint32_t page_flags = heap_flags_to_page_flags(heap, PAGE_FLAGS_PRESENT);
    for (i = old_size; i < new_size; i += PAGE_SIZE) {
        frame_alloc(page_get(heap->start + i, kernel_directory, true), page_flags);
    }
    heap->end = heap->start + new_size;
}

/* Place a blockheader at a particular address. Address should be 4-bytes aligned. */
static struct blockheader* place_header(uintptr_t address, size_t size, block_flags_t flags)
{
    struct blockheader* header = (struct blockheader*)address;
    memset(header, 0, sizeof(*header));
    header->magic = BLOCK_MAGIC;
    header->size  = size;
    header->flags = (uint32_t)flags;
    return header;
}

/* Place a blockfooter to match a given blockheader. */
static struct blockfooter* place_footer(struct blockheader* header)
{
    struct blockfooter* footer = (struct blockfooter*)get_footer_address(header);
    memset(footer, 0, sizeof(*footer));
    footer->magic  = BLOCK_MAGIC;
    footer->header = header;
    return footer;
}

/* Get the index of the last block added to the blocklist. */
static uint32_t get_last_block(const struct heap* heap)
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
    size_t        alloc_size,
    block_flags_t flags)
{
    if (index >= 0) {
        address = (uintptr_t)sorted_array_get(heap->blocklist, index);
        DEBUG_ASSERT(address != 0);
    }
    struct blockheader* header = place_header(address, alloc_size, flags);
    struct blockfooter* footer = place_footer(header);
    DEBUG_ASSERT(footer != NULL);
    sorted_array_add(heap->blocklist, (void*)header);
    return header;
}

/* Create a hole at address. The hole may be smaller than hole_size if address+hole_size is past the end of the heap. */
static struct blockheader* create_hole(struct heap* heap, uintptr_t address, size_t hole_size)
{
    /* Make sure we don't write past the end of the heap.
     */
    if (address + total_block_size(hole_size) > heap->end) {
        printk(PRINTK_WARNING "Trying to create hole past end of heap");
        hole_size -= sizeof(struct blockfooter);
    }
    struct blockheader* header = place_header(address, hole_size, BLOCK_FLAGS_AVAILABLE);
    place_footer(header);
    sorted_array_add(heap->blocklist, (void*)header);
    return header;
}

/* Create a hole at the end of a heap and try to allocate within it. */
static void* create_hole_and_alloc(struct heap* heap, size_t alloc_size, bool page_align)
{
    /* Expand the heap.
     */
    const uint32_t old_end = heap->end;
    const uint32_t old_len = get_heap_size(heap);
    size_t block_size = total_block_size(alloc_size);
    heap_expand(heap, old_len + block_size);
    const uint32_t new_len = get_heap_size(heap);
    DEBUG_ASSERT(new_len >= alloc_size);
    /* Find the header furthest to the end of the heap.
     */
    create_or_update_block(heap, get_last_block(heap), old_end, alloc_size, BLOCK_FLAGS_AVAILABLE);
    /* Try to allocate again.
     */
    return heap_alloc(heap, alloc_size, page_align);
}

/* Allocate "inside" a hole. */
static void* alloc_with_hole(struct heap* heap, int32_t hole, size_t alloc_size, bool page_align)
{
    DEBUG_ASSERT(hole >= 0);
    struct blockheader* original_header = (struct blockheader*)sorted_array_get(heap->blocklist, hole);
    DEBUG_ASSERT(original_header != NULL);
    size_t    block_size    = total_block_size(alloc_size);
    uintptr_t original_addr = (uintptr_t)original_header;
    size_t    original_size = original_header->size;
    if (original_size - block_size <= MINIMUM_BLOCK_SIZE) {
        /* If there is space left over, but not enough to create a hole, expand the allocated memory to include the
         * extra space.
         */
        alloc_size += original_size - block_size;
        block_size = total_block_size(alloc_size);
    }
    if (page_align && !(IS_PAGE_ALIGNED(original_addr))) {
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
    struct blockheader* header = create_or_update_block(heap, -1, original_addr, block_size, BLOCK_FLAGS_ALLOCATED);
    if (alloc_size - block_size > MINIMUM_BLOCK_SIZE) {
        /* Create a new hole after the allocated block.
         */
        const uintptr_t hole_address = original_addr + sizeof(struct blockheader) + alloc_size + sizeof(struct blockfooter);
        const size_t    hole_size    = original_size - MINIMUM_BLOCK_SIZE;
        DEBUG_ASSERT(hole_size > 0);
        create_hole(heap, hole_address, hole_size);
    }
    /* Return a pointer to the usable memory.
     */
    return header;
}

void* heap_alloc(struct heap* heap, size_t size, bool page_align)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(heap != NULL);
    DEBUG_ASSERT(size != 0);
    /* Find the smallest hole big enough to contain the allocated memory.
     */
    const size_t  block_size = total_block_size(size);
    const int32_t hole       = get_smallest_hole(heap, block_size, page_align);
    if (hole < 0) {
        /* No hole big enough - try to create a new hole.
         */
        void* ptr = create_hole_and_alloc(heap, size, page_align);
        RESTORE_INTERRUPT_STATE;
        return ptr;
    }
    /* Allocate using the hole we found.
     */
    struct blockheader* header = alloc_with_hole(heap, hole, size, page_align);
    /* Update statistics & return the allocated block (usable part).
     */
    heap->alloc_count++;
    heap->bytes_allocd += header->size;
    RESTORE_INTERRUPT_STATE;
    return (void*)get_usable_address(header);
}

static uint32_t heap_contract(struct heap* heap, size_t new_size)
{
    uint32_t old_size = get_heap_size(heap);
    DEBUG_ASSERT(new_size < old_size);
    MAKE_PAGE_ALIGNED(new_size);
    if (new_size < MINIMUM_HEAP_SIZE) {
        new_size = MINIMUM_HEAP_SIZE;
    }
    uint32_t i = 0;
    for (i = old_size - PAGE_SIZE; i > new_size; i -= PAGE_SIZE) {
        frame_free(page_get(heap->start + i, kernel_directory, false));
    }
    heap->end = heap->start + new_size;
    return new_size;
}

enum {
    NO_UNIFY    = 0,
    UNIFY_LEFT  = 1 << 0,
    UNIFY_RIGHT = 1 << 1
};

static int unify_left(struct blockheader** pheader, struct blockfooter* footer)
{
    struct blockfooter* test_footer = (struct blockfooter*)((uintptr_t)*pheader - sizeof(*test_footer));
    if (test_footer->magic == BLOCK_MAGIC && TEST_FLAG(test_footer->header->flags, BLOCK_FLAGS_AVAILABLE)) {
       /* Unify with the hole to the left.
        */
        size_t cached_size = (*pheader)->size;
        *pheader = test_footer->header;
        footer->header = *pheader;
        (*pheader)->size += cached_size;
        return UNIFY_LEFT;
    }
    return NO_UNIFY;
}

static int unify_right(struct heap* heap, struct blockheader* header, struct blockfooter** pfooter)
{
    struct blockheader* test_header = (struct blockheader*)((uintptr_t)*pfooter + sizeof(**pfooter));
    if (test_header->magic == BLOCK_MAGIC && TEST_FLAG(test_header->flags, BLOCK_FLAGS_AVAILABLE)) {
        /* Unify with the hole to the right.
         */
        header->size += test_header->size;
        *pfooter = (struct blockfooter*)get_footer_address(test_header);
        (*pfooter)->magic  = BLOCK_MAGIC;
        (*pfooter)->header = header;
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
        return UNIFY_RIGHT;
    }
    return NO_UNIFY;
}

/* Try to unify a hole with holes to the left or right. Returns 1 if unified left, 2 if unified right, 3 if both, 0 if neither. */
static int unify_holes(struct heap* heap, struct blockheader** header, struct blockfooter** footer)
{
    DEBUG_ASSERT(header != NULL);
    DEBUG_ASSERT(*header != NULL);
    DEBUG_ASSERT(footer != NULL);
    DEBUG_ASSERT(*footer != NULL);
    return unify_left(header, *footer) | unify_right(heap, *header, footer);
}

void heap_free(struct heap* heap, void* ptr)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(heap != NULL);
    if (ptr == NULL) {
        RESTORE_INTERRUPT_STATE;
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
    header->flags |= BLOCK_FLAGS_AVAILABLE;
    /* Attempt to unify the hole with adjacent holes.
     */
    const int  unify_result = unify_holes(heap, &header, &footer);
    const bool unified_left = TEST_FLAG(unify_result, UNIFY_LEFT);
    const bool add_to_list  = !(unified_left);
    if (get_footer_end(footer) >= heap->end) {
        /* Contract the heap.
         */
        const size_t old_len = get_heap_size(heap);
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
    DEBUG_ASSERT(heap->bytes_allocd <= get_heap_size(heap)); /* Check we haven't allocated more bytes than available. */
    RESTORE_INTERRUPT_STATE;
}


void heap_init(void)
{
    SAVE_INTERRUPT_STATE;
    const uint32_t start = get_heap_region(INITIAL_HEAP_SIZE);
    DEBUG_ASSERT(UINT32_MAX - INITIAL_HEAP_SIZE > start);
    /* Try to create the heap.
     */
    __kernel_heap__ = create_heap(start, start + INITIAL_HEAP_SIZE, INITIAL_HEAP_SIZE, HEAP_FLAGS_WRITEABLE);
    if (__kernel_heap__ == NULL) {
        panic("failed to create heap");
    }
    /* Enable kmalloc/kfree and disable static_alloc.
     */
    enable_kmalloc();
    RESTORE_INTERRUPT_STATE;
}

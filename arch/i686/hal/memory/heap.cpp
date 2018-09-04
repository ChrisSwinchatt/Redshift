/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include <libk/sorted_array.hpp>
#include <redshift/hal/memory.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/interrupt.hpp>

extern "C" uint32_t heap_addr;

using ::libk::sorted_array;

namespace redshift { namespace hal { namespace memory_detail {
    heap* heap::kernel_heap = nullptr;

    heap* heap::create(
        uintptr_t start,
        size_t    init_size,
        size_t    max_size = 0,
        size_t    min_size = 0,
        flags_t   flags    = flags::supervisor
    )
    {
        // Allocate the heap.
        heap* address = static_alloc(sizeof(*address));
        if (TEST_FLAG(flags, flags::supervisor)) {
            kernel_heap = address;
        }
        // Construct it in place and return it.
        return new (address) heap(start, init_size, max_size, min_size, flags);
    }

    void* heap::allocate(size_t size)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        DEBUG_ASSERT(size > 0);
        const int32_t hole = get_smallest_hole(size + MINIMUM_BLOCK_SIZE, ::page);
        if (hole < 0) {
            return create_hole_and_alloc(size, ::page);
        }
        blockheader* header = alloc_with_hole(hole, size, ::page);
        m_alloc_count++;
        m_bytes_allocated += header->size;
        return reinterpret_cast<void*>(header->usable_address());
    }

    void heap::free(void* ptr)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        if (ptr == nullptr) {
            return;
        }
        // Get the header and footer.
        blockheader* header = blockheader::get(ptr);
        DEBUG_ASSERT(header != nullptr);
        DEBUG_ASSERT(header->magic == BLOCK_MAGIC);
        blockfooter* footer = blockfooter(header);
        DEBUG_ASSERT(footer->magic  == BLOCK_MAGIC);
        DEBUG_ASSERT(footer->header == header);
        const size_t original_size = header->size;
        // Mark the block as a hole.
        header->flags &= ~(block_flags::allocated);
        header->flags |= block_flags::available;
        // Try to unify with adjacent hole(s). If we unified to the left, the resulting hole is in the blocklist.
        // If we could not unify or only unified right, the hole needs to be added later.
        const int  unify_result = unify_holes(&header, &footer);
        const bool unified_left = unify_result == 1 || unify_result == 3;
        const bool add_to_list  = !(unified_left);
        // If the block being freed is at the end of the heap, contract the heap.
        if (footer->end_address() >= address() + m_size) {
            const size_t old_len = m_size;
            contract(reinterpret_cast<uintptr_t>(header) - address());
            const size_t new_len = m_size;
            const size_t diff    = old_len - new_len;
            // Resize the block if its size is greater than the contraction size.
            if (header->size > diff) {
                header->size   = diff;
                footer->magic  = BLOCK_INVALID;
                footer         = blockfooter::place(header);
            } else {
                // Remove the block from the blocklist.
                header->magic = BLOCK_INVALID;
                footer->magic = BLOCK_INVALID;
                m_blocklist.remove(header);
            }
        }
        if (add_to_list) {
            blocklist.add(header);
        }
        m_free_count++;
        m_bytes_allocd -= original_size;
        DEBUG_ASSERT(m_free_count <= m_alloc_count); // Check for double-free.
        DEBUG_ASSERT(m_bytes_allocd <= m_size);          // Check we haven't allocated more bytes than available.
    }

    void* heap::resize(void* ptr, size_t new_size)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        blockheader* header = blockheader::get(ptr);
        blockfooter* footer = blockfooter::place(header);
        if (new_size < header->size) {
            const size_t hole_size = header->size - new_size;
            // Shrink the block.
            header->size  = new_size;
            footer->magic = BLOCK_INVALID;
            footer = blockfooter::place(header);
            if (hole_size > MINIMUM_BLOCK_SIZE) {
                // Create a hole at the end.
                create_hole(footer->end_address(), hole_size);
            } else {
                // Expand the allocated block to fill the extra space.
                header->size += hole_size;
            }
            // Original pointer is still valid.
            return ptr;
        } else if (new_size > header->size) {
            // Check if there's a hole immediately to the right.
            blockheader* next = blockheader::get(footer->end_address());
            DEBUG_ASSERT(next->magic == BLOCK_MAGIC);
            if (TEST_FLAG(next->flags, block_flags::available)) {
                // Expand into the hole.
                next->magic   = BLOCK_INVALID;
                header->size  = new_size;
                footer->magic = BLOCK_INVALID;
                footer = blockfooter::place(header);
                if (hole_size > MINIMUM_BLOCK_SIZE) {
                    // Create a new hole at the end.
                    create_hole(footer->end_address(), hole_size);
                } else {
                    // Expand the allocated block to fill the extra space.
                    header->size += hole_size;
                }
            } else {
                // Try to allocate a new block.
                void* new_ptr = allocate(new_size);
                if (new_ptr == nullptr) {
                    return nullptr;
                }
                // Copy the data.
                libk::memory::copy(new_ptr, ptr, header->size);
                // Free this one.
                free(ptr);
                return new_ptr;
            }
        }
        return ptr;
    }

    size_t heap::size() const
    {
        return m_size;
    }

    size_t heap::min_size() const
    {
        return m_min_size;
    }

    size_t heap::max_size() const
    {
        return m_max_size;
    }

    intmax_t heap::alloc_count() const
    {
        return m_alloc_count;
    }

    intmax_t free_count() const
    {
        return m_free_count;
    }

    uintmax_t heap::bytes_allocated() const
    {
        return m_bytes_allocated;
    }

    uintptr_t heap::address() const
    {
        return reinterpret_cast<uintptr_t>(m_blocklist);
    }

    bool heap::blockheader::ascending_size_predicate(blockheader* a, blockheader* b)
    {
        return a->size < b->size;
    }

    heap::blockheader* heap::blockheader::get(void* ptr)
    {
        return reinterpret_cast<heap::blockheader*>(ptr);
    }

    heap::blockheader* heap::blockheader::get(uintptr_t address)
    {
        return reinterpret_cast<heap::blockheader*>(address);
    }

    heap::blockheader* heap::blockheader::place(void* ptr, size_t size, block_flags_t flags)
    {
        blockheader* header = blockheader::get(ptr);
        return new (header) blockheader(size, flags);
    }

    heap::blockheader* heap::blockheader::place(uintptr_t address, size_t size, block_flags_t flags)
    {
        blockheader* header = blockheader::get(address);
        return new (header) blockheader(size, flags);
    }

    heap::blockheader::blockheader(size_t size_, block_flags_t flags_)
    : magic(BLOCK_MAGIC)
    , size(size_)
    , flags(flags_)
    {
        // Do nothing.
    }

    uintptr_t heap::blockheader::usable_address() const
    {
        return reinterpret_cast<uintptr_t>(this) + sizeof(*this);
    }

    blockfooter* heap::blockheader::footer() const
    {
        return usable_address() + size;
    }

    size_t heap::blockheader::total_size() const
    {
        return size + MINIMUM_BLOCK_SIZE;
    }

    heap::blockfooter* heap::blockfooter::place(blockheader* header_)
    {
        blockfooter* footer = header_->footer();
        return new (footer) blockfooter(header_);
    }

    heap::blockfooter::blockfooter(blockheader* header_)
    : magic(BLOCK_MAGIC)
    , header(header_)
    {
        // Do nothing.
    }

    uintptr_t heap::blockfooter::end_address() const
    {
        return reinterpret_cast<uintptr_t>(this) + sizeof(*this);
    }

    uintptr_t heap::get_heap_region(size_t size) const
    {
        DEBUG_ASSERT(size > 0);
        auto memory_map = hal::memory::get_memory_map();
        DEBUG_ASSERT(memory_map.size() > 0);
        for (size_t i = 0; i < memory_map.size(); ++i) {
            const hal::memory::region& region = memory_map[i];
            if (region.size() < size) {
                continue;
            } else if (region.type == hal::memory::region_type::available) {
                uintptr_t address = static_cast<uintptr_t>(region.start);
                // Check we're not about to overwrite static kernel memory.
                if (region.start < heap_addr && heap_addr <= region.end) {
                    address = heap_addr;
                }
                // Page align the address.
                MAKE_PAGE_ALIGNED(address);
                // Make sure we can still fit the heap into this region.
                if (address + size <= region.end) {
                    // FIXME: The static allocator will continue allocating at the end of the kernel heap. This could
                    // result in writing into reserved memory and screwing something up, like ACPI structures or the
                    // user's hard drive. This requires redesigning the memory manager somewhat so we'll add it to the
                    // to-do list.
                    heap_addr = address + size;
                    return address;
                }
            } else if (region.type == hal::memory::region_type::reclaimable) {
                printk(PRINTK_DEBUG "Reclaimable memory: <start=0x%qX,end=0x%qX>\n", region.start, region.end);
            }
        }
        panic("no memory region large enough for kernel heap (%lluK)", size/1024);
    }

    int32_t heap::get_smallest_hole(size_t size,  align) const
    {
        for (uint32_t i = 0, j = m_blocklist.size(); i < j; ++i) {
            blockheader* header = m_blocklist[i];
            if (aligned) {
                const uintptr_t address = header->usable_address();
                int32_t offset = 0;
                if (!(IS_PAGE_ALIGNED(address))) {
                    offset = PAGE_SIZE - address%PAGE_SIZE;
                }
                const int32_t hole_size = static_cast<int32_t>(header->size - offset);
                if (hole_size >= static_cast<int32_t>(size)) {
                    return i;
                }
            } else if (header->size >= size) {
                return i;
            }
        }
        return -1;
    }

    uint32_t heap::get_last_block() const
    {
        uintptr_t last_value = 0;
        int32_t   last_index = -1;
        for (size_t i = 0, j = m_blocklist.size(); i < j; ++i) {
            uintptr_t value = static_cast<uintptr_t>(m_blocklist[i]);
            if (value > last_value) {
                DEBUG_ASSERT(i < (uint32_t)INT32_MAX);
                last_value = value;
                last_index = (int32_t)i;
            }
        }
        return last_index;
    }

    blockheader* heap::create_or_update_block(int32_t index, uintptr_t address, size_t size, block_flags_t flags)
    {
        if (index >= 0) {
            address = static_cast<uintptr_t>(m_blocklist[index]);
            DEBUG_ASSERT(address != 0);
        }
        blockheader* header = blockheader::place(address, size, flags);
        blockfooter* footer = blockfooter::place(header);
        DEBUG_ASSERT(footer != nullptr);
        m_blocklist.add(header);
        return header;
    }

    blockheader* heap::create_hole(uintptr_t address, size_t size)
    {
        if (address + size + MINIMUM_BLOCK_SIZE > address() + m_size) {
            // Make sure we don't write past the end of the heap.
            printk(PRINTK_WARNING "Trying to create hole past end of heap");
            hole_size -= sizeof(blockfooter);
        }
        blockheader* header = blockheader::place(address, size, block_flags::available);
        blockfooter::place(header);
        m_blocklist.add(header);
        return header;
    }

    void* heap::create_hole_and_alloc(size_t alloc_size,  align)
    {
        // Expand the heap.
        const size_t old_end    = address() + m_size;
        const size_t old_len    = m_size;
        const size_t block_size = alloc_size + MINIMUM_BLOCK_SIZE;
        expand(old_len + block_size);
        const uint32_t new_len  = m_size;
        DEBUG_ASSERT(new_len >= alloc_size);
        // Find the header furthest to the end of the heap.
        create_or_update_block(get_last_block(), old_end, alloc_size, block_flags::available);
        // Try to allocate again.
        return allocate(alloc_size, page_align);
    }

    blockheader* heap::alloc_with_hole(int32_t hole, size_t alloc_size,  align)
    {
        DEBUG_ASSERT(hole >= 0);
        blockheader* original_header = m_blocklist[hole];
        DEBUG_ASSERT(original_header != nullptr);
        size_t    block_size    = total_block_size(alloc_size);
        uintptr_t original_addr = (uintptr_t)original_header;
        size_t    original_size = original_header->size;
        if (original_size - block_size <= MINIMUM_BLOCK_SIZE) {
            // If there is space left over, but not enough to create a hole, expand the allocated memory to include the
            // extra space.
            alloc_size += original_size - block_size;
            block_size = alloc_size + MINIMUM_BLOCK_SIZE;
        }
        if (align == ::page && !(IS_PAGE_ALIGNED(original_addr))) {
            // If address needs to be aligned, page-align it and create a new hole in front.
            const size_t       adjusted_size = PAGE_SIZE - (original_addr & 0xFFF) - sizeof(blockheader);
            const uintptr_t    new_addr      = original_addr + adjusted_size;
            const blockheader* header        = create_hole(original_addr, adjusted_size);
            original_addr  = new_addr;
            original_size -= header->size;
            DEBUG_ASSERT(original_size > 0);
        } else {
            // Delete the hole.
            m_blocklist.remove(hole);
        }
        // Create the block.
        blockheader* header = create_or_update_block(-1, original_addr, block_size, block_flags::allocated);
        if (alloc_size - block_size > MINIMUM_BLOCK_SIZE) {
            // Create a new hole after the allocated block.
            const uintptr_t hole_address = original_addr + sizeof(*header) + alloc_size + sizeof(blockfooter);
            const size_t    hole_size    = original_size - MINIMUM_BLOCK_SIZE;
            DEBUG_ASSERT(hole_size > 0);
            create_hole(hole_address, hole_size);
        }
        // Return the header
        return header;
    }

    void heap::expand(size_t new_size)
    {
        const size_t old_size = m_size;
        DEBUG_ASSERT(new_size > old_size);
        MAKE_PAGE_ALIGNED(new_size);
        printk(
            PRINTK_DEBUG "Expanding heap: <from=%luK,by%luK,to=%luK,max_size=%luK>\n",
            old_size/1024UL,
            (new_size - old_size)/1024UL,
            new_size/1024UL,
            m_max_size/1024UL
        );
        DEBUG_ASSERT(new_size <= m_max_size);
        uint32_t i;
        const uint32_t page_flags = heap_flags_to_page_flags(heap, PAGE_FLAGS_PRESENT);
        for (i = old_size; i < new_size; i += PAGE_SIZE) {
            frame_alloc(page_get(address() + i, kernel_directory, true), page_flags);
        }
        m_size = new_size;
    }

    void heap::contract(size_t new_size)
    {
        uint32_t old_size = m_size;
        DEBUG_ASSERT(new_size < old_size);
        MAKE_PAGE_ALIGNED(new_size);
        if (new_size < m_min_size) {
            new_size = m_min_size;
        }
        uint32_t i = 0;
        for (i = old_size - PAGE_SIZE; i > new_size; i -= PAGE_SIZE) {
            frame_free(page_get(address() + i, kernel_directory, false));
        }
        m_size = new_size;
        return new_size;
    }

    int heap::unify_left(blockheader** pheader, blockfooter* footer)
    {
        blockfooter* test_footer = (blockfooter*)((uintptr_t)*pheader - sizeof(*test_footer));
        if (test_footer->magic == BLOCK_MAGIC && TEST_FLAG(test_footer->header->flags, block_flags::available)) {
           // Unify with the hole to the left.
            size_t cached_size = (*pheader)->size;
            *pheader = test_footer->header;
            footer->header = *pheader;
            (*pheader)->size += cached_size;
            return 1;
        }
        return 0;
    }

    int heap::unify_right(blockheader* header, blockfooter** pfooter)
    {
        blockheader* test_header = (blockheader*)((uintptr_t)*pfooter + sizeof(**pfooter));
        if (test_header->magic == BLOCK_MAGIC && TEST_FLAG(test_header->flags, block_flags::available)) {
            // Unify with the hole to the right.

            header->size += test_header->size;
            *pfooter = (blockfooter*)get_footer_address(test_header);
            (*pfooter)->magic  = BLOCK_MAGIC;
            (*pfooter)->header = header;
            // Remove the header from the blocklist.
            uint32_t i, j;
            for (i = 0, j = m_blocklist.size(); i < j; ++i) {
                if (m_blocklist[i] == test_header) {
                    break;
                }
            }
            DEBUG_ASSERT(i < j);
            m_blocklist.remove_at(i);
            return 2;
        }
        return 0;
    }

    int heap::unify_holes(blockheader** pheader, blockfooter** pfooter)
    {
        DEBUG_ASSERT(pheader != nullptr);
        DEBUG_ASSERT(*pheader != nullptr);
        DEBUG_ASSERT(pfooter != nullptr);
        DEBUG_ASSERT(*pfooter != nullptr);
        return unify_left(pheader, *pfooter) | unify_right(*pheader, pfooter);
    }
}}} // redshift::hal::memory_detail

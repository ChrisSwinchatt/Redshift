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
#include <redshift/boot/multiboot2.hpp>
#include <redshift/hal/memory.hpp>
#include <redshift/kernel.hpp>


namespace redshift { namespace hal {
    void memory::init(const multiboot2_tag* mb_tags)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        get_memory_size(mb_tags);
        init_static();
        init_paging();
        heap::init();
        get_memory_map(mb_tags);
    }

    const memory::memory_map& memory::get_memory_map()
    {
        return m_memory_map;
    }

    size_t memory::size_lower()
    {
        return m_size_lower;
    }

    size_t memory::size_upper()
    {
        return m_size_upper;
    }

    size_t memory::total_size()
    {
        return size_lower() + size_upper();
    }

    void memory::get_memory_size(struct multiboot2_tag* mb_tags)
    {
        DEBUG_ASSERT(mb_tags != nullptr);
        for (
            const multiboot2_tag* tag = reinterpret_cast<const multiboot2_tag*>(reinterpret_cast<uint8_t*>(mb_tags) + 8);
            tag->type != MULTIBOOT2_TAG_TYPE_END;
            tag = reinterpret_cast<multiboot2_tag*>(reinterpret_cast<uint8_t*>(tag) + ((tag->size + 7) & ~7))
        ) {
            switch (tag->type) {
                case MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO:
                {
                    m_size_lower = reinterpret_cast<multiboot2_tag_basic_meminfo*>(tag)->mem_lower;
                    m_size_upper = reinterpret_cast<multiboot2_tag_basic_meminfo*>(tag)->mem_upper;
                    printk(PRINTK_DEBUG "Installed memory: %luK\n", total_size()/1024U);
                    break;
                }
                default:
                    break;
            }
        }
    }

    void memory::init_memory_map(multiboot2_tag* mb_tags)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        DEBUG_ASSERT(mb_tags != nullptr);
        // Save/convert the multiboot2 memory map.
        for (
            multiboot2_tag* tag = reinterpret_cast<multiboot2_tag*>(reinterpret_cast<uint8_t*>(mb_tags) + 8);
            tag->type != MULTIBOOT2_TAG_TYPE_END;
            tag = reinterpret_cast<multiboot2_tag*>(reinterpret_cast<uint8_t*>(tag) + ((tag->size + 7) & ~7))
        ) {
            if (tag->type == MB2MULTIBOOT2_TAG_TYPE_MMAP) {
                printk(PRINTK_DEBUG "Memory map:\n");
                for (
                    multiboot2_mmap_entry* mmap = reinterpret_cast<multiboot2_tag_mmap*>(tag)->entries;
                    reinterpret_cast<uint8_t*>(mmap) < reinterpret_cast<uint8_t*>(tag) + tag->size;
                    mmap = (multiboot2_mmap_entry*)((uint32_t)mmap + ((multiboot2_tag_mmap*)tag)->entry_size)
                ) {
                    // Store the entry.
                    region entry;
                    entry.type  = multiboot2_memory_type_to_region_type(mmap->type);
                    entry.start = mmap->addr;
                    entry.end   = mmap->addr + mmap->len - 1;
                    m_memory_map.append(region)
                    // Log info about the region.
                    {
                        const uint64_t kib = mmap->len / 1024UL;
                        const char* type_str = nullptr;
                        switch (memmap->type) {
                          case MEMORY_TYPE_AVAILABLE:   type_str = "available";   break;
                          case MEMORY_TYPE_RECLAIMABLE: type_str = "reclaimable"; break;
                          default:                      type_str = "unavailable"; break;
                        }
                        printk(
                            PRINTK_DEBUG "Memory map: %6LuK from 0x%016LX to 0x%016LX (%s)\n",
                            kib,
                            memmap->start,
                            memmap->end,
                            type_str
                        );
                    }
                }
            }
        }
        // TODO: sort the map and combine overlapping regions.
    }

    bool memory::memory_map_ascending_order_predicate(region* a, region* b)
    {
        return a->start < b->start;
    }

    memory::region_type memory::multiboot2_memory_type_to_region_type(multiboot2m_memory_type_t type)
    {
        switch (type) {
            case MULTIBOOT2_MEMORY_INVALID:     return region_type::invalid;
            case MULTIBOOT2_MEMORY_AVAILABLE:   return region_type::available;
            case MULTIBOOT2_MEMORY_RESERVED:    return region_type::reserved;
            case MULTIBOOT2_MEMORY_RECLAIMABLE: return region_type::reclaimable;
            case MULTIBOOT2_MEMORY_NVS:         return region_type::nvs;
            default:                            UNREACHABLE("No switch case for type=%u", static_cast<unsigned>(type));
        }
    }
}} // redshift::hal

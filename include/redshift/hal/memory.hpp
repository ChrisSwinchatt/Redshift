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
#ifndef REDSHIFT_HAL_MEMORY_HPP
#define REDSHIFT_HAL_MEMORY_HPP

#include <libk/forward_list.hpp>
#include <redshift/boot/multiboot2.hpp>
#include <redshift/hal/memory/common.hpp>
#include <redshift/hal/memory/heap.hpp>
#include <redshift/hal/memory/paging.hpp>
#include <redshift/hal/memory/static.hpp>
#include <redshift/kernel.hpp>

namespace redshift { namespace hal {
    /// Memory HAL.
    class memory {
    public:
        using alignment      = memory_detail::alignment;
        using page_directory = memory_detail::page_directory;
        using heap           = memory_detail::heap;

        /// Memory region type.
        enum class region_type {
            invalid     = MULTIBOOT2_MEMORY_INVALID,     ///< Invalid memory.
            available   = MULTIBOOT2_MEMORY_AVAILABLE,   ///< Available memory.
            reserved    = MULTIBOOT2_MEMORY_RESERVED,    ///< Unavailable memory.
            reclaimable = MULTIBOOT2_MEMORY_RECLAIMABLE, ///< Unavailable memory which can be reclaimed.
            nvs         = MULTIBOOT2_MEMORY_NVS          ///< Memory used by ACPI.
        };
        /// Memory region.
        struct region {
            region_type type;  ///< Memory region type.
            uint64_t    start; ///< Memory region start.
            uint64_t    end;   ///< Memory region end.

            uint64_t size() const
            {
                return end - start;
            }
        };
        /// Memory map.
        using memory_map = ::libk::forward_list<region>;

        /// Initialise from multiboot tag list.
        static void init(const multiboot2_tag* mb_tags);

        /// Get a const reference to the memory map.
        /// \return A const reference to the memory map.
        static const memory_map& get_memory_map();

        /// Get the size of memory in kiB (lower part).
        /// \return The size of memory in kiB (lower part).
        static size_t size_lower();

        /// Get the size of memory in kiB (upper part).
        /// \return The size of memory in kiB (upper part).
        static size_t size_upper();

        /// Get the total size of memory in kiB.
        /// \return The total size of memory in kiB.
        static size_t total_size();
    private:
        static memory_map m_map;
        static size_t     m_size_lower;
        static size_t     m_size_upper;

        static void get_memory_size(const multiboot2_tag* mb_tags);

        static void init_memory_map(const multiboot2_tag* mb_tags);

        static bool memory_map_ascending_order_predicate(region* a, region* b);

        static region_type multiboot2_memory_type_to_region_type(multiboot2_memory_type_t type);
    };
}} // redshift::hal

#endif // ! REDSHIFT_HAL_MEMORY_HPP

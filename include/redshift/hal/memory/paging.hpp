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
#ifndef REDSHIFT_HAL_MEMORY_PAGING_HPP
#define REDSHIFT_HAL_MEMORY_PAGING_HPP

#include <libk/array.hpp>
#include <libk/vector.hpp>
#include <redshift/hal/memory/common.hpp>
#include <redshift/kernel.hpp>

namespace redshift { namespace hal { namespace memory_detail {
    /// Initialise paging.
    void init_paging();

    /// Enable paging.
    void enable_paging();

    /// Disable paging.
    void disable_paging();

    /// Page.
    struct page {
        unsigned present  :  1;
        unsigned rw       :  1;
        unsigned user     :  1;
        unsigned accessed :  1;
        unsigned written  :  1;
        unsigned reserved :  7;
        unsigned frame    : 20;
    } __packed;

    /// Page flags.
    SCOPED_ENUM(page_flags,
        none      = 0,      ///< No flags.
        present   = 1 << 0, ///< Present.
        user_mode = 1 << 1, ///< User-mode page (default is privileged).
        writeable = 1 << 2  ///< Writeable page (default is read-only).
    );

    /// Page directory.
    class page_directory {
        friend void init_paging();
    public:
        /// Page directory type.
        enum class type {
            kernel, ///< Kernel page directory.
            user    ///< User page directory.
        };

        /// The kernel page directory. Only one may exist at a time.
        static page_directory* kernel; // paging.cpp
        /// The currently-active page directory.
        static page_directory* current; // paging.cpp

        static void init_frames();

        /// Create a page directory.
        static page_directory* create(type type_);

        /// Load the page directory.
        void load();

        /// Get a page from the directory.
        /// \param address The physical address of the page, e.g. page 1 is adress 0x1000.
        /// \param create Whether to create the page if it doesn't exist.
        /// \return The page if it exists or was created, otherwise nullptr.
        page* get_page(uintptr_t address, bool create);

        /// Map pages into the directory.
        /// \param first
        size_t map_pages(size_t first, size_t last, bool create, bool idmap);

        /// Allocate a page frame.
        /// \param page The page.
        /// \param flags The flags.
        void alloc_frame(page* page, page_flags_t flags);

        /// Free a page frame.
        /// \param page The page.
        void free_frame(page* page);
    protected:
        page_directory() = delete;
    private:
        static constexpr unsigned PAGE_ENTRIES = 1024U;
        static constexpr unsigned PAGE_TABLES  = 1024U;

        struct page_table {
            page pages[PAGE_ENTRIES];
        };

        static libk::vector<uint32_t> m_frames;

        libk::array<page_table*, PAGE_TABLES> m_tables;
        libk::array<uint32_t,    PAGE_TABLES> m_physical_tables[PAGE_TABLES];
        uint32_t                              m_physical_address;

        void set_frame(uint32_t   addr);
        void clear_frame(uint32_t addr);
        uint32_t get_first_free_frame();
    };
}}} // redshift::hal::memory_detail

#endif // ! REDSHIFT_HAL_MEMORY_PAGING_HPP

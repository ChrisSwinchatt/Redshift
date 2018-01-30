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
#ifndef REDSHIFT_MEM_HEAP_HPP
#define REDSHIFT_MEM_HEAP_HPP

#include <libk/sorted_array.hpp>
#include <redshift/kernel.hpp>
#include <redshift/mem/common.hpp>

namespace redshift { namespace mem {
    /// Heap.
    class heap {
    public:
        /// Heap flags.
        SCOPED_ENUM(flags,
            supervisor = 0,      ///< Make supervisor-mode heap.
            user_mode  = 1 << 0, ///< Make user-mode heap.
            writeable  = 1 << 1  ///< Make heap writeable.
        );

        /// Create a heap.
        /// \param start The start address of the heap.
        /// \param init_size Initial size.
        /// \param [max_size] Maximum size of the heap. Use init_size if 0.
        /// \param [min_size] Minimum size of the heap. Use init_size if 0.
        /// \return The newly created heap.
        static heap* create(
            uintptr_t start,
            size_t    init_size,
            size_t    max_size = 0,
            size_t    min_size = 0,
            flags_t   flags    = flags::supervisor
        );

        /// Allocate a block of memory on the heap.
        /// \param size The size of the block to allocate.
        /// \return The allocated block is returned on success; otherwise nullptr is returned.
        void* allocate(size_t size);

        /// Free an allocated block.
        /// \param ptr A pointer to the block to free.
        void free(void* ptr);

        /// Attempt to resize an allocated block of memory.
        /// \param ptr A pointer to the block to resize.
        /// \param new_size The new size of the block.
        /// \return A pointer to the resized block is returned on success. On failure, `nullptr` is returned.
        /// \note `ptr` is invalidated if the operation succeeds; the return value of resize should be used instead.
        /// However, if the block cannot be resized, it is still valid and retains its original size and contents.
        void* resize(void* ptr, size_t new_size);

        /// Get the current size.
        /// \return The current size.
        size_t size() const;

        /// Get the minimum size.
        /// \return The minimum size.
        size_t min_size() const;

        /// Get the maximum size.
        /// \return The maximum size.
        size_t max_size() const;

        /// Get the total number of allocations.
        /// \return The total number of allocations.
        intmax_t alloc_count() const;

        /// Get the total number of frees.
        /// \return The total number of frees.
        intmax_t free_count() const;

        /// Get the total number of bytes currently allocated.
        /// \return The total number of bytes currently allocated.
        uintmax_t bytes_allocated() const;

        /// Get the address of the heap's contents.
        /// \return The address of the heap's contents.
        uintptr_t address() const;
    protected:
        heap(uintptr_t start, size_t init_size, size_t max_size, size_t min_size, flags_t flags);
    private:
        SCOPED_ENUM(block_flags : uint32_t,
            available = 0,
            allocated = 1 << 0
        );
        struct blockfooter;
        struct blockheader {
            uint32_t magic;
            uint32_t flags;
            uint32_t size;   // Size of the usable area (i.e. exluding header & footer).
            uint8_t  pad[4]; // Pad to 16 bytes.

            // Returns true if block a is smaller than block b.
            static bool ascending_size_predicate(blockheader* a, blockheader* b);

            // Get the blockheader BEFORE a given address/pointer.
            static blockheader* get(void* ptr);
            static blockheader* get(uintptr_t address);

            // Place a blockheader AT at a given address/pointer.
            static blockheader* place(void* ptr, size_t size, block_flags_t flags);
            static blockheader* place(uintptr_t address, size_t size, block_flags_t flags);

            blockheader(size_t size_, block_flags_t flags_);

            // Address of usable memory region.
            uintptr_t usable_address() const;

            // Get footer.
            blockfooter* footer() const;

            // Size of block including header & footer.
            size_t total_size() const;
        };
        struct blockfooter {
            uint32_t     magic;
            blockheader* header;
            uint8_t      pad[8]; // Pad to 16 bytes.

            // Construct in place.
            static blockfooter* place(blockheader* header_);

            blockfooter(blockheader* header_);

            uintptr_t end_address() const;
        };

        using blocklist_t = sorted_array<blockheader>;

        static constexpr uint32_t BLOCK_MAGIC    = 0x600DB10CUL;
        static constexpr uint32_t BLOCK_INVALID  = 0xBAADB10CUL;
        static constexpr uint32_t MIN_BLOCK_SIZE = sizeof(blockheader) + sizeof(blockfooter);

        blocklist_t m_blocklist;
        size_t      m_size;            // Current size.
        size_t      m_min_size;
        size_t      m_max_size;
        intmax_t    m_alloc_count;     // Number of times allocated.
        intmax_t    m_free_count;      // Number of times freed.
        uintmax_t   m_bytes_allocated; // Number of bytes *currently* allocated.
        flags_t     m_flags;

        // Scan memory map for a region large enough to store the heap.
        uintptr_t get_heap_region(size_t size) const;

        // Find the smallest hole bigger than `size` bytes long.
        int32_t get_smallest_hole(size_t size, alignment align) const;

        // Get the index of the last block.
        uint32_t get_last_block() const;

        // Create a new block or update an existing one.
        blockheader* create_or_update_block(int32_t index, uintptr_t address, size_t size, block_flags_t flags);

        // Create a new hole.
        blockheader* create_hole(uintptr_t address, size_t size);

        // Create a hole and allocate inside it.
        void* create_hole_and_alloc(size_t alloc_size, alignment align);

        // Allocate inside an existing hole.
        blockheader* alloc_with_hole(int32_t hole, size_t alloc_size, alignment align);

        // Expand the heap.
        void expand(size_t new_size);

        // Contract the heap.
        void contract(size_t new_size);

        // Attempt to unify a hole with a hole on the left. Returns 1 if successful, otherwise 0.
        int unify_left(blockheader** pheader, blockfooter* footer);

        // Attempt to unify a hole with a hole on the right. Returns 2 if successful, otherwise 0.
        int unify_right(blockheader* header, blockfooter** pfooter);

        // Attempt to unify a hole to the left and right. Returns 1 if unified left, 2 if unified right, 3 if both, or 0
        // if neither.
        int unify_holes(blockheader** pheader, blockfooter** pfooter);
    };
}} // redshift::mem

#endif // ! REDSHIFT_MEM_HEAP_HPP

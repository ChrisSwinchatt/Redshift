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
#ifndef REDSHIFT_LIBK_MEMORY_HPP
#define REDSHIFT_LIBK_MEMORY_HPP

#include <libk/assert.hpp>
#include <libk/types.hpp>

namespace libk { namespace memory {
    /// Copy memory from one region to another. The regions are allowed to overlap.
    /// \param dst Destination buffer.
    /// \param src Source buffer.
    /// \param n Number of bytes to copy.
    void copy(void* dst, const void* src, size_t n);

    /// Fill a memory region with zeros.
    /// \param ptr A pointer to the memory region.
    /// \param n The size of the region in bytes.
    void zero(void* ptr, size_t n);

    /// Fill a memory region with an 8-bit value.
    /// \param ptr A pointer to the memory region.
    /// \param value The value to fill.
    /// \param n The number of values to write.
    void fill8(void* ptr, uint8_t value, size_t n);

    /// Fill a memory region with a 16-bit value.
    /// \param ptr A pointer to the memory region.
    /// \param value The value to fill.
    /// \param n The number of values to write.
    void fill16(void* ptr, uint16_t value, size_t n);

    /// Fill a memory region with a 32-bit value.
    /// \param ptr A pointer to the memory region.
    /// \param value The value to fill.
    /// \param n The number of values to write.
    void fill32(void* ptr, uint32_t value, size_t n);

    /// Fill a memory region with a 64-bit value.
    /// \param ptr A pointer to the memory region.
    /// \param value The value to fill.
    /// \param n The number of values to write.
    void fill64(void* ptr, uint64_t value, size_t n);

    /// Compare two memory regions byte-wise.
    /// \param p1 The first memory region.
    /// \param p2 The second memory region.
    /// \param n The number of bytes to compare.
    /// \return 0 if the two regions are identical; <0 if the first pair of unequal values is greater in p2; >0 if the first
    /// pair of unequal values is greater in p1.
    int compare(const void* p1, const void* p2, size_t n);

    template <typename T>
    void fill(T* ptr, const T& value, size_t n)
    {
        switch (sizeof(T)) {
            case sizeof(uint8_t):  fill8(reinterpret_cast<uint8_t*>(ptr),   reinterpret_cast<uint8_t&>(value), n);
            case sizeof(uint16_t): fill16(reinterpret_cast<uint16_t*>(ptr), reinterpret_cast<uint16_t&>(value), n);
            case sizeof(uint32_t): fill32(reinterpret_cast<uint32_t*>(ptr), reinterpret_cast<uint32_t&>(value), n);
            case sizeof(uint64_t): fill64(reinterpret_cast<uint64_t*>(ptr), reinterpret_cast<uint64_t&>(value), n);
            default:
                UNREACHABLE("can't fill memory unless type size matches uint8/16/32/64_t (got %zu)", sizeof(T));
        }
    }
}} // libk::memory

#endif // ! REDSHIFT_LIBK_KMEMORY_HPP

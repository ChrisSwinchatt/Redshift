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
#include <libk/memory.hpp>

#include "memory_copy.hpp"
#include "memory_fill.hpp"

namespace libk { namespace memory {

    void copy(void* dst, const void* src, size_t n)
    {
        if (dst < src) {
            kcopy(static_cast<char*>(dst), static_cast<const char*>(src), n, COPY_BACKWARD);
        } else if (dst > src) {
            kcopy(static_cast<char*>(dst), static_cast<const char*>(src), n, COPY_FORWARD);
        }
    }

    void zero(void* ptr, size_t n)
    {
        kfill8(static_cast<uint8_t*>(ptr), 0, n);
    }

    void fill8(void* ptr, uint8_t value, size_t n)
    {
        kfill8(static_cast<uint8_t*>(ptr), value, n);
    }

    void fill16(void* ptr, uint16_t value, size_t n)
    {
        kfill16(static_cast<uint16_t*>(ptr), value, n);
    }

    void fill32(void* ptr, uint32_t value, size_t n)
    {
        kfill32(static_cast<uint32_t*>(ptr), value, n);
    }

    void fill64(void* ptr, uint64_t value, size_t n)
    {
        kfill64(static_cast<uint64_t*>(ptr), value, n);
    }

    int compare(const void* p1, const void* p2, size_t n)
    {
        if (p1 != p2) {
            const uint8_t* a1 = static_cast<const uint8_t*>(p1);
            const uint8_t* a2 = static_cast<const uint8_t*>(p2);
            for (; n > 0; ++a1, ++a2, --n) {
                int cmp = *(int*)a1 - *(int*)a2;
                if (cmp) {
                    return cmp;
                }
            }
        }
        return 0;
    }
}} // libk::memory

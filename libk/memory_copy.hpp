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
#ifndef REDSHIFT_LIBK_MEMORY_COPY_HPP
#define REDSHIFT_LIBK_MEMORY_COPY_HPP

#include <libk/types.hpp>

enum {
    COPY_FORWARD  = +1,
    COPY_BACKWARD = -1
};

#define KCOPY_TYPE_LIST(F)  \
    F(64)             \
    F(32)             \
    F(16)             \
    F(8)

#define DEFINE_KCOPY_FUNCTION(X)                                                                        \
    static inline void kcopy##X(uint##X##_t** pdst, const uint##X##_t** psrc, size_t n, int direction)  \
    {                                                                                                   \
        uint##X##_t*       dst = *pdst;                                                                 \
        const uint##X##_t* src = *psrc;                                                                 \
        for (; n > 0; --n, src += direction, dst += direction) {                                        \
            *dst = *src;                                                                                \
        }                                                                                               \
        *pdst = dst;                                                                                    \
        *psrc = src;                                                                                    \
    }

KCOPY_TYPE_LIST(DEFINE_KCOPY_FUNCTION)

#undef DEFINE_KCOPY_FUNCTION
#undef KCOPY_TYPE_LIST

static inline void kcopy(char* dst, const char* src, size_t n, int direction)
{
    // Byte copy to align at 4 bytes.
    for (uintptr_t address = (uintptr_t)dst;
        (address & 3) != 0 && n > 0;
        address += direction, dst += direction, src += direction, --n)
    {
        *dst = *src;
    }
    // Try to copy 8, 4 and 2 bytes at a time.
    if (n >= 8) {
        n >>= 3;
        kcopy64((uint64_t**)&dst, (const uint64_t**)&src, n, direction);
        n &= 7;
    }
    if (n >= 4) {
        n >>= 2;
        kcopy32((uint32_t**)&dst, (const uint32_t**)&src, n, direction);
        n &= 3;
    }
    if (n >= 2) {
        n >>= 1;
        kcopy16((uint16_t**)&dst, (const uint16_t**)&src, n, direction);
        n &= 1;
    }
    // Byte-copy until the end of the array.
    kcopy8((uint8_t**)&dst, (const uint8_t**)&src, n, direction);
}

#endif // ! REDSHIFT_LIBK_MEMORY_COPY_HPP

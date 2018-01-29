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
#ifndef REDSHIFT_LIBK_MEMORY_FILL_H
#define REDSHIFT_LIBK_MEMORY_FILL_H

#include <libk/macro.hpp>
#include <libk/types.hpp>

#define KFILL_TYPE_LIST(F)  \
    F(64)                   \
    F(32)                   \
    F(16)

#define DEFINE_KFILL_FUNCTION(X)                                                        \
    static inline uint##X##_t* kfill##X(uint##X##_t* ptr, uint##X##_t value, size_t n)  \
    {                                                                                   \
        for (; n > 0; --n, ++ptr) {                                                     \
            *ptr = value;                                                               \
        }                                                                               \
        return ptr;                                                                     \
    }

KFILL_TYPE_LIST(DEFINE_KFILL_FUNCTION)

#undef DEFINE_KFILL_FUNCTION
#undef KFILL_TYPE_LIST

static inline void kfill8(uint8_t* ptr, uint8_t value, size_t n)
{
    // Values for kfillX.
    const uint16_t value16 = ((uint16_t)value   << 8)  | ((uint16_t)value   & 0xFF);
    const uint32_t value32 = ((uint32_t)value16 << 16) | ((uint32_t)value16 & 0xFFFF);
    const uint32_t value64 = ((uint64_t)value32 << 32) | ((uint64_t)value32 & 0xFFFFFFFF);
    // Align to 4 bytes boundary.
    for (uintptr_t address = (uintptr_t)ptr; (address & 3) != 0 && n > 0; ++address, ++ptr, --n) {
       *ptr = value;
    }
    // Try to write 8, 4 and 2 bytes at a time.
    if (n >= 8) {
        n >>= 3;
        ptr = reinterpret_cast<uint8_t*>(kfill64(reinterpret_cast<uint64_t*>(ptr), value64, n));
        n &= 7;
    }
    if (n >= 4) {
        n >>= 2;
        ptr = reinterpret_cast<uint8_t*>(kfill32(reinterpret_cast<uint32_t*>(ptr), value32, n));
        n &= 3;
    }
    if (n >= 2) {
        n >>= 1;
        ptr = reinterpret_cast<uint8_t*>(kfill16(reinterpret_cast<uint16_t*>(ptr), value16, n));
        n &= 1;
    }
    // Write any remaining bytes.
    for (; n > 0; --n, ++ptr) {
        *ptr = value;
    }
}

#endif // ! REDSHIFT_LIBK_MEMORY_FILL_H

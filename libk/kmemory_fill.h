/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef REDSHIFT_LIBK_KFILL_H
#define REDSHIFT_LIBK_KFILL_H

#include <libk/ktypes.h>
#include <libk/kmacro.h>

static inline void __always_inline kfill8(uint8_t* p, uint8_t value, size_t n)
{
    for (; ((p & 3) != 0) || ((n & 1) != 0); ++p, --n) {
        *p = value;
    }
    const uint16_t value16 = ((uint16_t)value << 16) | (uint16_t)value;
    kfill16((uint16_t*)p, value16, n >> 1);
}

static inline void __always_inline kfill16(uint16_t* p, uint16_t value, size_t n)
{
    for (; ((p & 3) != 0) || ((n & 1) != 0); ++p, --n) {
        *p = value;
    }
    const uint32_t value32 = ((uint32_t)value << 32) | (uint32_t)value;
    kfill32((uint32_t*)p, value32, n >> 1);
}

static inline void __always_inline kfill32(uint32_t* p, uint32_t value, size_t n)
{
    for (; ((p & 3) != 0) || ((n & 1) != 0); ++p, --n) {
        *p = value;
    }
    const uint64_t value64 = ((uint64_t)value << 64) | (uint64_t)value;
    kfill64((uint64_t*)p, value64, n >> 1);
}

static inline void __always_inline kfill64(uint64_t* p, uint64_t value, size_t n)
{
    for (; n > 0; --n) {
        *p = value;
    }
}

#endif /* ! REDSHIFT_LIBK_KFILL_H */

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
#ifndef REDSHIFT_LIBK_KCOPY_H
#define REDSHIFT_LIBK_KCOPY_H

#include <stdint.h>

enum {
    COPY_FORWARD  = +1,
    COPY_BACKWARD = -1
};

#define COPY(DST, SRC, N, OP) *DST OP = * SRC OP; N--;

static inline void internal_copy_forwards(char* dst, const char* src, size_t n)
{
    /* Byte copy to align at 8 bytes. */
    while (n-- && (((uintptr_t)dst) & ~(sizeof(uint64_t) - 1)) && (((uintptr_t)src) & ~(sizeof(uint64_t) - 1)))
    {
        COPY(dst, src, n, ++);
    }

    /* Copy 32 bytes at a time as long as possible */
    uint64_t* dst64 = (uint64_t*)dst;
    uint64_t* src64 = (uint64_t*)src;
    while (n > 4)
    {
        COPY(dst64, src64, n, ++);
        COPY(dst64, src64, n, ++);
        COPY(dst64, src64, n, ++);
        COPY(dst64, src64, n, ++);
    }

    /* Copy 8 bytes at a time as long as possible. */
    while (n > 0)
    {
        COPY(dst64, src64, n, ++);
    }

    /* Byte copy remaining bytes. */
    dst = (char*)dst64;
    src = (const char*)src64;
    while (n > 0)
    {
        COPY(dst, src, n, ++);
    }
}

static inline void internal_copy_backwards(char* dst, const char* src, size_t n)
{
    dst += n;
    src += n;
    /* Byte copy to align at 8 bytes. */
    while (n-- && (((uintptr_t)dst) & ~(sizeof(uint64_t) - 1)) && (((uintptr_t)src) & ~(sizeof(uint64_t) - 1)))
    {
        COPY(dst, src, n, --);
    }

    /* Copy 32 bytes at a time as long as possible */
    uint64_t* dst64 = (uint64_t*)dst;
    uint64_t* src64 = (uint64_t*)src;
    while (n > 4)
    {
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
    }

    /* Copy 8 bytes at a time as long as possible. */
    while (n > 0)
    {
        COPY(dst64, src64, n, --);
    }

    /* Byte copy remaining bytes. */
    dst = (char*)dst64;
    src = (const char*)src64;
    while (n > 0)
    {
        COPY(dst, src, n, --);
    }
}

#endif /* ! REDSHIFT_LIBK_KCOPY_H */

/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#include <libk/kmemory.h>

#define COPY(DST, SRC, N, OP) *(DST)OP = *(SRC)OP; (N)--;

#define UNALIGNED(X, SIZE) (((uintptr_t)(X)) & ~((SIZE) - 1))

static inline void internal_copy_forwards(char* dst, const char* src, size_t n)
{
    /* Byte copy to align at 8 bytes. */
    while (n > 0 && UNALIGNED(src, sizeof(uint64_t)) && UNALIGNED(dst, sizeof(uint64_t)))
    {
        COPY(dst, src, n, ++);
    }

    /* Copy 32 bytes at a time as long as possible */
    uint64_t* dst64 = (uint64_t*)dst;
    uint64_t* src64 = (uint64_t*)src;
    while (n > 4*sizeof(uint64_t))
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
    while (n > 0 && UNALIGNED(src, sizeof(uint64_t)) && UNALIGNED(dst, sizeof(uint64_t)))
    {
        COPY(dst, src, n, --);
    }

    uint64_t* dst64 = (uint64_t*)dst;
    uint64_t* src64 = (uint64_t*)src;
    while (n > 4*sizeof(uint64_t))
    {
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
        COPY(dst64, src64, n, --);
    }

    while (n > 0)
    {
        COPY(dst64, src64, n, --);
    }

    dst = (char*)dst64;
    src = (const char*)src64;
    while (n > 0)
    {
        COPY(dst, src, n, --);
    }
}

void kmemory_copy(void* dst, const void* src, size_t n)
{
    if (dst == src) {
        return;
    }
    
    if (dst < src || dst >= src + n) {
        internal_copy_forwards(dst, src, n);
        return;
    }

    internal_copy_backwards(dst + n, src + n, n);
}

#define KFILL_TYPE_LIST(F)  \
    F(64)                   \
    F(32)                   \
    F(16)

#define DEFINE_KFILL_FUNCTION(X)                                                    \
    static inline void kfill##X(uint##X##_t** pptr, uint##X##_t value, size_t n)    \
    {                                                                               \
        uint##X##_t* ptr = *pptr;                                                   \
        for (; n > 0; --n, ++ptr) {                                                 \
            *ptr = value;                                                           \
        }                                                                           \
        *pptr = ptr;                                                                \
    }

KFILL_TYPE_LIST(DEFINE_KFILL_FUNCTION)

#undef DEFINE_KFILL_FUNCTION
#undef KFILL_TYPE_LIST

static inline void kfill8(uint8_t* ptr, uint8_t value, size_t n)
{
    /* Values for kfillX.
     */
    const uint16_t value16 = ((uint16_t)value   << 8)  | ((uint16_t)value   & 0xFF);
    const uint32_t value32 = ((uint32_t)value16 << 16) | ((uint32_t)value16 & 0xFFFF);
    const uint32_t value64 = ((uint64_t)value32 << 32) | ((uint64_t)value32 & 0xFFFFFFFF);
    /* Align to 4 bytes boundary.
     */
    for (uintptr_t address = (uintptr_t)ptr; (address & 3) != 0 && n > 0; ++address, ++ptr, --n) {
       *ptr = value;
    }
    /* Try to write 8, 4 and 2 bytes at a time.
     */
    if (n >= 8) {
        n >>= 3;
        kfill64((uint64_t**)&ptr, value64, n);
        n &= 7;
    }
    if (n >= 4) {
        n >>= 2;
        kfill32((uint32_t**)&ptr, value32, n);
        n &= 3;
    }
    if (n >= 2) {
        n >>= 1;
        kfill16((uint16_t**)&ptr, value16, n);
        n &= 1;
    }
    /* Write any remaining bytes.
     */
    for (; n > 0; --n, ++ptr) {
        *ptr = value;
    }
}

void kmemory_zero(void* ptr, size_t n)
{
    kfill8((uint8_t*)ptr, 0, n);
}

void kmemory_fill8(void* ptr, uint8_t value, size_t n)
{
    kfill8((uint8_t*)ptr, value, n);
}

void kmemory_fill16(void* ptr, uint16_t value, size_t n)
{
    kfill16((uint16_t**)&ptr, value, n);
}

void kmemory_fill32(void* ptr, uint32_t value, size_t n)
{
    kfill32((uint32_t**)&ptr, value, n);
}

void kmemory_fill64(void* ptr, uint64_t value, size_t n)
{
    kfill64((uint64_t**)&ptr, value, n);
}

int kmemory_compare(const void* p1, const void* p2, size_t n)
{
    if (p1 != p2) {
        const uint8_t* a1 = p1;
        const uint8_t* a2 = p2;
        for (; n > 0; ++a1, ++a2, --n) {
            int cmp = *(int*)a1 - *(int*)a2;
            if (cmp) {
                return cmp;
            }
        }
    }
    return 0;
}

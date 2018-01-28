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
#include <libk/kmemory.h>

#include "kmemory_copy.h"
#include "kmemory_fill.h"

void kmemory_copy(void* dst, const void* src, size_t n)
{
    if (dst < src) {
        KCOPY_BACKWARD(dst, src, n);
    } else if (dst > src) {
        KCOPY_FORWARD(dst, src, n);
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
    kfill16((uint16_t*)ptr, value, n);
}

void kmemory_fill32(void* ptr, uint32_t value, size_t n)
{
    kfill32((uint32_t*)ptr, value, n);
}

void kmemory_fill64(void* ptr, uint64_t value, size_t n)
{
    kfill64((uint64_t*)ptr, value, n);
}

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
#ifndef REDSHIFT_LIBK_KMEMORY_H
#define REDSHIFT_LIBK_KMEMORY_H

#include <libk/ktypes.h>

/**
 * Copy memory from one region to another. The regions are allowed to overlap.
 * \param dst Destination buffer.
 * \param src Source buffer.
 * \param n Number of bytes to copy.
 */
void kmemory_copy(void* dst, const void* src, size_t n);

/**
 * Fill a memory region with zeros.
 * \param ptr A pointer to the memory region.
 * \param n The size of the region in bytes.
 */
void kmemory_zero(void* ptr, size_t n);

/**
 * Fill a memory region with an 8-bit value.
 * \param ptr A pointer to the memory region.
 * \param value The value to fill.
 * \param n The number of values to write.
 */
void kmemory_fill8(void* ptr, uint8_t value, size_t n);

/**
 * Fill a memory region with a 16-bit value.
 * \param ptr A pointer to the memory region.
 * \param value The value to fill.
 * \param n The number of values to write.
 */
void kmemory_fill16(void* ptr, uint16_t value, size_t n);

/**
 * Fill a memory region with a 32-bit value.
 * \param ptr A pointer to the memory region.
 * \param value The value to fill.
 * \param n The number of values to write.
 */
void kmemory_fill32(void* ptr, uint32_t value, size_t n);

/**
 * Fill a memory region with a 64-bit value.
 * \param ptr A pointer to the memory region.
 * \param value The value to fill.
 * \param n The number of values to write.
 */
void kmemory_fill64(void* ptr, uint64_t value, size_t n);

/**
 * Compare two memory regions byte-wise.
 * \param p1 The first memory region.
 * \param p2 The second memory region.
 * \param n The number of bytes to compare.
 * \return
 */
int kmemory_compare(const void* p1, const void* p2, size_t n);

#endif /* ! REDSHIFT_LIBK_KMEMORY_H */

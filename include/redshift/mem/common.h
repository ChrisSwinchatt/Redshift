/*
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef REDSHIFT_MEM_COMMON_H
#define REDSHIFT_MEM_COMMON_H

#include <redshift/util/macro.h>

/** Generate a 32-bit mask for alignment to ALIGNMENT bytes. */
#define ALIGN_MASK(ALIGNMENT)       (~(ALIGNMENT - 1))

/** Test whether X is aligned to ALIGNMENT bytes. */
#define IS_ALIGNED(X, ALIGNMENT)    (((uintptr_t)(X) & ALIGN_MASK(ALIGNMENT)) == 0)

/** Get the address of X when aligned to ALIGNMENT bytes. Returns uintptr_t. */
#define ALIGNED_ADDR(X, ALIGNMENT)  (((uintptr_t)(X) & ALIGN_MASK(ALIGNMENT)) + ALIGNMENT)

/** Align X to ALIGNMENT byte boundary. */
#define MAKE_ALIGNED(X, ALIGNMENT)         \
    do {                                   \
        if (!(IS_ALIGNED(X, ALIGNMENT))) { \
            X = ALIGNED_ADDR(X, ALIGNMENT);\
        }                                  \
    } while (0)

/** Test whether X is page-aligned. */
#define IS_PAGE_ALIGNED(X)          (IS_ALIGNED((X), PAGE_SIZE))

/** Get the address of X when aligned to page size. Returns uintptr_t. */
#define PAGE_ALIGNED_ADDR(X)        (ALIGNED_ADDR(X, PAGE_SIZE))

/** Align X to page boundary. */
#define MAKE_PAGE_ALIGNED(X)        MAKE_ALIGNED(X, PAGE_SIZE)

/** Test whether X is aligned according to its size. */
#define IS_SIZE_ALIGNED(X)          (IS_ALIGNED((X), sizeof(X)))

/** Get the address of X when aligned to its own size. Returns uintptr_t. */
#define SIZE_ALIGNED_ADDR(X)        (ALIGNED_ADDR((X), sizeof(X)))

/** Align X according to its size. */
#define MAKE_SIZE_ALIGNED(X)        (MAKE_ALIGNED((X), sizeof(X)))

/**
 * Align an address or pointer during allocation. Allocators have to return pointers which are sufficiently aligned for
 * whatever data they will contain. Per Sys V ABI, data types have to be aligned according to their size, i.e. 4 byte
 * alignment for 8 or 4 byte objects, and 2 bytes for 2 byte objects. Structures are aligned to their maximum alignment
 * alignment (i.e. a struct with one 4-byte member and one two-byte member should be aligned to 4 bytes). For gcc >= 4.5
 * the stack also has to be aligned to 16 bytes.
 *
 * To handle all these requirements without wasting (much) memory, we align 2 or 3 byte allocations to 2 byte boundaries
 * 4-16 byte allocations to 4 byte boundaries, and, on the assumption that a stack will always be at least 16 bytes long
 * anything bigger gets aligned to a 16 byte boundary.
 */
#define MAKE_ALLOC_ALIGNED(X, SIZE)\
    do {                           \
        if (size >= 16) {          \
            MAKE_ALIGNED(X, 16);   \
        } else if (size >= 4) {    \
            MAKE_ALIGNED(X, 4);    \
        } else if (size >= 2) {    \
            MAKE_ALIGNED(X, 2);    \
        }                          \
    } while (0)

/** Page size (4 kiB). */
INTEGER_CONSTANT(PAGE_SIZE, 0x1000UL);

/** Allocator flags. */
typedef enum {
    ALLOC_SIZE_ALIGN = 0,      /**< Align according to allocation size. */
    ALLOC_PAGE_ALIGN = 1 << 0  /**< Align to page boundary.             */
} alloc_flags_t;

#endif /* ! REDSHIFT_MEM_COMMON_H */

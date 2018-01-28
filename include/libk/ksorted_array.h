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
#ifndef REDSHIFT_LIBK_Ksorted_array_H
#define REDSHIFT_LIBK_Ksorted_array_H

#include <libk/ktypes.h>

/** Declare predefined order predicates. */
#define KSORTED_ARRAY_PREDICATE_TYPE_LIST(F)    \
    F(char)                                     \
    F(int8_t)                                   \
    F(int16_t)                                  \
    F(int32_t)                                  \
    F(int64_t)                                  \
    F(intmax_t)                                 \
    F(ssize_t)                                  \
    F(ptrdiff_t)                                \
    F(void_ptr_t)                               \
    F(uint8_t)                                  \
    F(uint16_t)                                 \
    F(uint32_t)                                 \
    F(uint64_t)                                 \
    F(uintmax_t)                                \
    F(size_t)

/** ksorted_array forward declaration. */
struct ksorted_array;

/**  */
typedef enum {
    KSORTED_ARRAY_STATIC  = 0,
    KSORTED_ARRAY_DYNAMIC = 1 << 0
} ksorted_array_flags_t;

/** Order predicate function. Returns true if its first argument logically comes before its second argument. */
typedef bool(* korder_predicate_fn)(void*, void*);

/** Declare ascending and descending order predicates for type T. */
#define DECLARE_ORDER_PREDICATES(T)                         \
    bool T##_ascending_order_predicate(void* pa, void* pb); \
    bool T##_descending_order_predicate(void* pa, void* pb);

KSORTED_ARRAY_PREDICATE_TYPE_LIST(DECLARE_ORDER_PREDICATES)

#undef DECLARE_ORDER_PREDICATES

struct ksorted_array* ksorted_array_create(size_t capacity, ksorted_array_flags_t flags, korder_predicate_fn predicate);

struct ksorted_array* ksorted_array_place(void* address, size_t capacity, ksorted_array_flags_t flags, korder_predicate_fn predicate);

void ksorted_array_delete(struct ksorted_array* list);

size_t ksorted_array_add(struct ksorted_array* list, void* element);

void* ksorted_array_get(const struct ksorted_array* list, size_t index);

void ksorted_array_remove(struct ksorted_array* list, size_t index);

size_t ksorted_array_count(const struct ksorted_array* list);

#endif /* ! REDSHIFT_LIBK_Ksorted_array_H */

/**
 * \file kernel/ksorted_array.c
 * Automatically-sorted array.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#include <libk/kassert.h>
#include <libk/kextern.h>
#include <libk/kmacro.h>
#include <libk/kmemory.h>
#include <libk/ksorted_array.h>
#include <libk/kstring.h>

struct ksorted_array {
    void**              elements;
    uint8_t             freeable;
    size_t              count;
    size_t              capacity;
    korder_predicate_fn predicate;
};

/** Define ascending and descending order predicates for type T. */
#define DEFINE_ORDER_PREDICATES(T)                          \
    bool T##_ascending_order_predicate(void* pa, void* pb)  \
    {                                                       \
        DEBUG_ASSERT(pa != NULL);                           \
        DEBUG_ASSERT(pb != NULL);                           \
        T a = *(T*)pa;                                      \
        T b = *(T*)pb;                                      \
        return a < b;                                       \
    }                                                       \
    bool T##_descending_order_predicate(void* pa, void* pb) \
    {                                                       \
        DEBUG_ASSERT(pa != NULL);                           \
        DEBUG_ASSERT(pb != NULL);                           \
        T a = *(T*)pa;                                      \
        T b = *(T*)pb;                                      \
        return a > b;                                       \
    }

KSORTED_ARRAY_PREDICATE_TYPE_array(DEFINE_ORDER_PREDICATES)

#undef DEFINE_ORDER_PREDICATES

struct ksorted_array* ksorted_array_create(size_t capacity, ksorted_array_flags_t flags, korder_predicate_fn predicate)
{
    const size_t size = sizeof(struct ksorted_array);
    void* address;
    if (TEST_FLAG(flags, KSORTED_ARRAY_DYNAMIC)) {
        address = kextern_dynamic_allocate(size);
    } else {
        address = kextern_static_allocate(size);
    }
    DEBUG_ASSERT(address != NULL);
    return ksorted_array_place(address, capacity, flags, predicate);
}

struct ksorted_array* ksorted_array_place(void* address, size_t capacity, ksorted_array_flags_t flags, korder_predicate_fn predicate)
{
    DEBUG_ASSERT(address != NULL);
    DEBUG_ASSERT(capacity > 0);
    DEBUG_ASSERT(predicate != NULL);
    struct ksorted_array* array = address;
    size_t count = sizeof(*(array->elements))*capacity;
    if (TEST_FLAG(flags, KSORTED_ARRAY_DYNAMIC)) {
        array->elements = kextern_dynamic_allocate(count);
        array->freeable = 1;
    } else {
        array->elements = kextern_static_allocate(count);
        array->freeable = 0;
    }
    DEBUG_ASSERT(array->elements != NULL);
    if (sizeof(*(array->elements)) == 4) {
        kmemory_fill32(array->elements, 0UL, capacity);
    } else if (sizeof(*(array->elements)) == 8) {
        kmemory_fill64(array->elements, 0UL, capacity);
    } else {
        UNREACHABLE("sizeof(void*) = %zu, should be 4 or 8", sizeof(*(array->elements)));
    }
    array->count     = 0;
    array->capacity  = capacity;
    array->predicate = predicate;
    return array;
}

void ksorted_array_delete(struct ksorted_array* array)
{
    if (array && array->freeable) {
        kextern_dynamic_free(array->elements);
        kextern_dynamic_free(array);
    }
}

size_t ksorted_array_add(struct ksorted_array* array, void* element)
{
    DEBUG_ASSERT(array != NULL);
    DEBUG_ASSERT(array->elements != NULL);
    DEBUG_ASSERT(array->predicate != NULL);
    DEBUG_ASSERT(element != NULL);
    if (array->count == 0) {
        array->elements[0] = element;
        array->count++;
        return array->count;
    }
    size_t i = 0;
    for (; i < array->count && array->predicate(array->elements[i], element); ++i) {
        DO_NOTHING();
    }
    if (i >= array->count) {
        /* Append.
         */
        array->elements[array->count++] = element;
    } else {
        /* Move each pointer from the ith element to the end of the array one space to the "right". Then insert the new
         * element at position i.
         */
        kmemory_copy(array->elements + i + 1, array->elements + i, array->count - i - 1);
        array->elements[i] = element;
        array->count++;
    }
    /* Ensure we haven't run past the end of the array. TODO return failure when this happens instead of just crashing.
     */
    RUNTIME_CHECK(array->count <= array->capacity);
    return i;
}

void* ksorted_array_get(const struct ksorted_array* array, size_t index)
{
    DEBUG_ASSERT(index < array->count);
    return array->elements[index];
}

void ksorted_array_remove(struct ksorted_array* array, size_t index)
{
    DEBUG_ASSERT(index < array->count);
    for (; index < array->count; ++index) {
        array->elements[index] = array->elements[index + 1];
    }
    array->count--;
}

size_t ksorted_array_count(const struct ksorted_array* array)
{
    return array->count;
}

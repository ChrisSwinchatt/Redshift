/**
 * \file kernel/ksorted_array.c
 * Automatically-sorted list.
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

KSORTED_ARRAY_PREDICATE_TYPE_LIST(DEFINE_ORDER_PREDICATES)

#undef DEFINE_ORDER_PREDICATES

struct ksorted_array* ksorted_array_create(size_t capacity, ksorted_array_flags_t flags, korder_predicate_fn predicate)
{
    const size_t size = sizeof(struct ksorted_array*);
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
    struct ksorted_array* list = address;
    size_t count = sizeof(*(list->elements))*capacity;
    if (TEST_FLAG(flags, KSORTED_ARRAY_DYNAMIC)) {
        list->elements = kextern_dynamic_allocate(count);
        list->freeable = 1;
    } else {
        list->elements = kextern_static_allocate(count);
        list->freeable = 0;
    }
    DEBUG_ASSERT(list->elements != NULL);
    if (sizeof(*(list->elements)) == 4) {
        kmemory_fill32(list->elements, 0UL, capacity);
    } else if (sizeof(*(list->elements)) == 2) {
        kmemory_fill64(list->elements, 0UL, capacity);
    } else {
        UNREACHABLE("sizeof(void*) = %zu, should be 4 or 8", sizeof(*(list->elements)));
    }
    list->count     = 0;
    list->capacity  = capacity;
    list->predicate = predicate;
    return list;
}

void ksorted_array_delete(struct ksorted_array* list)
{
    if (list && list->freeable) {
        kextern_dynamic_free(list->elements);
        kextern_dynamic_free(list);
    }
}

size_t ksorted_array_add(struct ksorted_array* list, void* element)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->elements != NULL);
    DEBUG_ASSERT(list->predicate != NULL);
    DEBUG_ASSERT(element != NULL);
    if (list->count == 0) {
        list->elements[0] = element;
        list->count++;
        return list->count;
    }
    size_t i = 0;
    for (; i < list->count && list->predicate(list->elements[i], element); ++i) {
        DO_NOTHING();
    }
    if (i >= list->count) {
        /* Append.
         */
        list->elements[list->count++] = element;
    } else {
        /* Move each pointer from the ith element to the end of the list one space to the "right". Then insert the new
         * element at position i.
         */
        kmemory_copy(list->elements + i + 1, list->elements + i, list->count - i - 1);
        list->elements[i] = element;
        list->count++;
    }
    /* Ensure we haven't run past the end of the array. TODO return failure when this happens instead of just crashing.
     */
    RUNTIME_CHECK(list->count <= list->capacity);
    return i;
}

void* ksorted_array_get(const struct ksorted_array* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    return list->elements[index];
}

void ksorted_array_remove(struct ksorted_array* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    for (; index < list->count; ++index) {
        list->elements[index] = list->elements[index + 1];
    }
    --list->count;
}

size_t ksorted_array_count(const struct ksorted_array* list)
{
    return list->count;
}

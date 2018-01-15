/**
 * \file kernel/sorted_array.c
 * Automatically-sorted array.
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
#include <redshift/util/sorted_array.h>
#include <redshift/kernel.h>
#include <redshift/mem/static.h>
#include <redshift/mem/heap.h>
#include <string.h>

struct sorted_array {
    void**               elements;
    sorted_array_flags_t flags;
    size_t               count;
    size_t               capacity;
    order_predicate      predicate;
};

bool uint32_t_ascending_order_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    uint32_t a = *(uint32_t*)pa;
    uint32_t b = *(uint32_t*)pb;
    return a < b;
}

bool uint32_t_descending_order_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    uint32_t a = *(uint32_t*)pa;
    uint32_t b = *(uint32_t*)pb;
    return a > b;
}

struct sorted_array* create_sorted_array(size_t capacity, sorted_array_flags_t flags, order_predicate predicate)
{
    const size_t size = sizeof(struct sorted_array*);
    void* address;
    if (TEST_FLAG(flags, SORTED_ARRAY_DYNAMIC)) {
        address = kmalloc(size);
    } else {
        address = static_alloc(size);
    }
    DEBUG_ASSERT(address != NULL);
    return place_sorted_array(address, capacity, flags, predicate);
}

struct sorted_array* place_sorted_array(void* address, size_t capacity, sorted_array_flags_t flags, order_predicate predicate)
{
    DEBUG_ASSERT(address != NULL);
    DEBUG_ASSERT(capacity > 0);
    DEBUG_ASSERT(predicate != NULL);
    DEBUG_ASSERT((uintptr_t)predicate >= (uintptr_t)__code_start__ && (uintptr_t)predicate <= (uintptr_t)__code_end__);
    struct sorted_array* array = address;
    memset(array, 0, sizeof(array));
    const size_t bytes = sizeof(*(array->elements))*capacity;
    if (TEST_FLAG(flags, SORTED_ARRAY_DYNAMIC)) {
        array->elements = kmalloc(bytes);
    } else {
        array->elements = static_alloc(bytes);
    }
    DEBUG_ASSERT(array->elements != NULL);
    memset(array->elements, 0, bytes);
    array->count     = 0;
    array->capacity  = capacity;
    array->predicate = predicate;
    array->flags     = flags;
    return array;
}

void delete_sorted_array(struct sorted_array* array)
{
    if (array && TEST_FLAG(array->flags, SORTED_ARRAY_DYNAMIC)) {
        kfree(array->elements);
        array->elements = NULL;
        kfree(array);
    }
}

intmax_t sorted_array_add(struct sorted_array* array, void* element)
{
    DEBUG_ASSERT(array != NULL);
    DEBUG_ASSERT(array->elements != NULL);
    DEBUG_ASSERT(array->predicate != NULL);
    DEBUG_ASSERT(element != NULL);
    if (array->count == 0) {
        array->elements[0] = element;
        array->count++;
        return (intmax_t)array->count;
    }
    size_t i = 0;
    for (i = 0; i < array->count; ++i) {
        DEBUG_ASSERT(array->elements[i] != NULL);
        /* Break if the order predicate says element should come before the current element in the array.
         */
        if (!(array->predicate(array->elements[i], element))) {
            break;
        }
    }
    if (i >= array->count) {
        /* Append.
         */
        array->elements[array->count] = element;
    } else {
        /* Copy each pointer after the ith element right one space. Then insert the new element at position i.
         */
        void* tmp = array->elements[i];
        array->elements[i] = element;
        for (++i; i < array->count; ++i) {
            void* tmp2 = array->elements[i];
            array->elements[i] = tmp;
            tmp = tmp2;
        }
        array->elements[i] = element;
    }
    array->count++;
    DEBUG_ASSERT(array->elements[i] != NULL);
    /* Ensure we haven't run past the end of the array. TODO return failure when this happens instead of just crashing.
     */
    RUNTIME_CHECK(array->count <= array->capacity);
    return (intmax_t)i;
}

void* sorted_array_get(const struct sorted_array* array, size_t index)
{
    DEBUG_ASSERT(index < array->count);
    return array->elements[index];
}

void sorted_array_remove(struct sorted_array* array, size_t index)
{
    DEBUG_ASSERT(index < array->count);
    for (; index < array->count; ++index) {
        array->elements[index] = array->elements[index + 1];
    }
    --array->count;
}

size_t sorted_array_count(const struct sorted_array* array)
{
    return array->count;
}

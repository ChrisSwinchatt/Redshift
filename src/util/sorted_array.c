/**
 * \file kernel/sorted_array.c
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
#include <redshift/util/sorted_array.h>
#include <redshift/kernel.h>
#include <redshift/mem/static.h>
#include <redshift/mem/heap.h>
#include <string.h>

struct sorted_array {
    uintptr_t*      elements;   /* Element addresses.                                                 */
    uint8_t         freeable;   /* Whether the array was allocated statically (0) or dynamically (1). */
    size_t          count;      /* How many element addresses are currently stored.                   */
    size_t          capacity;   /* Maximum number of element addresses that can be stored.            */
    order_predicate predicate;  /* Function which determines where an element should be inserted.     */
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

struct sorted_array* create_sorted_array(size_t capacity, bool static_, order_predicate predicate)
{
    struct sorted_array* array;
    const size_t size = sizeof(*array);
    void* address = NULL;
    if (static_) {
        address = static_alloc(size);
    } else {
        address = kmalloc(size);
    }
    DEBUG_ASSERT(address != NULL);
    array = place_sorted_array(address, capacity, static_, predicate);
    return array;
}

struct sorted_array* place_sorted_array(void* address, size_t capacity, bool static_, order_predicate predicate)
{
    DEBUG_ASSERT(address != NULL);
    DEBUG_ASSERT(capacity > 0);
    DEBUG_ASSERT(predicate != NULL);
    struct sorted_array* list = address;
    const size_t size = sizeof(*(list->elements))*capacity;
    memset(list, 0, sizeof(*list));
    if (static_) {
        list->elements = static_alloc(size);
        list->freeable = 0;
    } else {
        list->elements = kmalloc(size);
        list->freeable = 1;
    }
    DEBUG_ASSERT(list->elements != NULL);
    memset(list->elements, 0, size);
    list->count     = 0;
    list->capacity  = capacity;
    list->predicate = predicate;
    return list;
}

void delete_sorted_array(struct sorted_array* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->elements != NULL);
    DEBUG_ASSERT(list->freeable == 1);
    kfree(list->elements);
    kfree(list);
}

size_t sorted_array_add(struct sorted_array* list, void* element)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->elements != NULL);
    DEBUG_ASSERT(list->predicate != NULL);
    DEBUG_ASSERT(element != NULL);
    const uintptr_t element_address = (uintptr_t)element;
    void* predicate = list->predicate;
    if (list->count == 0) {
        list->elements[0] = element_address;
        list->count++;
        DEBUG_ASSERT(list->predicate == predicate);
        return list->count;
    }
    size_t i = 0;
    for (i = 0; i < list->count; ++i) {
        DEBUG_ASSERT(list->elements[i] != 0);
        if (!(list->predicate((void*)list->elements[i], element))) {
            break;
        }
    }
    if (i >= list->count) {
        /* Append.
         */
        list->elements[list->count++] = element_address;
    } else {
        /* Move each address from the ith element to the end of the list one space to the "right". Then insert the new
         * element at position i.
         */
        for (size_t j = list->count; j > i; --j) {
            list->elements[j] = list->elements[j - 1];
        }
        list->elements[i] = element_address;
        list->count++;
    }
    /* Ensure we haven't run past the end of the array. TODO return failure when this happens instead of just crashing.
     */
    RUNTIME_CHECK(list->count <= list->capacity);
    return i;
}

void* sorted_array_get(const struct sorted_array* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    return (void*)(list->elements[index]);
}

void sorted_array_remove(struct sorted_array* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    for (; index < list->count; ++index) {
        list->elements[index] = list->elements[index + 1];
    }
    --list->count;
}

size_t sorted_array_count(const struct sorted_array* list)
{
    return list->count;
}

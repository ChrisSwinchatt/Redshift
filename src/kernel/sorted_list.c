/**
 * \file kernel/sorted_list.c
 * \brief Automatically-sorted list.
 * \author Chris Swinchatt <c.swinchatt1@uni.brighton.ac.uk>
 * \copyright Copyright (c) 2012-2014 Chris Swinchatt.
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
#include <kernel/sorted_list.h>
#include <kernel/redshift.h>
#include <mem/static.h>
#include <mem/heap.h>
#include <string.h>

struct sorted_list {
    void**          elements;
    uint8_t         freeable;
    size_t          count;
    size_t          capacity;
    order_predicate predicate;
};

bool uint32_t_ascending_order_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    uint32_t a = *(uint32_t*)pa;
    uint32_t b = *(uint32_t*)pb;
    return (a < b);
}

bool uint32_t_descending_order_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    uint32_t a = *(uint32_t*)pa;
    uint32_t b = *(uint32_t*)pb;
    return (a > b);
}

struct sorted_list* create_sorted_list(size_t capacity, bool static_, order_predicate predicate)
{
    return place_sorted_list(static_alloc(sizeof(struct sorted_list*)), capacity, static_, predicate);
}

struct sorted_list* place_sorted_list(void* address, size_t capacity, bool static_, order_predicate predicate)
{
    DEBUG_ASSERT(address != NULL);
    DEBUG_ASSERT(capacity > 0);
    DEBUG_ASSERT(predicate != NULL);
    struct sorted_list* list = address;
    size_t count = sizeof(*(list->elements)) * capacity;
    if (static_) {
        list->elements = (void**)static_alloc(count);
        list->freeable = 0;
    } else {
        list->elements = kmalloc(sizeof(*(list->elements)) * capacity);
        list->freeable = 1;
    }
    if (!(list->elements)) {
        return NULL;
    }
    memset(list->elements, 0, sizeof(*(list->elements)) * capacity);
    list->count = 0;
    list->capacity = capacity;
    list->predicate = predicate;
    return list;
}

void delete_sorted_list(struct sorted_list* list)
{
    if (list && list->freeable) {
        kfree(list->elements);
        kfree(list);
    }
}

size_t sorted_list_add(struct sorted_list* list, void* element)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->elements != NULL);
    DEBUG_ASSERT(list->predicate != NULL);
    DEBUG_ASSERT(element != NULL);
    size_t i;
    for (i = 0; i < list->count && list->predicate(list->elements[i], element); ++i)
        ;
    if (i >= list->count) {
        list->elements[list->count++] = element;
    } else {
        size_t j = i;
        void* tmp = list->elements[j];
        list->elements[j] = element;
        for (; j < list->count; ++j) {
            void* tmp2 = list->elements[j];
            list->elements[j] = tmp;
            tmp = tmp2;
        }
        list->count++;
    }
    return i;
}

void* sorted_list_get(const struct sorted_list* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    return list->elements[index];
}

void sorted_list_remove(struct sorted_list* list, size_t index)
{
    DEBUG_ASSERT(index < list->count);
    for (; index < list->count; ++index) {
        list->elements[index] = list->elements[index + 1];
    }
    --list->count;
}

size_t sorted_list_count(const struct sorted_list* list)
{
    return list->count;
}

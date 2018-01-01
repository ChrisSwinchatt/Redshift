/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free_fn of charge, to any person obtaining a copy
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
#include <redshift/kernel.h>
#include <redshift/util/container.h>
#include <redshift/util/forward_list.h>

struct forward_list_node {
    void*                     data;
    struct forward_list_node* next;
};

enum {
    IS_DUPLICATE = 1 << 0
};

struct forward_list {
    struct forward_list_node* head;
    struct forward_list_node* last;
    size_t                    size;
    alloc_fn_t                alloc_fn;
    free_fn_t                 free_fn;
    uint32_t                  flags;
};

static struct forward_list_node* get_node_by_index(struct forward_list* list, size_t index)
{
    if (index == 0) {
        return list->head;
    } else if (index + 1 == list->size) {
        return list->last;
    }
    struct forward_list_node* node = list->head;
    for (size_t i = 0; i <= index; ++i) {
        DEBUG_ASSERT(node->next != NULL);
        DEBUG_ASSERT(node->next != list->last);
        node = node->next;
    }
    return node;
}

struct forward_list* forward_list_create(alloc_fn_t alloc_fn, free_fn_t free_fn, size_t size, void* data)
{
    DEBUG_ASSERT(alloc_fn != NULL);
    struct forward_list* list = alloc_fn(sizeof(*list));
    list->head  = NULL;
    list->last  = NULL;
    list->size  = 0;
    list->alloc_fn = alloc_fn;
    list->free_fn  = free_fn;
    list->flags = 0;
    while (list->size < size) {
        forward_list_append(list, data);
    }
    return list;
}

struct forward_list* forward_list_append(struct forward_list* list, void* data)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->alloc_fn != NULL);
    struct forward_list_node* node = list->alloc_fn(sizeof(*node));
    RUNTIME_CHECK(node != NULL);
    node->data = data;
    node->next = NULL;
    if (NULL == list->head) {
        list->head = node;
    } else {
        list->last->next = node;
    }
    list->last = node;
    list->size += 1;
    return list;
}

struct forward_list* forward_list_prepend(struct forward_list* list, void* data)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->alloc_fn != NULL);
    struct forward_list_node* node = list->alloc_fn(sizeof(*node));
    RUNTIME_CHECK(node != NULL);
    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size += 1;
    return list;
}

struct forward_list* forward_list_insert(struct forward_list* list, void* data, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->alloc_fn != NULL);
    RUNTIME_CHECK(index <= list->size);
    if (index == 0) {
        return forward_list_prepend(list, data);
    } else if (index == list->size) {
        return forward_list_append(list, data);
    }
    /* Insert replaces the element at the index given, so we need the element before that.
     */
    struct forward_list_node* node = get_node_by_index(list, index - 1);
    struct forward_list_node* next = node->next;
    node->next = list->alloc_fn(sizeof(*node));
    RUNTIME_CHECK(node->next != NULL);
    node->next->data = data;
    node->next->next = next;
    list->size += 1;
    return list;
}

struct forward_list* forward_list_update(struct forward_list* list, size_t index, void* data)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct forward_list_node* node = get_node_by_index(list, index);
    DEBUG_ASSERT(node != NULL);
    node->data = data;
    return list;
}

struct forward_list* forward_list_grow(struct forward_list* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->alloc_fn != NULL);
    RUNTIME_CHECK(size > list->size);
    for (size_t i = 0; i < size; ++i) {
        forward_list_append(list, NULL);
    }
    DEBUG_ASSERT(list->size == size);
    return list;
}

struct forward_list* forward_list_shallow_shrink(struct forward_list* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    RUNTIME_CHECK(list->size > size);
    while (list->size > size) {
        forward_list_pop(list);
    }
    return list;
}

struct forward_list* forward_list_deep_shrink(struct forward_list* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    RUNTIME_CHECK(list->size > size);
    while (list->size > size) {
        void* data = forward_list_pop(list);
        list->free_fn(data);
    }
    return list;
}

void* forward_list_pop(struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    DEBUG_ASSERT(list->last != NULL);
    if (list->size == 1 || list->head == list->last) {
        /* Delete the only element in the list but don't delete the list.
         */
        DEBUG_ASSERT(list->size == 1);
        DEBUG_ASSERT(list->head == list->last);
        void* data = list->head->data;
        list->free_fn(list->head);
        list->head = NULL;
        list->last = NULL;
        return data;
    }
    /* Find the penultimate node.
     */
    struct forward_list_node* node = list->head;
    void* data = list->last->data;
    while (node->next != list->last) {
        DEBUG_ASSERT(node->next != NULL);
        node = node->next;
    }
    node->next = NULL;
    list->last->data = NULL;
    list->free_fn(list->last);
    list->last = node;
    list->size -= 1;
    return data;
}

struct forward_list* forward_list_shallow_remove(struct forward_list* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct forward_list_node* node = get_node_by_index(list, index);
    struct forward_list_node* next = node->next->next;
    node->next->data = NULL;
    node->next->next = NULL;
    list->free_fn(node->next);
    node->next = next;
    list->size -= 1;
    return list;
}

struct forward_list* forward_list_deep_remove(struct forward_list* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct forward_list_node* node = get_node_by_index(list, index);
    struct forward_list_node* next = node->next->next;
    node->next->next = NULL;
    list->free_fn(node->next->data);
    node->next->data = NULL;
    list->free_fn(node->next);
    node->next = next;
    list->size -= 1;
    return list;
}

void forward_list_shallow_free(struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    if (!(TEST_FLAG(list->flags, IS_DUPLICATE))) {
        struct forward_list_node* node = list->head;
        while (node != NULL) {
            struct forward_list_node* next = node->next;
            node->data = NULL;
            node->next = NULL;
            list->free_fn(node);
            node = next;
        }
    }
    list->head = NULL;
    list->last = NULL;
    list->size = 0;
    list->free_fn(list);
}

void forward_list_deep_free(struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    if (!(TEST_FLAG(list->flags, IS_DUPLICATE))) {
        struct forward_list_node* node = list->head;
        while (node != NULL) {
            struct forward_list_node* next = node->next;
            node->next = NULL;
            list->free_fn(node->data);
            node->data = NULL;
            list->free_fn(node);
            node = next;
        }
    }
    list->head = NULL;
    list->last = NULL;
    list->size = 0;
    list->free_fn(list);
}

void* forward_list_head(const struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    return list->head->data;
}

struct forward_list* forward_list_tail(const struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->alloc_fn != NULL);
    DEBUG_ASSERT(list->free_fn != NULL);
    DEBUG_ASSERT(list->head != NULL);
    DEBUG_ASSERT(list->last != NULL);
    if (list->size == 1) {
        DEBUG_ASSERT(list->head == list->last);
        return NULL;
    }
    struct forward_list* tail = list->alloc_fn(sizeof(*tail));
    tail->alloc_fn = list->alloc_fn;
    tail->free_fn  = list->free_fn;
    tail->head  = list->head->next;
    tail->last  = list->last;
    tail->size  = list->size - 1;
    tail->flags |= IS_DUPLICATE;
    return tail;
}

size_t forward_list_size(const struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    return list->size;
}

void* forward_list_index(const struct forward_list* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    DEBUG_ASSERT(list->last != NULL);
    RUNTIME_CHECK(index < list->size);
    if (index + 1 == list->size) {
        return list->last->data;
    }
    struct forward_list_node* node = list->head;
    for (size_t i = 0; i < index; ++i) {
        DEBUG_ASSERT(node->next != NULL);
        node = node->next;
    }
    return node->data;
}

void* forward_list_last(const struct forward_list* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->last != NULL);
    return list->last->data;
}

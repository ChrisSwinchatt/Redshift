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
#include <libk/kassert.h>
#include <libk/kextern.h>
#include <libk/klist.h>
#include <libk/kmacro.h>
#include <libk/ktypes.h>

struct klist_node {
    void*              data;
    struct klist_node* next;
};

struct klist {
    struct klist_node* head;
    struct klist_node* last;
    size_t             size;
    klist_flags_t      flags;
};

static struct klist_node* get_node_by_index(struct klist* list, size_t index)
{
    if (index == 0) {
        return list->head;
    } else if (index + 1 == list->size) {
        return list->last;
    }
    struct klist_node* node = list->head;
    for (size_t i = 0; i <= index; ++i) {
        DEBUG_ASSERT(node->next != NULL);
        DEBUG_ASSERT(node->next != list->last);
        node = node->next;
    }
    return node;
}

struct klist* klist_create(klist_flags_t flags, size_t size, void* data)
{
    struct klist* list = NULL;
    if (TEST_FLAG(flags, KLIST_FLAGS_DYNAMIC)) {
        list = kextern_dynamic_allocate(sizeof(*list));
    } else {
        list = kextern_static_allocate(sizeof(*list));
    }
    RUNTIME_CHECK(list != NULL);
    list->head  = NULL;
    list->last  = NULL;
    list->size  = 0;
    list->flags = flags;
    while (list->size < size) {
        klist_append(list, data);
    }
    return list;
}

struct klist* klist_append(struct klist* list, void* data)
{
    DEBUG_ASSERT(list != NULL);
    struct klist_node* node = NULL;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        node = kextern_dynamic_allocate(sizeof(*node));
    } else {
        node = kextern_static_allocate(sizeof(*node));
    }
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

struct klist* klist_prepend(struct klist* list, void* data)
{
    DEBUG_ASSERT(list != NULL);
    struct klist_node* node = NULL;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        node = kextern_dynamic_allocate(sizeof(*node));
    } else {
        node = kextern_static_allocate(sizeof(*node));
    }
    RUNTIME_CHECK(node != NULL);
    node->data = data;
    node->next = list->head;
    list->head = node;
    list->size += 1;
    return list;
}

struct klist* klist_insert(struct klist* list, void* data, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    RUNTIME_CHECK(index <= list->size);
    if (index == 0) {
        return klist_prepend(list, data);
    } else if (index == list->size) {
        return klist_append(list, data);
    }
    /* Insert replaces the element at the index given, so we need the element before that.
     */
    struct klist_node* node = get_node_by_index(list, index - 1);
    struct klist_node* next = node->next;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        node->next = kextern_dynamic_allocate(sizeof(*node));
    } else {
        node->next = kextern_static_allocate(sizeof(*node));
    }
    RUNTIME_CHECK(node->next != NULL);
    node->next->data = data;
    node->next->next = next;
    list->size += 1;
    return list;
}

struct klist* klist_update(struct klist* list, size_t index, void* data)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct klist_node* node = get_node_by_index(list, index);
    DEBUG_ASSERT(node != NULL);
    node->data = data;
    return list;
}

struct klist* klist_grow(struct klist* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    RUNTIME_CHECK(size > list->size);
    for (size_t i = 0; i < size; ++i) {
        klist_append(list, NULL);
    }
    DEBUG_ASSERT(list->size == size);
    return list;
}

struct klist* klist_shallow_shrink(struct klist* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    RUNTIME_CHECK(list->size > size);
    while (list->size > size) {
        klist_pop(list);
    }
    return list;
}

struct klist* klist_deep_shrink(struct klist* list, size_t size)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    RUNTIME_CHECK(list->size > size);
    while (list->size > size) {
        void* data = klist_pop(list);
        DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
        kextern_dynamic_free(data);
    }
    return list;
}

void* klist_pop(struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    DEBUG_ASSERT(list->last != NULL);
    if (list->size == 1 || list->head == list->last) {
        /* Delete the only element in the list but don't delete the list.
         */
        DEBUG_ASSERT(list->size == 1);
        DEBUG_ASSERT(list->head == list->last);
        void* data = list->head->data;
        kextern_dynamic_free(list->head);
        list->head = NULL;
        list->last = NULL;
        return data;
    }
    /* Find the penultimate node.
     */
    struct klist_node* node = list->head;
    void* data = list->last->data;
    while (node->next != list->last) {
        DEBUG_ASSERT(node->next != NULL);
        node = node->next;
    }
    node->next = NULL;
    list->last->data = NULL;
    kextern_dynamic_free(list->last);
    list->last = node;
    list->size -= 1;
    return data;
}

struct klist* klist_shallow_remove(struct klist* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct klist_node* node = get_node_by_index(list, index);
    struct klist_node* next = node->next->next;
    node->next->data = NULL;
    node->next->next = NULL;
    kextern_dynamic_free(node->next);
    node->next = next;
    list->size -= 1;
    return list;
}

struct klist* klist_deep_remove(struct klist* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    DEBUG_ASSERT(list->head != NULL);
    RUNTIME_CHECK(index < list->size);
    struct klist_node* node = get_node_by_index(list, index);
    struct klist_node* next = node->next->next;
    node->next->next = NULL;
    kextern_dynamic_free(node->next->data);
    node->next->data = NULL;
    kextern_dynamic_free(node->next);
    node->next = next;
    list->size -= 1;
    return list;
}

void klist_shallow_free(struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    if (!(TEST_FLAG(list->flags, KLIST_FLAGS_IS_DUPLICATE))) {
        struct klist_node* node = list->head;
        while (node != NULL) {
            struct klist_node* next = node->next;
            node->data = NULL;
            node->next = NULL;
            kextern_dynamic_free(node);
            node = next;
        }
    }
    list->head = NULL;
    list->last = NULL;
    list->size = 0;
    kextern_dynamic_free(list);
}

void klist_deep_free(struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(!(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)));
    if (!(TEST_FLAG(list->flags, KLIST_FLAGS_IS_DUPLICATE))) {
        struct klist_node* node = list->head;
        while (node != NULL) {
            struct klist_node* next = node->next;
            node->next = NULL;
            kextern_dynamic_free(node->data);
            node->data = NULL;
            kextern_dynamic_free(node);
            node = next;
        }
    }
    list->head = NULL;
    list->last = NULL;
    list->size = 0;
    kextern_dynamic_free(list);
}

void* klist_head(const struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    return list->head->data;
}

struct klist* klist_tail(const struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    DEBUG_ASSERT(list->last != NULL);
    if (list->size == 1) {
        DEBUG_ASSERT(list->head == list->last);
        return NULL;
    }
    struct klist* tail = NULL;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        tail = kextern_dynamic_allocate(sizeof(*tail));
    } else {
        tail = kextern_static_allocate(sizeof(*tail));
    }
    RUNTIME_CHECK(tail != NULL);
    tail->head  = list->head->next;
    tail->last  = list->last;
    tail->size  = list->size - 1;
    tail->flags = tail->flags | KLIST_FLAGS_IS_DUPLICATE;
    return tail;
}

size_t klist_size(const struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    return list->size;
}

void* klist_index(const struct klist* list, size_t index)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->head != NULL);
    DEBUG_ASSERT(list->last != NULL);
    RUNTIME_CHECK(index < list->size);
    if (index + 1 == list->size) {
        return list->last->data;
    }
    struct klist_node* node = list->head;
    for (size_t i = 0; i < index; ++i) {
        DEBUG_ASSERT(node->next != NULL);
        node = node->next;
    }
    return node->data;
}

void* klist_last(const struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    DEBUG_ASSERT(list->last != NULL);
    return list->last->data;
}

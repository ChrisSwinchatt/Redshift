/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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

/* For calling deep remove/free/shrink from shallow functions. */
static void null_callback(void* arg)
{
    UNUSED(arg);
}

static struct klist_node* get_node_by_index(struct klist* list, size_t index)
{
    DEBUG_ASSERT_SIZE_T(index, <, list->size);

    if (index == 0) {
        return list->head;
    } else if (index + 1 == list->size) {
        return list->last;
    }
    struct klist_node* node = list->head;
    for (size_t i = 0; i <= index; i++) {
        DEBUG_ASSERT_PTR(node->next, !=, NULL);
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

void klist_append(struct klist* list, void* data)
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    struct klist_node* node = NULL;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        node = kextern_dynamic_allocate(sizeof(*node));
    } else {
        node = kextern_static_allocate(sizeof(*node));
    }
    RUNTIME_CHECK_PTR(node, !=, NULL);
    node->data = data;
    node->next = NULL;
    if (NULL == list->head) {
        list->head = node;
    } else {
        list->last->next = node;
    }
    list->last = node;
    list->size++;
}

void klist_prepend(struct klist* list, void* data)
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    struct klist_node* node = NULL;
    if (TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC)) {
        node = kextern_dynamic_allocate(sizeof(*node));
    } else {
        node = kextern_static_allocate(sizeof(*node));
    }
    RUNTIME_CHECK(node != NULL);
    node->data = (void*)data;
    node->next = list->head;
    list->head = node;
    list->size += 1;
}

void klist_insert(struct klist* list, void* data, size_t index)
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    RUNTIME_CHECK(index <= list->size);
    if (index == 0) {
        return klist_prepend(list, data);
    } else if (index >= list->size - 1) {
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
    RUNTIME_CHECK_PTR(node->next, !=, NULL);
    node->next->data = (void*)data;
    node->next->next = next;
    list->size += 1;
}

void klist_update(struct klist* list, size_t index, void* data)
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    DEBUG_ASSERT_PTR(list->head, !=, NULL);
    RUNTIME_CHECK_SIZE_T(index, <, list->size);
    struct klist_node* node = get_node_by_index(list, index);
    DEBUG_ASSERT(node != NULL);
    node->data = (void*)data;
}

void klist_grow(struct klist* list, size_t size)
{
    DEBUG_ASSERT_PTR(list,     !=, NULL);
    RUNTIME_CHECK_SIZE_T(size, >,  list->size);
    for (size_t i = 0; i < size; ++i) {
        klist_append(list, NULL);
    }
    DEBUG_ASSERT(list->size == size);
}

void klist_shallow_shrink(struct klist* list, size_t size)
{
    return klist_deep_shrink(list, size, null_callback);
}

void klist_deep_shrink(struct klist* list, size_t size, void(* callback)(void*))
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    DEBUG_ASSERT(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC));
    RUNTIME_CHECK_SIZE_T(list->size, >, size);
    while (list->size > size) {
        void* data = klist_pop(list);
        callback(data);
        kextern_dynamic_free(data);
    }
}

void* klist_pop(struct klist* list)
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    DEBUG_ASSERT(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC));
    DEBUG_ASSERT_PTR(list->last, !=, NULL);
    if (list->size == 1 || list->head == list->last) {
        /* Delete the only element in the list but don't delete the list.
         */
        DEBUG_ASSERT_SIZE_T(list->size, ==, 1);
        DEBUG_ASSERT_PTR(list->head, ==, list->last);
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
        DEBUG_ASSERT_PTR(node->next, !=, NULL);
        node = node->next;
    }
    node->next = NULL;
    list->last->data = NULL;
    kextern_dynamic_free(list->last);
    list->last = node;
    list->size -= 1;
    return data;
}

void klist_shallow_remove(struct klist* list, size_t index)
{
    return klist_deep_remove(list, index, null_callback);
}

void klist_deep_remove(struct klist* list, size_t index, void(* callback)(void*))
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    DEBUG_ASSERT(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC));
    DEBUG_ASSERT_PTR(list->head, !=, NULL);

    RUNTIME_CHECK_SIZE_T(index, <, list->size);

    if (index == 0)
    {
        struct klist_node* old = list->head;
        list->head = old->next;
        callback(old->data);
        kextern_dynamic_free(old);
        return;
    }

    struct klist_node* prev;
    if (index == 1)
    {
        prev = list->head;
    }
    else
    {
        prev = get_node_by_index(list, index - 1);
    }

    // Cut out the node.
    struct klist_node* node = prev->next;
    struct klist_node* next = node->next;
    prev->next = next;

    // Free the data.
    callback(node->data);
    kextern_dynamic_free(node);
}

void klist_shallow_free(struct klist* list)
{
    klist_deep_free(list, null_callback);
}

void klist_deep_free(struct klist* list, void(* callback)(void*))
{
    DEBUG_ASSERT_PTR(list, !=, NULL);
    RUNTIME_CHECK(TEST_FLAG(list->flags, KLIST_FLAGS_DYNAMIC));
    if (!(TEST_FLAG(list->flags, KLIST_FLAGS_IS_DUPLICATE))) {
        struct klist_node* node = list->head;
        while (node != NULL) {
            struct klist_node* next = node->next;
            callback(node->data);
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
    DEBUG_ASSERT_PTR(list, !=, NULL);
    DEBUG_ASSERT_PTR(list->head, !=, NULL);
    return list->head->data;
}

size_t klist_size(const struct klist* list)
{
    DEBUG_ASSERT(list != NULL);
    return list->size;
}

void* klist_index(const struct klist* list, size_t index)
{
    DEBUG_ASSERT_PTR(list,       !=, NULL);
    DEBUG_ASSERT_PTR(list->head, !=, NULL);
    DEBUG_ASSERT_PTR(list->last, !=, NULL);
    RUNTIME_CHECK_SIZE_T(index,  <,  list->size);
    if (index + 1 == list->size) {
        return list->last->data;
    }
    struct klist_node* node = list->head;
    while (index-- > 0) {
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

void klist_foreach(struct klist* list, void(* callback)(size_t, void*))
{
    DEBUG_ASSERT(list != NULL);
    struct klist_node* node = list->head;
    size_t i = 0;
    while (node != NULL)
    {
        callback(i, node->data);
        node = node->next;
    }
}

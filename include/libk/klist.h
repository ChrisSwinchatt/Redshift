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
#ifndef REDSHIFT_LIBK_KLIST_H
#define REDSHIFT_LIBK_KLIST_H

#include <libk/ktypes.h>

/** Forward list. */
struct klist;

typedef enum {
    KLIST_FLAGS_STATIC       = 0,
    KLIST_FLAGS_DYNAMIC      = 1 << 0,
    KLIST_FLAGS_IS_DUPLICATE = 1 << 1
} klist_flags_t;

/**
 * Create a new forward list.
 * \param flags The list flags.
 * \param size The number of elements to preallocate. May be zero, in which case no elements are preallocated.
 * \param data A pointer to the data to initialise elements with. May be NULL.
 * \return The list is returned.
 */
struct klist* klist_create(klist_flags_t flags, size_t size, void* data);

/**
 * Add an element to the end of the list.
 * \param list The list to append to.
 * \param data The data to append.
 * \return The list is returned.
 */
void klist_append(struct klist* list, void* data);

/**
 * Add an element to the beginning of the list.
 * \param list The list to prepend to.
 * \param data The data to prepend.
 * \return The list is returned.
 */
void klist_prepend(struct klist* list, void* data);

/**
 * Insert an element anywhere in the list.
 * \param list The list to insert into.
 * \param data The data to prepend.
 * \param index The index to insert at. Must be less than or equal to the size of the list. The element that previously
 * occupied the given index follows the inserted element, so inserting at 0 is equivalent to prepending while inserting
 * at klist_size is equivalent to appending.
 * \return The list is returned.
 */
void klist_insert(struct klist* list, void* data, size_t index);

/**
 * Update the data pointer of an element in the list.
 * \param list The list to update.
 * \param index The index of the element to update. Must be strictly less than the size of the list.
 * \param data The pointer to update to.
 * \return The list is returned.
 */
void klist_update(struct klist* list, size_t index, void* data);

/**
 * Grow the list by allocating new, empty elements.
 * \param list The list to grow.
 * \param size The size of the list after growing. Must be strictly larger than the original size of the list.
 * \return The list is returned.
 */
void klist_grow(struct klist* list, size_t size);

/**
 * Shrink the list by deallocating elements from the end, without deallocating the data they contain. It's a bad idea
 * to use this if the list items contain dynamic memory.
 * \param list The list to shrink.
 * \param size The size of the list after shrinking. Must be strictly smaller than the original size of the list.
 * \return The list is returned.
 */
void klist_shallow_shrink(struct klist* list, size_t size);

/**
 * Shrink the list by deallocating elements from the end, using a callback to free the data contained by each node.
 * \param list The list to shrink.
 * \param size The size of the list after shrinking. Must be strictly smaller than the original size of the list.
 * \param callback A callback which frees the data pointed by each node in the list.
 * \return The list is returned.
 */
void klist_deep_shrink(struct klist* list, size_t size, void(* callback)(void*));

/**
 * Remove the last element from the list, deallocate it (but not the data it contains), and return the live data.
 * \param list The list to pop.
 * \return The data contained in the last element of the list is returned.
 */
void* klist_pop(struct klist* list);

/**
 * Remove an element from the list and deallocate it without deallocating the data it contains.
 * \param list The list to remove from.
 * \param index The index of the element to remove. Must be strictly smaller than the size of the list.
 * \return The list is returned,
 */
void klist_shallow_remove(struct klist* list, size_t index);

/**
 * Remove an element from the list and deallocate it, including the data it contains.
 * \param list The list to remove from.
 * \param index The index of the element to remove. Must be strictly smaller than the size of the list.
 * \param callback A callback which frees the data pointed by the node.
 * \return The list is returned,
 */
void klist_deep_remove(struct klist* list, size_t index, void(* callback)(void*));

/**
 * Deallocate a list without deallocating the data it contains.
 * \param list The list to deallocate.
 */
void klist_shallow_free(struct klist* list);

/**
 * Deallocate a list and the data it contains.
 * \param list The list to deallocate.
 * \param callback A callback which frees the data pointed by the node.
 */
void klist_deep_free(struct klist* list, void(* callback)(void*));

/**
 * Get the data stored in the element at the head of the list.
 * \param list The list.
 * \return The data stored in the element at the head of the list.
 */
void* klist_head(const struct klist* list);

/**
 * Get the size of (number of elements in) a list.
 * \param list The list.
 * \return The size of the list.
 */
size_t klist_size(const struct klist* list);

/**
 * Get the value of the element with the given index.
 * \param list The list.
 * \param index The index of the element to get. Must be strictly less than the size of the list.
 * \return The value of the element at the given index.
 */
void* klist_index(const struct klist* list, size_t index);

/**
 * Get the value of the element at the end of the list.
 * \param list The list.
 * \return The value of the element at the end of the list.
 */
void* klist_last(const struct klist* list);

/**
 * Iterate a klist, calling a function on every (index, data) pair in the list.
 */
void klist_foreach(struct klist* list, void(* callback)(size_t, void*));

#endif /* ! REDSHIFT_LIBK_KLIST_H */

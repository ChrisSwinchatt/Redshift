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
#ifndef REDSHIFT_UTIL_FORWARD_LIST_H
#define REDSHIFT_UTIL_FORWARD_LIST_H

#include <stddef.h>
#include <stdint.h>
#include <redshift/util/container.h>

/** Forward list. */
struct forward_list;

/**
 * Create a new forward list.
 * \param alloc_fn The allocation function e.g. malloc.
 * \param free_fn A function which frees memory e.g. free. May be NULL, but calling forward_list_pop,
 * forward_list_shallow_shrink, forward_list_shallow_free, forward_list_shallow_remove, forward_list_deep_shrink,
 * forward_list_deep_free or forward_list_deep_remove will result in undefined behaviour.
 * \param size The number of elements to preallocate. May be zero, in which case no elements are preallocated.
 * \param data A pointer to the data to initialise elements with. May be NULL.
 * \return The list is returned.
 */
struct forward_list* forward_list_create(alloc_fn_t alloc_fn, free_fn_t free_fn, size_t size, void* data);

/**
 * Add an element to the end of the list.
 * \param list The list to append to.
 * \param data The data to append.
 * \return The list is returned.
 */
struct forward_list* forward_list_append(struct forward_list* list, void* data);

/**
 * Add an element to the beginning of the list.
 * \param list The list to prepend to.
 * \param data The data to prepend.
 * \return The list is returned.
 */
struct forward_list* forward_list_prepend(struct forward_list* list, void* data);

/**
 * Insert an element anywhere in the list.
 * \param list The list to insert into.
 * \param data The data to prepend.
 * \param index The index to insert at. Must be less than or equal to the size of the list. The element that previously
 * occupied the given index follows the inserted element, so inserting at 0 is equivalent to prepending while inserting
 * at forward_list_size is equivalent to appending.
 * \return The list is returned.
 */
struct forward_list* forward_list_insert(struct forward_list* list, void* data, size_t index);

/**
 * Update the data pointer of an element in the list.
 * \param list The list to update.
 * \param index The index of the element to update. Must be strictly less than the size of the list.
 * \param data The pointer to update to.
 * \return The list is returned.
 */
struct forward_list* forward_list_update(struct forward_list* list, size_t index, void* data);

/**
 * Grow the list by allocating new, empty elements.
 * \param list The list to grow.
 * \param size The size of the list after growing. Must be strictly larger than the original size of the list.
 * \return The list is returned.
 */
struct forward_list* forward_list_grow(struct forward_list* list, size_t size);

/**
 * Shrink the list by deallocating elements from the end, without deallocating the data they contain.
 * \param list The list to shrink.
 * \param size The size of the list after shrinking. Must be strictly smaller than the original size of the list.
 * \return The list is returned.
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
struct forward_list* forward_list_shallow_shrink(struct forward_list* list, size_t size);

/**
 * Shrink the list by deallocating elements from the end, including the data they contain.
 * \param list The list to shrink.
 * \param size The size of the list after shrinking. Must be strictly smaller than the original size of the list.
 * \return The list is returned.
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
struct forward_list* forward_list_deep_shrink(struct forward_list* list, size_t size);

/**
 * Remove the last element from the list, deallocate it (but not the data it contains), and return its data.
 * \param list The list to pop.
 * \return The data contained in the last element of the list is returned.
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
void* forward_list_pop(struct forward_list* list);

/**
 * Remove an element from the list and deallocate it without deallocating the data it contains.
 * \param list The list to remove from.
 * \param index The index of the element to remove. Must be strictly smaller than the size of the list.
 * \return The list is returned,
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
struct forward_list* forward_list_shallow_remove(struct forward_list* list, size_t index);

/**
 * Deallocate a list without deallocating the data it contains.
 * \param list The list to deallocate.
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
void forward_list_shallow_free(struct forward_list* list);

/**
 * Remove an element from the list and deallocate it, including the data it contains.
 * \param list The list to remove from.
 * \param index The index of the element to remove. Must be strictly smaller than the size of the list.
 * \return The list is returned,
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
struct forward_list* forward_list_deep_remove(struct forward_list* list, size_t index);

/**
 * Deallocate a list and the data it contains.
 * \param list The list to deallocate.
 * \warning It is undefined behaviour to call this function if free_fn was NULL when forward_list_create created the list.
 */
void forward_list_deep_free(struct forward_list* list);

/**
 * Get the data stored in the element at the head of the list.
 * \param list The list.
 * \return The data stored in the element at the head of the list.
 */
void* forward_list_head(const struct forward_list* list);

/**
 * Get a new list containing all the elements in an existing list except the head element.
 * \param list The original list.
 * \return A new list containing all the elements of the original list except the head element.
 * \warning The tail list is created without copying or copy-on-write, so any modifications to either list will be made
 * to both lists (except when modifying the head element of the original list).
 * \warning When the tail list is allocated a flag is set indicating that it is a duplicate list. Calling
 * forward_list_shallow_free or forward_list_deep_free on a tail list will free_fn the list, but not the elements it
 * contains, so the original list will still be valid.
 */
struct forward_list* forward_list_tail(const struct forward_list* list);

/**
 * Get the size of (number of elements in) a list.
 * \param list The list.
 * \return The size of the list.
 */
size_t forward_list_size(const struct forward_list* list);

/**
 * Get the value of the element with the given index.
 * \param list The list.
 * \param index The index of the element to get. Must be strictly less than the size of the list.
 * \return The value of the element at the given index.
 */
void* forward_list_index(const struct forward_list* list, size_t index);

/**
 * Get the value of the element at the end of the list.
 * \param list The list.
 * \return The value of the element at the end of the list.
 */
void* forward_list_last(const struct forward_list* list);

#endif /* ! REDSHIFT_UTIL_FORWARD_LIST_H */

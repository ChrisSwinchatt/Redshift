/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
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
#ifndef REDSHIFT_UTIL_SORTED_ARRAY_H
#define REDSHIFT_UTIL_SORTED_ARRAY_H

#include <redshift/kernel.h>

typedef enum {
    SORTED_ARRAY_STATIC  = 0,
    SORTED_ARRAY_DYNAMIC = 1 << 0
} sorted_array_flags_t;

/** Sorted array. */
struct sorted_array;

/**
 * Sorting order predicate function. Determines whether element A comes before or after element B.
 * \param pa Pointer to element A.
 * \param pb Pointer to element B.
 * \return True if element A should come before element B, otherwise false.
 */
typedef bool(* order_predicate)(void* pa, void* pb);

/**
 * Ascending order predicate for uint32_t.
 * \param pa Pointer to element A.
 * \param pb Pointer to element B.
 * \return True if element A should come before element B, otherwise false.
 */
bool uint32_t_ascending_order_predicate(void* pa, void* pb);

/**
 * Descending order predicate for uint32_T.
 * \param pa Pointer to element A.
 * \param pb Pointer to element B.
 * \return True if element A should come before element B, otherwise false.
 */
bool uint32_t_descending_order_predicate(void* pa, void* pb);

/**
 * Allocate memory and create a sorted array.
 * \param capacity The number of elements the array can initially hold.
 * \param flags The flags.
 * \param predicate The order predicate.
 * \return The sorted array is returned.
 */
struct sorted_array* create_sorted_array(size_t capacity, sorted_array_flags_t flags, order_predicate predicate);

/**
 * Create a sorted array in an existing memory location.
 * \param address Where to place the array.
 * \param capacity The number of elements the array can initially hold.
 * \param flags The flags.
 * \param predicate The order predicate.
 * \return The sorted array is returned.
 */
struct sorted_array* place_sorted_array(void* address, size_t capacity, sorted_array_flags_t flags, order_predicate predicate);

/**
 * Delete a sorted array, freeing the memory allocated to it. Does nothing if the array was allocated statically.
 * \param array The sorted array to be freed.
 */
void delete_sorted_array(struct sorted_array* array);

/**
 * Add an element to a sorted array.
 * \param array The array.
 * \param element The element to add.
 * \return The index of the new element on success or -1 on error. This is just for error checking - subsequent add
 * operations invalidate indices as elements can be moved.
 */
intmax_t sorted_array_add(struct sorted_array* array, void* element);

/**
 * Get an element from the array by index.
 * \param array The array.
 * \param index The index.
 * \return A pointer to the element at the given index or NULL if the element does not exist.
 */
void* sorted_array_get(const struct sorted_array* array, size_t index);

/**
 * Remove an element from an array by index. In debug mode, fails if the index is outside of the range of the array.
 * \param array The array.
 * \param index The index of the element to remove.
 */
void sorted_array_remove(struct sorted_array* array, size_t index);

/**
 * Get the number of elements in an array.
 * \param array The array.
 * \return The number of elements in an array.
 */
size_t sorted_array_count(const struct sorted_array* array);

#endif /* ! REDSHIFT_UTIL_SORTED_ARRAY_H */

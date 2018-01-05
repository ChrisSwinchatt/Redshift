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

struct sorted_array;

typedef bool(* order_predicate)(void*, void*);

bool uint32_t_ascending_order_predicate(void* pa, void* pb);

bool uint32_t_descending_order_predicate(void* pa, void* pb);

struct sorted_array* create_sorted_array(size_t capacity, bool static_, order_predicate predicate);

struct sorted_array* place_sorted_array(void* address, size_t capacity, bool static_, order_predicate predicate);

void delete_sorted_array(struct sorted_array* list);

size_t sorted_array_add(struct sorted_array* list, void* element);

void* sorted_array_get(const struct sorted_array* list, size_t index);

void sorted_array_remove(struct sorted_array* list, size_t index);

size_t sorted_array_count(const struct sorted_array* list);

#endif /* ! REDSHIFT_UTIL_SORTED_ARRAY_H */

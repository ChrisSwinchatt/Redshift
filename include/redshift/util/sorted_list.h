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
#ifndef REDSHIFT_UTIL_SORTED_LIST_H
#define REDSHIFT_UTIL_SORTED_LIST_H

#include <redshift/kernel.h>

struct sorted_list;

typedef bool(* order_predicate)(void*, void*);

bool uint32_t_ascending_order_predicate(void* pa, void* pb);

bool uint32_t_descending_order_predicate(void* pa, void* pb);

struct sorted_list* create_sorted_list(size_t capacity, bool static_, order_predicate predicate);

struct sorted_list* place_sorted_list(void* address, size_t capacity, bool static_, order_predicate predicate);

void delete_sorted_list(struct sorted_list* list);

size_t sorted_list_add(struct sorted_list* list, void* element);

void* sorted_list_get(const struct sorted_list* list, size_t index);

void sorted_list_remove(struct sorted_list* list, size_t index);

size_t sorted_list_count(const struct sorted_list* list);

#endif /* ! REDSHIFT_UTIL_SORTED_LIST_H */

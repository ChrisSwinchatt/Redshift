/**
 * \file mem/static.h
 * \brief Static heap allocator. Allocates memory statically.
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
#ifndef REDSHIFT_MEM_STATIC_H
#define REDSHIFT_MEM_STATIC_H 1

#include <kernel/redshift.h>

/**
 * Initialise the static heap allocator.
 */
void static_init(void);

/**
 * \brief Allocate static memory.
 * \param size Amount of memory to allocate.
 * \param align Whether to page-align the address. Addresses are always aligned according to the size of the object.
 * \param phys Optional. Stores the physical address of the object.
 * \return The address of the memory block is returned.
 */
uint32_t static_alloc_base(size_t size, bool align, uint32_t* phys);

/**
 * \brief Allocate static memory.
 * \param size Amount of memory to allocate.
 * \return A pointer to the allocated memory is returned.
 */
void* static_alloc(size_t size);

#endif /* ! REDSHIFT_MEM_STATIC_H */

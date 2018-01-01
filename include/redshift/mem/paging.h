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
#ifndef REDSHIFT_MEM_PAGING_H
#define REDSHIFT_MEM_PAGING_H

#include <redshift/kernel.h>
#include <redshift/kernel/interrupt.h>

#define PAGE_ENTRIES 1024
#define PAGE_TABLES  1024

struct page;
extern struct page_directory* kernel_directory;

/**
 * Initialises paging.
 * \param mem_size The size of memory in kiB.
 * \return On success, 0 is returned. On error, -1 is returned.
 */
int paging_init(uint32_t mem_size);

/**
 * Loads a page directory.
 */
void page_directory_load(struct page_directory* dir);

/**
 * Gets a page from memory.
 * \param addr The page address.
 * \param dir The page directory.
 * \param create Whether to create the page if it doesn't exist.
 * \return The desired page.
 */
struct page* page_get(uint32_t addr, struct page_directory* dir, bool create);


void frame_alloc(struct page* page, bool mode, bool write);

void frame_free(struct page* page);

#endif /* ! REDSHIFT_MEM_PAGING_H */

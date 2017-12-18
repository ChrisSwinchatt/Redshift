/**
 * \file mem/paging.c
 * \brief Page allocator.
 * \author Chris Swinchatt <c.swinchatt1@uni.brighton.ac.uk>
 * \copyright Copyright (c) 2012-2014 Chris Swinchatt.
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
#include <kernel/redshift.h>
#include <mem/paging.h>
#include <kernel/interrupt.h>
#include <mem/heap.h>
#include <mem/static.h>
#include <string.h>

#define BIT_INDEX(a)  ((a) / (8 * 4))
#define BIT_OFFSET(a) ((a) % (8 * 4))

struct page {
    unsigned present  :  1;
    unsigned rw       :  1;
    unsigned user     :  1;
    unsigned accessed :  1;
    unsigned written  :  1;
    unsigned reserved :  7;
    unsigned frame    : 20;
};

struct page_table {
    struct page pages[PAGE_ENTRIES];
};

struct page_directory {
    struct page_table* tables[PAGE_TABLES];
    uint32_t physical_tables[PAGE_TABLES];
    uint32_t physical_address;
};

extern uint32_t heap_addr;
struct page_directory* kernel_directory;
static struct page_directory* current_directory;
static uint32_t* frames;
static uint32_t frames_count;

static void frame_set(uint32_t addr)
{
    uint32_t frame = addr / 0x1000;
    set_bit(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}

static void frame_clear(uint32_t addr)
{
    uint32_t frame = addr / 0x1000;
    clear_bit(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}

static uint32_t frame_test(uint32_t addr)
{
    uint32_t frame = addr / 0x1000;
    return test_bit(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}

static uint32_t frame_get_first_free()
{
    uint32_t i, j;
    for (i = 0; i < BIT_INDEX(frames_count); ++i) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; ++j) {
                if (!(test_bit(frames[i], j)))
                    return i * 4 * 8 + j;
            }
        }
    }
    return 0;
}

void frame_alloc(struct page* page, bool mode, bool write)
{
    if (page->frame)
        return; /* Already allocated. */
    uint32_t i = frame_get_first_free();
    if (i >= ((uint32_t)-1))
        panic("Out of memory");
    frame_set(i * 0x1000);
    page->present = 1;
    page->rw = (write) ? 1 : 0;
    page->user = (mode) ? 0 : 1;
    page->frame = i;
}

void frame_free(struct page* page)
{
    uint32_t frame = page->frame;
    if (!(frame)) {
        return; /* Already freed. */
    }
    frame_clear(frame);
    page->frame = 0;
}

static void page_fault_handler(const struct cpu_state* registers)
{
    uint32_t address = 0;
    asm("mov %%cr2, %0":"=r"(address));
    printk(PRINTK_ERROR "Page fault at 0x%08lX", address);
    if (test_bit(registers->errorcode, 2)) {
        printk(PRINTK_ERROR "in user mode ");
    } else {
        printk(PRINTK_ERROR "in kernel mode ");
    }
    if (test_bit(registers->errorcode, 1)) {
        printk(PRINTK_ERROR "when writing ");
    } else {
        printk(PRINTK_ERROR "when reading ");
    }
    if (!(test_bit(registers->errorcode, 0))) {
        printk(PRINTK_ERROR "because the page was not present ");
    } else if (test_bit(registers->errorcode, 3)) {
        printk(PRINTK_ERROR "because of an invalid write ");
    }
    if (test_bit(registers->errorcode, 4)) {
        printk(PRINTK_ERROR "during an instruction fetch ");
    }
    printk("\n");
    if (current_directory == kernel_directory) {
        panic("Kernel bug: page fault in kernel code.");
    }
}

void page_enable(void)
{
    uint32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":"=r"(cr0));
}

void page_disable(void)
{
    uint32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 &= ~0x80000000;
    asm volatile("mov %0, %%cr0":"=r"(cr0));
}

void page_directory_load(struct page_directory* dir)
{
    current_directory = dir;
    asm volatile("mov %0, %%cr3"::"r"(&dir->physical_tables));
    page_enable();
}

struct page* page_get(uint32_t addr, struct page_directory* dir, bool create)
{
    addr /= 0x1000;
    uint32_t i = addr / PAGE_ENTRIES;
    if (dir->tables[i]) {
        return &(dir->tables[i]->pages[addr % PAGE_ENTRIES]);
    } else if (create) {
        uint32_t tmp;
        dir->tables[i] = (struct page_table*)static_alloc_base(sizeof(struct page_table), true, &tmp);
        memset(dir->tables[i], 0, 0x1000);
        dir->physical_tables[i] = tmp | 0x07;
        return &(dir->tables[i]->pages[addr % PAGE_ENTRIES]);
    }
    return NULL;
}

#define HEAP_ADDRESS   0x1000000
#define HEAP_SIZE_INIT 0x1000

int paging_init(uint32_t mem_size)
{
    uint64_t mem_end = (uint64_t)mem_size * 1024ULL;
    frames_count = (uint32_t)(mem_end / 0x1000ULL);
    frames = (uint32_t*)static_alloc(BIT_INDEX(frames_count));
    memset(frames, 0, BIT_INDEX(frames_count));
    /* Create kernel page directory.
     */
    kernel_directory = static_alloc(sizeof(*kernel_directory));
    memset(kernel_directory, 0, sizeof(*kernel_directory));
    /* Map some pages to the kernel heap.
     */
    uint32_t i;
    for (i = HEAP_ADDRESS; i < HEAP_ADDRESS + HEAP_SIZE_INIT; i += 0x1000) {
        page_get(i, kernel_directory, true);
    }
    /* Create, identity map & allocate pages.
     */
    for (i = 0; i < heap_addr; i += 0x1000) {
        frame_alloc(page_get(i, kernel_directory, true), false, false);
    }
    /* Set page fault handler.
     */
    set_interrupt_handler(14, page_fault_handler);
    page_directory_load(kernel_directory);
    return 0;
}

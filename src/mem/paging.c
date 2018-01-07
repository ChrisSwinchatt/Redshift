/**
 * \file mem/paging.c
 * Page allocator.
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
#include <redshift/kernel.h>
#include <redshift/mem/common.h>
#include <redshift/mem/paging.h>
#include <redshift/kernel/interrupt.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/static.h>
#include <string.h>

#define BIT_INDEX(a)  ((a)/(8*4))
#define BIT_OFFSET(a) ((a)%(8*4))

struct page {
    unsigned present  :  1;
    unsigned rw       :  1;
    unsigned user     :  1;
    unsigned accessed :  1;
    unsigned written  :  1;
    unsigned reserved :  7;
    unsigned frame    : 20;
} __packed;

struct page_table {
    struct page pages[PAGE_ENTRIES];
};

struct page_directory {
    struct page_table* tables[PAGE_TABLES];
    uint32_t physical_tables[PAGE_TABLES];
    uint32_t physical_address;
};

extern uint32_t               heap_addr;
struct page_directory*        kernel_directory;
static struct page_directory* current_directory;
static uint32_t*              frames;
static uint32_t               frames_count;

static void frame_set(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    SET_BIT(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}

static void frame_clear(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    CLEAR_BIT(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}

#if 0
static uint32_t frame_test(uint32_t addr)
{
    uint32_t frame = addr / PAGE_SIZE;
    return TEST_BIT(frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
}
#endif

static uint32_t frame_get_first_free(void)
{
    uint32_t i, j;
    for (i = 0; i < BIT_INDEX(frames_count); ++i) {
        if (frames[i] != 0xFFFFFFFF) {
            for (j = 0; j < 32; ++j) {
                if (!(TEST_BIT(frames[i], j))) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
    return 0;
}

void frame_alloc(struct page* page, page_flags_t flags)
{
    SAVE_INTERRUPT_STATE;
    if (page->frame) {
        return; /* Already allocated. */
    }
    uint32_t i = frame_get_first_free();
    if (i >= ((uint32_t)-1)) {
        panic("%s: out of memory", __func__);
    }
    frame_set(i * PAGE_SIZE);
    page->present = TEST_FLAG(flags, PAGE_FLAGS_PRESENT)   ? 1 : 0;
    page->rw      = TEST_FLAG(flags, PAGE_FLAGS_WRITEABLE) ? 1 : 0;
    page->user    = TEST_FLAG(flags, PAGE_FLAGS_USER_MODE) ? 1 : 0;
    page->frame   = i;
    RESTORE_INTERRUPT_STATE;
}

void frame_free(struct page* page)
{
    SAVE_INTERRUPT_STATE;
    uint32_t frame = page->frame;
    if (!(frame)) {
        return; /* Already freed. */
    }
    frame_clear(frame);
    page->frame = 0;
    RESTORE_INTERRUPT_STATE;
}

static void page_fault_handler(const struct cpu_state* regs)
{
    uint32_t address = 0;
    asm("mov %%cr2, %0":"=r"(address));
    bool user    = TEST_BIT(regs->error_code, 2);
    bool rw      = TEST_BIT(regs->error_code, 1);
    bool present = TEST_BIT(regs->error_code, 0);
    printk(
        PRINTK_ERROR "Page fault at 0x%8lX in %s mode when %s because %s\n",
        address,
        user    ? "user"                       : "kernel",
        rw      ? "writing"                    : "reading",
        present ? "there was an invalid write" : "the page was not marked present"
    );
    if (!(user)) {
        panic("bug: kernel triggered page fault");
    }
}

void page_enable(void)
{
    SAVE_INTERRUPT_STATE;
    uint32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":"=r"(cr0));
    RESTORE_INTERRUPT_STATE;
}

void page_disable(void)
{
    SAVE_INTERRUPT_STATE;
    uint32_t cr0;
    asm volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 &= ~0x80000000;
    asm volatile("mov %0, %%cr0":"=r"(cr0));
    RESTORE_INTERRUPT_STATE;
}

void page_directory_load(struct page_directory* dir)
{
    SAVE_INTERRUPT_STATE;
    current_directory = dir;
    asm volatile("mov %0, %%cr3"::"r"(&dir->physical_tables));
    RESTORE_INTERRUPT_STATE;
}

struct page* page_get(uint32_t addr, struct page_directory* dir, bool create)
{
    SAVE_INTERRUPT_STATE;
    addr /= PAGE_SIZE;
    uint32_t i = addr / PAGE_ENTRIES;
    if (dir->tables[i]) {
        RESTORE_INTERRUPT_STATE;
        return &(dir->tables[i]->pages[addr % PAGE_ENTRIES]);
    } else if (create) {
        uint32_t tmp;
        dir->tables[i] = (struct page_table*)static_alloc_base(sizeof(struct page_table), true, &tmp);
        memset(dir->tables[i], 0, PAGE_SIZE);
        dir->physical_tables[i] = tmp | 0x07;
        return &(dir->tables[i]->pages[addr % PAGE_ENTRIES]);
    }
    RESTORE_INTERRUPT_STATE;
    return NULL;
}

enum {
    HEAP_ADDRESS   = 0x1000000,
    HEAP_SIZE_INIT = 0x1000
};

int paging_init(uint32_t mem_size)
{
    SAVE_INTERRUPT_STATE;
    uint64_t mem_end = (uint64_t)mem_size * 1024ULL;
    frames_count = (uint32_t)(mem_end / 0x1000ULL);
    frames = (uint32_t*)static_alloc(BIT_INDEX(frames_count));
    memset(frames, 0, BIT_INDEX(frames_count));
    /* Create kernel page directory.
     */
    kernel_directory = static_alloc(sizeof(*kernel_directory));
    memset(kernel_directory, 0, sizeof(*kernel_directory));
    /* Identity map pages up to heap address. We make the first page non-present so that NULL-pointer dereferences cause
     * a page fault.
     */
    frame_alloc(page_get(0, kernel_directory, true), 0);
    uint32_t i = 0;
    for (i = PAGE_SIZE; i < heap_addr; i += PAGE_SIZE) {
        frame_alloc(page_get(i, kernel_directory, true), PAGE_FLAGS_PRESENT);
    }
    /* Map pages for the heap.
    */
    for (; i < HEAP_ADDRESS + HEAP_SIZE_INIT; i += PAGE_SIZE) {
        page_get(i, kernel_directory, true);
    }
    /* Set page fault handler.
     */
    set_interrupt_handler(ISR_PAGE_FAULT, page_fault_handler);
    page_directory_load(kernel_directory);
    page_enable();
    RESTORE_INTERRUPT_STATE;
    return 0;
}

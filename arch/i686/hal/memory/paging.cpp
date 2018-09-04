/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include <libk/asciz.hpp>
#include <libk/memory.hpp>
#include <libk/vector.hpp>
#include <redshift/kernel.hpp>
#include <redshift/hal/cpu.hpp>
#include <redshift/hal/memory.hpp>
#include <redshift/kernel/interrupt.hpp>

extern "C" uint32_t heap_addr;

namespace redshift { namespace hal { namespace memory_detail {
    page_directory* page_directory::kernel  = nullptr;
    page_directory* page_directory::current = nullptr;

    enum {
        HEAP_ADDRESS   = 0x1000000,
        HEAP_SIZE_INIT = 0x1000
    };

    page_directory* page_directory::create(type type_)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        hal::memory::total_size();
        page_directory* dir = static_alloc(sizeof(*dir));
        libk::memory::fil8(dir, 0, sizeof(*dir));
        if (type_ == type::kernel) {
            page_directory::kernel = dir;
        }
        return dir;
    }

    void page_directory::init_frames()
    {
        // Create page frames.
        const uint64_t mem_end    = static_cast<uint64_t>(hal::memory::total_size())*0x1000ULL;
        const size_t   num_frames = static_cast<size_t>(mem_end/0x1000ULL);
        m_frames = libk::vector<uint32_t>(static_alloc(BIT_INDEX(num_frames)), num_frames);
        libk::memory::fill8(m_frames, 0, BIT_INDEX(num_frames));
    }

    void page_directory::load()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        current_directory = this;
        asm volatile("mov %0, %%cr3"::"r"(&m_physical_tables));
    }

    page* page_directory::get_page(uintptr_t address, bool create)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        addr /= PAGE_SIZE;
        uint32_t i = addr / PAGE_ENTRIES;
        if (m_tables[i]) {
            return &(m_tables[i]->pages[addr % PAGE_ENTRIES]);
        } else if (create) {
            uintptr_t phys = 0;
            m_tables[i] = (page_table*)static_alloc_base(sizeof(m_tables[i]), true, &phys);
            libk::memory::fill8(m_tables[i], 0, PAGE_SIZE);
            m_physical_tables[i] = phys | 0x07;
            return &(m_tables[i]->pages[addr % PAGE_ENTRIES]);
        }
        return nullptr;
    }

    size_t page_directory::map_pages(size_t first, size_t last, bool create, bool idmap)
    {
        MAKE_PAGE_ALIGNED(start);
        for (size_t i = first; i < last; ++i) {
            page* page = page_get(i*PAGE_SIZE, create);
            if (page && idmap) {
                alloc_frame(page);
            }
        }
        return last - first;
    }

    void page_directory::alloc_frame(page* page, page_flags_t flags)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        if (page->frame) {
            return; // Already allocated.
        }
        uint32_t i = get_first_free_frame();
        if (i >= ((uint32_t)-1)) {
            panic("%s: out of memory", __func__);
        }
        set_frame(i * PAGE_SIZE);
        page->present = TEST_FLAG(flags, PAGE_FLAGS_PRESENT)   ? 1 : 0;
        page->rw      = TEST_FLAG(flags, PAGE_FLAGS_WRITEABLE) ? 1 : 0;
        page->user    = TEST_FLAG(flags, PAGE_FLAGS_USER_MODE) ? 1 : 0;
        page->frame   = i;
    }

    void page_directory::free_frame(page* page)
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t frame = page->frame;
        if (!(frame)) {
            return; // Already freed.
        }
        clear_frame(frame);
        page->frame = 0;
    }

    void page_directory::set_frame(uint32_t addr)
    {
        uint32_t frame = addr / PAGE_SIZE;
        SET_BIT(m_frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
    }

    void page_directory::clear_frame(uint32_t addr)
    {
        uint32_t frame = addr / PAGE_SIZE;
        CLEAR_BIT(m_frames[BIT_INDEX(frame)], BIT_OFFSET(frame));
    }

    uint32_t page_directory::get_first_free_frame()
    {
        uint32_t i, j;
        for (i = 0; i < BIT_INDEX(m_frames.size()); ++i) {
            if (m_frames[i] != 0xFFFFFFFF) {
                for (j = 0; j < 32; ++j) {
                    if (!(TEST_BIT(m_frames[i], j))) {
                        return i * 4 * 8 + j;
                    }
                }
            }
        }
        return 0;
    }

    static void handle_page_fault(const cpu::state* regs)
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

    void enable_paging()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t cr0;
        asm volatile("mov %%cr0, %0":"=r"(cr0));
        cr0 |= 0x80000000;
        asm volatile("mov %0, %%cr0":"=r"(cr0));
    }

    void disable_paging()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        uint32_t cr0;
        asm volatile("mov %%cr0, %0":"=r"(cr0));
        cr0 &= ~0x80000000;
        asm volatile("mov %0, %%cr0":"=r"(cr0));
    }

    int init_paging()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        // Create page frames.
        page_directory::init_frames();
        // Create & initialise kernel page directory.
        page_directory* directory = page_directory::create(page_directory::type::kernel);
        // ID map pages up to the heap address. We skip page 0 so nullptr dereferences cause a page fault.
        const size_t first_page = 1;
        const size_t heap_page  = heap_addr/PAGE_SIZE + 1;
        const size_t last_page  = (heap_addr + HEAP_SIZE_INIT)/PAGE_SIZE + 1;
        directory->map_pages(first_page, heap_page, true, true);
        // Map heap pages.
        directory->map_pages(heap_page,  last_page, true, false);
        // Set page fault handler.
        set_interrupt_handler(ISR_PAGE_FAULT, page_fault_handler);
        // Load directory and enable paging.
        directory->load();
        page_enable();
        return 0;
    }

}}} // redshift::hal::memory_detail

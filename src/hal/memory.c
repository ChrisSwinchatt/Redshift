/**
 * \file hal/memory.c
 * Memory abstraction.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Copyright (c) 2012-2018 Chris Swinchatt.
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
#include <redshift/boot/multiboot.h>
#include <redshift/hal/memory.h>
#include <redshift/mem/static.h>
#include <redshift/kernel.h>

static struct memory {
    size_t             size_lower;
    size_t             size_upper;
    struct memory_map* map;
    size_t             size_map;
} memory;

void memory_init(struct multiboot_tag* mb_tags)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(mb_tags != NULL);
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)mb_tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                memory.size_lower = ((struct multiboot_tag_basic_meminfo*)tag)->mem_lower;
                memory.size_upper = ((struct multiboot_tag_basic_meminfo*)tag)->mem_upper;
                char prefix;
                uint32_t mem = memory.size_lower + memory.size_upper;
                if (mem > 1024*1024) {
                    mem /= 1024*1024;
                    ++mem;
                    prefix = 'G';
                } else if (mem > 1024) {
                    mem /= 1024;
                    ++mem;
                    prefix = 'M';
                } else {
                    prefix = 'k';
                }
                printk(PRINTK_DEBUG "System has %lu %ciB RAM\n", mem, prefix);
                break;
            default:
                break;
        }
    }
    RESTORE_INTERRUPT_STATE;
}

void memory_map_init(struct multiboot_tag* mb_tags)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(mb_tags != NULL);
    struct multiboot_mmap_entry* mmap = NULL;
    struct memory_map* memmap = NULL;
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)mb_tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MMAP:
                printk(PRINTK_DEBUG "Memory map:\n");
                for (mmap = ((struct multiboot_tag_mmap*)tag)->entries;
                     (uint8_t*)mmap < (uint8_t*)tag + tag->size;
                     mmap = (struct multiboot_mmap_entry*)((uint32_t)mmap +
                            ((struct multiboot_tag_mmap*)tag)->entry_size)) {
                    /* Store the entry into `info`.
                     */
                    if (memmap == NULL || memory.map == NULL) {
                        /* Initialise the list.
                         */
                        memory.map = (struct memory_map*)static_alloc(sizeof(*memory.map));
                        RUNTIME_CHECK(memory.map != NULL);
                        memmap = memory.map;
                    } else {
                        /* Add a new node to the list.
                         */
                        memmap->next = (struct memory_map*)static_alloc(sizeof(*memmap));
                        RUNTIME_CHECK(memmap->next != NULL);
                        memmap = (struct memory_map*)memmap->next; /* Ignore const. */
                    }
                    memmap->type  = mmap->type;
                    memmap->start = mmap->addr;
                    memmap->end   = mmap->addr + mmap->len - 1;
                    memmap->next  = NULL;
                    ++memory.size_map;
                    /* Print some debugging info about the memory region.
                     */
                    {
                        const uint64_t kib = mmap->len / 1024UL;
                        const char* type_str = NULL;
                        switch (memmap->type) {
                          case MEMORY_TYPE_AVAILABLE:   type_str = "available";   break;
                          case MEMORY_TYPE_RECLAIMABLE: type_str = "reclaimable"; break;
                          default:                      type_str = "unavailable"; break;
                        }
                        printk(PRINTK_DEBUG "%9Lu kiB at 0x%016LX - 0x%016LX: %s\n",
                               kib,
                               memmap->start,
                               memmap->end,
                               type_str);
                    }
                }
                break;
            default:
                break;
        }
    }
    RESTORE_INTERRUPT_STATE;
}

const struct memory_map* memory_map_head(void)
{
    return memory.map;
}

size_t memory_map_size(void)
{
    return memory.size_map;
}

size_t memory_size_lower(void)
{
    return memory.size_lower;
}

size_t memory_size_upper(void)
{
    return memory.size_upper;
}

size_t memory_size_total(void)
{
    return memory.size_lower + memory.size_upper;
}

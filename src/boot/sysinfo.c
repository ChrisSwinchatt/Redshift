/**
 * \file boot/sysinfo.c
 * \brief Collect system details, primarily from multiboot tags.
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
#include <kernel/redshift.h>
#include <boot/multiboot.h>
#include <boot/sysinfo.h>
#include <mem/static.h>
#include <string.h>

struct sysinfo __sysinfo__;

void sysinfo_init_1(struct sysinfo* info, struct multiboot_tag* tags)
{
    DEBUG_ASSERT(info != NULL);
    DEBUG_ASSERT(tags != NULL);
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                info->cmdline = ((struct multiboot_tag_string*)tag)->string;
                printk(PRINTK_DEBUG " * Kernel command-line: \"%s\"\n", info->cmdline);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                info->bootloader = ((struct multiboot_tag_string*)tag)->string;
                printk(PRINTK_DEBUG " * Bootloader name: \"%s\"\n", info->bootloader);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                if (info->modules_end < ((struct multiboot_tag_module*)tag)->end) {
                    info->modules_end = ((struct multiboot_tag_module*)tag)->end;
                }
                break;
            case MULTIBOOT_TAG_TYPE_BOOTDEV:
                info->device    = ((struct multiboot_tag_bootdev*)tag)->biosdev;
                info->slice     = ((struct multiboot_tag_bootdev*)tag)->slice;
                info->partition = ((struct multiboot_tag_bootdev*)tag)->part;
                printk(PRINTK_DEBUG " * Booted from BIOS device %02lX:%02lX:%02lX\n",
                       info->device,
                       info->slice,
                       info->partition);
                break;
            default:
                break;
        }
    }
}

/* TODO: store modules.
 */
void sysinfo_init_2(struct sysinfo* info, struct multiboot_tag* tags)
{
    DEBUG_ASSERT(info != NULL);
    DEBUG_ASSERT(tags != NULL);
    struct multiboot_mmap_entry* mmap;
    struct memmap* memmap = NULL;
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MODULE:
                printk(PRINTK_DEBUG " * Module at 0x%08lX to 0x%08lX: \"%s\"\n",
                       ((struct multiboot_tag_module*)tag)->start,
                       ((struct multiboot_tag_module*)tag)->end,
                       ((struct multiboot_tag_module*)tag)->cmdline);
                break;
            default:
                break;
        }
    }
}

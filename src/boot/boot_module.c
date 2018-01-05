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
#include <redshift/boot/multiboot.h>
#include <redshift/boot/boot_module.h>
#include <redshift/mem/static.h>
#include <string.h>

static struct {
    struct boot_module* head;
    size_t              count;
    uintptr_t           end;
} __modules__;

void discover_boot_modules(struct multiboot_tag* mb_tags)
{
    DEBUG_ASSERT(mb_tags != NULL);
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)mb_tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MODULE:
            {
                struct multiboot_tag_module* module = (struct multiboot_tag_module*)tag;
                if (module->end > __modules__.end) {
                    __modules__.end = module->end;
                }
                break;
            }
            default:
                break;
        }
    }
}

void save_boot_modules(struct multiboot_tag* mb_tags)
{
    DEBUG_ASSERT(mb_tags != NULL);
    struct boot_module* modlist = static_alloc(sizeof(*modlist));
    __modules__.head = modlist;
    for (struct multiboot_tag* tag = (struct multiboot_tag*)((uint8_t*)mb_tags + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7))) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_MODULE:
            {
                /* Copy data.
                 */
                struct multiboot_tag_module* module = (struct multiboot_tag_module*)tag;
                modlist->start = module->start;
                modlist->end   = module->end;
                size_t length  = strlen(module->cmdline);
                if (length > 0) {
                    modlist->cmdline = static_alloc(length);
                    strncpy((char*)(modlist->cmdline), module->cmdline, length); /* Ignore const modlist->cmdline; */
                }
                printk(PRINTK_DEBUG "Module: <start=0x%08lX,end=0x%08lX,cmdline=\"%s\">\n",
                       modlist->start,
                       modlist->end,
                       modlist->cmdline);
                /* Allocate next module and increment module count.
                 * FIXME this will always allocate an extra entry. Move allocation to the start of the loop.
                 */
                modlist->next = static_alloc(sizeof(*modlist));
                modlist = (struct boot_module*)modlist->next; /* Ignore const modlist. */
                ++__modules__.count;
                break;
            }
            default:
                break;
        }
    }
}

const struct boot_module* boot_modules_head(void)
{
    return __modules__.head;
}

size_t boot_modules_count(void)
{
    return __modules__.count;
}

uintptr_t boot_modules_end(void)
{
    return __modules__.end;
}

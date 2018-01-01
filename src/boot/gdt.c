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
#include <redshift/kernel.h>
#include <redshift/boot/gdt.h>
#include <redshift/boot/tss.h>

enum {
    GDT_ENTRIES_SIZE = 5
};

static struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __packed gdt_entries[GDT_ENTRIES_SIZE];

static struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __packed pgdt;

static void gdt_entry(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[i].base_low     = (base & 0xFFFF);
    gdt_entries[i].base_mid     = (base >> 16) & 0xFF;
    gdt_entries[i].base_high    = (base >> 24) & 0xFF;
    gdt_entries[i].limit_low    = (limit & 0xFFFF);
    gdt_entries[i].granularity  = (limit >> 16) & 0x0F;
    gdt_entries[i].granularity |= gran & 0xF0;
    gdt_entries[i].access       = access;
}

extern void loadgdt(uint32_t addr); /* loadgdt.asm */

void gdt_init(void)
{
    const uint32_t tss_base  = get_tss_base();
    const uint32_t tss_limit = tss_base + get_tss_size();
    pgdt.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES_SIZE) - 1;
    pgdt.base = (uint32_t)&gdt_entries;
    gdt_entry(0, 0x00000000, 0x00000000, 0x00, 0x00);
    gdt_entry(1, 0x00000000, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_entry(2, 0x00000000, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_entry(3, 0x00000000, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_entry(4, 0x00000000, 0xFFFFFFFF, 0xF2, 0xCF);
    //gdt_entry(5, tss_base,   tss_limit,  0xE9, 0x00);
    loadgdt((uint32_t)&pgdt);
    UNUSED(tss_limit);
}

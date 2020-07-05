/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#include <libk/kmemory.h>
#include <libk/kstring.h>
#include <redshift/kernel.h>
#include <redshift/boot/idt.h>
#include <redshift/kernel/asm.h>

enum {
    IDT_ENTRIES_SIZE = 256
};

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t base_high;
} __packed idt_entries[IDT_ENTRIES_SIZE];

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __packed pidt;

static void idt_entry(uint8_t i, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt_entries[i].base_low  = base & 0xffff;
    idt_entries[i].base_high = (base >> 16) & 0xffff;
    idt_entries[i].selector  = selector;
    idt_entries[i].reserved  = 0;
    idt_entries[i].flags     = flags;
}

extern void loadidt(uint32_t addr); /* loadidt.asm */

void idt_init(void)
{
    PUSH_INTERRUPT_STATE(0);
    pidt.limit = (sizeof(struct idt_entry) * IDT_ENTRIES_SIZE) - 1;
    pidt.base = (uint32_t)&idt_entries;
    kmemory_fill8(&idt_entries, 0, sizeof(struct idt_entry) * IDT_ENTRIES_SIZE);
    idt_entry( 0,  (uint32_t)isr0, 0x08, 0x8E);
    idt_entry( 1,  (uint32_t)isr1, 0x08, 0x8E);
    idt_entry( 2,  (uint32_t)isr2, 0x08, 0x8E);
    idt_entry( 3,  (uint32_t)isr3, 0x08, 0x8E);
    idt_entry( 4,  (uint32_t)isr4, 0x08, 0x8E);
    idt_entry( 5,  (uint32_t)isr5, 0x08, 0x8E);
    idt_entry( 6,  (uint32_t)isr6, 0x08, 0x8E);
    idt_entry( 7,  (uint32_t)isr7, 0x08, 0x8E);
    idt_entry( 8,  (uint32_t)isr8, 0x08, 0x8E);
    idt_entry( 9,  (uint32_t)isr9, 0x08, 0x8E);
    idt_entry(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_entry(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_entry(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_entry(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_entry(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_entry(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_entry(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_entry(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_entry(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_entry(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_entry(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_entry(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_entry(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_entry(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_entry(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_entry(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_entry(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_entry(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_entry(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_entry(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_entry(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_entry(31, (uint32_t)isr31, 0x08, 0x8E);
    idt_entry(32,  (uint32_t)irq0, 0x08, 0x8E);
    idt_entry(33,  (uint32_t)irq1, 0x08, 0x8E);
    idt_entry(34,  (uint32_t)irq2, 0x08, 0x8E);
    idt_entry(35,  (uint32_t)irq3, 0x08, 0x8E);
    idt_entry(36,  (uint32_t)irq4, 0x08, 0x8E);
    idt_entry(37,  (uint32_t)irq5, 0x08, 0x8E);
    idt_entry(38,  (uint32_t)irq6, 0x08, 0x8E);
    idt_entry(39,  (uint32_t)irq7, 0x08, 0x8E);
    idt_entry(40,  (uint32_t)irq8, 0x08, 0x8E);
    idt_entry(41,  (uint32_t)irq9, 0x08, 0x8E);
    idt_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_entry(47, (uint32_t)irq15, 0x08, 0x8E);
    loadidt((uint32_t)&pidt);
    POP_INTERRUPT_STATE();
}

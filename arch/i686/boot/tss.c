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
#include <libk/kstring.h>
#include <libk/kmemory.h>
#include <redshift/boot/tss.h>
#include <redshift/kernel.h>

static struct tss {
   uint32_t reserved;
   uint32_t esp0;
   uint32_t ss0;
   uint32_t esp1;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;
   uint32_t cs;
   uint32_t ss;
   uint32_t ds;
   uint32_t fs;
   uint32_t gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iobt;
} __packed tss;

void tss_init(void)
{
    kmemory_fill8(&tss, 0, sizeof(tss));
    tss.esp0 = (uint32_t)__stack_top__;
    tss.ss0  = 0x10;
    tss.iobt = sizeof(tss);
}

extern void loadtss(void); /* loadtss.asm */

void tss_load(void)
{
    SAVE_INTERRUPT_STATE;
    loadtss();
    RESTORE_INTERRUPT_STATE;
}

uint32_t get_tss_base(void)
{
    return (uint32_t)&tss;
}

size_t get_tss_size(void)
{
    return sizeof(tss);
}

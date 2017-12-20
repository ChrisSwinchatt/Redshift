/**
 * \file boot/boot.c
 * \brief Boot the kernel.
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
#include <boot/cpuinfo.h>
#include <boot/gdt.h>
#include <boot/idt.h>
#include <boot/multiboot.h>
#include <boot/pic.h>
#include <boot/pit.h>
#include <boot/sched.h>
#include <boot/sysinfo.h>
#include <boot/tss.h>
#include <kernel/asm.h>
#include <kernel/console.h>
#include <kernel/redshift.h>
#include <kernel/sleep.h>
#include <kernel/symbols.h>
#include <kernel/timer.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/static.h>

static void splash(void)
{
    static const char* splash_text = " ________________________\n"
                                     "|   ____ |  ______|  ___ \\   __      ___  __ ___\n"
                                     "|      __|  |_____| |   | | |_  |__|  |  |__  |\n"
                                     "|  |\\  \\ |  |_____| |___| | __| |  | _|_ |    |\n"
                                     "|__| \\__\\|________|______/\n";
    console.screen.foreground = CONSOLE_COLOR_RED;
    console_writestring(splash_text);
    console.screen.foreground = CONSOLE_COLOR_LIGHT_GRAY;
}

static void check_boot_env(uint32_t magic, uint32_t tags)
{
    printk("Checking boot environment...\n");
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("unsupported bootloader: bad magic number (expected 0x%08lX, got 0x%08lX).",
              (uint32_t)MULTIBOOT_BOOTLOADER_MAGIC,
              magic);
    }
    if (tags == 0) {
        panic("bootloader has not passed system info");
    }
}

static void init_interrupt_system(void)
{
    printk("Initialising interrupt system...\n");
    /* Note: gdt_init writes the TSS' GDT entry and loads the GDT, so we create (but don't load) a valid TSS first.
     */
    /*tss_init(); XXX */
    gdt_init();
    printk(PRINTK_DEBUG " * Loaded GDT\n");
    idt_init();
    printk(PRINTK_DEBUG " * Loaded IDT\n");
    /*tss_load();
    printk(PRINTK_DEBUG " * Loaded TSS\n"); XXX */
    pic_init();
    printk(PRINTK_DEBUG " * Initialised PIC\n");
    pit_init(TICK_RATE);
    printk(PRINTK_DEBUG " * Initialised PIT\n");
}

static void get_sysinfo_1(struct multiboot_tag* tags)
{
    /* We can't initialise the memory allocator without knowing some system information (how much memory and where we
     * can start storing data) but we can't save some system information (memory map, where modules are located) without
     * a memory allocator. To get around this, the information gathering has to be done in multiple steps.
     */
    printk("Gathering system info (1)...\n");
    sysinfo_init_1(&__sysinfo__, tags);
}

static void init_memory_1(void)
{
    printk("Initialising memory management (1)...\n");
    static_init();
    printk(PRINTK_DEBUG " * Initialised static allocator\n");
    paging_init(__sysinfo__.mem_lower + __sysinfo__.mem_upper);
    printk(PRINTK_DEBUG " * Initialised page allocator\n");
}

static void get_sysinfo_2(struct multiboot_tag* tags)
{
    printk("Gathering system info (2)...\n");
    sysinfo_init_2(&__sysinfo__, tags);
}

static void init_memory_2(void)
{
    printk("Initialising memory management (2)...\n");
    heap_init();
    printk(PRINTK_DEBUG " * Intialised heap allocator\n");
}

static void init_devices(void)
{
    /* TODO
     */
}

static void start_scheduler(void)
{
    printk("Starting scheduler...\n");
    sched_init();
    printk(PRINTK_DEBUG " * Started scheduler\n");
}

static void enable_interrupts(void)
{
    int_enable();
    printk(PRINTK_DEBUG " * Enabled interrupts\n");
}

void boot(uint32_t magic, uint32_t tags)
{
    int_disable();
    console_init();
    splash();
    check_boot_env(magic, tags);
    init_interrupt_system();
    get_sysinfo_1((struct multiboot_tag*)tags);
    init_memory_1();
    get_sysinfo_2((struct multiboot_tag*)tags);
    init_memory_2();
    init_devices();
    //start_scheduler();
    enable_interrupts();
    while (true)
        ;
}

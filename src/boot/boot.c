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
#include <boot/boot_module.h>
#include <boot/gdt.h>
#include <boot/idt.h>
#include <boot/multiboot.h>
#include <boot/pic.h>
#include <boot/pit.h>
#include <boot/sched.h>
#include <boot/tss.h>
#include <hal/cpu.h>
#include <hal/memory.h>
#include <kernel/asm.h>
#include <kernel/console.h>
#include <kernel/initrd.h>
#include <kernel/redshift.h>
#include <kernel/sleep.h>
#include <kernel/symbols.h>
#include <kernel/timer.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/static.h>
#include <sched/process.h>

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
    printk(PRINTK_DEBUG "Checking boot environment...\n");
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
    printk(PRINTK_DEBUG "Initialising interrupt system...\n");
    /* FIXME gdt_init writes the TSS' GDT entry and loads the GDT, so we create (but don't load) a valid TSS first.
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

static void init_boot_modules_1(struct multiboot_tag* tags)
{
    printk(PRINTK_DEBUG "Discovering boot modules...\n");
    discover_boot_modules(tags);
}

static void init_hal(struct multiboot_tag* tags)
{
    printk(PRINTK_DEBUG "Initialising hardware abstraction layer\n");
    cpu_init();
    printk(PRINTK_DEBUG " * Initialised CPU\n");
    memory_init(tags);
}

static void init_memory(struct multiboot_tag* tags)
{

    printk("Initialising memory manager...\n");
    static_init();
    printk(PRINTK_DEBUG " * Initialised static allocator\n");
    paging_init(memory_size_total());
    printk(PRINTK_DEBUG " * Initialised page allocator\n");
    memory_map_init(tags);
    printk(PRINTK_DEBUG " * Parsed memory map\n");
    heap_init();
    printk(PRINTK_DEBUG " * Intialised heap allocator\n");
}

static void init_boot_modules_2(struct multiboot_tag* tags)
{
    printk(PRINTK_DEBUG "Processing boot modules...\n");
    save_boot_modules(tags);
}

static void load_initrd(void)
{
    /* Initial ramdisk should always be the first boot module.
     */
    printk(PRINTK_DEBUG "Loading initial ramdisk...\n");
    const struct boot_module* module = boot_modules_head();
    initrd_init((const char*)module->start, module->end - module->start);
    printk(PRINTK_DEBUG " * Loaded initial ramdisk\n");
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

static void __noreturn handle_events(void)
{
    while (true) {
        ;
    }
}

void boot(uint32_t magic, uint32_t tags)
{
    struct multiboot_tag* mb_tags = (struct multiboot_tag*)tags;
    int_disable();
    console_init();
    splash();
    check_boot_env(magic, tags);
    init_interrupt_system();
    init_hal(mb_tags);
    init_boot_modules_1(mb_tags);
    init_memory(mb_tags);
    init_boot_modules_2(mb_tags);
    load_initrd();
    init_devices();
    start_scheduler();
    enable_interrupts();
    handle_events();
}

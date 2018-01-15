/**
 * \file boot/boot.c
 * Boot the kernel.
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
#include <redshift/boot/boot_module.h>
#include <redshift/boot/gdt.h>
#include <redshift/boot/idt.h>
#include <redshift/boot/multiboot.h>
#include <redshift/boot/pic.h>
#include <redshift/boot/pit.h>
#include <redshift/boot/sequence.h>
#include <redshift/boot/sched.h>
#include <redshift/boot/tss.h>
#include <redshift/hal/cpu.h>
#include <redshift/hal/memory.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel/initrd.h>
#include <redshift/kernel.h>
#include <redshift/kernel/sleep.h>
#include <redshift/kernel/symbols.h>
#include <redshift/kernel/timer.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/paging.h>
#include <redshift/mem/static.h>
#include <redshift/sched/process.h>

static struct multiboot_tag* mb_tags;

extern uint32_t __multiboot_bootloader_magic__; /* boot/_start.asm */
extern uint32_t __multiboot_bootloader_tags__;  /* boot/_start.asm */

static void __init(BOOT_SEQUNECE_INIT_CONSOLE) init_console(void)
{
    console_init();
    console_clear();
}

static void __init(BOOT_SEQUENCE_SPLASH) splash(void)
{
    static const char* splash_text[] = {
        "                          _   __  _   _      ___  __ ___",
        "                         |_| |__ | \\ |_  |_|  |  |__  |",
        "                         | \\ |__ |_/  _| | | _|_ |    |"
    };
    static const size_t splash_lines = ARRAY_SIZE(splash_text);
    const console_color_t foreground = console_get_foreground_color();
    console_set_foreground_color(CONSOLE_COLOR_LIGHT_RED);
    console_set_origin(0, 0);
    for (size_t i = 0; i < splash_lines; ++i) {
        console_write_line(splash_text[i]);
    }
    console_set_foreground_color(foreground);
    console_set_origin(0, splash_lines);
}

#define TYPE_LIST(F)\
    F(int8_t,    1)\
    F(int16_t,   2)\
    F(int32_t,   4)\
    F(int64_t,   8)\
    F(uint8_t,   1)\
    F(uint16_t,  2)\
    F(uint32_t,  4)\
    F(uint64_t,  8)\
    F(uintptr_t, 4)\
    F(size_t,    4)\
    F(void*,     4)

static void __init(BOOT_SEQUENCE_CHECK_BOOT_ENV) check_boot_env(void)
{
    printk(PRINTK_INFO "Checking boot environment\n");
    /* Check type sizes.
     */
#define CHECK_SIZE(T, S) RUNTIME_CHECK(sizeof(T) == S);
    TYPE_LIST(CHECK_SIZE)
#undef CHECK_SIZE
    /* Check bootloader info.
     */
    if (__multiboot_bootloader_magic__ != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("unsupported bootloader: bad magic number (expected 0x%08lX, got 0x%08lX)",
              (uint32_t)MULTIBOOT_BOOTLOADER_MAGIC,
              __multiboot_bootloader_magic__);
    }
    if (__multiboot_bootloader_tags__ == 0) {
        panic("bootloader has not passed system info");
    }
    mb_tags = (struct multiboot_tag*)__multiboot_bootloader_tags__;
}

#undef TYPE_LIST

static void __init(BOOT_SEQUENCE_INIT_INTERRUPT_SYSTEM) init_interrupt_system(void)
{
    printk(PRINTK_INFO "Initialising interrupt system\n");
    tss_init();
    printk(PRINTK_DEBUG "Loading GDT\n");
    gdt_init();
    printk(PRINTK_DEBUG "Loading IDT\n");
    idt_init();
    printk(PRINTK_DEBUG "Loading TSS\n");
    tss_load();
    printk(PRINTK_DEBUG "Initialising PIC\n");
    pic_init();
    printk(PRINTK_DEBUG "Initialising PIT\n");
    pit_init(TICK_RATE);
}

static void __init(BOOT_SEQUENCE_INIT_BOOT_MODULES_1) init_boot_modules_1(void)
{
    printk(PRINTK_INFO "Discovering boot modules\n");
    discover_boot_modules(mb_tags);
}

static void __init(BOOT_SEQUENCE_INIT_HAL) init_hal(void)
{
    printk(PRINTK_INFO "Initialising hardware abstraction layer\n");
    cpu_init();
    memory_init(mb_tags);
}

static void __init(BOOT_SEQUENCE_INIT_MEMORY) init_memory(void)
{

    printk(PRINTK_INFO "Initialising memory manager\n");
    printk(PRINTK_DEBUG "Initialising static allocator\n");
    static_init();
    printk(PRINTK_DEBUG "Initialising page allocator\n");
    paging_init(memory_size_total());
    memory_map_init(mb_tags);
    printk(PRINTK_DEBUG "Intialising heap allocator\n");
    heap_init();
}

static void __init(BOOT_SEQUENCE_INIT_BOOT_MODULES_2) init_boot_modules_2(void)
{
    printk(PRINTK_INFO "Processing boot modules\n");
    save_boot_modules(mb_tags);
}

static void __init(BOOT_SEQUENCE_LOAD_INITRD) load_initrd(void)
{
    /* Initial ramdisk should always be the first boot module.
     */
    printk(PRINTK_INFO "Loading initial ramdisk\n");
    if (boot_modules_count() < 1) {
        panic("no initial ramdisk (must be passed as first boot module)");
    }
    const struct boot_module* module = boot_modules_head();
    initrd_init((const char*)(module->start), module->end - module->start);
}

static void __init(BOOT_SEQUENCE_LOAD_SYMBOL_TABLE) init_symbol_table(void)
{
    printk(PRINTK_INFO "Loading symbol table\n");
    const struct initrd_file* symtab = initrd_get_file_by_name("boot/redshift.map");
    if (symtab == NULL) {
        panic("initial ramdisk does not contain the symbol table");
    }
    load_symbol_table(symtab->start, symtab->size);
}

static void __init(BOOT_SEQUENCE_INIT_DEVICES) init_devices(void)
{
    /* TODO
     */
}

static void __init(BOOT_SEQUENCE_START_SCHEDULER) start_scheduler(void)
{
    printk(PRINTK_INFO "Starting scheduler\n");
    sched_init();
}

void boot(void)
{
    disable_interrupts();
    console_init();
    console_clear();
    splash();
    check_boot_env();
    init_interrupt_system();
    init_hal();
    init_boot_modules_1();
    init_memory();
    init_boot_modules_2();
    load_initrd();
    init_symbol_table();
    init_devices();
    start_scheduler();
    enable_interrupts();
    process_yield();
    UNREACHABLE("%s should not return!", __func__);
}

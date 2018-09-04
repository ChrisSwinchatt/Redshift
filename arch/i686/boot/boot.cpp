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
#include <redshift/boot/boot_module.hpp>
#include <redshift/boot/multiboot2.hpp>
#include <redshift/boot/sched.hpp>
#include <redshift/hal/cpu.hpp>
#include <redshift/hal/memory.hpp>
#include <redshift/hal/timer.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/console.hpp>
#include <redshift/kernel/initrd.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/sleep.hpp>
#include <redshift/kernel/symbols.hpp>
#include <redshift/sched/process.hpp>

extern "C" uint32_t __multiboot2_bootloader_magic__; // boot/_start.asm
extern "C" uint32_t __multiboot2_bootloader_tags__;  // boot/_start.asm

namespace redshift { namespace boot {
    static struct multiboot2_tag* mb_tags;
    static void init_console()
    {
        console_init();
        console_clear();
    }

    static void splash()
    {
        static const char* splash_text[] = {
            "                          _   __  _   _      ___  __ ___",
            "                         |_| |__ | \\ |_  |_|  |  |__  |",
            "                         | \\ |__ |_/  _| | | _|_ |    |"
        };
        static const size_t splash_lines = array_size(splash_text);
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

    static void check_boot_env()
    {
        printk(PRINTK_INFO "Checking boot environment\n");
        // Check type sizes.
#define CHECK_SIZE(T, S) RUNTIME_CHECK(sizeof(T) == S);
        TYPE_LIST(CHECK_SIZE)
#undef CHECK_SIZE
        // Check bootloader info.
        if (__multiboot2_bootloader_magic__ != MULTIBOOT2_BOOTLOADER_MAGIC) {
            panic("unsupported bootloader: bad magic number (expected 0x%08lX, got 0x%08lX)",
                  (uint32_t)MULTIBOOT2_BOOTLOADER_MAGIC,
                  __multiboot2_bootloader_magic__);
        }
        if (__multiboot2_bootloader_tags__ == 0) {
            panic("bootloader has not passed system info");
        }
        mb_tags = (struct multiboot2_tag*)__multiboot2_bootloader_tags__;
    }

#undef TYPE_LIST

    static void init_boot_modules_1()
    {
        printk(PRINTK_INFO "Discovering boot modules\n");
        discover_boot_modules(mb_tags);
    }

    static void init_hal()
    {
        printk(PRINTK_INFO "Initialising hardware abstraction layer\n");
        hal::cpu::init();
        hal::memory::init(mb_tags);
        hal::timer::init();
    }

    static void init_boot_modules_2()
    {
        printk(PRINTK_INFO "Processing boot modules\n");
        save_boot_modules(mb_tags);
    }

    static void load_initrd()
    {
        // Initial ramdisk should always be the first boot module.

        printk(PRINTK_INFO "Loading initial ramdisk\n");
        if (boot_modules_count() < 1) {
            panic("no initial ramdisk (must be passed as first boot module)");
        }
        const struct boot_module* module = boot_modules_head();
        initrd_init((const char*)(module->start), module->end - module->start);
    }

    static void init_symbol_table()
    {
        printk(PRINTK_INFO "Loading symbol table\n");
        const struct initrd_file* symtab = initrd_get_file_by_name("boot/redshift.map");
        if (symtab == nullptr) {
            panic("initial ramdisk does not contain the symbol table");
        }
        load_symbol_table(symtab->start, symtab->size);
    }

    static void init_devices()
    {
        // TODO

    }

    static void start_scheduler()
    {
        printk(PRINTK_INFO "Starting scheduler\n");
        sched_init();
    }

    extern "C" void kernel_early_main()
    {
        init_console();
        splash();
        check_boot_env();
        init_hal();
        init_boot_modules_1();
        init_boot_modules_2();
        load_initrd();
        init_symbol_table();
        init_devices();
        start_scheduler();
        enable_interrupts();
        process_yield();
        UNREACHABLE("%s should not return!", __func__);
    }
}} // redshift::boot

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
#ifndef REDSHIFT_KERNEL_H
#define REDSHIFT_KERNEL_H

#include <libk/kassert.h>
#include <libk/kmacro.h>
#include <libk/ktypes.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/kmalloc.h>
#include <redshift/kernel/panic.h>
#include <redshift/kernel/printk.h>

/**
 * Save interrupt state and disable interrupts. Can only be done once per function, ideally before doing anything else.
 */
#define SAVE_INTERRUPT_STATE        int __saved_interrupt_state__ = get_interrupt_state(); disable_interrupts()

/** Restore previous interrupt state. Can only be done after SAVE_INTERRUPT_STATE. */
#define RESTORE_INTERRUPT_STATE     do { if (__saved_interrupt_state__) { enable_interrupts(); } } while (0)

/** Stack size. */
#define STACK_SIZE (size_t)((uintptr_t)__stack_top__ - (uintptr_t)__stack_bottom__)

extern symbol_t __kernel_start__; /**< Start of kernel (linker script). */
extern symbol_t __code_start__;   /**< Start of code section (linker script).           */
extern symbol_t __code_end__;     /**< End of code section (linker script).             */
extern symbol_t __data_start__;   /**< Start of data section (linker script).           */
extern symbol_t __data_end__;     /**< End of data section (linker script).             */
extern symbol_t __rodata_start__; /**< Start of read-only data section (linker script). */
extern symbol_t __rodata_end__;   /**< End of read-only data section (linker script).   */
extern symbol_t __bss_start__;    /**< Start of .bss section (linker script).           */
extern symbol_t __bss_end__;      /**< End of .bss section (linker script).             */
extern symbol_t __kernel_end__;   /**< End of executable (linker script).               */
extern symbol_t __stack_bottom__; /**< Bottom of stack (_start.asm).                    */
extern symbol_t __stack_top__;    /**< Top of stack (_start.asm).                       */

enum {
    /** Timer tick rate. */
    TICK_RATE = 100
};

#endif /* ! REDSHIFT_KERNEL_H */

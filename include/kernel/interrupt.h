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
#ifndef REDSHIFT_KERNEL_ISR_H
#define REDSHIFT_KERNEL_ISR_H

#include <kernel/cpu_state.h>
#include <kernel/redshift.h>

#define  IRQ0 32
#define  IRQ1 33
#define  IRQ2 34
#define  IRQ3 35
#define  IRQ4 36
#define  IRQ5 37
#define  IRQ6 38
#define  IRQ7 39
#define  IRQ8 40
#define  IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/** ISR handler function */
typedef void(* isr_handler_t)(const struct cpu_state*);

/**
 * \brief Create an ISR handler
 * \param n ISR number
 * \param fp Function pointer
 */
void set_interrupt_handler(uint8_t n, isr_handler_t fp);

#endif /* ! REDSHIFT_KERNEL_ISR_H */

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
#ifndef REDSHIFT_KERNEL_ASM_HPP
#define REDSHIFT_KERNEL_ASM_HPP

#include <libk/types.hpp>
#include <libk/macro.hpp>

/// Disables interrupts.
void disable_interrupts();

/// Enables interrupts.
void enable_interrupts();

/// Waits until the next interrupt.
/// NB: If called after disable_interrupts, the CPU will never wake up.
void wait_for_interrupt();

/// Get the current interrupt state.
/// \return 1 if interrupts are enabled, otherwise 0.
int get_interrupt_state();

/// Hang the computer forever.
extern void __noreturn hang();

/// 64-bit unsigned division algorithm.
/// \param n The numerator.
/// \param d The denominator.
/// \return The result of division is returned.
extern uint64_t udiv64(uint64_t n, uint64_t d);

/// 64-bit signed division algorithm.
/// \param n The numerator.
/// \param d The denominator.
/// \return The result of division is returned.
extern int64_t idiv64(int64_t n, int64_t d);

#endif // ! REDSHIFT_KERNEL_ASM_HPP

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
#ifndef REDSHIFT_KERNEL_PANIC_HPP
#define REDSHIFT_KERNEL_PANIC_HPP

#include <libk/macro.hpp>
#include <libk/variadic.hpp>
#include <redshift/hal/cpu.hpp>

namespace redshift { namespace kernel {
    using redshift::hal::cpu;

    /// Print an error message and then stop execution.
    /// \param fmt A formatted error message.
    /// \param ... Format arguments.
    extern "C" void __noreturn panic(const char* fmt, ...);

    /// Print an error message and then stop execution.
    /// \param fmt A formatted error message.
    /// \param args Format arguments.
    void __noreturn vpanic(const char* fmt, va_list args);

    /// Print an error message and CPU state, then cease execution.
    /// \param state The CPU state.
    /// \param fmt A formatted error message.
    /// \param ... Format arguments.
    void __noreturn panic_with_state(const cpu::state* state, const char* fmt, ...);

    /// Print an error message and CPU state, then cease execution.
    /// \param state The CPU state.
    /// \param fmt A formatted error message.
    /// \param ... Format arguments.
    void __noreturn vpanic_with_state(const cpu::state* state, const char* fmt, va_list args);
}} // redshift::kernel

#endif // ! REDSHIFT_KERNEL_PANIC_HPP

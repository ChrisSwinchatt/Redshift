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
#include <libk/char.hpp>
#include <libk/asciz.hpp>
#include <redshift/debug/dump_registers.hpp>
#include <redshift/debug/dump_stack.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/console.hpp>
#include <redshift/kernel/power.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/panic.hpp>
#include <redshift/kernel/printk.hpp>
#include <redshift/kernel/sleep.hpp>
#include <redshift/mem/static.hpp>

static bool in_panic = false;

static void vpanic_common_start(const char* fmt, va_list ap)
{
    disable_interrupts();
    if (in_panic) {
        hang();
    }
    in_panic = true;
    disable_interrupts();
    const size_t fmt_len = asciz::length(fmt);
    const size_t size    = array_size(PRINTK_ERROR) + fmt_len;
    char* buffer         = static_alloc(size + 1);
    libk::memory::zero(buffer, size + 1);
    asciz::copy(buffer, PRINTK_ERROR, size);
    asciz::concat(buffer, fmt, fmt_len);
    printk(PRINTK_ERROR "\n\aKernel panic - ");
    vprintk(buffer, ap);
}

static void __noreturn vpanic_common_end()
{
    printk(PRINTK_ERROR "\nStack trace:\n");
    dump_stack();
    printk(PRINTK_ERROR "\n\aSystem halted.");
    hang();
}

void __noreturn vpanic(const char* fmt, va_list args)
{
    vpanic_common_start(fmt, args);
    vpanic_common_end();
}

void __noreturn vpanic_with_state(const cpu::state* state, const char* fmt, va_list args)
{
    vpanic_common_start(fmt, args);
    printk(PRINTK_ERROR "\nCPU state:\n");
    dump_registers(state);
    vpanic_common_end();
}

void __noreturn panic(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vpanic(fmt, ap);
    va_end(ap);
    UNREACHABLE("panic", nullptr);
}

void __noreturn panic_with_state(const cpu::state* state, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vpanic_with_state(state, fmt, ap);
    va_end(ap);
    UNREACHABLE("panic", nullptr);
}

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
#include <libk/kchar.h>
#include <libk/kstring.h>
#include <redshift/debug/dump_registers.h>
#include <redshift/debug/dump_stack.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel/power.h>
#include <redshift/kernel.h>
#include <redshift/kernel/panic.h>
#include <redshift/kernel/printk.h>
#include <redshift/kernel/sleep.h>
#include <redshift/mem/static.h>

static bool in_panic = false;

static void vpanic_common_start(const char* fmt, va_list ap)
{
    if (in_panic) {
        hang();
    }
    in_panic = true;
    disable_interrupts();
    const size_t fmt_len = kstring_length(fmt);
    const size_t size    = ARRAY_SIZE(PRINTK_ERROR) + fmt_len;
    char* buffer         = static_alloc(size + 1);
    kmemory_zero(buffer, size + 1);
    kstring_copy(buffer, PRINTK_ERROR, size);
    kstring_concatenate(buffer, fmt, fmt_len);
    printk(PRINTK_ERROR "\n\aKernel panic - ");
    vprintk(buffer, ap);
}

static void vpanic_common_end(void)
{
    printk(PRINTK_ERROR "\nStack trace:\n");
    dump_stack();
    printk(PRINTK_ERROR "\n\aSystem halted.");
}

static void vpanic(const char* fmt, va_list ap)
{
    vpanic_common_start(fmt, ap);
    vpanic_common_end();
}

static void vpanic_with_state(const struct cpu_state* state, const char* fmt, va_list ap)
{
    vpanic_common_start(fmt, ap);
    printk(PRINTK_ERROR "\n\nCPU state:\n");
    dump_registers(state);
    vpanic_common_end();
}

void __noreturn panic(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vpanic(fmt, ap);
    va_end(ap);
    hang();
}

void __noreturn panic_with_state(const struct cpu_state* state, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vpanic_with_state(state, fmt, ap);
    va_end(ap);
    hang();
}

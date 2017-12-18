/* Copyright (c) 2012 Chris Swinchatt.
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
#include <ctype.h>
#include <debug/dump_registers.h>
#include <debug/dump_stack.h>
#include <kernel/asm.h>
#include <kernel/console.h>
#include <kernel/power.h>
#include <kernel/redshift.h>
#include <kernel/sleep.h>
#include <string.h>

void __noreturn panic(const char* fmt, ...)
{
    char buffer[strlen(PRINTK_ERROR) + strlen(fmt) + 1];
    int_disable();
    memset(buffer,  0,   arraysize(buffer));
    strncat(buffer, fmt, strlen(fmt));
    printk("Kernel panic - ");
    {
        va_list ap;
        va_start(ap, fmt);
        vprintk(buffer, ap);
        va_end(ap);
    }
    printk("\nRegister dump:\n");
    {
        struct cpu_state state;
        get_cpu_state(&state);
        dump_registers(&state);
    }
    printk("\nStack trace:\n");
    dump_stack();
    printk("\nSystem will hang.");
    hang();
}

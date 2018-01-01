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
#include <ctype.h>
#include <redshift/debug/dump_registers.h>
#include <redshift/debug/dump_stack.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel/power.h>
#include <redshift/kernel.h>
#include <redshift/kernel/panic.h>
#include <redshift/kernel/printk.h>
#include <redshift/kernel/sleep.h>
#include <string.h>

void __noreturn panic(const char* fmt, ...)
{
    struct cpu_state state;
    get_cpu_state(&state);
    char buffer[strlen(PRINTK_ERROR) + strlen(fmt) + 1];
    int_disable();
    memset(buffer,  0,            ARRAY_SIZE(buffer));
    strncpy(buffer, PRINTK_ERROR, ARRAY_SIZE(buffer));
    strncat(buffer, fmt,          strlen(fmt));
    printk(PRINTK_ERROR "Kernel panic - ");
    {
        va_list ap;
        va_start(ap, fmt);
        vprintk(buffer, ap);
        va_end(ap);
    }
    printk(PRINTK_ERROR "\nStack trace:\n");
    dump_stack();
    printk(PRINTK_ERROR "\nSystem will hang.");
    hang();
}

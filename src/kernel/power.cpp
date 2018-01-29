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
#include <redshift/kernel/asm.h>
#include <redshift/kernel/power.h>
#include <redshift/kernel.h>

#define KBD_KDATA_BIT        0
#define KBD_UDATA_BIT        1

void __noreturn reboot(void)
{
    uint8_t tmp;
    disable_interrupts();
    do {
        tmp = io_inb(KEYBOARD_CMND);
        if (TEST_FLAG(tmp, KBD_KDATA_BIT) != 0)
            io_inb(KEYBOARD_DATA);
    } while (TEST_FLAG(tmp, KBD_UDATA_BIT) != 0);
    io_outb(KEYBOARD_CMND, KEYBOARD_RESET);
    printk(PRINTK_ERROR "Rebooting");
    hang();
}

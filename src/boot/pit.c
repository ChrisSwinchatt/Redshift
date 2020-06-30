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
#include <redshift/boot/pit.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/interrupt.h>
#include <redshift/kernel/timer.h>
#include <redshift/sched/process.h>

static uint32_t _tick_rate;

static void pit_handler(const struct cpu_state* regs)
{
    process_timer_queue(1000 / _tick_rate, (void*)regs);
}

int pit_init(uint32_t freq)
{
    SAVE_INTERRUPT_STATE;
    static int handler_registered = 0;
    if (!(handler_registered)) {
        set_interrupt_handler(IRQ0, &pit_handler);
        handler_registered = 1;
    }
    if (!(freq)) {
        return -1;
    }
    uint32_t div = 1193180 / freq;
    io_outb(PIT_CMND, 0x36);
    io_outb(PIT_DATA, ((uint8_t)(div & 0xff)));
    io_outb(PIT_DATA, ((uint8_t)((div >> 8) & 0xff)));
    _tick_rate = freq;
    RESTORE_INTERRUPT_STATE;
    return 0;
}

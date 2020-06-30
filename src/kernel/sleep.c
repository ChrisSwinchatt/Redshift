/**
 * \file kernel/sleep.c
 * Put the calling CPU to sleep.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <redshift/kernel.h>
#include <redshift/kernel/sleep.h>

void sleep(uint32_t sec)
{
    msleep((uint64_t)sec * 1000ULL);
}

void msleep(uint64_t msec)
{
    usleep(msec * 1000ULL);
}

void usleep(uint64_t usec)
{
    SAVE_INTERRUPT_STATE;
    uint64_t ticks = 0;
    for (ticks = usec*(uint64_t)TICK_RATE; ticks > 0ULL; ticks -= 1000000ULL) {
        enable_interrupts();
        wait_for_interrupt();
        disable_interrupts();
        if (ticks > 0ULL && ticks < 1000000ULL) {
            /* Prevent ticks underflowing.
             */
            ticks = 1000000ULL;
        }
    }
    RESTORE_INTERRUPT_STATE;
}

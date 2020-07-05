/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#ifndef REDSHIFT_KERNEL_SLEEP_H
#define REDSHIFT_KERNEL_SLEEP_H

#include <redshift/kernel.h>

/**
 * Sleep.
 * \param ticks The number of scheduler ticks to sleep for.
 * \note The calling thread will be blocked until at least 'ticks' ticks have passed.
 * \example sleep(TICKS_PER_SEC) // sleep 1 second.
 */
void sleep(uintmax_t ticks);

#endif /* ! REDSHIFT_KERNEL_SLEEP_H */

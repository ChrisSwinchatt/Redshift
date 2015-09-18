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
#ifndef REDSHIFT_KERNEL_TIMER_H
#define REDSHIFT_KERNEL_TIMER_H

/**
 * \brief Calls a function periodically.
 * \param period The period of time to allow in between function calls (ms).
 * \param callback A pointer to a function to call when the time period elapses.
 * \param arg A pointer to an argument to pass to the callback function.
 */
void add_timer_event(uint32_t period, void(* callback)(void*), void* arg);

/**
 * \brief Processes the timer queue.
 * \param elapsed_time The time elapsed since the last time the function was
 * called (ms).
 */
void process_timer_queue(uint32_t elapsed_time);

#endif /* ! REDSHIFT_KERNEL_TIMER_H */

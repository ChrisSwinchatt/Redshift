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
#ifndef REDSHIFT_SCHED_PROCESS_H
#define REDSHIFT_SCHED_PROCESS_H

#include <redshift/kernel.h>
#include <redshift/kernel/interrupt.h>
#include <redshift/mem/paging.h>

#define MAX_PROCESS_PRIORITY 15

/**
 * Spawns a new process.
 * \param entry The entry point of the process.
 * \param page_dir The page directory.
 * \param priority The process priority (0..MAX_PROCESS_PRIORITY).
 * \return The process ID is returned.
 */
int process_spawn(uint32_t entry, struct page_directory* page_dir, uint8_t priority);

/**
 * Switches to the next process.
 * \param ignored Parameter ignored..
 */
void process_switch(void* ignored);

#endif /* ! REDSHIFT_SCHED_PROCESS_H */

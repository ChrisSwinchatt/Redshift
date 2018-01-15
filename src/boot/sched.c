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
#include <redshift/boot/sched.h>
#include <redshift/hal/cpu.h>
#include <redshift/kernel.h>
#include <redshift/kernel/timer.h>
#include <redshift/sched/idle.h>
#include <redshift/sched/process.h>

extern struct cpu_state cpu_state; /* cpu_state.asm */

extern void kernel_main(void);

int sched_init(void)
{
    disable_interrupts();
    /* Start main process, which uses the main kernel stack.
     */
    int main_id = process_spawn(
        (uintptr_t)kernel_main,
        kernel_directory,
        PROCESS_PRIORITY_LOW,
        0,
        STACK_SIZE,
        PROCESS_FLAGS_SUPERVISOR
    );
    if (main_id < 0) {
        panic("unable to spawn main process");
    }
    /* Start idle process, which gets its own stack allocated by process_spawn.
     */
    int idle_id = process_spawn(
        (uintptr_t)idle,
        kernel_directory,
        PROCESS_PRIORITY_MIN,
        0,
        STACK_SIZE,
        PROCESS_FLAGS_SUPERVISOR
    );
    if (idle_id < 0) {
        panic("unable to spawn idle process");
    }
    /* Add timer event and enable interrupts.
     */
    add_timer_event("sched", SCHED_PERIOD, process_switch, &cpu_state);
    enable_interrupts();
    return 0;
}

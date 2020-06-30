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
#include <libk/kstring.h>
#include <libk/kmemory.h>
#include <redshift/hal/cpu/state.h>
#include <redshift/kernel.h>
#include <redshift/mem/heap.h>
#include <redshift/mem/paging.h>
#include <redshift/sched/process.h>

/**
 * Process table entry.
 */
struct process {
    int                    id;         /** Process ID.                                  */
    bool                   blocked;    /** Whether the process is blocked e.g. for I/O. */
    struct page_directory* page_dir;   /** Process' page directory.                     */
    struct cpu_state       state;      /** Process register state.                      */
    uint8_t*               stack;      /** Process stack (bottom).                      */
    size_t                 stack_size; /** Stack size.                                  */
    process_flags_t        flags;      /** Process flags.                               */
    struct process*        next;       /** Next process in the queue.                   */
};

/**
 * Process queue.
 */
static struct process_queue {
    struct process* last;
} processes[PROCESS_PRIORITY_MAX + 1];

/** The currently executing process. */
static struct process* current_process;

static uint32_t num_processes;

int process_spawn(
    uintptr_t              entry_point,
    struct page_directory* page_dir,
    process_priority_t     priority,
    uintptr_t              stack_addr,
    size_t                 stack_size,
    process_flags_t        flags)
{
    PUSH_INTERRUPT_STATE(0);
    DEBUG_ASSERT(entry_point > 0);
    DEBUG_ASSERT(stack_size > 0);
    if (priority > PROCESS_PRIORITY_MAX) {
        printk(PRINTK_ERROR "Invalid process priority: %u\n", priority);
        return -1;
    }
    struct process_queue* queue = &(processes[priority]);
    /* Create the new process.
     */
    struct process* process = kmalloc(sizeof(*process));
    if (!(process)) {
        panic("failed to create process: out of memory");
    }
    kmemory_fill8(process, 0, sizeof(*process));
    kmemory_fill8(&(process->state), 0, sizeof(process->state));
    process->id       = num_processes++;
    process->blocked  = false;
    process->page_dir = page_dir;
    process->flags    = flags;
    /* Set up the process' stack.
     */
    if (stack_addr == 0) {
        process->stack = kmalloc(stack_size);
        if (!(process->stack)) {
            panic("failed to create stack: out of memory");
        }
    } else {
        process->stack = (uint8_t*)stack_addr;
    }
    process->stack_size = stack_size;
    /* Set up process registers.
     */
    if (TEST_FLAG(process->flags, PROCESS_FLAGS_SUPERVISOR)) {
        process->state.cs = 0x08;
        process->state.ds = 0x10;
        process->state.es = 0x10;
        process->state.fs = 0x10;
        process->state.gs = 0x10;
        process->state.ss = 0x10;
    } else {
        process->state.cs = 0x1B;
        process->state.ds = 0x23;
        process->state.es = 0x23;
        process->state.fs = 0x23;
        process->state.gs = 0x23;
        process->state.ss = 0x23;
    }
    process->state.eip = entry_point;
    process->state.esp = (uintptr_t)process->stack + process->stack_size; /* Top of the stack. */
    /* Add the process to its queue.
     */
    if (queue->last == NULL) {
        /* Initialise the queue with the process.
         */
        queue->last     = process;
        process->next   = process;
        current_process = process;
    } else {
        /* Add the process after whatever executed last, then set it to be last in the queue.
         */
        process->next     = queue->last->next;
        queue->last->next = process;
        queue->last       = process;
    }
    printk(PRINTK_DEBUG "Spawned process: <id=%d,priority=%d,entry_point=0x%08lX>\n", process->id, priority, entry_point);
    POP_INTERRUPT_STATE();
    return process->id;
}

extern void __noreturn set_state_and_jump(const struct cpu_state* regs);

static void __noreturn switch_to(struct process* process)
{
    printk(PRINTK_DEBUG "Switching process: <id=%d,eip=0x%08lX>\n", process->id, process->state.eip);
    page_directory_load(process->page_dir);
    set_state_and_jump(&(process->state));
}

void __non_reentrant process_switch(void* regs)
{
    PUSH_INTERRUPT_STATE(0);
    /* Update the register state of the process we just switched from.
     */
    if (regs != NULL && current_process != NULL) {
        kmemory_copy(&(current_process->state), regs, sizeof(current_process->state));
    }
    /* Select a process queue, starting at the highest.
     */
    for (int priority = (int)PROCESS_PRIORITY_MAX; priority >= 0; --priority) {
        struct process_queue* queue = &(processes[priority]);
        struct process* process = queue->last;
        if (process == NULL) {
            /* Queue is empty - skip.
             */
            continue;
        }
        /* Go through the queue until we find a process which isn't blocked, then switch to it. If we get back to
         * queue->last without switching, move on to the next queue.
         */
        do {
            process = process->next;
            if (!(process->blocked)) {
                queue->last     = process;
                current_process = process;
                switch_to(process);
                UNREACHABLE("switch to process %d returned", process->id);
            }
        } while (process != queue->last);
    }
    POP_INTERRUPT_STATE();
}

const struct process* __non_reentrant get_current_process(void)
{
    return current_process;
}

int get_process_id(const struct process* process)
{
    return process->id;
}

int __non_reentrant get_current_process_id(void)
{
    return current_process->id;
}

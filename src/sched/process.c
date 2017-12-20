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
#include <kernel/redshift.h>
#include <mem/heap.h>
#include <mem/paging.h>
#include <sched/process.h>
#include <string.h>

static struct process {
    int                    id;
    uint32_t               eip;
    struct page_directory* page_dir;
    struct process*        next;
} * processes[MAX_PROCESS_PRIORITY + 1];

static uint32_t num_processes;

int process_spawn(uint32_t entry, struct page_directory* page_dir, uint8_t priority)
{
    if (priority > MAX_PROCESS_PRIORITY)
        return -1;
    /* Append the new process to the end of the process queue.
     */
    struct process* process = processes[priority];
    while (process->next)
        process = process->next;
    process->next           = kmalloc(sizeof(*processes));
    process->next->id       = num_processes++;
    process->next->eip      = entry;
    process->next->page_dir = page_dir;
    process->next->next     = NULL;
    return process->next->id;
}

static void jump(uint32_t address)
{
    printk("Jump to 0x%08lX\n", address);
    //__asm__ __volatile__("jmp *%0"::"r"(address));
}

static void do_process_switch(struct process** process)
{
    /* Reverse the process queue before moving on to the next priority queue.
     */
    DEBUG_ASSERT(process != NULL && *process != NULL);
    struct process* list = NULL;
    while (*process) {
        struct process* tmp = *process;
        *process = (*process)->next;
        tmp->next = list;
        list = tmp;
    }
    *process = list;
    /* Load the process' page directory and jump to its EIP.
     */
    page_directory_load((*process)->page_dir);
    jump((*process)->eip);
}

void process_switch(void* ignored)
{
    /* Select a process queue, starting at the highest priority (U8_MAX).
     */
    uint8_t n = MAX_PROCESS_PRIORITY;
    printk("Switching task...\n");
    while (n --> 0) {
        if (processes[n]->next) {
            /* Switch to the next process in this priority queue and then return.
             */
            do_process_switch(&(processes[n]->next));
            return;
        }
    }
    UNUSED(ignored);
}

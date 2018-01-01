/* Copyinc (c) 2012 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the incs
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyinc notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYinc HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <redshift/kernel.h>
#include <redshift/kernel/timer.h>
#include <redshift/mem/heap.h>

static struct timer_event {
    uint32_t period;
    uint32_t elapsed_time; /* Time elapsed since event last raised. */
    void(*   callback)(void*);
    void*    arg;
    struct timer_event* next;
} * events;

void add_timer_event(uint32_t period, void(* callback)(void*), void* arg)
{
    if (!(callback)) {
        return;
    }
    struct timer_event* event = kmalloc(sizeof(*events));
    if (event == NULL) {
        panic("can't allocate memory");
    }
    event->period       = period;
    event->elapsed_time = 0;
    event->callback     = callback;
    event->arg          = arg;
    event->next         = NULL;
    if (events) {
        /* Append the event handler.
         */
        while (events->next) {
            events = events->next;
        }
        events->next = event;
    } else {
        /* Initialise the list.
         */
        events = event;
    }
}

void process_timer_queue(uint32_t elapsed_time)
{
    struct timer_event* queue = events;
    /* Process queue.
     */
    int i = 0;
    while (queue) {
        ++i;
        if (queue->elapsed_time + elapsed_time >= queue->period) {
            /* Call event.
             */
            if (queue->callback) {
                printk(PRINTK_DEBUG "Event raised (0x%p)\n", queue->callback);
                queue->callback(queue->arg);
            }
            queue->elapsed_time = 0;
        } else {
            queue->elapsed_time += elapsed_time;
        }
        queue = queue->next;
    }
}

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
#include <debug/dump_stack.h>
#include <kernel/redshift.h>
#include <kernel/symbols.h>

static unsigned __noinline stacktrace(unsigned* array, unsigned max_frames)
{
    unsigned* frame = &max_frames - 2;
    unsigned i;
    for (i = 0; i < max_frames; ++i) {
        unsigned ip = frame[1];
        if (ip == 0) {
            break;
        }
        array[i] = ip;
        frame = (unsigned*)frame[0];
    }
    return i;
}

#define MAX_FRAMES 10

void dump_stack(void)
{
    static unsigned array[MAX_FRAMES];
    unsigned j = stacktrace(array, MAX_FRAMES);
    for (unsigned i = 0; i < j; ++i) {
        printk("%d. At 0x%08X (aka <%s>)\n", i + 1, array[i], get_symbol_name(array[i]));
    }
}

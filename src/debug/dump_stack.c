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
#include <redshift/debug/dump_stack.h>
#include <redshift/kernel.h>
#include <redshift/kernel/symbols.h>

enum {
    MAX_FRAMES  = 10U, /* Maximum number of frames to rewind. */
    SKIP_FRAMES = 0U   /* Number of frames to skip when printing. A value of 0 will include walk_stack, dump_stack and
                        * the calling function while 1 skips walk_stack, 2 additionally skips dump_stack and 3 skips all
                        * three.
                        */
};

static unsigned __noinline walk_stack(unsigned* array)
{
    uint32_t* ebp = (uint32_t*)(&array - 2);
    unsigned i = 0;
    for (; i < MAX_FRAMES; ++i) {
        uint32_t eip = ebp[1];
        if (eip == 0) {
            break;
        }
        array[i] = eip;
        ebp = (uint32_t*)ebp[0];
    }
    return i;
}

void dump_stack(void)
{
    static unsigned array[MAX_FRAMES];
    unsigned j = walk_stack(array);
    for (unsigned i = SKIP_FRAMES; i < j; ++i) {
        const char* symbol = get_symbol_name(array[i]);
        if (symbol == NULL) {
            symbol = "<unresolved symbol>";
        }
        printk("%d. At 0x%08X in %s\n", i + 1 - SKIP_FRAMES, array[i], symbol);
    }
}

/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include <libk/memory.hpp>
#include <redshift/debug/dump_stack.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/symbols.hpp>

enum {
    // Number of frames to skip when printing. A value of 0 will include walk_stack, dump_stack and  the calling
    // function while 1 skips walk_stack, 2 additionally skips dump_stack and 3 skips all three.

    SKIP_FRAMES = 0U,
    // Maximum number of frames to rewind.
    MAX_FRAMES  = SKIP_FRAMES + 10U
};

extern unsigned walk_stack(unsigned* addresses, unsigned max);

void dump_stack()
{
    interrupt_state_guard guard(interrupt_state::disable);
    static unsigned addresses[MAX_FRAMES];
    libk::memory::zero(addresses, MAX_FRAMES*sizeof(*addresses));
    unsigned j = walk_stack(addresses, MAX_FRAMES);
    for (unsigned i = SKIP_FRAMES; i < j; ++i) {
        const char* symbol = get_symbol_name(addresses[i]);
        if (symbol == nullptr) {
            symbol = "??";
        }
        printk("%d. At 0x%08X in %s\n", i + 1 - SKIP_FRAMES, addresses[i], symbol);
    }
    
}

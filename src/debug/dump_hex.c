/* Copyright (c) 2014-2018 Chris Swinchatt.
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
#include <ctype.h>
#include <debug/dump_hex.h>
#include <kernel/console.h>
#include <kernel/redshift.h>

enum {
    BYTES_PER_LINE = 16
};

static uint8_t* memory = (uint8_t*)0;

static void write_offset(uint32_t offset)
{
    console.screen.background = CONSOLE_COLOR_RED;
    console.screen.foreground = CONSOLE_COLOR_WHITE;
    printk("0x%08lX:", offset);
    console.screen.background = CONSOLE_DEFAULT_BACKGROUND;
    console.screen.foreground = CONSOLE_DEFAULT_FOREGROUND;
}

static void write_hex(uint32_t offset, uint32_t count)
{
    for (uint32_t i = 0; i < count; ++i) {
        console.screen.background = CONSOLE_COLOR_GREEN;
        console.screen.foreground = CONSOLE_COLOR_WHITE;
        printk("%02X", memory[offset + i]);
    console.screen.background = CONSOLE_DEFAULT_BACKGROUND;
    console.screen.foreground = CONSOLE_DEFAULT_FOREGROUND;
        if (i + 1 < count) {
            console_writechar(' ');
        }
    }
}

static void write_ascii(uint32_t offset, uint32_t count)
{
    console.screen.background = CONSOLE_COLOR_BLUE;
    console.screen.foreground = CONSOLE_COLOR_WHITE;
    for (uint32_t i = 0; i < count; ++i) {
        int c = (int)memory[offset + i];
        printk("%c", iscntrl(c) ? '.' : c);
    }
    console.screen.background = CONSOLE_DEFAULT_BACKGROUND;
    console.screen.foreground = CONSOLE_DEFAULT_FOREGROUND;
}

void dump_hex(uint32_t start, size_t count)
{
    for (size_t i = 0; i < count; i += BYTES_PER_LINE) {
        write_offset(start + i);
        console_writestring("   ");
        write_hex(i, BYTES_PER_LINE);
        console_writestring("   ");
        write_ascii(i, BYTES_PER_LINE);
        console_writechar('\n');
    }
}

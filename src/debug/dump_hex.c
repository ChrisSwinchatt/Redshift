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
#include <libk/kchar.h>
#include <redshift/debug/dump_hex.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel.h>

enum {
    BYTES_PER_LINE = 16
};

static uint8_t* memory = (uint8_t*)0;

static void write_offset(uint32_t offset)
{
    console_color_t foreground, background;
    console_get_color(&foreground, &background);
    console_set_color(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_RED);
    printk(PRINTK_INFO "0x%08lX:", offset);
    console_set_color(foreground, background);
}

static void write_hex(uint32_t offset, uint32_t count)
{
    console_color_t foreground, background;
    console_get_color(&foreground, &background);
    for (uint32_t i = 0; i < count; ++i) {
        console_set_color(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_GREEN);
        printk("%02X", memory[offset + i]);
        console_set_color(foreground, background);
        if (i + 1 < count) {
            console_write_char(' ');
        }
    }
}

static void write_ascii(uint32_t offset, uint32_t count)
{
    console_color_t foreground, background;
    console_get_color(&foreground, &background);
    console_set_color(CONSOLE_COLOR_WHITE, CONSOLE_COLOR_BLUE);
    for (uint32_t i = 0; i < count; ++i) {
        int c = (int)memory[offset + i];
        printk("%c", kchar_is_control(c) ? '.' : c);
    }
    console_set_color(foreground, background);
}

void dump_hex(uint32_t start, size_t count)
{
    for (size_t i = 0; i < count; i += BYTES_PER_LINE) {
        write_offset(start + i);
        console_write_string("   ");
        write_hex(i, BYTES_PER_LINE);
        console_write_string("   ");
        write_ascii(i, BYTES_PER_LINE);
        console_write_char('\n');
    }
}

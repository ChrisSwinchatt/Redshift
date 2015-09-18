/**
 * \file kernel/console.c
 * \brief Simple console output.
 * \author Chris Swinchatt <c.swinchatt1@uni.brighton.ac.uk> 
 * \copyright Copyright (c) 2012-2014 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <kernel/console.h>
#include <kernel/redshift.h>

struct console console;

struct {
    uint16_t* ptr;      /**< The framebuffer.       */
    uint32_t size;      /**< The framebuffer size.  */
} buffer;

static uint32_t buffer_index(void)
{
    return console.cursor.y * console.screen.columns + console.cursor.x;
}

static uint8_t attribute(void)
{
    return (console.screen.background << 4) | (console.screen.foreground & 0x0f);
}

void console_init(void)
{
    console.screen.columns    = CONSOLE_DEFAULT_COLUMNS;
    console.screen.rows       = CONSOLE_DEFAULT_ROWS;
    console.screen.foreground = CONSOLE_DEFAULT_FOREGROUND;
    console.screen.background = CONSOLE_DEFAULT_BACKGROUND;
    console.cursor.x          = 0;
    console.cursor.y          = 0;
    console.cursor.flags      = CONSOLE_DEFAULT_CURSOR_FLAGS;
    buffer.ptr                = (uint16_t*)CONSOLE_DEFAULT_FRAMEBUFFER;
    buffer.size               = CONSOLE_DEFAULT_BUFFER_SIZE;
    console_clear();
}

void console_enable_cursor()
{
    console.cursor.flags &= ~CONSOLE_CURSOR_DISABLED;
    console.cursor.flags |= CONSOLE_CURSOR_ENABLED;
}

void console_disable_cursor()
{
    console.cursor.flags &= ~CONSOLE_CURSOR_ENABLED;
    console.cursor.flags |= CONSOLE_CURSOR_DISABLED;
}

void console_writechar(int c)
{
    switch (c) {
        case '\b':
            if (console.cursor.x) {
                --console.cursor.x;
            }
            break;
        case '\n':
            ++console.cursor.y;
            /* FALL THROUGH */
        case '\r':
            console.cursor.x = 0;
            break;
        case '\t':
            console.cursor.x = (console.cursor.x + 8) & ~7;
            break;
        default:
            buffer.ptr[buffer_index()] = c | (attribute() << 8);
            ++console.cursor.x;
            break;
    }
    if (console.cursor.x > console.screen.columns) {
        console.cursor.x = 0;
        ++console.cursor.y;
    }
    if (console.cursor.y >= console.screen.rows) {
        console_scroll();
    }
}

long console_writestring(const char* string)
{
    long index = 0;
    while (string[index])
        console_writechar(string[index++]);
    console_update_cursor();
    return index;
}

long console_writeline(const char* line)
{
    long count = console_writestring(line);
    console_writechar('\n');
    return count + 1;
}

void console_scroll(void)
{
    unsigned i = 0;
    for (i = 0; i < console.screen.columns * (console.screen.rows - 1); ++i) {
        buffer.ptr[i] = buffer.ptr[i + console.screen.columns];
    }
    for (; i < console.screen.columns * console.screen.rows; ++i) {
        buffer.ptr[i] = ' ' | (attribute() << 8);
    }
    console.cursor.y = console.screen.rows - 1;
    console_update_cursor();
}

static void hide_cursor(void)
{
    io_outb(VGA_CMND, 0x0e);
    io_outb(VGA_DATA, 0xff);
    io_outb(VGA_CMND, 0x0f);
    io_outb(VGA_DATA, 0xff);
}

static void set_cursor_shape(void)
{
    uint8_t tmp = 0;
    if (test_flag(console.cursor.flags, CONSOLE_CURSOR_UNDERLINE))
        tmp = 255;
    clear_bit(tmp, 5);
    io_outb(VGA_CMND, 0x0a);
    io_outb(VGA_DATA, tmp);
}

static void set_cursor_position(void)
{
    io_outb(VGA_CMND, 0x0e);
    io_outb(VGA_DATA, (buffer_index() >> 8));
    io_outb(VGA_CMND, 0x0f);
    io_outb(VGA_DATA, buffer_index());
}

void console_update_cursor(void)
{
    if (test_flag(console.cursor.flags, CONSOLE_CURSOR_DISABLED)) {
        hide_cursor();
    } else {
        set_cursor_shape();
        set_cursor_position();
    }
}

void console_clearline(void)
{
    console.cursor.x = 0;
    while (console.cursor.x < console.screen.columns) {
        buffer.ptr[buffer_index()] = ' ' | (attribute() << 8);
        ++console.cursor.x;
    }
    console.cursor.x = 0;
    console_update_cursor();
}

void console_clear(void)
{
    console.cursor.y = 0;
    while (console.cursor.y < console.screen.rows) {
        console_clearline();
        ++console.cursor.y;
    }
    console.cursor.y = 0;
    console_update_cursor();
}

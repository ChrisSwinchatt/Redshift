/**
 * \file kernel/console.c
 * Simple console output.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
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
#include <redshift/kernel/console.h>
#include <redshift/kernel.h>

static struct console {
    struct {
        uint32_t columns;    /**< The number of columns. */
        uint32_t rows;       /**< The number of rows.    */
        uint8_t  background; /**< The background colour. */
        uint8_t  foreground; /**< The foreground colour. */
    } screen;
    struct {
        uint32_t x;          /**< The x co-ordinate.     */
        uint32_t y;          /**< The y co-ordinate.     */
        uint32_t flags;      /**< The cursor flags.      */
    } cursor;
    struct {
        uint16_t* ptr;      /**< The frameconsole.buffer.       */
        uint32_t size;      /**< The framebuffer size.  */
    } buffer;
} console;

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
    console.buffer.ptr        = (uint16_t*)CONSOLE_DEFAULT_FRAMEBUFFER;
    console.buffer.size       = CONSOLE_DEFAULT_BUFFER_SIZE;
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

void console_write_char(int c)
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
            console.buffer.ptr[buffer_index()] = c | (attribute() << 8);
            ++console.cursor.x;
            break;
    }
    if (console.cursor.x >= console.screen.columns) {
        console.cursor.x = 0;
        ++console.cursor.y;
    }
    if (console.cursor.y >= console.screen.rows) {
        console_scroll();
    }
}

long console_write_string(const char* string)
{
    long index = 0;
    while (string[index])
        console_write_char(string[index++]);
    console_update_cursor();
    return index;
}

long console_write_line(const char* line)
{
    long count = console_write_string(line);
    console_write_char('\n');
    return count + 1;
}

void console_scroll(void)
{
    unsigned i = 0;
    for (i = 0; i < console.screen.columns * (console.screen.rows - 1); ++i) {
        console.buffer.ptr[i] = console.buffer.ptr[i + console.screen.columns];
    }
    for (; i < console.screen.columns * console.screen.rows; ++i) {
        console.buffer.ptr[i] = ' ' | (attribute() << 8);
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
    if (TEST_FLAG(console.cursor.flags, CONSOLE_CURSOR_UNDERLINE))
        tmp = 255;
    CLEAR_BIT(tmp, 5);
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
    if (TEST_FLAG(console.cursor.flags, CONSOLE_CURSOR_DISABLED)) {
        hide_cursor();
    } else {
        set_cursor_shape();
        set_cursor_position();
    }
}

void console_clear_line(void)
{
    console.cursor.x = 0;
    while (console.cursor.x < console.screen.columns) {
        console.buffer.ptr[buffer_index()] = ' ' | (attribute() << 8);
        ++console.cursor.x;
    }
    console.cursor.x = 0;
    console_update_cursor();
}

void console_clear(void)
{
    console.cursor.y = 0;
    while (console.cursor.y < console.screen.rows) {
        console_clear_line();
        ++console.cursor.y;
    }
    console.cursor.y = 0;
    console_update_cursor();
}

void console_set_foreground_color(console_color_t color)
{
    console.screen.foreground = color;
}

void console_set_background_color(console_color_t color)
{
    console.screen.background = color;
}

void console_set_color(console_color_t foreground, console_color_t background)
{
    console.screen.foreground = foreground;
    console.screen.background = background;
}

console_color_t console_get_foreground_color(void)
{
    return console.screen.foreground;
}

console_color_t console_get_background_color(void)
{
    return console.screen.background;
}

void console_get_color(console_color_t* foreground, console_color_t* background)
{
    *foreground = console.screen.foreground;
    *background = console.screen.background;
}

void console_set_cursor(uint32_t x, uint32_t y)
{
    console.cursor.x = x;
    console.cursor.y = y;
}

void console_get_cursor(uint32_t* x, uint32_t* y)
{
    *x = console.cursor.x;
    *y = console.cursor.y;
}

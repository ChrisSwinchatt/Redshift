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
#include <ctype.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel.h>
#include <string.h>

static struct console {
    struct {
        uint32_t  columns;
        uint32_t  rows;
        uint8_t   background;
        uint8_t   foreground;
        uint16_t* buffer;
    } screen;
    struct {
        uint32_t x;
        uint32_t y;
        uint32_t mode;
        uint32_t x_origin;
        uint32_t y_origin;
    } cursor;
} console;

/* Current buffer index. */
#define BUFFER_INDEX        (console.cursor.y*console.screen.columns + console.cursor.x)

/* Pointer to current buffer location. */
#define BUFFER_PTR          (console.screen.buffer + (BUFFER_INDEX))

/* Number of cells in buffer. */
#define BUFFER_NUM_CELLS    (console.screen.columns*console.screen.rows)

/* Size of buffer (bytes). */
#define BUFFER_SIZE         ((sizeof(*(console.screen.buffer)))*(BUFFER_NUM_CELLS))

/* Current attribute byte (foreground & background colour). */
#define ATTRIBUTE           ((console.screen.background << 4) | (console.screen.foreground & 0x0F))

/* Convert 8-bit char into 16-bit char with attribute. */
#define CHAR_WITH_ATTRIB(C) ((C) | ((ATTRIBUTE) << 8))

/* Blank character. */
#define BLANK               CHAR_WITH_ATTRIB(' ')

/* Screen origin index. */
#define ORIGIN              (console.cursor.y_origin*console.screen.columns + console.cursor.x_origin)

/* Set character and attribute at current buffer location. */
static void set_buffer_char(uint8_t c)
{
    *BUFFER_PTR = CHAR_WITH_ATTRIB(c);
}

void console_init(void)
{
    SAVE_INTERRUPT_STATE;
    console.screen.columns    = CONSOLE_DEFAULT_COLUMNS;
    console.screen.rows       = CONSOLE_DEFAULT_ROWS;
    console.screen.foreground = CONSOLE_DEFAULT_FOREGROUND;
    console.screen.background = CONSOLE_DEFAULT_BACKGROUND;
    console.cursor.x_origin   = 0;
    console.cursor.y_origin   = 0;
    console.cursor.x          = console.cursor.x_origin;
    console.cursor.y          = console.cursor.y_origin;
    console.cursor.mode       = CONSOLE_DEFAULT_CURSOR_MODE;
    console.screen.buffer     = (uint16_t*)CONSOLE_DEFAULT_FRAMEBUFFER;
    RESTORE_INTERRUPT_STATE;
}

void console_write_char(int c)
{
    SAVE_INTERRUPT_STATE;
    switch (c) {
        case '\b':
            if (console.cursor.x) {
                --console.cursor.x;
            }
            break;
        case '\n':
            ++console.cursor.y;
            FALL_THROUGH;
        case '\r':
            console.cursor.x = console.cursor.x_origin;
            break;
        case '\t':
            console.cursor.x = (console.cursor.x + CONSOLE_DEFAULT_TAB_SIZE) & ~(CONSOLE_DEFAULT_TAB_SIZE - 1);
            break;
        default:
            if (isprint(c)) {
                set_buffer_char(c);
                ++console.cursor.x;
            }
            break;
    }
    if (console.cursor.x >= console.screen.columns) {
        console.cursor.x =  console.cursor.x_origin;
        ++console.cursor.y;
    }
    if (console.cursor.y >= console.screen.rows) {
        console_scroll();
    }
    RESTORE_INTERRUPT_STATE;
}

long console_write_string(const char* string)
{
    SAVE_INTERRUPT_STATE;
    long i = 0;
    for (; string[i] != 0; ++i) {
        console_write_char(string[i]);
    }
    console_update_cursor();
    RESTORE_INTERRUPT_STATE;
    return i;
}

long console_write_line(const char* line)
{
    SAVE_INTERRUPT_STATE;
    long count = console_write_string(line);
    console_write_char('\n');
    ++count;
    RESTORE_INTERRUPT_STATE;
    return count;
}

void console_scroll(void)
{
    SAVE_INTERRUPT_STATE;
    memcpy(
        console.screen.buffer + ORIGIN,
        console.screen.buffer + ORIGIN + console.screen.columns,
        BUFFER_SIZE - ORIGIN - console.screen.columns
    );
    console.cursor.y = console.screen.rows;
    console_clear_line();
    console.cursor.y = console.screen.rows - 1;
    console_update_cursor();
    RESTORE_INTERRUPT_STATE;
}

void console_clear_line(void)
{
    SAVE_INTERRUPT_STATE;
    memsetw(
        console.screen.buffer + (console.cursor.y - 1)*console.screen.columns + console.cursor.x_origin,
        BLANK,
        console.screen.columns - console.cursor.x_origin
    );
    console.cursor.x = console.cursor.x_origin;
    console_update_cursor();
    RESTORE_INTERRUPT_STATE;
}

void console_clear(void)
{
    SAVE_INTERRUPT_STATE;
    memsetw(console.screen.buffer + ORIGIN, BLANK, BUFFER_SIZE - ORIGIN);
    console.cursor.x = console.cursor.x_origin;
    console.cursor.y = console.cursor.y_origin;
    console_update_cursor();
    RESTORE_INTERRUPT_STATE;
}

static void hide_cursor(void)
{
    io_outb(VGA_CMND, 0x0A);
    io_outb(VGA_DATA, 0x20);
}

/* Set start and end scanlines of cursor. Valid values are 0 to 15. */
static void set_cursor_shape(uint8_t start, uint8_t end)
{
    io_outb(VGA_CMND, 0x0A);
    io_outb(VGA_DATA, start | (io_inb(VGA_DATA) & 0xC0));
    io_outb(VGA_CMND, 0x0B);
    io_outb(VGA_DATA, end   | (io_inb(0x3E0) & 0xE0));
}

static void set_cursor_position(void)
{
    io_outb(VGA_CMND, 0x0E);
    io_outb(VGA_DATA, BUFFER_INDEX >> 8);
    io_outb(VGA_CMND, 0x0F);
    io_outb(VGA_DATA, BUFFER_INDEX);
}

static void draw_cursor(void)
{
    switch (console.cursor.mode) {
        case CONSOLE_CURSOR_UNDERLINE: set_cursor_shape(14, 15); break;
        case CONSOLE_CURSOR_BLOCK:     set_cursor_shape(0,  15); break;
        default:                       UNREACHABLE("no switch case for console.cursor.mode=%d", console.cursor.mode);
    }
    set_cursor_position();
}

void console_set_cursor_mode(console_cursor_mode_t mode)
{
    SAVE_INTERRUPT_STATE;
    console.cursor.mode = mode;
    console_update_cursor();
    RESTORE_INTERRUPT_STATE;
}

void console_update_cursor(void)
{
    SAVE_INTERRUPT_STATE;
    if (console.cursor.mode == CONSOLE_CURSOR_DISABLED) {
        hide_cursor();
    } else {
        draw_cursor();
    }
    RESTORE_INTERRUPT_STATE;
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
    SAVE_INTERRUPT_STATE;
    console.screen.foreground = foreground;
    console.screen.background = background;
    RESTORE_INTERRUPT_STATE;
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
    SAVE_INTERRUPT_STATE;
    *foreground = console.screen.foreground;
    *background = console.screen.background;
    RESTORE_INTERRUPT_STATE;
}

void console_set_cursor(uint32_t x, uint32_t y)
{
    SAVE_INTERRUPT_STATE;
    console.cursor.x = x;
    console.cursor.y = y;
    RESTORE_INTERRUPT_STATE;
}

void console_get_cursor(uint32_t* x, uint32_t* y)
{
    SAVE_INTERRUPT_STATE;
    *x = console.cursor.x;
    *y = console.cursor.y;
    RESTORE_INTERRUPT_STATE;
}

void console_set_origin(uint32_t x, uint32_t y)
{
    SAVE_INTERRUPT_STATE;
    console.cursor.x_origin = x;
    console.cursor.y_origin = y;
    if (console.cursor.x < console.cursor.x_origin) {
        console.cursor.x = console.cursor.x_origin;
    }
    if (console.cursor.y < console.cursor.y_origin) {
        console.cursor.y = console.cursor.y_origin;
    }
    RESTORE_INTERRUPT_STATE;
}

void console_get_origin(uint32_t* x_origin, uint32_t* y_origin)
{
    SAVE_INTERRUPT_STATE;
    *x_origin = console.cursor.x_origin;
    *y_origin = console.cursor.y_origin;
    RESTORE_INTERRUPT_STATE;
}

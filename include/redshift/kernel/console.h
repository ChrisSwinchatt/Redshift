/**
 * \file kernel/console.h
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
#ifndef REDSHIFT_IO_CONSOLE_H
#define REDSHIFT_IO_CONSOLE_H 1

#include <redshift/kernel.h>

/** Console output colours. */
typedef enum console_color {
    /* NB: Order is significant here.
     */
    CONSOLE_COLOR_BLACK,
    CONSOLE_COLOR_BLUE,
    CONSOLE_COLOR_GREEN,
    CONSOLE_COLOR_CYAN,
    CONSOLE_COLOR_RED,
    CONSOLE_COLOR_MAGENTA,
    CONSOLE_COLOR_BROWN,
    CONSOLE_COLOR_LIGHT_GRAY,
    CONSOLE_COLOR_DARK_GRAY,
    CONSOLE_COLOR_LIGHT_BLUE,
    CONSOLE_COLOR_LIGHT_GREEN,
    CONSOLE_COLOR_LIGHT_CYAN,
    CONSOLE_COLOR_LIGHT_RED,
    CONSOLE_COLOR_LIGHT_MAGENTA,
    CONSOLE_COLOR_YELLOW,
    CONSOLE_COLOR_WHITE
} console_color_t;

/** Console cursor flags. */
typedef enum console_cursor_mode {
    CONSOLE_CURSOR_DISABLED,  /**< Cursor disabled.      */
    CONSOLE_CURSOR_UNDERLINE, /**< Underline cursor (_). */
    CONSOLE_CURSOR_BLOCK,     /**< Block cursor ([]).    */
} console_cursor_mode_t;

enum {
    CONSOLE_DEFAULT_COLUMNS     = 80,
    CONSOLE_DEFAULT_ROWS        = 25,
    CONSOLE_DEFAULT_FRAMEBUFFER = 0xB8000,
    CONSOLE_DEFAULT_TAB_SIZE    = 8,
    CONSOLE_DEFAULT_BACKGROUND  = CONSOLE_COLOR_BLACK,
    CONSOLE_DEFAULT_FOREGROUND  = CONSOLE_COLOR_LIGHT_GRAY,
    CONSOLE_DEFAULT_CURSOR_MODE = CONSOLE_CURSOR_UNDERLINE
};

/**
 * Initialises the console.
 */
void console_init(void);

/**
 * Adds a key to the console key buffer.
 * \param c The key to add.
 */
void console_add_key(int c);

/**
 * Set the cursor mode.
 */
void console_set_cursor_mode(console_cursor_mode_t mode);

/**
 * Disables the cursor.
 */
void console_disable_cursor(void);

/**
 * Writes a character to the console.
 * \param c The character to write.
 */
void console_write_char(int c);

/**
 * Writes a string to the console.
 * \param string The string to write.
 * \return The number of characters written is returned.
 */
long console_write_string(const char* string);

/**
 * Writes a line to the console.
 * \param line The line to write.
 * \return The number of characters written is returned.
 */
long console_write_line(const char* line);

/**
 * Scrolls the screen up.
 */
void console_scroll(void);

/**
 * Updates the hardware cursor.
 */
void console_update_cursor(void);

/**
 * Clears the current line.
 */
void console_clear_line(void);

/**
 * Clears the screen.
 */
void console_clear(void);

/**
 * Set the foreground colour.
 * \param color The new foreground colour.
 */
void console_set_foreground_color(console_color_t color);

/**
 * Set the background colour.
 * \param color The new background colour.
 */
void console_set_background_color(console_color_t color);

/**
 * Set the foreground and background colours.
 * \param foreground The new foreground colour.
 * \param background The new background colour.
 */
void console_set_color(console_color_t foreground, console_color_t background);

/**
 * Get the current foreground colour.
 * \return The current foreground colour.
 */
console_color_t console_get_foreground_color(void);


/**
 * Get the current foreground colour.
 * \return The current foreground colour.
 */
console_color_t console_get_background_color(void);


/**
 * Get the current foreground colour.
 * \return The current foreground colour.
 */
void console_get_color(console_color_t* foreground, console_color_t* background);

/**
 * Set the cursor position.
 * \param x The x co-ordinate of the new cursor position.
 * \param y The y co-ordinate of the new cursor position.
 */
void console_set_cursor(uint32_t x, uint32_t y);


/**
 * Get the current cursor position.
 * \param x_origin Contains the x co-ordinate of the current cursor position.
 * \param y_origin Contains the y co-ordinate of the current cursor position.
 */
void console_get_cursor(uint32_t* x, uint32_t* y);

/**
 * Set the cursor origin. Scroll and clear operations will not affect positions where x < x_origin OR y < y_origin.
 * \param x_origin The x origin.
 * \param y_origin The y origin.
 */
void console_set_origin(uint32_t x_origin, uint32_t y_origin);


/**
 * Get the cursor origin.
 * \param x_origin Contains the x origin.
 * \param y_origin Contains the y origin.
 */
void console_get_origin(uint32_t* x_origin, uint32_t* y_origin);

#endif /* ! REDSHIFT_KERNEL_CONSOLE_H */

/**
 * \file kernel/console.h
 * \brief Simple console output.
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

#include <kernel/redshift.h>

/** Console output colours. */
enum {
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
};

/** Console cursor flags. */
enum {
    CONSOLE_CURSOR_DISABLED,  /**< Cursor disabled.       */
    CONSOLE_CURSOR_ENABLED,   /**< Cursor enabled.        */
    CONSOLE_CURSOR_UNDERLINE, /**< Underline cursor (_).  */
    CONSOLE_CURSOR_BLOCK,     /**< Block cursor ([]).     */
};


enum {
    CONSOLE_DEFAULT_COLUMNS      = 80,
    CONSOLE_DEFAULT_ROWS         = 25,
    CONSOLE_DEFAULT_BACKGROUND   = CONSOLE_COLOR_BLACK,
    CONSOLE_DEFAULT_FOREGROUND   = CONSOLE_COLOR_LIGHT_GRAY,
    CONSOLE_DEFAULT_CURSOR_FLAGS = CONSOLE_CURSOR_DISABLED,
    CONSOLE_DEFAULT_BUFFER_SIZE  = CONSOLE_DEFAULT_COLUMNS * CONSOLE_DEFAULT_ROWS,
    CONSOLE_DEFAULT_FRAMEBUFFER  = 0xB8000
};

/**  Console structure. */
extern struct console {
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
} console; /* console.c */

/**
 * \brief Initialises the console.
 */
void console_init(void);

/**
 * \brief Adds a key to the console key buffer.
 * \param c The key to add.
 */
void console_add_key(int c);

/**
 * \brief Enables the cursor.
 */
void console_enable_cursor();

/**
 * \brief Disables the cursor.
 */
void console_disable_cursor();

/**
 * \brief Writes a character to the console.
 * \param c The character to write.
 */
void console_writechar(int c);

/**
 * \brief Writes a string to the console.
 * \param string The string to write.
 * \return The number of characters written is returned.
 */
long console_writestring(const char* string);

/**
 * \brief Writes a line to the console.
 * \param line The line to write.
 * \return The number of characters written is returned.
 */
long console_writeline(const char* line);

/**
 * \brief Scrolls the screen up.
 */
void console_scroll(void);

/**
 * \brief Updates the hardware cursor.
 */
void console_update_cursor(void);

/**
 * \brief Clears the current line.
 */
void console_clearline(void);

/**
 * \brief Clears the screen.
 */
void console_clear(void);

#endif /* ! REDSHIFT_KERNEL_CONSOLE_H */

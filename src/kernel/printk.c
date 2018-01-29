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
#include <libk/kchar.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel.h>
#include <libk/kstring.h>
#include <libk/kstring.h>

#define VPRINTK_BUFFER_SIZE 2048

#ifdef NDEBUG
# define MINIMUM_LOG_LEVEL  LOGLEVEL_INFO
#else
# define MINIMUM_LOG_LEVEL  LOGLEVEL_DEBUG
#endif /* NDEBUG */

typedef enum {
    LOGLEVEL_DEBUG   = 0,
    LOGLEVEL_INFO    = 1,
    LOGLEVEL_WARNING = 8,
    LOGLEVEL_ERROR   = 9
} loglevel;

static loglevel handle_printk_level(const char** pfmt)
{
    const char* fmt = (*pfmt);
    loglevel level  = 1;
    if (kstring_length(fmt) >= 3 && fmt[0] == '<' && kchar_is_digit(fmt[1]) && fmt[2] == '>') {
        level = fmt[1] - '0';
        *pfmt += 3;
    }
    return level;
}

static void set_level_formatting(loglevel level)
{
    switch (level) {
        case LOGLEVEL_DEBUG:
            console_set_foreground_color(CONSOLE_COLOR_LIGHT_GRAY);
            break;
        case LOGLEVEL_INFO:
            console_set_foreground_color(CONSOLE_COLOR_WHITE);
            break;
        case LOGLEVEL_WARNING:
            console_set_foreground_color(CONSOLE_COLOR_RED);
            break;
        case LOGLEVEL_ERROR:
            console_set_foreground_color(CONSOLE_COLOR_LIGHT_RED);
            break;
        default:
            UNREACHABLE("no switch case for level=%d", level);
    }
}

static int do_print(const char* fmt, va_list ap)
{
    static char buffer[VPRINTK_BUFFER_SIZE];
    ssize_t count_1 = kstring_vformat(buffer, VPRINTK_BUFFER_SIZE, fmt, ap);
    ssize_t count_2 = console_write_string(buffer);
    ASSERT_EQUAL_SSIZE_T(count_1, count_2);
    return count_1;
}

int vprintk(const char* fmt, va_list ap)
{
    SAVE_INTERRUPT_STATE;
    int level = 0;
    if ((level = handle_printk_level(&fmt)) < MINIMUM_LOG_LEVEL) {
        return 0;
    }
    console_color_t foreground, background;
    console_get_color(&foreground, &background);
    set_level_formatting(level);
    int ret = do_print(fmt, ap);
    console_set_color(foreground, background);
    RESTORE_INTERRUPT_STATE;
    return ret;
}

int printk(const char* fmt, ...)
{
    SAVE_INTERRUPT_STATE;
    va_list ap;
    va_start(ap, fmt);
    int ret = vprintk(fmt, ap);
    va_end(ap);
    RESTORE_INTERRUPT_STATE;
    return ret;
}

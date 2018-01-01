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
#include <ctype.h>
#include <redshift/kernel/console.h>
#include <redshift/kernel.h>
#include <stdio.h>
#include <string.h>

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
    if (strlen(fmt) >= 3 && fmt[0] == '<' && isdigit(fmt[1]) && fmt[2] == '>') {
        level = fmt[1] - '0';
        *pfmt += 3;
    }
    return level;
}

int vprintk(const char* fmt, va_list ap)
{
    /* Print format string.
     */
    static char buffer[VPRINTK_BUFFER_SIZE];
    int ret   = 0;
    int level = 0;
    if ((level = handle_printk_level(&fmt)) < MINIMUM_LOG_LEVEL) {
        return 0;
    }
    ret = vsnprintf(buffer, VPRINTK_BUFFER_SIZE, fmt, ap);
    /* Print coloured output.
     */
    {
        const console_color_t foreground = console_get_foreground_color();
        switch (level) {
            case LOGLEVEL_DEBUG:   console_set_foreground_color(CONSOLE_COLOR_LIGHT_GRAY); break;
            case LOGLEVEL_INFO:    console_set_foreground_color(CONSOLE_COLOR_WHITE);      break;
            case LOGLEVEL_WARNING: console_set_foreground_color(CONSOLE_COLOR_RED);        break;
            case LOGLEVEL_ERROR:   console_set_foreground_color(CONSOLE_COLOR_LIGHT_RED);  break;
            default:               UNREACHABLE("no switch case for level %d", level);
        }
        console_write_string(buffer);
        console_set_foreground_color(foreground);
    }
    return ret;
}

int printk(const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vprintk(fmt, ap);
    va_end(ap);
    return ret;
}

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
#ifndef REDSHIFT_LIBC__CSTDIO_H
#define REDSHIFT_LIBC__CSTDIO_H

#include <redshift/kernel.h>
#include <stdarg.h>

/**
 * Write formatted text to a string
 * \param buffer Buffer to store formatted string
 * \param n Maximum characters to write
 * \param fmt Formatted string
 * \param ap Variable list of arguments
 * \return The number of characters actually written is returned
 */
int vsnprintf(char* buffer, uint32_t n, const char* fmt, va_list ap);

/**
 * Write formatted text to a string
 * \param buffer Buffer to store formatted string
 * \param n Maximum characters to write
 * \param fmt Formatted string
 * \param ... Variable list of arguments
 * \return The number of characters actually written is returned
 */
int __printf(3,4) snprintf(char* buffer, uint32_t n, const char* fmt, ...);

#endif /* ! REDSHIFT_LIBC__STDIO_H */

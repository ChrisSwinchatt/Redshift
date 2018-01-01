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
#ifndef REDSHIFT_KERNEL_PRINTK_H
#define REDSHIFT_KERNEL_PRINTK_H

#include <redshift/util/macro.h>
#include <stdarg.h>

/** Print debug-level message. */
#define PRINTK_DEBUG   "<0>"

/** Print info-level message. */
#define PRINTK_INFO    "<1>"

/** Print warning message. */
#define PRINTK_WARNING "<8>"

/** Print error message. */
#define PRINTK_ERROR   "<9>"

/**
 * Print a string
 * \param fmt Formatted string. Can optionally include a PRINTK_* string.
 * \param ap Variable list of arguments
 * \return The number of characters printed is returned.
 */
int vprintk(const char* fmt, va_list ap);

/**
 * Print a string
 * \param fmt Formatted string. Can optionally include a PRINTK_* string
 * (see include/common/kernel.h)
 * \param ... Variable list of arguments
 * \return The number of characters printed is returned.
 */
int printk(const char* fmt, ...);

#endif /* ! REDSHIFT_KERNEL_PRINTK_H */

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
#ifndef REDSHIFT_LIBK_KABORT_H
#define REDSHIFT_LIBK_KABORT_H

#include <libk/kextern.h>
#include <libk/ktypes.h>

#define KABORT_BUFFER_SIZE 1024

/**
 * Print an error message and abort.
 */
#define KABORT(MSG, ...)                                    \
    do {                                                    \
        char __kabort_buffer__[KABORT_BUFFER_SIZE];         \
        const ssize_t __kabort_count__ = kstring_format(    \
            __kabort_buffer__,                              \
            KABORT_BUFFER_SIZE - 1,                         \
            "At " __FILE_LINE__ " in %s: " #MSG,            \
            __func__,                                       \
            __VA_ARGS__                                     \
        );                                                  \
        __kabort_buffer__[__kabort_count__] = 0;            \
        kextern_print_string(__kabort_buffer__);            \
        kextern_abort();                                    \
    } while (0)

/**
 * Report error caused by code that should be unreachable e.g. switch-case statements.
 * \param MSG A custom error message.
 * \param ... Variadic arguments for MSG, or NULL.
 */
#define UNREACHABLE(MSG, ...) KABORT("unreachable code - " MSG, __VA_ARGS__)

#endif /* ! REDSHIFT_LIBK_KABORT_H */

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
#ifndef REDSHIFT_UTIL_ASSERT_H
#define REDSHIFT_UTIL_ASSERT_H

#include <redshift/util/macro.h>

/**
 * Check an expression at runtime and trigger a kernel panic if it fails.
 * \param X The expression to check.
 */
#define RUNTIME_CHECK(X)\
    do {\
        if (!(X)) {\
            panic("check/assert failed \"" #X "\" at " __FILE_LINE__ " in %s\n", __func__);\
        }\
    } while (0)

#ifndef NDEBUG
/** Make a debugging assertion. @see RUNTIME_CHECK */
# define DEBUG_ASSERT(X) RUNTIME_CHECK(X)
#else
# define DEBUG_ASSERT(X) ((void)X)
#endif /* ! NDEBUG */

/**
 * Report error caused by code that should be unreachable e.g. switch-case statements.
 */
#define UNREACHABLE(msg, ...) panic(__FILE_LINE__ ": in %s: " #msg, __func__, __VA_ARGS__)

#endif /* ! REDSHIFT_UTIL_ASSERT_H */

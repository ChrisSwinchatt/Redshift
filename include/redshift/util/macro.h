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
#ifndef REDSHIFT_UTIL_MACRO_H
#define REDSHIFT_UTIL_MACRO_H

/** Stringify helper. */
#define __STRINGIFY_HELPER(X) #X

/** Stringify something. */
#define STRINGIFY(X) __STRINGIFY_HELPER(X)

/** Current line number as a string. */
#define __LINE_NO__  STRINGIFY(__LINE__)

/** Current file and line number as a colon-delimited string. */
#define __FILE_LINE__ __FILE__ ":" __LINE_NO__

/** Suppress unused parameter/variable warning. */
#define UNUSED(X)                   ((void)(X))

/** Mark a structure as having packed storage. */
#undef  __packed
#define __packed                    __attribute__((packed))

/** Mark a function as using printf-like formatting. */
#undef  __printf
#define __printf(FMT, ARGS)         __attribute__((format(printf,FMT,ARGS)))

/** Mark a function as "always inline". */
#undef  __always_inline
#define __always_inline             __attribute__((always_inline))

/** Mark a function as "no inline". */
#undef  __noinline
#define __noinline                  __attribute__((noinline))

/** Mark a function as "no return". */
#undef  __noreturn
#define __noreturn                  __attribute__((noreturn))

/** Mark a function as non-reentrant. */
#define __non_reentrant

#define FALL_THROUGH                __attribute__((fallthrough))

#define DO_NOTHING                 do {} while (0)

/** Find the number of elements in 'array' if known at compile-time. */
#define ARRAY_SIZE(array)            (sizeof(array)/sizeof(*(array)))

/** Check if a flag is present in a bitflags variable. */
#define TEST_FLAG(var, flag)        ((var & flag) == flag)

/** Test the bit at 'pos' in 'var' */
#define TEST_BIT(var, pos)          ((var) & (1 << (pos)))

/** Flip the bit at 'pos' in 'var' */
#define FLIP_BIT(var, pos)          ((var) ^=  (1 << (pos)))

/** Set the bit at 'pos' in 'var' */
#define SET_BIT(var, pos)           ((var) |=  (1 << (pos)))

/** Clear the bit at 'pos' in 'var' */
#define CLEAR_BIT(var, pos)         ((var) &= ~(1 << (pos)))

#endif /* ! REDSHIFT_UTIL_MACRO_H */

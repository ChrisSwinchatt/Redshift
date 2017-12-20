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
#ifndef REDSHIFT_KERNEL_REDSHIFT_H
#define REDSHIFT_KERNEL_REDSHIFT_H

#include <kernel/asm.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/**
 * PIT tick rate (Hz).
 * As this is used in calculations (see sleep.c), it should stay within reasonable limits ([10,1000)).
 */
#define TICK_RATE 100

#define RUNTIME_CHECK(X)\
    do {\
        if (!(X))\
            panic("failed assertion \"%s\" at %s:%d in %s\n", #X, __FILE__, __LINE__, __func__);\
    } while (0)

#ifndef NDEBUG
/** Make a debug assertion. */
# define DEBUG_ASSERT(X) RUNTIME_CHECK(X)
#else
# define DEBUG_ASSERT(X) ((void)x)
#endif /* ! NDEBUG */

/**
 * Report error caused by code that should be unreachable e.g. switch-case statements.
 */
#define REDSHIFT_UNREACHABLE(msg, ...) panic(__FILE__ ":%d: in %s: ", __LINE__, #msg, __func__, __VA_ARGS__)

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

/** Find the number of elements in 'array' if known at compile-time. */
#define ARRAY_SIZE(array)            (sizeof(array)/sizeof(*(array)))

/** Check if a flag is present in a bitflags variable. */
#define test_flag(var, flag)        ((var & flag) == flag)

/** Test the bit at 'pos' in 'var' */
#define test_bit(var, pos)          ((var) & (1 << (pos)))

/** Flip the bit at 'pos' in 'var' */
#define flip_bit(var, pos)          ((var) ^=  (1 << (pos)))

/** Set the bit at 'pos' in 'var' */
#define set_bit(var, pos)           ((var) |=  (1 << (pos)))

/** Clear the bit at 'pos' in 'var' */
#define clear_bit(var, pos)         ((var) &= ~(1 << (pos)))

/** Logical XOR. */
#define lxor(a, b)                  (!(a) != !(b))

/** Suppress unused parameter/variable warning. */
#define UNUSED(X)                   ((void)(X))

/** Print debug-level message. */
#define PRINTK_DEBUG   "<0>"

/** Print info-level message. */
#define PRINTK_INFO    "<1>"

/** Print warning message. */
#define PRINTK_WARNING "<8>"

/** Print error message. */
#define PRINTK_ERROR   "<9>"

/** Hang the computer forever. */
extern void __noreturn hang(void);

struct sysinfo;
extern struct sysinfo __sysinfo__; /* boot/sysinfo.c */

struct heap;
extern struct heap* __kernel_heap__; /* mem/heap.c */

/** Type for linker script symbols. These are just addresses, so we define it as an array type because arrays
 * are handled as just addresses by default. */
typedef int symbol_t[];

/**
 * \brief Print a string
 * \param fmt Formatted string. Can optionally include a PRINTK_* string.
 * \param ap Variable list of arguments
 * \return The number of characters printed is returned.
 */
int vprintk(const char* fmt, va_list ap);

/**
 * \brief Print a string
 * \param fmt Formatted string. Can optionally include a PRINTK_* string
 * (see include/common/kernel/redshift.h)
 * \param ... Variable list of arguments
 * \return The number of characters printed is returned.
 */
int __printf(1,2) printk(const char* fmt, ...);

/**
 * \brief Print an error message and then loop infinitely
 * \param fmt Formatted error message
 * \param ... Format arguments
 * \return This function never returns
 */
void __noreturn __printf(1,2) panic(const char* fmt, ...);

#endif /* ! REDSHIFT_KERNEL_REDSHIFT_H */

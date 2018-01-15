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
#define __STRINGIFY_HELPER(X)       #X

/** Stringify something. */
#define STRINGIFY(X)                __STRINGIFY_HELPER(X)

/** Current line number as a string. */
#define __LINE_NO__                 STRINGIFY(__LINE__)

/** Current file and line number as a colon-delimited string. */
#define __FILE_LINE__               __FILE__ ":" __LINE_NO__

/** Concatenate two tokens. */
#define CONCAT(A, B)                A ## B

/** Define a compile-time integer constant. */
#define INTEGER_CONSTANT(ID, VALUE) enum { ID = VALUE }

/** Generate a unique identifier (one per line). */
#define UNIQUE_ID(ID)               ID ## __FILE__ ## __LINE_NO__

/** Return the biggest value out of A and B. */
#define MAX(A, B)                   ((A) > (B) ? (A) : (B))

/** Return the smallest value out of A and B. */
#define MIN(A, B)                   ((A) < (B) ? (A) : (B))

/** Suppress unused parameter/variable warning. */
#define UNUSED(X)                   ((void)(X))

/** Tell the compiler that an expression is likely to evaluate to true. */
#define LIKELY(X)                   (__builtin_expect((X), 1))

/** Tell the compiler that an expression is likely to evaluate to false. */
#define UNLIKELY(X)                 (__builtin_expect((X), 0))

/** Fall through e.g. to the next case label without triggering a warning. */
#define FALL_THROUGH                __attribute__((fallthrough))

/** Do nothing descriptively. Use for "NOP" instead of ;. */
#define DO_NOTHING                  do {} while (0)

/** Prevent compiler from changing structure alignment. */
#undef  __packed
#define __packed                    __attribute__((packed))

/** Mark a function as using printf-like formatting. */
#undef  __printf
#define __printf(FMT, ARGS)         __attribute__((format(printf,FMT,ARGS)))

/** Force compiler to inline a function. */
#undef  __always_inline
#define __always_inline             __attribute__((always_inline))

/** Prevent compiler from inlining a function. */
#undef  __noinline
#define __noinline                  __attribute__((noinline))

/** Mark a function as non-returning. */
#undef  __noreturn
#define __noreturn                  __attribute__((noreturn))

/** Mark a function as non-reentrant. */
#define __non_reentrant

/**
 * Mark a function as a constructor. Constructors are called in sequence during boot, before the scheduler is started.
 * \param PRIORITY Controls the order in which constructors are called (smallest first).
 */
#define __init(PRIORITY)            __attribute__((constructor(PRIORITY)))

/**
 * Mark a function as a destructor. Destructor are called in sequence during shutdown.
 * \param PRIORITY Controls the order in which destructors are called (largest first).
 */
#define __fini(PRIORITY)            __attribute__((destructor(PRIORITY)))

/**
 * Save interrupt state and disable interrupts. Can only be done once per function, ideally before doing anything else.
 */
#define SAVE_INTERRUPT_STATE        int __saved_interrupt_state__ = get_interrupt_state(); disable_interrupts()

/** Restore previous interrupt state. Can only be done after SAVE_INTERRUPT_STATE. */
#define RESTORE_INTERRUPT_STATE     do { if (__saved_interrupt_state__) { enable_interrupts(); } } while (0)

/** Find the number of elements in 'array' if known at compile-time. */
#define ARRAY_SIZE(ARRAY)           (sizeof(ARRAY)/sizeof(*(ARRAY)))

/** Check if a flag is present in a bitflags variable. */
#define TEST_FLAG(VAR, FLAG)        ((VAR & FLAG) == FLAG)

/** Test the bit at 'pos' in 'VAR' */
#define TEST_BIT(VAR, POS)          ((VAR) &   (1 << (POS)))

/** Flip the bit at 'pos' in 'VAR' */
#define FLIP_BIT(VAR, POS)          ((VAR) ^=  (1 << (POS)))

/** Set the bit at 'pos' in 'VAR' */
#define SET_BIT(VAR, POS)           ((VAR) |=  (1 << (POS)))

/** Clear the bit at 'pos' in 'VAR' */
#define CLEAR_BIT(VAR, POS)         ((VAR) &= ~(1 << (POS)))

#endif /* ! REDSHIFT_UTIL_MACRO_H */

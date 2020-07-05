/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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
#ifndef REDSHIFT_LIBK_KASSERT_H
#define REDSHIFT_LIBK_KASSERT_H

#include <libk/kabort.h>
#include <libk/kmacro.h>
#include <libk/kstring.h>

/**
 * Print error message and abort upon failed check/assertion.
 * \param NAME "check" or "assertion".
 * \param MSG A custom error message.
 * \param ... Variadic arguments for MSG, or NULL.
 */
#define __KASSERT_CHECK_FAILED(NAME, MSG, ...)  KABORT(NAME " failed - " MSG, __VA_ARGS__)

/**
 * Check an expression and call __KASSERT_CHECK_FAILED if it evaluates to false.
 * \param NAME "check" or "assertion"
 * \param X The expression.
 */
#define __KASSERT_DO_CHECK(NAME, X)                 \
    do {                                            \
        const bool  __kassert_eval_once__ = (X);    \
        if (!(__kassert_eval_once__)) {             \
            __KASSERT_CHECK_FAILED(NAME, #X, NULL); \
        }                                           \
    } while (0)

/**
 * Compare two expressions and call __KASSERT_CHECK_FAILED if the comparison fails.
 * \param NAME "check" or "assertion"
 * \param A The first expression.
 * \param B The second expression.
 * \param T The type of each expression.
 * \param FMT kstring_format format specifier.
 */
#define __KASSERT_CHECK(NAME, A, OP, B, T, FMT)                      \
    do {                                                             \
        const T __kassert_eval_once_a__ = (A);                       \
        const T __kassert_eval_once_b__ = (B);                       \
        if (!(__kassert_eval_once_a__ OP __kassert_eval_once_b__)) { \
            __KASSERT_CHECK_FAILED(                                  \
                NAME,                                                \
                #A " " #OP " " #B ": expected " FMT ", got " FMT,    \
                 __kassert_eval_once_a__,                            \
                 __kassert_eval_once_b__                             \
             );                                                      \
        }                                                            \
    } while (0)

/**
 * Check two strings for equality and call KASSERT_CHECK_FAILED if they are not equal.
 * \param NAME "check" or "assertion"
 * \param A The first string.
 * \param B The second string.
 */
#define __KASSERT_CHECK_EQUAL_STRING(NAME, A, B)                                       \
    do {                                                                               \
        const char* __kassert_eval_once_a__ = A;                                       \
        const char* __kassert_eval_once_b__ = B;                                       \
        if (kstring_compare(__kassert_eval_once_a__, __kassert_eval_once_b__) != 0) {  \
            __KASSERT_CHECK_FAILED(                                                    \
                NAME,                                                                  \
                "expected %s, got %s",                                                 \
                __kassert_eval_once_a__,                                               \
                __kassert_eval_once_b__                                                \
            );                                                                         \
        }                                                                              \
    } while (0)

/**
 * Check an expression at runtime and trigger a kernel panic if it fails.
 * \param X The expression to check.
 * @see __CHECK_ASSERT
 */
#define RUNTIME_CHECK(X)                  __KASSERT_DO_CHECK("check", X)

#define RUNTIME_CHECK_INT8_T(A,    OP, B) __KASSERT_CHECK("check",  A, OP, B, int8_t,    "%hhd")
#define RUNTIME_CHECK_INT16_T(A,   OP, B) __KASSERT_CHECK("check",  A, OP, B, int16_t,   "%hd")
#define RUNTIME_CHECK_INT32_T(A,   OP, B) __KASSERT_CHECK("check",  A, OP, B, int32_t,   "%d")
#define RUNTIME_CHECK_INT64_T(A,   OP, B) __KASSERT_CHECK("check",  A, OP, B, int64_t,   "%qd")
#define RUNTIME_CHECK_INTMAX_T(A,  OP, B) __KASSERT_CHECK("check",  A, OP, B, intmax_t,  "%jd")
#define RUNTIME_CHECK_SSIZE_T(A,   OP, B) __KASSERT_CHECK("check",  A, OP, B, ssize_t,   "%zd")
#define RUNTIME_CHECK_PTRDIFF_T(A, OP, B) __KASSERT_CHECK("check",  A, OP, B, ptrdiff_t, "%td")
#define RUNTIME_CHECK_UINT8_T(A,   OP, B) __KASSERT_CHECK("check",  A, OP, B, uint8_t,   "%hhu")
#define RUNTIME_CHECK_UINT16_T(A,  OP, B) __KASSERT_CHECK("check",  A, OP, B, uint16_t,  "%hu")
#define RUNTIME_CHECK_UINT32_T(A,  OP, B) __KASSERT_CHECK("check",  A, OP, B, uint32_t,  "%u")
#define RUNTIME_CHECK_UINT64_T(A,  OP, B) __KASSERT_CHECK("check",  A, OP, B, uint64_t,  "%qu")
#define RUNTIME_CHECK_UINTMAX_T(A, OP, B) __KASSERT_CHECK("check",  A, OP, B, uintmax_t, "%ju")
#define RUNTIME_CHECK_SIZE_T(A,    OP, B) __KASSERT_CHECK("check",  A, OP, B, size_t,    "%zu")
#define RUNTIME_CHECK_PTR(A,       OP, B) __KASSERT_CHECK("check",  A, OP, B, void*,     "%#p")
#define RUNTIME_CHECK_BIN8(A,      OP, B) __KASSERT_CHECK("check",  A, OP, B, uint8_t,   "%#08hhb")
#define RUNTIME_CHECK_BIN16(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint16_t,  "%#016hb")
#define RUNTIME_CHECK_BIN32(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint32_t,  "%#032b")
#define RUNTIME_CHECK_BIN64(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint64_t,  "%#064qb")
#define RUNTIME_CHECK_OCT8(A,      OP, B) __KASSERT_CHECK("check",  A, OP, B, uint8_t,   "%#hho")
#define RUNTIME_CHECK_OCT16(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint16_t,  "%#ho")
#define RUNTIME_CHECK_OCT32(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint32_t,  "%#o")
#define RUNTIME_CHECK_OCT64(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint64_t,  "%#qo")
#define RUNTIME_CHECK_HEX8(A,      OP, B) __KASSERT_CHECK("check",  A, OP, B, uint8_t,   "%#02hhX")
#define RUNTIME_CHECK_HEX16(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint16_t,  "%#04hX")
#define RUNTIME_CHECK_HEX32(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint32_t,  "%#08X")
#define RUNTIME_CHECK_HEX64(A,     OP, B) __KASSERT_CHECK("check",  A, OP, B, uint64_t,  "%#016qX")
#define RUNTIME_CHECK_CHAR(A,      OP, B) __KASSERT_CHECK("check",  A, OP, B, char,      "%c")
#define RUNTIME_CHECK_EQUAL_STRING(A,  B) __KASSERT_CHECK_EQUAL_STRING("check", A, B)

#ifndef NDEBUG
/** Make a debugging assertion. @see __KASSERT_DO_CHECK */
# define DEBUG_ASSERT(X)                 __KASSERT_DO_CHECK("assertion", X)

# define DEBUG_ASSERT_INT8_T(A,    OP, B) __KASSERT_CHECK("assertion",  A, OP, B, int8_t,    "%hhd")
# define DEBUG_ASSERT_INT16_T(A,   OP, B) __KASSERT_CHECK("assertion",  A, OP, B, int16_t,   "%hd")
# define DEBUG_ASSERT_INT32_T(A,   OP, B) __KASSERT_CHECK("assertion",  A, OP, B, int32_t,   "%d")
# define DEBUG_ASSERT_INT64_T(A,   OP, B) __KASSERT_CHECK("assertion",  A, OP, B, int64_t,   "%qd")
# define DEBUG_ASSERT_INTMAX_T(A,  OP, B) __KASSERT_CHECK("assertion",  A, OP, B, intmax_t,  "%jd")
# define DEBUG_ASSERT_SSIZE_T(A,   OP, B) __KASSERT_CHECK("assertion",  A, OP, B, ssize_t,   "%zd")
# define DEBUG_ASSERT_PTRDIFF_T(A, OP, B) __KASSERT_CHECK("assertion",  A, OP, B, ptrdiff_t, "%td")
# define DEBUG_ASSERT_UINT8_T(A,   OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint8_t,   "%hhu")
# define DEBUG_ASSERT_UINT16_T(A,  OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint16_t,  "%hu")
# define DEBUG_ASSERT_UINT32_T(A,  OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint32_t,  "%u")
# define DEBUG_ASSERT_UINT64_T(A,  OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint64_t,  "%qu")
# define DEBUG_ASSERT_UINTMAX_T(A, OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uintmax_t, "%ju")
# define DEBUG_ASSERT_SIZE_T(A,    OP, B) __KASSERT_CHECK("assertion",  A, OP, B, size_t,    "%zu")
# define DEBUG_ASSERT_PTR(A,       OP, B) __KASSERT_CHECK("assertion",  A, OP, B, void*,     "%#p")
# define DEBUG_ASSERT_BIN8(A,      OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint8_t,   "%#08hhb")
# define DEBUG_ASSERT_BIN16(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint16_t,  "%#016hb")
# define DEBUG_ASSERT_BIN32(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint32_t,  "%#032b")
# define DEBUG_ASSERT_BIN64(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint64_t,  "%#064qb")
# define DEBUG_ASSERT_OCT8(A,      OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint8_t,   "%#hho")
# define DEBUG_ASSERT_OCT16(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint16_t,  "%#ho")
# define DEBUG_ASSERT_OCT32(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint32_t,  "%#o")
# define DEBUG_ASSERT_OCT64(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint64_t,  "%#qo")
# define DEBUG_ASSERT_HEX8(A,      OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint8_t,   "%#02hhX")
# define DEBUG_ASSERT_HEX16(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint16_t,  "%#04hX")
# define DEBUG_ASSERT_HEX32(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint32_t,  "%#08X")
# define DEBUG_ASSERT_HEX64(A,     OP, B) __KASSERT_CHECK("assertion",  A, OP, B, uint64_t,  "%#016qX")
# define DEBUG_ASSERT_CHAR(A,      OP, B) __KASSERT_CHECK("assertion",  A, OP, B, char,      "%c")
# define DEBUG_ASSERT_EQUAL_STRING(A,  B) __KASSERT_CHECK_EQUAL_STRING("assertion", A, OP, B)
#else
# define DEBUG_ASSERT(X)                 UNUSED((X))
# define DEBUG_ASSERT_INT8_T(A,    OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_INT16_T(A,   OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_INT32_T(A,   OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_INT64_T(A,   OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_INTMAX_T(A,  OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_SSIZE_T(A,   OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_PTRDIFF_T(A, OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_UINT8_T(A,   OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_UINT16_T(A,  OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_UINT32_T(A,  OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_UINT64_T(A,  OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_UINTMAX_T(A, OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_SIZE_T(A,    OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_PTR(A,       OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_BIN8(A,      OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_BIN16(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_BIN32(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_BIN64(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_OCT8(A,      OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_OCT16(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_OCT32(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_OCT64(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_HEX8(A,      OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_HEX16(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_HEX32(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_HEX64(A,     OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_CHAR(A,      OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define DEBUG_ASSERT_STRING(A,    OP, B)    do { UNUSED(A); UNUSED(B); } while (0)
#endif /* ! NDEBUG */

#endif /* ! REDSHIFT_LIBK_KASSERT_H */

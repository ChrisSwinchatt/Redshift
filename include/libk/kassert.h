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
 * Check two expressions for equality and call __KASSERT_CHECK_FAILED if they are not equal.
 * \param NAME "check" or "assertion"
 * \param A The first expression.
 * \param B The second expression.
 * \param T The type of each expression.
 * \param FMT kstring_format format specifier.
 */
#define __KASSERT_CHECK_EQUAL(NAME, A, B, T, FMT)                   \
    do {                                                            \
        const T __kassert_eval_once_a__ = (A);                      \
        const T __kassert_eval_once_b__ = (B);                      \
        if (__kassert_eval_once_a__ != __kassert_eval_once_b__) {   \
            __KASSERT_CHECK_FAILED(                                 \
                NAME,                                               \
                "expected " FMT ", got " FMT,                       \
                 __kassert_eval_once_a__,                           \
                 __kassert_eval_once_b__                            \
             );                                                     \
        }                                                           \
    } while (0)

/**
 * Check two strings for equality and call KASSERT_CHECK_FAILED if they are not equal.
 * \param NAME "check" or "assertion"
 * \param A The first string.
 * \param B The second string.
 */
#define __KASSERT_CHECK_STRING(NAME, A, B)                                              \
    do {                                                                                \
        const char* __kassert_eval_once_a__ = A;                                        \
        const char* __kassert_eval_once_b__ = B;                                        \
        if (kstring_compare(__kassert_eval_once_a__, __kassert_eval_once_b__) != 0) {   \
            __KASSERT_CHECK_FAILED(                                                     \
                NAME,                                                                   \
                "expected %s, got %s",                                                  \
                __kassert_eval_once_a__,                                                \
                __kassert_eval_once_b__                                                 \
            );                                                                          \
        }                                                                               \
    } while (0)

/**
 * Check an expression at runtime and trigger a kernel panic if it fails.
 * \param X The expression to check.
 * @see __CHECK_ASSERT
 */
#define RUNTIME_CHECK(X)                __KASSERT_DO_CHECK("check", X)

#define CHECK_EQUAL_INT8_T(A,    B)    __KASSERT_CHECK_EQUAL("check",  A, B, int8_t,    "%hhd")
#define CHECK_EQUAL_INT16_T(A,   B)    __KASSERT_CHECK_EQUAL("check",  A, B, int16_t,   "%hd")
#define CHECK_EQUAL_INT32_T(A,   B)    __KASSERT_CHECK_EQUAL("check",  A, B, int32_t,   "%d")
#define CHECK_EQUAL_INT64_T(A,   B)    __KASSERT_CHECK_EQUAL("check",  A, B, int64_t,   "%qd")
#define CHECK_EQUAL_INTMAX_T(A,  B)    __KASSERT_CHECK_EQUAL("check",  A, B, intmax_t,  "%jd")
#define CHECK_EQUAL_SSIZE_T(A,   B)    __KASSERT_CHECK_EQUAL("check",  A, B, ssize_t,   "%zd")
#define CHECK_EQUAL_PTRDIFF_T(A, B)    __KASSERT_CHECK_EQUAL("check",  A, B, ptrdiff_t, "%td")
#define CHECK_EQUAL_UINT8_T(A,   B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint8_t,   "%hhu")
#define CHECK_EQUAL_UINT16_T(A,  B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint16_t,  "%hu")
#define CHECK_EQUAL_UINT32_T(A,  B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint32_t,  "%u")
#define CHECK_EQUAL_UINT64_T(A,  B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint64_t,  "%qu")
#define CHECK_EQUAL_UINTMAX_T(A, B)    __KASSERT_CHECK_EQUAL("check",  A, B, uintmax_t, "%ju")
#define CHECK_EQUAL_SIZE_T(A,    B)    __KASSERT_CHECK_EQUAL("check",  A, B, size_t,    "%zu")
#define CHECK_EQUAL_PTR(A,       B)    __KASSERT_CHECK_EQUAL("check",  A, B, void*,     "%#p")
#define CHECK_EQUAL_BIN8(A,      B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint8_t,   "%#08hhb")
#define CHECK_EQUAL_BIN16(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint16_t,  "%#016hb")
#define CHECK_EQUAL_BIN32(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint32_t,  "%#032b")
#define CHECK_EQUAL_BIN64(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint64_t,  "%#064qb")
#define CHECK_EQUAL_OCT8(A,      B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint8_t,   "%#hho")
#define CHECK_EQUAL_OCT16(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint16_t,  "%#ho")
#define CHECK_EQUAL_OCT32(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint32_t,  "%#o")
#define CHECK_EQUAL_OCT64(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint64_t,  "%#qo")
#define CHECK_EQUAL_HEX8(A,      B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint8_t,   "%#02hhX")
#define CHECK_EQUAL_HEX16(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint16_t,  "%#04hX")
#define CHECK_EQUAL_HEX32(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint32_t,  "%#08X")
#define CHECK_EQUAL_HEX64(A,     B)    __KASSERT_CHECK_EQUAL("check",  A, B, uint64_t,  "%#016qX")
#define CHECK_EQUAL_CHAR(A,      B)    __KASSERT_CHECK_EQUAL("check",  A, B, char,      "%c")
#define CHECK_EQUAL_STRING(A,    B)    __KASSERT_CHECK_STRING("check", A, B)

#ifndef NDEBUG
/** Make a debugging assertion. @see __KASSERT_DO_CHECK */
# define DEBUG_ASSERT(X)                __KASSERT_DO_CHECK("assertion", X)

# define ASSERT_EQUAL_INT8_T(A,    B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, int8_t,    "%hhd")
# define ASSERT_EQUAL_INT16_T(A,   B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, int16_t,   "%hd")
# define ASSERT_EQUAL_INT32_T(A,   B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, int32_t,   "%d")
# define ASSERT_EQUAL_INT64_T(A,   B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, int64_t,   "%qd")
# define ASSERT_EQUAL_INTMAX_T(A,  B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, intmax_t,  "%jd")
# define ASSERT_EQUAL_SSIZE_T(A,   B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, ssize_t,   "%zd")
# define ASSERT_EQUAL_PTRDIFF_T(A, B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, ptrdiff_t, "%td")
# define ASSERT_EQUAL_UINT8_T(A,   B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint8_t,   "%hhu")
# define ASSERT_EQUAL_UINT16_T(A,  B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint16_t,  "%hu")
# define ASSERT_EQUAL_UINT32_T(A,  B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint32_t,  "%u")
# define ASSERT_EQUAL_UINT64_T(A,  B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint64_t,  "%qu")
# define ASSERT_EQUAL_UINTMAX_T(A, B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uintmax_t, "%ju")
# define ASSERT_EQUAL_SIZE_T(A,    B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, size_t,    "%zu")
# define ASSERT_EQUAL_PTR(A,       B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, void*,     "%#p")
# define ASSERT_EQUAL_BIN8(A,      B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint8_t,   "%#08hhb")
# define ASSERT_EQUAL_BIN16(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint16_t,  "%#016hb")
# define ASSERT_EQUAL_BIN32(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint32_t,  "%#032b")
# define ASSERT_EQUAL_BIN64(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint64_t,  "%#064qb")
# define ASSERT_EQUAL_OCT8(A,      B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint8_t,   "%#hho")
# define ASSERT_EQUAL_OCT16(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint16_t,  "%#ho")
# define ASSERT_EQUAL_OCT32(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint32_t,  "%#o")
# define ASSERT_EQUAL_OCT64(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint64_t,  "%#qo")
# define ASSERT_EQUAL_HEX8(A,      B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint8_t,   "%#02hhX")
# define ASSERT_EQUAL_HEX16(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint16_t,  "%#04hX")
# define ASSERT_EQUAL_HEX32(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint32_t,  "%#08X")
# define ASSERT_EQUAL_HEX64(A,     B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, uint64_t,  "%#016qX")
# define ASSERT_EQUAL_CHAR(A,      B)    __KASSERT_CHECK_EQUAL("assertion",  A, B, char,      "%c")
# define ASSERT_EQUAL_STRING(A,    B)    __KASSERT_CHECK_STRING("assertion", A, B)
#else
# define DEBUG_ASSERT(X) ((void)X)
# define ASSERT_EQUAL_INT8_T(A,    B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_INT16_T(A,   B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_INT32_T(A,   B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_INT64_T(A,   B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_INTMAX_T(A,  B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_SSIZE_T(A,   B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_PTRDIFF_T(A, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_UINT8_T(A,   B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_UINT16_T(A,  B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_UINT32_T(A,  B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_UINT64_T(A,  B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_UINTMAX_T(A, B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_SIZE_T(A,    B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_PTR(A,       B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_BIN8(A,      B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_BIN16(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_BIN32(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_BIN64(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_OCT8(A,      B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_OCT16(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_OCT32(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_OCT64(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_HEX8(A,      B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_HEX16(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_HEX32(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_HEX64(A,     B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_CHAR(A,      B)    do { UNUSED(A); UNUSED(B); } while (0)
# define ASSERT_EQUAL_STRING(A,    B)    do { UNUSED(A); UNUSED(B); } while (0)
#endif /* ! NDEBUG */

#endif /* ! REDSHIFT_LIBK_KASSERT_H */

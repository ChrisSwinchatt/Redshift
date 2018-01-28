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
#ifndef REDSHIFT_LIBK_KCOPY_H
#define REDSHIFT_LIBK_KCOPY_H

#include <stdint.h>

/**************************************
 * KCOPY_FORWARD                       *
 **************************************/
/** Forwards byte-wise copy. */
#define KCOPY_FORWARD_8(DST, SRC, N)                            \
    do {                                                        \
        uint8_t* dst8 = (uint8_t*)(DST);                        \
        const uint8_t* src8 = (const uint8_t*)(SRC);            \
        for (size_t i8 = 0; i8 < (N); ++i8, ++dst8, ++src8) {   \
            *dst8 = *src8;                                      \
        }                                                       \
    } while (0)

/** Forwards word-wise copy. */
#define KCOPY_FORWARD_16(DST, SRC, N)                                               \
    do {                                                                            \
        uint16_t* dst16 = (uint16_t*)(DST);                                         \
        const uint16_t* src16 = (const uint16_t*)(SRC);                             \
        for (size_t i16 = 0; i16 < (N)/sizeof(uint16_t); ++i16, ++dst16, ++src16) { \
            *dst16 = *src16;                                                        \
        }                                                                           \
        KCOPY_FORWARD_8(dst16, src16, N%sizeof(uint16_t));                          \
    } while (0)

/** Forwards dword-wise copy. */
#define KCOPY_FORWARD_32(DST, SRC, N)                                               \
    do {                                                                            \
        uint32_t* dst32 = (uint32_t*)(DST);                                         \
        const uint32_t* src32 = (const uint32_t*)(SRC);                             \
        for (size_t i32 = 0; i32 < (N)/sizeof(uint32_t); ++i32, ++dst32, ++src32) { \
            *dst32 = *src32;                                                        \
        }                                                                           \
        KCOPY_FORWARD_16(dst32, src32, N%sizeof(uint32_t));                         \
    } while (0)

/** Forwards qword-wise copy. */
#define KCOPY_FORWARD_64(DST, SRC, N)                                               \
    do {                                                                            \
        uint64_t* dst64 = (uint64_t*)(DST);                                         \
        const uint64_t* src64 = (const uint64_t*)(SRC);                             \
        for (size_t i64 = 0; i64 < (N)/sizeof(uint64_t); ++i64, ++dst64, ++src64) { \
            *dst64 = *src64;                                                        \
        }                                                                           \
        KCOPY_FORWARD_32(dst64, src64, N%sizeof(uint64_t));                         \
    } while (0)

/**
 * Forwards copy. Copies N bytes of memory forwards from SRC to DST. Reduces number of loop iterations by copying
 * qwords, dwords, words and finally bytes.
 * \param DST Destination buffer.
 * \param SRC Source buffer.
 * \param N Number of bytes to copy.
 */
#define KCOPY_FORWARD(DST, SRC, N)                          \
    do {                                                    \
        for (; ((DST) & 3) != 0; ++(DST), ++(SRC), --(N)) { \
            *(DST) = *(SRC);                                \
        }                                                   \
        KCOPY_FORWARD_64((DST), (SRC), (N))                 \
    } while (0)

/**************************************
 * KCOPY_BACKWARD                      *
 **************************************/
 /** Backwards byte-wise copy. */
#define KCOPY_BACKWARD_8(DST, SRC, N)                                               \
    do {                                                                            \
        uint8_t* dst8 = ((uint8_t*)(DST) + (N));                                    \
        const uint8_t* src8 = (const uint8_t*)((SRC) + (N));                        \
        for (size_t i8 = 0; i8 < (N); ++i8, --dst8, --src8) {                       \
            *dst8 = *src8;                                                          \
        }                                                                           \
    } while (0)

/** Backwards word-wise copy. */
#define KCOPY_BACKWARD_16(DST, SRC, N)                                              \
    do {                                                                            \
        uint16_t* dst16 = ((uint16_t*)(DST) + (N));                                 \
        const uint16_t* src16 = (const uint16_t*)((SRC) + (N));                     \
        for (size_t i16 = 0; i16 < (N)/sizeof(uint16_t); ++i16, --dst16, --src16) { \
            *dst16 = *src16;                                                        \
        }                                                                           \
        KCOPY_BACKWARD_8((DST), (SRC), N%sizeof(uint16_t));                         \
    } while (0)

/** Backwards dword-wise copy. */
#define KCOPY_BACKWARD_32(DST, SRC, N)                                              \
    do {                                                                            \
        uint32_t* dst32 = ((uint32_t*)(DST) + (N));                                 \
        const uint32_t* src32 = (const uint32_t*)((SRC) + (N));                     \
        for (size_t i32 = 0; i32 < (N)/sizeof(uint32_t); ++i32, --dst32, --src32) { \
            *dst32 = *src32;                                                        \
        }                                                                           \
        KCOPY_BACKWARD_16((DST), (SRC), N%sizeof(uint32_t));                        \
    } while (0)

/** Backwards qword-wise copy. */
#define KCOPY_BACKWARD_64(DST, SRC, N)                                              \
    do {                                                                            \
        uint64_t* dst64 = ((uint64_t*)(DST) + (N));                                 \
        const uint64_t* src64 = (const uint64_t*)((SRC) + (N));                     \
        for (size_t i64 = 0; i64 < (N)/sizeof(uint64_t); ++i64, --dst64, --src64) { \
            *dst64 = *src64;                                                        \
        }                                                                           \
        KCOPY_BACKWARD_32((DST), (SRC), N%sizeof(uint64_t));                        \
    } while (0)

/**
 * Forwards copy. Copies N bytes of memory backwards from SRC to DST. Reduces number of loop iterations by copying
 * qwords, dwords, words and finally bytes.
 * \param DST Destination buffer.
 * \param SRC Source buffer.
 * \param N Number of bytes to copy.
 */
#define KCOPY_BACKWARD(DST, SRC, N)                         \
    do {                                                    \
        for (; ((DST) & 3) != 0; --(DST), --(SRC), --(N)) { \
            *(DST) = *(SRC);                                \
        }                                                   \
        KCOPY_BACKWARD_64((DST), (SRC), (N))                \
    } while (0)

#endif /* ! REDSHIFT_LIBK_KCOPY_H */

/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#ifndef REDSHIFT_LIBK_BITS_HPP
#define REDSHIFT_LIBK_BITS_HPP

#include <libk/types.hpp>

/// Check if a flag is present in a bitflags variable.
#define TEST_FLAG(VAR, FLAG)        ((VAR & FLAG) == FLAG)

/// Set a flag.
#define SET_FLAG(VAR, FLAG)         ((VAR) |= (FLAG))

/// Clear a flag.
#define CLEAR_FLAG(VAR, FLAG)       ((VAR) &= ~(FLAG))

/// Toggle a flag.
#define FLIP_FLAG(VAR, FLAG)        ((VAR) ^= (FLAG))

/// Test the bit at 'pos' in 'var'
#define TEST_BIT(VAR, POS)          (TEST_FLAG((VAR), (1U << (POS))))

/// Flip the bit at 'pos' in 'VAR'
#define FLIP_BIT(VAR, POS)          (FLIP_FLAG((VAR), (1U << (POS))))

/// Set the bit at 'pos' in 'VAR'
#define SET_BIT(VAR, POS)           (SET_FLAG((VAR), (1U << (POS))))

/// Clear the bit at 'pos' in 'VAR'
#define CLEAR_BIT(VAR, POS)         (CLEAR_FLAG((VAR), (1U << (POS))))

#define BIT_INDEX(a)                ((a)/(sizeof(uint32_t)*CHAR_BIT))

#define BIT_OFFSET(a)               ((a)%(sizeof(uint32_t)*CHAR_BIT))

#define U8_TO_U16(B1, B0)           ()

#define U8_TO_U32(B3, B2, B1, B0)   U16_TO_U32(U8_TO_U16(B3, B2), U8_TO_U16(B1, B0))

#define U8_TO_U64(B7, B6, B5, B4, B3, B2, B1, B0)           \
    U32_TO_U64(                                             \
        U16_TO_U32(U8_TO_U16(B7, B6), U8_TO_U16(B5, B4)),   \
        U16_TO_U32(U8_TO_U16(B3, B2), U8_TO_U16(B1, B0))    \
    )

#define U16_TO_U32(W1, W0)          ((static_cast<uint32_t>(W1) << 32UL)  | (static_cast<uint32_t>(W0) & 0xFFFFUL))

#define U16_TO_U64(W3, W2, W1, W0)  U32_TO_U64(U16_TO_U32(W3, W2), U16_TO_U32(W1, W0))

#define U32_TO_U64(D1, D0)          ((static_cast<uint64_t>(D1) << 64ULL) | (static_cast<uint64_t>(D0) & 0xFFFFFFFFULL))

#endif // ! REDSHIFT_LIBK_BITS_HPP

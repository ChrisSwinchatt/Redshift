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
#ifndef REDSHIFT_LIBK_CHAR_HPP
#define REDSHIFT_LIBK_CHAR_HPP

#include <libk/macro.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Alphabetic case.
    enum class char_case { upper, lower };

    /// Character class.
    enum class char_class {
        alnum  = 1 <<  0,
        alpha  = 1 <<  1,
        blank  = 1 <<  2,
        cntrl  = 1 <<  3,
        digit  = 1 <<  4,
        graph  = 1 <<  5,
        lower  = 1 <<  6,
        print  = 1 <<  7,
        punct  = 1 <<  8,
        space  = 1 <<  9,
        upper  = 1 << 10,
        xdigit = 1 << 11
    };

#include <libk/detail/char_table.hpp>

    /// Character.
    struct char_ {
        /// The character's value.
        const char value;

        char_(int c) : value(c) {}

        /// True if the character is alphanumeric.
        bool is_alnum()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::alnum));
        }

        /// True if the character is alphabetic.
        bool is_alpha()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::alpha));
        }

        /// True if the character is blank (i.e. is a space or control character).
        bool is_blank()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::blank));
        }

        /// True if the character is a control character.
        bool is_control()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::cntrl));
        }

        /// True if the character is a base-10 digit.
        bool is_digit()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::digit));
        }

        /// True if the character is graphical (printable and not blank).
        bool is_graphical()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::graph));
        }

        /// True if the character is a hexademical digit (0-9a-fA-F).
        bool is_hex_digit()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::xdigit));
        }

        /// True if the character is a lower-case alphabetic character.
        bool is_lower_case()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::lower));
        }

        /// True if the character is printable (not a control character).
        bool is_printable()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::print));
        }

        /// True if the character is punctuation.
        bool is_punctuation()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::punct));
        }

        /// True if the character is space, tab or newline.
        bool is_space()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::space));
        }

        /// True if the character is an upper-case alphabetic character.
        bool is_upper_case()
        {
            return TEST_FLAG(detail::char_table[static_cast<int>(value)], static_cast<int>(char_class::upper));
        }

        char to_lower_case()
        {
            if (!(is_alpha()) || is_lower_case()) {
                return value;
            }
            return ((value - 'A') + 'a');
        }

        char to_upper_case()
        {
            if (!(is_alpha()) || is_upper_case()) {
                return value;
            }
            return ((value - 'a') + 'A');
        }
    };
}

#endif /// ! REDSHIFT_LIBK_KCHAR_H

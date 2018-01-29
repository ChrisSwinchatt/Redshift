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
#ifndef REDSHIFT_LIBK_ASCIZ_H
#define REDSHIFT_LIBK_ASCIZ_H

#include <libk/char.hpp>
#include <libk/types.hpp>
#include <libk/variadic.hpp>

namespace libk { namespace asciz {
    /// Convert an integer to a string.
    /// \param buf Buffer to store the string in.
    /// \param n The maximum number of digits to store in buf.
    /// \param i Integer to convert.
    /// \param base The numeric base to use to represent the number. Must be >= 2.
    /// \param case_ Whether to use upper or lower case.
    /// \return The number of digits written.
    ssize_t format_intmax(char* buffer, size_t n, intmax_t i, int base, char_case case_);

    /// Convert an unsigned integer to a string.
    /// \param buf Buffer to store the string in.
    /// \param n The maximum number of digits to store in buf.
    /// \param u Unsigned integer to convert.
    /// \param base The numeric base to use to represent the number. Must be >= 2.
    /// \param case_ Whether to use upper or lower case.
    /// \return The number of digits written.
    ssize_t format_uintmax(char* buffer, size_t n, uintmax_t value, int base, char_case case_);

    /// Parse an integer encoded as a string.
    /// \param s The string containing the integer.
    /// \param base The numeric base used to represent the number. Must be >= 2.
    intmax_t parse_intmax(const char* s, int base);

    /// Generate a formatted string.
    /// \param buffer The buffer to store the formatted string.
    /// \param n The maximum number of bytes allowed in the buffer.
    /// \param fmt The format string. @see vformat
    /// \param ... The format arguments. May contain signed or unsigned integers, pointers, strings and single characters.
    /// Floating point is not supported.
    /// \return The number of bytes written to buffer if successful, otherwise -1.
    /// \note Destination buffer is always zero-terminated.
    ssize_t format(char* buffer, size_t n, const char* fmt, ...);

    /// Generate a formatted string.
    /// \param buffer The buffer to store the formatted string.
    /// \param n The maximum number of bytes allowed in the buffer.
    /// \param fmt A string which may contain printf-like format specifiers. (But floating point is not supported.)
    /// \param args The format arguments.
    /// \return The number of bytes written to buffer if successful, otherwise -1.
    /// \note Destination buffer is always zero-terminated.
    ssize_t vformat(char* buffer, size_t n, const char* fmt, va_list args);

    /// Compute the 32-bit hash of a string.
    /// \param s The string.
    /// \param n The size of the string.
    uint32_t hash32(const char* s, size_t n);

    /// Search for 'c' in 's'.
    /// \param s String to search.
    /// \param c Character to search for.
    /// \return A pointer to the first occurrence of 'c' in 's' or nullptr if 'c' is not
    /// found in 's'
    char* find_char(const char* s, int c);

    /// Find the lenth of 's'.
    /// \param s A string.
    /// \return The length of 's' is returned.
    size_t length(const char* s);

    /// Append max. 'n' characters from 'src' to 'dst'
    /// \param dst Destination buffer.
    /// \param src Source buffer.
    /// \param n Maximum number of characters to append.
    /// \return A pointer to the new end of 'dst' is returned.
    /// \note Destination buffer is always zero-terminated.
    char* concatenate(char* dst, const char* src, size_t n);

    /// Compare a maximum of 'n' characters from 's1' with 's2'.
    /// \param s1 A string.
    /// \param s2 Another string.
    /// \param n Maximum number of characters to compare.
    /// \return If the two strings are equal, 0 is returned. If two characters that
    /// don't match are encountered, (*s1 - *s2) is returned.
    int compare(const char* s1, const char* s2, size_t n);

    /// Copy a maximum of 'n' characters from 'src' to 'dst'.
    /// \param dst Destination string.
    /// \param src Source string.
    /// \param n Maximum number of characters to copy.
    /// \return A pointer to the end of 'dst' is returned.
    /// \note Destination buffer is always zero-terminated.
    char* copy(char* dst, const char* src, size_t n);

    /// Reverse a string.
    /// \param s String to be reversed.
    /// \param n The maximum size of s.
    /// \return The string is returned.
    /// \note Destination buffer is always zero-terminated.
    char* reverse(char* s, size_t n);

    /// Find the first instance of 's2' in 's1'.
    /// \param s1 String to search.
    /// \param s2 String to search for.
    /// \return A pointer to the first instance of 's2' in 's1' is returned, or nullptr if 's2' is not found in 's1'.
    /// \note Destination buffer is always zero-terminated.
    char* find_str(const char* s1, const char* s2);

    /// Converts any upper-case characters in a string to lower-case.
    /// \param s A string.
    /// \return A pointer to s is returned.
    /// \note Destination buffer is always zero-terminated.
    char* to_lower_case(char* s);

    /// Converts any lower-case characters in a string to upper-case.
    /// \param s A string.
    /// \return A pointer to s is returned.
    /// \note Destination buffer is always zero-terminated.
    char* to_upper_case(char* s);

    /// Duplicate a string.
    /// \param s The string to duplicate.
    /// \return A newly-allocated string which is identical to the original.
    /// \note Destination buffer is always zero-terminated.
    char* duplicate(const char* s);
}} // libk::asciz

#endif // ! REDSHIFT_LIBK_ASCIZ_H

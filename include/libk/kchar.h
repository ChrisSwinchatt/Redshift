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
#ifndef REDSHIFT_LIBK_KCHAR_H
#define REDSHIFT_LIBK_KCHAR_H

#include <libk/ktypes.h>

typedef enum {
    KCHAR_UPPER_CASE,
    KCHAR_LOWER_CASE
} kchar_case_t;

/**
 * Return true if the given character is alphanumeric, otherwise false.
 * \param c The character.
 * \return True if the given character is alphanumeric, otherwise false.
 */
bool kchar_is_alnum(int c);

/**
 * Return true if the given character is alphabetic, otherwise false.
 * \param c The character.
 * \return True if the given character is alphabetic, otherwise false.
 */
bool kchar_is_alpha(int c);

/**
 * Return true if the given character is non-printing, otherwise false.
 * \param c The character.
 * \return True if the given character is non-printing, otherwise false.
 */
bool kchar_is_blank(int c);

/**
 * Return true if the given character is a control character, otherwise false.
 * \param c The character.
 * \return True if the given character is a control character, otherwise false.
 */
bool kchar_is_control(int c);

/**
 * Return true if the given character is a digit 0-9, otherwise false.
 * \param c The character.
 * \return True if the given character is a digit 0-9, otherwise false.
 */
bool kchar_is_digit(int c);

/**
 * Return true if the given character is graphical, otherwise false.
 * \param c The character.
 * \return True if the given character is graphical, otherwise false.
 */
bool kchar_is_graphical(int c);

/**
 * Return true if the given character is a lower-case alphabet character, otherwise false.
 * \param c The character.
 * \return True if the given character is a lower-case alphabet character, otherwise false.
 */
bool kchar_is_lower_case(int c);

/**
 * Return true if the given character is printable, otherwise false.
 * \param c The character.
 * \return True if the given character is printable, otherwise false.
 */
bool kchar_is_printable(int c);

/**
 * Return true if the given character is punctuation, otherwise false.
 * \param c The character.
 * \return True if the given character is punctuation, otherwise false.
 */
bool kchar_is_punctuation(int c);

/**
 * Return true if the given character is a space character, otherwise false.
 * \param c The character.
 * \return True if the given character is a space character, otherwise false.
 */
bool kchar_is_space(int c);

/**
 * Return true if the given character is an upper-case alphabet character, otherwise false.
 * \param c The character.
 * \return True if the given character is an upper-case alphabet character, otherwise false.
 */
bool kchar_is_upper_case(int c);

/**
 * Return true if the given character is a hexadecimal digit (0-9, A-F), otherwise false.
 * \param c The character.
 * \return True if the given character is a hexadecimal digit (0-9, A-F), otherwise false.
 */
bool kchar_is_hex_digit(int c);

/**
 * Converts a character to lower case
 * \param c ASCII character
 * \return If the character is uppercase, it will be converted to lowercase. If
 * it is already lowercase or is not an alphabetic character it will be returned
 * verbatim.
 */
int kchar_to_lower_case(int c);

/**
 * Converts a character to upper case
 * \param c ASCII character
 * \return If the character is lowercase, it will be converted to uppercase. If
 * it is already uppercase or is not an alphabetic character it will be returned
 * verbatim.
 */
int kchar_to_upper_case(int c);

#endif /* ! REDSHIFT_LIBK_KCHAR_H */

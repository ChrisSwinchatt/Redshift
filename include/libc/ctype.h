/*
 * redshift/include/common/libc/ctype.h - C ctype library
 *
 * Copyright (c) 2012-2018 Chris Swinchatt.
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
#ifndef REDSHIFT_LIBC_CTYPE_H
#define REDSHIFT_LIBC_CTYPE_H

int isalnum(int c);

int isalpha(int c);

int isblank(int c);

int iscntrl(int c);

int isdigit(int c);

int isgraph(int c);

int islower(int c);

int isprint(int c);

int ispunct(int c);

int isspace(int c);

int isupper(int c);

int isxdigit(int c);

/**
 * Converts a character to lower case
 * \param c ASCII character
 * \return If the character is uppercase, it will be converted to lowercase. If
 * it is already lowercase or is not an alphabetic character it will be returned
 * verbatim.
 */
int tolower(int c);

/**
 * Converts a character to upper case
 * \param c ASCII character
 * \return If the character is lowercase, it will be converted to uppercase. If
 * it is already uppercase or is not an alphabetic character it will be returned
 * verbatim.
 */
int toupper(int c);

#endif /* ! REDSHIFT_LIBC_CTYPE_H */

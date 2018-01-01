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
#ifndef _CSTRING_H
#define _CSTRING_H

#include <redshift/kernel.h>
#include <stdint.h>

/**
 * Convert an integer to a string.
 * \param buf Buffer to store the string in. This must be large enough to store
 * any long integer. 20 characters should be enough.
 * \param i Integer to convert.
 * \param base The numeric base to use to represent the number.
 * \return A pointer to buf is returned.
 */
char* itos(long i, int base, char* buf, size_t n);

/**
 * Search for 'c' in 'array'.
 * \param array Array to search.
 * \param c Character to search for.
 * \param n Size of 'array'.
 * \return A pointer to 'c' in 'array' is returned if 'c' is found, NULL is
 * returned if 'c' is not found.
 */
void* memchr(const void* array, int c, size_t n);

/**
 * Compare 'p1' with 'p2'.
 * \param p1 Array 1.
 * \param p2 Array 2.
 * \param n Maximum number of bytes to compare.
 * \return If the two arrays are equal, 0 is returned. If two characters that
 * don't match are encountered, (*p1 - *p2) is returned.
 */
int memcmp(const void* p1, const void* p2, size_t n);

/**
 * Copy 'n' bytes from 'src' to 'dst', allowing overlap.
 * \param dst Destination buffer.
 * \param src Source buffer.
 * \param n Maximum number of bytes to copy.
 * \return dst is returned.
 */
void* memcpy(void* dst, const void* src, size_t n);

/**
 * Copy 'n' bytes from 'src' to 'dst', with overlap disallowed.
 * \param dst Destination buffer.
 * \param src Source buffer.
 * \param n Maximum number of bytes to copy.
 * \return dst is returned.
 */
void* memmove(void* dst, const void* src, size_t n);

/**
 * Set each byte in 'array' to 'c', up to max 'n' bytes.
 * \param array Array.
 * \param c Value to set.
 * \param n Maximum number of bytes to write.
 * \return A pointer to 'array' is returned.
 */
void* memset(void* array, int c, size_t n);

/**
 * Append 'src' to 'dst'.
 * \param dst Destination string.
 * \param src source string.
 * \return dst is returned.
 */
char* strcat(char* dst, const char* src);

/**
 * Search for 'c' in 's'.
 * \param s String to search.
 * \param c Character to search for.
 * \return A pointer to the first occurrence of 'c' in 's' or NULL if 'c' is not
 * found in 's'
 */
char* strchr(const char* s, int c);

/**
 * Compare 's1' with 's2'.
 * \param s1 String 1.
 * \param s2 String 2.
 * \return If the two strings are equal, 0 is returned. If two characters that
 * don't match are encountered, (*s1 - *s2) is returned.
 */
int strcmp(const char* s1, const char* s2);

/**
 * Copy 'src' into 'dst'.
 * \param dst Destination buffer.
 * \param src Source buffer.
 * \return A pointer to dst is returned.
 */
char* strcpy(char* dst, const char* src);

/**
 * Retrieve the error message associated with 'errnum'.
 * \param errnum Error message to get.
 * \return The associated error message is returned or NULL if none was found.
 */
char* strerror(int errnum);

/**
 * Find the lenth of 's'.
 * \param s A string.
 * \return The length of 's' is returned.
 */
size_t strlen(const char* s);

/**
 * Append max. 'n' characters from 'src' to 'dst'
 * \param dst Destination buffer.
 * \param src Source buffer.
 * \param n Maximum number of characters to append.
 * \return A pointer to 'dst' is returned.
 */
char* strncat(char* dst, const char* src, size_t n);

/**
 * Compare a maximum of 'n' characters from 's1' with 's2'.
 * \param s1 A string.
 * \param s2 Another string.
 * \param n Maximum number of characters to compare.
 * \return If the two strings are equal, 0 is returned. If two characters that
 * don't match are encountered, (*s1 - *s2) is returned.
 */
int strncmp(const char* s1, const char* s2, size_t n);

/**
 * Copy a maximum of 'n' characters from 'src' to 'dst'.
 * \param dst Destination string.
 * \param src Source string.
 * \param n Maximum number of characters to copy.
 * \return A pointer to 'dst' is returned.
 */
char* strncpy(char* dst, const char* src, size_t n);

/**
 * Reverse a string.
 * \param s String to be reversed.
 * \return The string is returned.
 */
char* strreverse(char* s);

/**
 * Find the first instance of 's2' in 's1'.
 * \param s1 String to search.
 * \param s2 String to search for.
 * \return A pointer to the first instance of 's2' in 's1' is returned, or NULL
 * if 's2' is not found in 's1'.
 */
char* strstr(const char* dst, const char* src);

/**
 * Converts any upper-case characters in a string to lower-case.
 * \param s A string.
 * \return A pointer to s is returned.
 */
char* stolower(char* s);

/**
 * Converts any lower-case characters in a string to upper-case.
 * \param s A string.
 * \return A pointer to s is returned.
 */
char* stoupper(char* s);

#endif /* ! _CSTRING_H */

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
#include <libk/kabort.h>
#include <libk/kextern.h>
#include <libk/kmacro.h>
#include <libk/kmemory.h>
#include <libk/kchar.h>
#include <libk/kstring.h>

ssize_t kstring_format_intmax(char* buf, size_t n, intmax_t i, int base, kchar_case_t case_)
{
    uintmax_t u = (uintmax_t)i;
    char* p = buf;
    if (i < 0) {
        *p++ = '-';
        u = -i;
        n -= 1;
    }
    return kstring_format_uintmax(p, n, u, base, case_);
}

ssize_t kstring_format_uintmax(char* buffer, size_t n, uintmax_t value, int base, kchar_case_t case_)
{
    if (base < 2) {
        return 0;
    }
    uintmax_t divisor = (uintmax_t)base;
    const int alpha = (case_ == KCHAR_UPPER_CASE) ? 'A' : 'a';
    char* p = buffer;
    const char* q = buffer + n;
    do {
        uintmax_t rem = value % divisor;
        if (rem < 10) {
            *p++ = '0' + rem;
        } else {
            *p++ = alpha + rem - 10;
        }
        value /= divisor;
    } while (value > 0 && p < q);
    kstring_reverse(buffer, p - buffer);
    return p - buffer;
}

intmax_t kstring_parse_intmax(const char* s, int base)
{
    intmax_t number = 0;
    bool     neg    = false;
    if (base < 2) {
        return 0;
    }
    if (*s == '-') {
        neg = true;
        ++s;
    }
    for (; *s != 0; ++s) {
        int c = kchar_to_upper_case(*s);
        int digit = 0;
        if (kchar_is_digit(c)) {
            digit = c - '0';      /* '0' -> 0, '1' -> 1, etc.   */
        } else if (kchar_is_alpha(c)) {
            digit = c - 'A' + 10; /* 'A' -> 10, 'B' -> 11, etc. */
        } else {
            return 0;
        }
        number *= base;
        number += digit;
    }
    if (neg) {
        return -number;
    }
    return number;
}

uint32_t kstring_hash32(const char* s, size_t n)
{
    uint32_t a = 33;
    uint32_t h = 5381;
    size_t i;
    for (i = 0; i < n && s[i] != 0; ++i) {
        h = s[i] + h*a;
    }
    return h;
}

char* kstring_find_char(const char* s, int c)
{
    for (; *s != 0; ++s) {
        if (*s == c) {
            return (char*)s;
        }
    }
    return NULL;
}

size_t kstring_length(const char* s)
{
    const char* p = s;
    for (; *p != 0; ++p) {
        DO_NOTHING();
    }
    return p - s;
}

char* kstring_concatenate(char* dst, const char* src, size_t n)
{
    const size_t offset = kstring_length(dst);
    return kstring_copy(dst + offset, src, n);
}

int kstring_compare(const char* s1, const char* s2, size_t n)
{
    for (; n --> 0 && *s1 != 0 && *s2 != 0; ++s1, ++s2) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}

char* kstring_copy(char* dst, const char* src, size_t n)
{
    char* p = dst;
    char* q = dst + n;
    for (; *src != 0 && p < q; ++p, ++src) {
        *p = *src;
    }
    *p = 0;
    return p;
}

char* kstring_reverse(char* s, size_t n)
{
    char* p = s;
    char* q = s + n;
    while (*q == 0 && q > p) {
        --q;
    }
    for (; p < q; ++p, --q) {
        char c = *q;
        *q = *p;
        *p = c;
    }
    return s;
}

char* kstring_find_str(const char* dst, const char* src)
{
    while (*dst++ != 0) {
        const char* pdst = dst;
        const char* psrc = src;
        while ((*pdst++ == *psrc++) && *pdst && *psrc)
            ;
        if (*psrc == 0) {
            return (char*)dst;
        }
    }
    return NULL;
}

char* kstring_to_lower_case(char* s)
{
    for (char* p = s; *p != 0; ++p) {
        *p = kchar_to_lower_case(*p);
    }
    return s;
}

char* kstring_to_upper_case(char* s)
{
    for (char* p = s; *p != 0; ++p) {
        *p = kchar_to_upper_case(*p);
    }
    return s;
}

char* kstring_duplicate(const char* s)
{
    size_t size = kstring_length(s);
    char* dup = kextern_dynamic_allocate(size + 1);
    if (dup == NULL) {
        KABORT("failed to allocate memory", NULL);
    }
    kstring_copy(dup, s, size);
    return dup;
}

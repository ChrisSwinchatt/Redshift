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
#include <libk/abort.hpp>
#include <libk/asciz.hpp>
#include <libk/char.hpp>
#include <libk/extern.hpp>
#include <libk/macro.hpp>
#include <libk/memory.hpp>

namespace libk { namespace asciz {
    ssize_t format_intmax(char* buf, size_t n, intmax_t i, int base, char_case case_)
    {
        uintmax_t u = (uintmax_t)i;
        char* p = buf;
        if (i < 0) {
            *p++ = '-';
            u = -i;
            n -= 1;
        }
        return format_uintmax(p, n, u, base, case_);
    }

    ssize_t format_uintmax(char* buffer, size_t n, uintmax_t value, int base, char_case case_)
    {
        if (base < 2) {
            return 0;
        }
        uintmax_t divisor = (uintmax_t)base;
        const int alpha = (case_ == char_case::upper) ? 'A' : 'a';
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
        reverse(buffer, p - buffer);
        return p - buffer;
    }

    intmax_t parse_intmax(const char* s, int base)
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
            int c = char_(*s).to_upper_case();
            int digit = 0;
            if (char_(c).is_digit()) {
                digit = c - '0';      // '0' -> 0, '1' -> 1, etc.
            } else if (char_(c).is_alpha()) {
                digit = c - 'A' + 10; // 'A' -> 10, 'B' -> 11, etc.
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

    uint32_t hash32(const char* s, size_t n)
    {
        uint32_t a = 33;
        uint32_t h = 5381;
        size_t i;
        for (i = 0; i < n && s[i] != 0; ++i) {
            h = s[i] + h*a;
        }
        return h;
    }

    char* find_char(const char* s, int c)
    {
        for (; *s != 0; ++s) {
            if (*s == c) {
                return (char*)s;
            }
        }
        return nullptr;
    }

    size_t length(const char* s)
    {
        const char* p = s;
        for (; *p != 0; ++p) {
            DO_NOTHING;
        }
        return p - s;
    }

    char* concat(char* dst, const char* src, size_t n)
    {
        const size_t offset = length(dst);
        return copy(dst + offset, src, n);
    }

    int compare(const char* s1, const char* s2, size_t n)
    {
        for (; n --> 0 && *s1 != 0 && *s2 != 0; ++s1, ++s2) {
            if (*s1 != *s2) {
                return *s1 - *s2;
            }
        }
        return 0;
    }

    char* copy(char* dst, const char* src, size_t n)
    {
        char* p = dst;
        char* q = dst + n;
        for (; *src != 0 && p < q; ++p, ++src) {
            *p = *src;
        }
        *p = 0;
        return p;
    }

    char* reverse(char* s, size_t n)
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

    char* find_str(const char* dst, const char* src)
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
        return nullptr;
    }

    char* to_lower_case(char* s)
    {
        for (char* p = s; *p != 0; ++p) {
            *p = char_(*p).to_lower_case();
        }
        return s;
    }

    char* to_upper_case(char* s)
    {
        for (char* p = s; *p != 0; ++p) {
            *p = char_(*p).to_upper_case();
        }
        return s;
    }

    char* duplicate(const char* s)
    {
        size_t size = length(s);
        char* dup = static_cast<char*>(libk_extern::dynamic_allocate(size + 1));
        if (dup == nullptr) {
            LIBK_ABORT("failed to allocate memory", nullptr);
        }
        copy(dup, s, size);
        return dup;
    }
}} // libk::asciz

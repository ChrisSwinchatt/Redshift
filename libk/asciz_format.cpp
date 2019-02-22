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
#include <libk/macro.hpp>
#include <libk/types.hpp>
#include <libk/variadic.hpp>

using namespace libk;

enum {
    // Justify.
    FMT_LEFT        = 0, // Left-align.
    FMT_RIGHT       = 1, // Right-align.
    // Sign.
    FMT_SIGN_NEG    = 0, // Sign negative numbers with minus sign.
    FMT_SIGN_POS    = 1, // Also sign positive numbers with plus sign.
    FMT_SIGN_SPACE  = 2, // Sign positive numbers with space.
    // Type width.
    FMT_CHAR        = 0, // Use signed/unsigned char.
    FMT_SHORT       = 1, // Use signed/unsigned short.
    FMT_INT         = 2, // Use signed/unsigned int.
    FMT_LONG        = 3, // Use signed/unsigned long.
    FMT_LONG_LONG   = 4, // Use signed/unsigned long long.
    FMT_INTMAX      = 5, // Use int/uintmax_t.
    FMT_SIZE_T      = 6, // Use size_t.
    FMT_PTRDIFF     = 7, // Use ptrdiff_t.
    FMT_LONG_DOUBLE = 8, // Use long double.
    // Signed or unsigned.
    FMT_SIGNED      = 0, // Signed.
    FMT_UNSIGNED    = 1  // Unsigned.
};

static const struct format_flags {
    unsigned left_pad    : 1; // Whether to pad on the left (1) or right (0).
    unsigned sign        : 2; // Whether to sign negative numbers only (0), or sign positive with - (1) or space (2).
    unsigned zero_pad    : 1; // Whether to pad with space (0) or zero (1).
    unsigned hex_upper   : 1; // Whether to write alphabetic hexadecimal digits in lower (0) or upper (1) case.
    unsigned type_width  : 4; // Whether to extract int as char/short/int/long/long long/intmax_t/size_t/ptrdiff_t.
    unsigned type_signed : 1; // Whether to extract signed or unsigned int.
    unsigned prefix      : 1; // Whether to prefix bin/oct/hex numbers.
    int      base;            // Numeric base (>= 2).
    int      field_width;     // Field width to pad to.
    int      precision;       // Floating point precision (decimal places).
} defaults = {
    .left_pad    = 1,
    .sign        = FMT_SIGN_NEG,
    .zero_pad    = 0,
    .hex_upper   = 0,
    .type_width  = FMT_INT,
    .type_signed = 1,
    .prefix      = 0,
    .base        = 10,
    .field_width = 0,
    .precision   = 0
};

enum { NUMBER_MAX = sizeof(uintmax_t)*CHAR_BIT };

static ssize_t do_string(const struct format_flags* flags, char** pp, const char* q, const char* s, size_t n, bool is_number)
{
    char* p        = *pp;
    char pad_char  = (is_number && flags->zero_pad) ? '0' : ' ';
    // Compute alignment padding.

    size_t padding = 0;
    if (n < (size_t)flags->field_width) {
        padding = (size_t)flags->field_width - n;
    }
    // Pad by inserting characters before the string.

    if (flags->left_pad) {
        for (; padding > 0 && p < q; --padding, ++p) {
            *p = pad_char;
        }
    }
    // Copy the string into the buffer.

    p = asciz::copy(p, s, MIN(((size_t)(p - q)), n)); // p is always >= q here.
    // Pad by inserting characters after the string.

    if (!(flags->left_pad)) {
        for (; padding > 0 && p < q; --padding, ++p) {
            *p = pad_char;
        }
    }
    *pp = p;
    return q - p;
}

static ssize_t format_number(const struct format_flags* flags, char** pp, const char* q, va_list args)
{
    char*   p   = *pp;
    // Extract the value as a uintmax_t.

    uintmax_t value = 0;
    bool      neg   = false;
    if (flags->type_signed) {
        intmax_t tmp = 0;
        switch (flags->type_width) {
            case FMT_CHAR: case FMT_SHORT:
            case FMT_INT:       tmp = va_arg(args, int); break;
            case FMT_LONG:      tmp = va_arg(args, long);      break;
            case FMT_LONG_LONG: tmp = va_arg(args, long long); break;
            case FMT_INTMAX:    tmp = va_arg(args, intmax_t);  break;
            case FMT_SIZE_T:    tmp = va_arg(args, ssize_t);   break;
            case FMT_PTRDIFF:   tmp = va_arg(args, ptrdiff_t); break;
            default:            UNREACHABLE("no switch case for flags->type_width=%d\n", flags->type_width);
        }
        value = tmp;
        if (tmp < 0) {
            neg   = true;
            value = -tmp;
        }
    } else {
        switch (flags->type_width) {
            case FMT_CHAR: case FMT_SHORT:
            case FMT_INT:       value = va_arg(args, unsigned); break;
            case FMT_LONG:      value = va_arg(args, unsigned long);      break;
            case FMT_LONG_LONG: value = va_arg(args, unsigned long long); break;
            case FMT_INTMAX:    value = va_arg(args, uintmax_t);          break;
            case FMT_SIZE_T:    value = va_arg(args, size_t);             break;
            case FMT_PTRDIFF:   value = va_arg(args, uintptr_t);          break;
            default:            UNREACHABLE("no switch case for flags->type_width=%d\n", flags->type_width);
        }
    }
    // Handle signing.

    if (neg) {
        *p++ = '-';
    } else if (flags->sign == FMT_SIGN_POS) {
        *p++ = '+';
    } else if (flags->sign == FMT_SIGN_SPACE) {
        *p++ = ' ';
    }
    // Write the bin/octal/hex prefix if needed.

    if (flags->prefix) {
        if (flags->base == 8 || flags->base == 16) {
            *p++ = '0';
            if (flags->base == 16) {
                *p++ = flags->hex_upper ? 'X' : 'x';
            }
        } else if (flags->base == 2) {
            *p++ = 'b';
        }
    }
    // Write the number and return the number of characters written.

    {
        static char buffer[NUMBER_MAX];
        size_t n = asciz::format_uintmax(
            buffer,
            NUMBER_MAX,
            value,
            flags->base,
            flags->hex_upper ? char_case::upper : char_case::lower
        );
        do_string(flags, &p, q, buffer, n, true);
    }
    *pp = p;
    return q - p;
}

static ssize_t format_string(const struct format_flags* flags, char** pp, const char* q, va_list args)
{
    const char*  s = va_arg(args, const char*);
    size_t n = asciz::length(s);
    if (flags->precision > 0 && (unsigned)flags->precision < n) {
        n = flags->precision;
    }
    return do_string(flags, pp, q, s, n, false);
}

static ssize_t format_char(const struct format_flags* flags, char** pp, const char* q, va_list args)
{
    char c = (char)va_arg(args, int);
    char s[] = { c, 0 };
    return do_string(flags, pp, q, s, 1, false);
}

static ssize_t format_pointer(const struct format_flags* flags, char** pp, const char* q, va_list args)
{
    UNUSED(flags);
    unsigned ptrtype = FMT_INT;
    if (sizeof(void*) == sizeof(long)) {
        ptrtype = FMT_LONG;
    } else if (sizeof(void*) == sizeof(long long)) {
        ptrtype = FMT_LONG_LONG;
    } else {
        UNREACHABLE("No case for sizeof(void*) = %zu", sizeof(void*));
    }
    struct format_flags tmp;
    tmp.left_pad    = 1;
    tmp.zero_pad    = 1;
    tmp.base        = 16;
    tmp.field_width = sizeof(void*)*CHAR_BIT/4;
    tmp.hex_upper   = 1;
    tmp.prefix      = flags->prefix;
    tmp.type_signed = 0;
    tmp.type_width  = ptrtype;
    return format_number(&tmp, pp, q, args);
}

static int extract_int(const char** pfmt) {
    static char buffer[NUMBER_MAX];
    char* p = buffer;
    const char* q = buffer + NUMBER_MAX;
    const char* fmt = *pfmt;
    while (p < q && char_(*fmt).is_digit()) {
        *p++ = *fmt++;
    }
    *p = 0;
    *pfmt = fmt;
    return (int)asciz::parse_intmax(buffer, 10);
}

static ssize_t format_arg(char** pp, const char* q, const char** pfmt, va_list args)
{
    struct format_flags flags = defaults;
    const char* fmt = *pfmt;
    char*   p   = *pp;
    ssize_t ret = 0;
    for (; p < q && *fmt != 0; ++fmt) {
        switch (*fmt) {
            case '-':
                flags.left_pad = 0;
                break;
            case '+':
                flags.sign = FMT_SIGN_POS;
                break;
            case ' ':
                flags.sign = FMT_SIGN_SPACE;
                break;
            case '#':
                flags.prefix = 1;
                break;
            case '0':
                flags.zero_pad = 1;
                break;
            case '*':
                flags.field_width = va_arg(args, int);
                break;
            case '.':
                if (char_(*(fmt + 1)).is_digit()) {
                    ++fmt;
                    flags.precision = extract_int(&fmt);
                    if (flags.precision < 0) {
                        ret = -1;
                        goto out;
                    }
                    --fmt; // fmt points to next char after extract_int, and will be incremented past it in for-loop.
                } else if (*(fmt + 1) == '*') {
                    flags.precision = va_arg(args, int);
                    ++fmt;
                } else {
                    flags.precision = 0;
                }
                break;
            case 'j': flags.type_width = FMT_INTMAX;      break;
            case 'z': flags.type_width = FMT_SIZE_T;      break;
            case 't': flags.type_width = FMT_PTRDIFF;     break;
            case 'L': flags.type_width = FMT_LONG_DOUBLE; break;
            case 'q': flags.type_width = FMT_LONG_LONG;   break;
            case 'h':
                if (*(fmt + 1) == 'h') {
                    flags.type_width = FMT_CHAR;
                } else {
                    flags.type_width = FMT_SHORT;
                }
                break;
            case 'l':
                if (*(fmt + 1) == 'l') {
                    flags.type_width = FMT_LONG_LONG;
                } else {
                    flags.type_width = FMT_LONG;
                }
                break;
            case 'b':
                flags.type_signed = 0;
                flags.base        = 2;
                ret = format_number(&flags, &p, q, args);
                goto out;
            case 'd': case 'i':
                flags.type_signed = 1;
                flags.base        = 10;
                ret = format_number(&flags, &p, q, args);
                goto out;
            case 'o':
                flags.type_signed = 0;
                flags.base        = 8;
                ret = format_number(&flags, &p, q, args);
                goto out;
            case 'u':
                flags.type_signed = 0;
                flags.base        = 10;
                ret = format_number(&flags, &p, q, args);
                goto out;
            case 'X':
                flags.hex_upper = 1;
                // Fall through.
            case 'x':
                flags.type_signed = 0;
                flags.base        = 16;
                ret = format_number(&flags, &p, q, args);
                goto out;
            case 'c': ret = format_char(&flags, &p, q, args);    goto out;
            case 's': ret = format_string(&flags, &p, q, args);  goto out;
            case 'p': ret = format_pointer(&flags, &p, q, args); goto out;
            case 'n':
            {
                ssize_t* pcount = va_arg(args, ssize_t*);
                if (pcount == nullptr) {
                    ret = -1;
                    goto out;
                }
                *pcount = *pp - p;
                goto out;
            }
            default:
                if (char_(*fmt).is_digit()) {
                    flags.field_width = extract_int(&fmt);
                    if (flags.field_width < 0) {
                        ret = -1;
                        goto out;
                    }
                    --fmt; // fmt points to next char after extract_int, and will be incremented past it in for-loop.
                } else {
                    ret = -1;
                    goto out; // Unexpected token.
                }
                break;
        }
    }
out:
    *pfmt = fmt;
    *pp   = p;
    return ret;
}

namespace libk { namespace asciz {
    ssize_t vformat(char* buffer, size_t size, const char* fmt, va_list args)
    {
        char* p = buffer;
        const char* q = buffer + size;
        int count = 0;
        for (; p < q && *fmt != 0; ++fmt) {
            if (*fmt == '%' && *(fmt + 1) != '%') {
                ++fmt;
                if (format_arg(&p, q, &fmt, args) < 0) {
                    return -1;
                }
            } else {
                if (buffer && (unsigned)count < size) {
                    *p++ = *fmt;
                }
            }
        }
        *p = 0;
        return p - buffer;
    }

    ssize_t format(char* buffer, size_t size, const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vformat(buffer, size, fmt, ap);
        va_end(ap);
        return ret;
    }
}} // libk

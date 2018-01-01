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
#include <ctype.h>
#include <string.h>
#include <redshift/mem/heap.h>

char* itos(long i, int base, char* buf, size_t n)
{
    unsigned long u = i;
    if (i < 0) {
        *buf++ = '-';
        u = -i;
    }
    do {
        int rem = u % base;
        *buf++ = (rem < 10) ? '0' + rem : 'a' + rem;
    } while (u /= base && n--);
    return strreverse(buf);
}

void* memchr(const void* array, int c, size_t n)
{
    const char* p = array;
    while (n --> 0) {
        if (*p++ == c)
            return (void*)p;
    }
    return NULL;
}

int memcmp(const void* p1, const void* p2, size_t n)
{
    const char* s1 = p1;
    const char* s2 = p2;
    while (n --> 0) {
        if (*s1 != *s2)
            return *s1 - *s2;
    }
    return 0;
}

void* memcpy(void* dst, const void* src, size_t n)
{
    if (dst != src) {
        char* pdst = dst;
        const char* psrc = src;
        while (n --> 0) {
            *pdst++ = *psrc++;
        }
    }
    return dst;
}

void* memmove(void* dst, const void* src, size_t n)
{
    char* pdst = (char*)dst;
    const char* psrc = (const char*)src;
    if (pdst == psrc) {
        return pdst;
    }
    if (pdst < psrc && pdst + n >= psrc) {
        /* Copy backwards.
         */
        while (n --> 0) {
            pdst[n] = psrc[n];
        }
        return pdst;
    }
    /* Use memcpy to copy forwards.
     */
    return memcpy(dst, src, n);
}

void* memset(void* array, int c, size_t n)
{
    char* parray = array;
    while (n --> 0) {
        *parray++ = c;
    }
    return array;
}

char* strcat(char* dst, const char* src)
{
    strcpy(dst + strlen(dst), src);
    return dst;
}

char* strchr(const char* s, int c)
{
    for (; *s != 0; ++s) {
        if (*s == c) {
            return (char* ) s;
        }
    }
    return NULL;
}

int strcmp(const char* s1, const char* s2)
{
    for (; *s1 != 0 && *s2 != 0; ++s1, ++s2) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}

char* strcpy(char* dst, const char* src)
{
    while (*dst != 0 && *src != 0) {
        *dst++ = *src++;
    }
    return dst;
}

char* strerror(int errnum)
{
    ((void)errnum);
    return NULL;
}

size_t strlen(const char* s)
{
    size_t count;
    for (count = 0; *s != 0; ++s, ++count)
        ;
    return count;
}

char* strncat(char* dst, const char* src, size_t n)
{
    strncpy(dst + strlen(dst), src, n);
    return dst;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    for (; n --> 0 && *s1 != 0 && *s2 != 0; ++s1, ++s2) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}

char* strncpy(char* dst, const char* src, size_t n)
{
    for (size_t i = 0; i < n && *src != 0; ++i, ++dst, ++src) {
        *dst = *src;
    }
    return dst;
}

char* strreverse(char* s)
{
    char* p1;
    char* p2;
    for (p1 = s, p2 = p1 + strlen(s); p1 < p2; ++p1, --p2) {
        int c = *p1;
        *p1 = *p2;
        *p2 = c;
    }
    return s;
}

char* strstr(const char* dst, const char* src)
{
    int c;
    while ((c = *dst++) != 0) {
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

char* stolower(char* s)
{
    for (char* p = s; *p != 0; ++p) {
        *p = tolower(*p);
    }
    return s;
}

char* stoupper(char* s)
{
    for (char* p = s; *p != 0; ++p) {
        *p = toupper(*p);
    }
    return s;
}

char* strdup(const char* s)
{
    size_t size = strlen(s);
    char* dup = kmalloc(size);
    if (dup == NULL) {
        panic("can't allocate memory");
    }
    strncpy(dup, s, size);
    printk("\"%s\" => \"%s\"\n", s, dup);
    hang();
    return dup;
}

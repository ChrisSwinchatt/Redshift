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
#include <redshift/kernel.h>
#include <redshift/kernel/symbols.h>
#include <redshift/mem/static.h>
#include <string.h>

static struct symbol {
    char*          name;
    uintptr_t      address;
    struct symbol* next;
} * __symbols__ = NULL;

static void skip_whitespace(const char* file, size_t size, size_t* pi, size_t* pcolumn, size_t* pline)
{
    size_t i      = *pi;
    size_t column = *pcolumn;
    size_t line   = *pline;
    for (; i < size && isspace(file[i]); ++i, ++column) {
        if (file[i] == '\n') {
            column = 1;
            ++line;
        }
    }
    *pi      = i;
    *pcolumn = column;
    *pline   = line;
}

static uintptr_t parse_address(const char* file, size_t size, size_t* pi, size_t* pcolumn, size_t* pline)
{
    static char buffer[20];
    static const char* buffer_max = buffer + ARRAY_SIZE(buffer);
    /* Copy address digits into temporary buffer.
     */
    size_t i      = *pi;
    size_t column = *pcolumn;
    size_t line   = *pline;
    char* p       = buffer;
    for (; i < size && isxdigit(file[i]) && p < buffer_max; ++i, ++column, ++p) {
        *p = file[i];
    }
    *pi      = i;
    *pcolumn = column;
    *pline   = line;
    /* Compute address in buffer.
     */
    uintptr_t address = 0;
    char* q = p;
    p = buffer;
    for (; p < q; ++p) {
        unsigned value = 0;
        if (isdigit(*p)) {
            value = *p - '0';      /* '0' -> 0, '1' -> 1, etc. */
        } else {
            value = *p - 'A' + 10; /* 'A' -> 10, 'B' -> 11, etc. */
        }
        address *= 16;
        address += value;
    }
    return address;
}

static const char* parse_identifier(const char* file, size_t size, size_t* pi, size_t* pcolumn, size_t* pline)
{
    size_t i      = *pi;
    size_t column = *pcolumn;
    size_t line   = *pline;
    const char* p = file + i;
    for (; i < size && (file[i] == '_' || isalnum(file[i])); ++i, ++column)
        ;
    *pi      = i;
    *pcolumn = column;
    *pline   = line;
    return p;
}

void symbols_load(uintptr_t address, size_t size)
{
    DEBUG_ASSERT(address != 0);
    struct symbol* symbol = static_alloc(sizeof(*__symbols__));
    const  char*   file   = (const char*)address;
    size_t line   = 1;
    size_t column = 1;
    __symbols__ = symbol;
    for (size_t i = 0; i < size; ++i, ++column) {
        /* Skip leading whitespace/blank lines.
         */
        skip_whitespace(file, size, &i, &column, &line);
        /* Parse address.
         */
        if (!(isxdigit(file[i]))) {
            panic("syntax error in symbol table: %lu:%lu: expected digit, got \'%c\'", line, column, file[i]);
        }
        symbol->address = parse_address(file, size, &i, &column, &line);
        /* Skip whitespace in between.
         */
        if (!(isspace(file[i]))) {
            panic("syntax error in symbol table: %lu:%lu: expected space, got \'%c\'", line, column, file[i]);
        }
        skip_whitespace(file, size, &i, &column, &line);
        /* Parse identifier.
         */
        const char* name = parse_identifier(file, size, &i, &column, &line);
        /* Copy identifier name into symbol.
         */
        {
            size_t length = file + i - name;
            symbol->name = static_alloc(length);
            strncpy(symbol->name, name, length);
        }
        printk(PRINTK_DEBUG "0x%lX %s\n", symbol->address, symbol->name);
        /* Allocate next symbol.
         */
        symbol->next = static_alloc(sizeof(*symbol));
        symbol = symbol->next;
    }
}

const void* get_symbol(const char* name)
{
    const struct symbol* symbol = __symbols__;
    while (symbol) {
        if (strcmp(symbol->name, name) == 0) {
            return (const void*)symbol->address;
        }
        symbol = symbol->next;
    }
    return NULL;
}

const char* get_symbol_name(uintptr_t address)
{
    const struct symbol* symbol = __symbols__;
    while (symbol) {
        if (address == symbol->address) {
            return symbol->name;
        }
        symbol = symbol->next;
    }
    return NULL;
}

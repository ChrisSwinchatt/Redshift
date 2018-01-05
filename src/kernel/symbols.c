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

#define SYNTAX_ERROR(FMT, ...)     panic("syntax error in symbol table: %lu:%lu: " FMT, line, column, __VA_ARGS__)

#define UNEXPECTED_TOKEN(EXPECTED) SYNTAX_ERROR("expected %s, got \"%c\"", EXPECTED, file[i])

struct symbol {
    char*          name;
    uintptr_t      address;
    struct symbol* next;
};

static struct symbol* __symbols__ = NULL;

static uintptr_t parse_address(const char* p, const char* q)
{
    uintptr_t address = 0;
    for (; p < q; ++p) {
        unsigned value = 0;
        if (isdigit(*p)) {
            value = *p - '0';      /* '0' -> 0, '1' -> 1, etc. */
        } else if (isalpha(*p)) {
            value = *p - 'A' + 10; /* 'A' -> 10, 'B' -> 11, etc. */
        }
        address *= 16;
        address += value;
    }
    return address;
}

enum {
    ADDRESS_MAX = (2 << sizeof(uintptr_t))/4,
    NAME_MAX    = 2048
};

enum { ADDRESS = 0, SYMBOL = 1 };

void symbols_load(uintptr_t ptr, size_t size)
{
    DEBUG_ASSERT(ptr != 0);
    __symbols__  = static_alloc(sizeof(*__symbols__));
    struct symbol* symbol = __symbols__;
    const  char*   file   = (const char*)ptr;
    char address[ADDRESS_MAX];
    char name[NAME_MAX];
    size_t line   = 1;
    size_t column = 1;
    int    state  = ADDRESS;
    char*  p      = address;
    for (size_t i = 0; i < size && file[i] != 0; ++i, ++column) {
        if (isspace(file[i])) {
            if (file[i] == '\n') {
                if (column == 1) {
                    /* Stop on empty line.
                     */
                    break;
                }
                column = 1;
                ++line;
            }
            switch (state) {
                case ADDRESS:
                    /* Copy address.
                     */
                    symbol->address = parse_address(address, p);
                    state = SYMBOL;
                    p     = name;
                    break;
                case SYMBOL:
                    /* Copy symbol name and advance to the next symbol.
                     */
                    if (name[0] != '_' && !(isalpha(name[0]))) {
                        *++p = 0;
                        SYNTAX_ERROR("invalid symbol name \"%s\"", name);
                    }
                    symbol->name = static_alloc(p - name);
                    strncpy(symbol->name, name, p - name);
                    symbol->next = static_alloc(sizeof(*(symbol->next)));
                    memset(symbol->next, 0, sizeof(*(symbol->next)));
                    symbol = symbol->next;
                    state  = ADDRESS;
                    p      = address;
                    break;
                default:
                    UNREACHABLE("no switch case for state %d", state);
            }
        } else {
            switch (state) {
                case ADDRESS:
                    if (!(isxdigit(file[i]))) {
                        UNEXPECTED_TOKEN("digit");
                    }
                    *p++ = file[i];
                    break;
                case SYMBOL:
                    if (file[i] != '_' && !(isalnum(file[i]))) {
                        UNEXPECTED_TOKEN("digit, letter or underscore");
                    }
                    *p++ = file[i];
                    break;
                default:
                    UNREACHABLE("no switch case for state %d", state);
            }
        }
    }
}

const void* get_symbol_address(const char* name)
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
    const struct symbol* symbol  = __symbols__;
    const struct symbol* closest = symbol;
    if (address < (uintptr_t)__code_start__ || address > (uintptr_t)__code_end__) {
        return NULL;
    }
    do {
        /* Find the symbol with the highest address which is
         */
        if (symbol->address >= closest->address && symbol->address <= address) {
            closest  = symbol;
        }
        symbol = symbol->next;
    } while (symbol);
    return closest->name;
}

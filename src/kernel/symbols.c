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
#include <redshift/util/sorted_array.h>
#include <string.h>

#define SYNTAX_ERROR(FMT, ...)     panic("syntax error in symbol table: %lu:%lu: " FMT, line, column, __VA_ARGS__)

#define UNEXPECTED_TOKEN(EXPECTED) SYNTAX_ERROR("expected %s, got \"%c\"", EXPECTED, file[i])

enum {
    MAX_SYMBOLS = 512
};

struct symbol {
    char*     name;
    uintptr_t address;
};

static struct sorted_array* symbol_table;

/* Sorting predicate for symbol_table. Sorts symbols by descending address. */
static bool symbol_order_predicate(void* pa, void* pb)
{
    DEBUG_ASSERT(pa != NULL);
    DEBUG_ASSERT(pb != NULL);
    struct symbol* a = pa;
    struct symbol* b = pb;
    return a->address < b->address;
}

static uintptr_t parse_address(const char* p, const char* q)
{
    uintptr_t address = 0;
    for (; p < q; ++p) {
        unsigned value = 0;
        if (isdigit(*p)) {
            value = *p - '0';      /* '0' -> 0, '1' -> 1, etc.   */
        } else if (isalpha(*p)) {
            value = *p - 'A' + 10; /* 'A' -> 10, 'B' -> 11, etc. */
        }
        address <<= 4;
        address +=  value;
    }
    return address;
}

enum {
    ADDRESS_MAX = (2 << sizeof(uintptr_t))/4,
    NAME_MAX    = 2048
};

enum { ADDRESS = 0, SYMBOL = 1 };

void load_symbol_table(uintptr_t ptr, size_t size)
{
    SAVE_INTERRUPT_STATE;
    DEBUG_ASSERT(ptr != 0);
    symbol_table = create_sorted_array(MAX_SYMBOLS, SORTED_ARRAY_STATIC, symbol_order_predicate);
    const  char*   file   = (const char*)ptr;
    char address[ADDRESS_MAX];
    char name[NAME_MAX];
    size_t line   = 1;
    size_t column = 1;
    int    state  = ADDRESS;
    char*  p      = address;
    struct symbol* symbol = kmalloc(sizeof(*symbol));
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
                        SYNTAX_ERROR("invalid symbol name \"%s\"", name);
                    }
                    size_t size = p - name;
                    symbol->name = kmalloc(size);
                    strncpy(symbol->name, name, size);
                    printk(PRINTK_DEBUG "Symbol: <address=0x%08lX,name=%s>\n", symbol->address, symbol->name);
                    sorted_array_add(symbol_table, symbol);
                    state  = ADDRESS;
                    p      = address;
                    symbol = kmalloc(sizeof(*symbol));
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
    RESTORE_INTERRUPT_STATE;
}

const void* get_symbol_address(const char* name)
{
    SAVE_INTERRUPT_STATE;
    for (size_t i = 0; i < sorted_array_count(symbol_table); ++i) {
        struct symbol* symbol = sorted_array_get(symbol_table, i);
        if (strcmp(symbol->name, name) == 0) {
            return (const void*)(symbol->address);
        }
    }
    RESTORE_INTERRUPT_STATE;
    return NULL;
}

const char* get_symbol_name(uintptr_t address)
{
    if (address < (uintptr_t)__code_start__ || address > (uintptr_t)__code_end__) {
        return NULL;
    }
    /* Find the symbol with the largest address which is below or equal to the given address. Since our symbol table is
     * in descending order we can just find the first symbol whose address <= the search address.
     */
    SAVE_INTERRUPT_STATE;
    for (size_t i = 0; i < sorted_array_count(symbol_table); ++i) {
        struct symbol* symbol = sorted_array_get(symbol_table, i);
        if (symbol->address <= address) {
            RESTORE_INTERRUPT_STATE;
            return symbol->name;
        }
    }
    RESTORE_INTERRUPT_STATE;
    return NULL;
}

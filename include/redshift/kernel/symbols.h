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
#ifndef REDSHIFT_KERNEL_SYMBOLS_H
#define REDSHIFT_KERNEL_SYMBOLS_H

#include <redshift/kernel.h>

/**
 * Load the symbol table from a file in the format <hexadecimal address> <symbol name> (one per line).
 * \param ptr The address of the file in memory.
 * \param size The size of the file in memory.
 */
void load_symbol_table(uintptr_t ptr, size_t size);

/**
 * Resolve a symbol name to an address.
 * \param name The name of the symbol.
 * \return A pointer to the named symbol if it exists, otherwise NULL.
 */
const void* get_symbol_address(const char* name);

/**
 * Get the name of a symbol by its address.
 * \param address The address of the symbol.
 * \return The symbol's name.
 */
const char* get_symbol_name(uintptr_t address);

#endif /* ! REDSHIFT_KERNEL_SYMBOLS_H */

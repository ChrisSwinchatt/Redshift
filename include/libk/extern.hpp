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
#ifndef REDSHIFT_LIBK_KEXTERN_HPP
#define REDSHIFT_LIBK_KEXTERN_HPP

#include <libk/macro.hpp>
#include <libk/types.hpp>

namespace libk_extern {
     /// Function to call when abort is triggered (e.g. upon failed assertion).
     /// \param fmt A formatted error message to print.
     /// \param ... Format arguments.
    extern void __noreturn abort(const char* fmt, ...);

     /// Function to call to print a string.
     /// \param s The string to print.
    extern void print_string(const char* s);

     /// Function to call to allocate static memory.
     /// \param size The amount of static memory to allocate.
     /// \return A pointer to the allocated memory on success; otherwise nullptr.
    extern void* static_allocate(size_t size);

     /// Function to call to allocate dynamic memory.
     /// \param size The amount of dynamic memory to allocate.
     /// \return A pointer to the allocated memory on success; otherwise nullptr.
    extern void* dynamic_allocate(size_t size);

     /// Function to call to free dynamic memory.
     /// \param ptr A pointer to the memory to be freed.
    extern void dynamic_free(void* ptr);
} // libk_extern

#endif // ! REDSHIFT_LIBK_KEXTERN_HPP

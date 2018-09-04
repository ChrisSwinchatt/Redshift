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
#include <libk/extern.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/console.hpp>
#include <redshift/mem/heap.hpp>
#include <redshift/mem/static.hpp>

namespace libk_extern {
    void __noreturn abort(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vpanic(fmt, ap);
        va_end(ap);
    }

    void print_string(const char* s)
    {
        redshift::kernel::console::write_string(s);
    }

    void* static_allocate(size_t size)
    {
        return redshift::mem::static_alloc(size);
    }

    void* dynamic_allocate(size_t size)
    {
        DEBUG_ASSERT(::redshift::mem::heap::kernel_heap != nullptr);
        return ::redshift::mem::heap::kernel_heap::allocate(size);
    }

    void dynamic_free(void* ptr)
    {
        DEBUG_ASSERT(::redshift::mem::heap::kernel_heap != nullptr);
        ::redshift::mem::kernel_heap::heap::free(ptr);
    }

    void* dynamic_resize(void* ptr, size_t new_size)
    {
        DEBUG_ASSERT(::redshift::mem::heap::kernel_heap != nullptr);
        return ::redshift::mem::kernel_heap::heap::resize(ptr, new_size);
    }
} // libk_extern

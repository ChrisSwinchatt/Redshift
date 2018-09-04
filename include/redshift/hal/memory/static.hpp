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
#ifndef REDSHIFT_HAL_MEMORY_STATIC_HPP
#define REDSHIFT_HAL_MEMORY_STATIC_HPP 1

#include <redshift/hal/memory/common.hpp>
#include <redshift/kernel.hpp>

namespace redshift { namespace hal { namespace memory_detail {
    /// Initialise the static heap allocator.
    void static_init();

    /// Allocate static memory.
    /// \param size Amount of memory to allocate.
    /// \param flags Allocation flags.
    /// \param phys Optional. Stores the physical address of the object.
    /// \return The address of the memory block is returned.
    uintptr_t static_alloc_base(size_t size, alignment align, uintptr_t* phys);

    /// Allocate static memory.
    /// \param size Amount of memory to allocate.
    /// \return A pointer to the allocated memory is returned.
    void* static_alloc(size_t size);
}}} // redshift::hal

#endif // ! REDSHIFT_HAL_MEMORY_STATIC_HPP

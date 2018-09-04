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
#ifndef REDSHIFT_KERNEL_HPP
#define REDSHIFT_KERNEL_HPP

#include <libk/abort.hpp>
#include <libk/assert.hpp>
#include <libk/bits.hpp>
#include <libk/macro.hpp>
#include <libk/meta.hpp>
#include <libk/types.hpp>
#include <redshift/kernel/asm.hpp>
#include <redshift/kernel/fwd.hpp>

using libk::array_size;

// Bring forward-declarations into global namespace.
using namespace redshift::kernel::fwd;

/// Size of the kernel stack.
#define STACK_SIZE  \
    (static_cast<size_t>(static_cast<uintptr_t>(__stack_top__) - static_cast<uintptr_t>(__stack_bottom__)))

/// Start of kernel (linker script).
extern symbol_t __kernel_start__;

/// Start of code section (linker script).
extern symbol_t __code_start__;

/// End of code section (linker script).
extern symbol_t __code_end__;

/// Start of data section (linker script).
extern symbol_t __data_start__;

/// End of data section (linker script).
extern symbol_t __data_end__;

/// Start of eh_frame section (linker script).
extern symbol_t __eh_frame_start__;

/// End of eh_frame section (linker script).
extern symbol_t __eh_frame_end__;

/// Start of read-only data section (linker script).
extern symbol_t __rodata_start__;

/// End of read-only data section (linker script).
extern symbol_t __rodata_end__;

/// Start of .bss section (linker script).
extern symbol_t __bss_start__;

/// End of .bss section (linker script).
extern symbol_t __bss_end__;

/// End of executable (linker script).
extern symbol_t __kernel_end__;

/// Bottom of stack (_start.S).
extern symbol_t __stack_bottom__;

/// Top of stack (_start.S).
extern symbol_t __stack_top__;


#endif // ! REDSHIFT_KERNEL_HPP

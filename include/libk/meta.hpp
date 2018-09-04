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
#ifndef REDSHIFT_LIBK_META_HPP
#define REDSHIFT_LIBK_META_HPP

#include <libk/macro.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Get the size of an array if known at compile-time.
    template <typename T, size_t S>
    constexpr size_t array_size(const T(& array)[S])
    {
        UNUSED(array);
        return S;
    }

    /// Metaclass for approximating logarithms at compile-time.
    template <intmax_t Value, intmax_t Base>
    struct log {
        static constexpr intmax_t value = 1 + log<Value/Base, Base>;
    };

    template <intmax_t Base>
    struct log<0, Base> {
        static constexpr intmax_t value = 0;
    };

    template <intmax_t Base>
    struct log<1, Base> {
        static constexpr intmax_t value = 1;
    };

    template <intmax_t Value>
    using log2 = log<Value, 2>;

    template <intmax_t Value>
    using log10 = log<Value, 10>;
}

#endif // ! REDSHIFT_LIBK_META_HPP

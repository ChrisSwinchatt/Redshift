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
#ifndef REDSHIFT_LIBK_ARRAY_HPP
#define REDSHIFT_LIBK_ARRAY_HPP

#include <libk/macro.hpp>
#include <libk/memory.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Fixed-size, statically-allocated array.
    template <typename T, size_t S>
    class array {
    public:
        using size_type       = size_t;
        using value_type      = T;
        using reference       = value_type&;
        using const_reference = const reference;
        using pointer         = value_type*;
        using const_pointer   = const pointer;

        /// Default constructor.
        array() = default;

        /// Constant constructor.
        constexpr array(const_reference buffer[S])
        : m_buffer(buffer)
        {
            // Do nothing.
        }

        constexpr array(const initializer_list& init)
        : m_buffer(&init[0])
        {
            static_assert(S == init.size(), "Invalid size of initializer_list");
        }

        /// Construct from initial array.
        /// \param buffer The initial array.
        array(const_reference buffer[S])
        {
            memory::copy(m_buffer, buffer, S);
        }

        /// Construct from initial value.
        /// \param value The initial value.
        explicit array(const_reference value)
        {
            memory::fill(m_buffer, value, S);
        }

        /// Get the number of elements in the array.
        /// \return The number of elements in the array.
        constexpr size_type size() const
        {
            return S;
        }

        /// Get a const reference to the element at a given index.
        /// \param index The index.
        /// \return A const reference to the element at a given index.
        /// \note This function does not do bounds checking.
        const_reference operator[](size_type index) const
        {
            return m_buffer[index];
        }

        /// Get a reference to the element at a given index.
        /// \param index The index.
        /// \return A reference to the element at a given index.
        /// \note This function does not do bounds checking.
        reference operator[](size_type index)
        {
            return m_buffer[index];
        }
    private:
        T m_buffer[S];
    };
}

#endif // ! REDSHIFT_LIBK_ARRAY_HPP

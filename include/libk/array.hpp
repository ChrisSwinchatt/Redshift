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

#include <libk/memory.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Fixed-size, statically-allocated array.
    template <typename T, size_t S>
    class array {
    public:
        /// Default constructor.
        array() = default;

        /// Construct from initial array.
        /// \param buffer The initial array.
        array(T buffer[S])
        {
            memory::copy(m_buffer, buffer, S);
        }

        /// Construct from initial value.
        /// \param value The initial value.
        explicit array(T value)
        {
            memory::fill(m_buffer, value, S);
        }

        /// Get the number of elements in the array.
        /// \return The number of elements in the array.
        constexpr size_t size() const
        {
            return S;
        }

        /// Get a const reference to the element at a given index.
        /// \param index The index.
        /// \return A const reference to the element at a given index.
        /// \note This function does not do bounds checking.
        const T& operator[](size_t index) const
        {
            return m_buffer[index];
        }

        /// Get a reference to the element at a given index.
        /// \param index The index.
        /// \return A reference to the element at a given index.
        /// \note This function does not do bounds checking.
        T& operator[](size_t index)
        {
            return m_buffer[index];
        }
    private:
        T m_buffer[S];
    };
}

#endif // ! REDSHIFT_LIBK_ARRAY_HPP

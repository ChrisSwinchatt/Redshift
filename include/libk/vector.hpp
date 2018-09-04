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
#ifndef REDSHIFT_LIBK_VECTOR_HPP
#define REDSHIFT_LIBK_VECTOR_HPP

#include <libk/extern.hpp>
#include <libk/memory.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Resizable array-like container.
    template <typename T>
    class vector {
    public:
        using size_type       = size_t;
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const pointer;
        using reference       = value_type&;
        using const_reference = const reference;
        using iterator        = pointer;
        using const_iterator  = const iterator;

        /// Construct empty vector.
        vector() = default;

        vector(const initializer_list<value_type>& init)
        : m_data(new T[init.size()])
        , m_size(init.size())
        , m_capacity(init.size())
        {
            static_assert(init.size() == S, "Invalid size of initializer_list");
            memory::copy(
                reinterpret_cast<void*>(m_buffer),
                reinterpret_cast<const void*>(&init[0]),
                m_size
            );
        }

        /// Construct with initial size and value.
        /// \param size The intial size.
        /// \param value The initial value.
        vector(size_type size, const_reference value = T())
        : m_data(new T[size])
        , m_size(size)
        , m_capacity(size)
        {
            memory::fill(m_data, value, m_size);
        }

        /// Construct with initial size, capacity and value.
        /// \param size The initial number of elements.
        /// \param capacity The initial capacity.
        /// \param value The initial value.
        vector(size_type size, size_type capacity, const_reference value = T())
        : m_data(new T[capacity])
        , m_size(size)
        , m_capacity(capacity)
        {
            memory::fill(m_data, value, m_size);
        }

        /// Construct from existing buffer.
        /// \param data The existing buffer.
        /// \param size The size of the existing buffer.
        vector(pointer data, size_type size)
        : m_data(data)
        , m_size(size)
        , m_capacity(size)
        {
            // Do nothing.
        }

        /// Copy from existing buffer.
        /// \param data The existing buffer.
        /// \param size The size of the existing buffer.
        vector(const_pointer data, size_type size)
        : m_data(new T[size])
        , m_size(size)
        , m_capacity(size)
        {
            memory::copy(
                reinterpret_cast<void*>(m_buffer),
                reinterpret_cast<const void*>(data),
                m_size
            );
        }

        /// Destroy vector.
        ~vector()
        {
            if (m_data != nullptr) {
                delete[] m_data;
                m_data = nullptr;
            }
        }

        /// Grow the vector's capacity without adding new elements.
        /// \param new_capacity Capacity of the vector after growing. Nothing happens if it's <= current capacity.
        void grow(size_type new_capacity)
        {
            if (new_capacity > m_capacity) {
                pointer tmp = libk_extern::dynamic_resize(reinterpret_cast<void*>(m_data), new_capacity);
                if (tmp != nullptr) {
                    m_data     = tmp;
                    m_capacity = new_capacity;
                }
            }
        }

        /// Shrink the vector capacity to match its size, i.e. the number of elements present.
        void shrink_to_fit()
        {
            pointer tmp = libk_extern::dynamic_resize(reinterpret_cast<void*>(m_data), m_size);
            if (tmp != nullptr) {
                m_data     = tmp;
                m_capacity = m_size;
            }
        }

        /// Resize the vector by adding or removing elements.
        /// \param new_size The new size of the vector. If smaller than the current size, any elements RIGHT of the
        /// element at index `new_size` will be deleted and the capacity of the vector will be changed to match the new
        /// size. If new_size is larger than the current size, new elements will be added using the value of `value`.
        /// \param value The value of the new elements added, if any.
        void resize(size_type new_size, const_reference value = value_type())
        {
            if (new_size < m_size) {
                m_size = new_size;
                shrink_to_fit();
            } else if (new_size > m_size) {
                const size_type old_size = m_size;
                if (new_size > m_capacity) {
                    grow(new_size);
                }
                for (size_type i = old_size; i < m_size; ++i) {
                    m_data[i] = value;
                }
            }
        }

        /// Erase elements by iterator.
        /// \param start First element to erase.
        /// \param end Last element to erase.
        void erase(iterator start, iterator end)
        {
            const size_type count = end - start;
            // Shift elements left.
            memory::copy(start, end, m_size - count);
            m_size -= count;
            // Zero the memory.
            memory::zero(start, count);
        }

        /// Erase elements by index.
        /// \param index The index of the first element to erase.
        /// \param [count] The number of elements to erase.
        void erase(size_type index, size_type count = 1)
        {
            auto start = begin() + index;
            erase(start, start + count - 1);
        }

        /// Append an element. The array will be grown/resized if necessary.
        /// \param value The value to append.
        void append(const_reference value)
        {
            if (m_size < capacity) {
                m_data[m_size] = value;
            } else {
                resize(m_size + 1);
                append(value);
            }
        }

        /// Get the size of the vector.
        /// \return The size of the vector
        size_type size() const
        {
            return m_size;
        }

        /// Get the capacity of the vector.
        /// \return The capacity of the vector
        size_type capacity() const
        {
            return m_capacity;
        }

        /// Get a reference to the element at the given index.
        /// \param index The element of the index to get.
        /// \return A reference to the element at the given index.
        reference operator[](size_type index)
        {
            return m_data[index];
        }

        /// Get a const reference to the element at the given index.
        /// \param index The element of the index to get.
        /// \return A const reference to the element at the given index.
        const_reference operator[](size_type index) const
        {
            return m_data[index];
        }

        /// Get an iterator at the first element.
        /// \return An iterator at the first element.
        iterator begin()
        {
            return m_data;
        }

        /// Get an iterator at the last element.
        /// \return An iterator at the last element.
        iterator end()
        {
            return m_data + m_size;
        }
    private:
        pointer   m_data;
        size_type m_size;
        size_type m_capacity;
    };
}

#endif // ! REDSHIFT_LIBK_VECTOR_HPP

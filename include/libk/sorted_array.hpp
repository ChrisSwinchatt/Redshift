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
#ifndef REDSHIFT_LIBK_SORTED_ARRAY_HPP
#define REDSHIFT_LIBK_SORTED_ARRAY_HPP

#include <libk/abort.hpp>
#include <libk/assert.hpp>
#include <libk/memory.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Returns true if a is smaller than b.
    template <typename T>
    bool ascending_order_predicate(const T& a, const T& b)
    {
        return a < b;
    }

    /// Returns true if a is larger than b.
    template <typename T>
    bool descending_order_predicate(const T& a, const T& b)
    {
        return a > b;
    }

    /// Array which enforces a user-specified ordering on its elements.
    template <typename T>
    class sorted_array {
    public:
        using size_type       = size_t;
        using value_type      = T;
        using reference       = value_type&;
        using const_reference = const reference;
        using pointer         = value_type*;
        using const_pointer   = const pointer;

        typedef bool(* predicate_type)(const_reference, const_reference);

        /// Flags.
        enum class flags {
            static_          = 0,      ///< Use static allocation (array cannot be resized).
            dynamic          = 1 << 0, ///< Use dynamic allocation (array can be resized).
            allow_duplicates = 1 << 1  ///< Allow duplicate values (default behaviour is to ignore duplicates).
        };

        /// Construct from capacity, sort order and optional flags.
        /// \param capacity The initial capacity of the array.
        /// \param [flags] Optional flags.
        /// \param [predicate] The order predicate. Defaults to ascending order.
        explicit sorted_array(
            size_type       capacity,
            flags           flags_   = flags::static_,
            predicate_type predicate = ascending_order_predicate<T>
        )
        : m_array(new value_type[size])
        , m_count(size)
        , m_predicate(predicate)
        {
            // Do nothing.
        }

        /// Construct in place from capacity, sort order and optional flags.
        /// \param address Memory location to store elements..
        /// \param capacity The initial capacity of the array.
        /// \param [flags] Optional flags.
        /// \param [predicate] The order predicate. Defaults to ascending order.
        sorted_array(
            pointer         address,
            size_type       capacity,
            flags           flags_   = flags::static_,
            predicate_type predicate = ascending_order_predicate<T>
        )
        : m_array(address)
        , m_count(size)
        , m_predicate(predicate)
        {
            // Do nothing.
        }


        /// Destroy sorted array.
        ~sorted_array()
        {
            delete[] m_array;
        }

        /// Get the number of elements currently in the array.
        /// \return The number of elements currently in the array.
        size_type size() const
        {
            return m_count;
        }

        /// Get the current capacity of the array.
        /// \return The current capacity of the array.
        size_type capacity() const
        {
            return m_capacity;
        }

        /// Get a const reference to the element at the given index.
        /// \return A const reference to the element at the given index.
        /// \note This is const because mutation could break sort order. To mutate an element, first remove it, then add
        /// it again so it can be inserted at the correct position.
        const_reference operator[](size_type index) const
        {
            DEBUG_ASSERT(index < m_count);
            return m_array[index];
        }

        /// Add an element to the array.
        /// \param value The value to insert.
        /// \return False is returned if the capacity of the array has been reached. Otherwise, true is returned.
        /// \note If allow_duplicates was not set in flags, and an element with the given value is already present,
        /// the value is not inserted, but true is returned as no error has occurred.
        bool add(const_reference value)
        {
            if (m_count == m_capacity) {
                return false;
            }
            size_t index = get_insert_index(value);
            if (index >= m_count) {
                m_array[m_count] = value;
            } else if (!(TEST_FLAG(m_flags, flags::allow_duplicates)) && m_array[index] == value) {
                // Don't insert a duplicate, but return true to indicate success since we are not supposed to allow
                // duplicated values.
                return true;
            } else {
                // Move elements right and insert.
                ::libk::memory::copy(m_array + index + 1, m_array + index, m_count - index - 1);
                m_array[index] = value;
            }
            m_count++;
            return true;
        }

        /// Remove an element by value.
        /// \param value The value to remove.
        /// \return true if an element was removed, otherwise false.
        /// \note If there are duplicates of the value, only the first will be removed. If allow_duplicates is set in
        /// flags, you should call this function repeatedly until it returns false.
        bool remove(const_reference value)
        {
            for (size_type i = 0; i < m_count; ++i) {
                if (m_array[i] == value) {
                    // Move elements left.
                    ::libk::memory::copy(m_array + i, m_array + i + 1, m_count - i - 1);
                    m_count--;
                    return true;
                }
            }
            return false;
        }

        /// Remove an element by index.
        /// \param index The index of the element to remove.
        /// \return true if an element was removed, otherwise false.
        /// \note If there are duplicates of the value, only the first will be removed. If allow_duplicates is set in
        /// flags, you should call this function repeatedly until it returns false.
        bool remove_at(size_t index)
        {
            return remove(m_array[index]);
        }

        /// Increase or decrease the capacity of the array.
        /// \param new_capacity The new capacity of the array. Must be >= the number of elements currently present in
        /// the array, otherwise abort is called.
        /// \warning Abort is also called if this array was allocated statically (flags::dynamic not set), or if
        /// dynamic_allocate returns nullptr.
        void resize(size_type new_capacity)
        {
            if (TEST_FLAG(m_flags, flags::static_)) {
                LIBK_ABORT("cannot resize static array", nullptr);
            }
            if (new_capacity < m_count) {
                LIBK_ABORT("cannot shrink occupied part of array. Use 'remove'.", nullptr);
            }
            if (new_capacity < m_capacity) {
                // Don't bother moving any data, just decrease the capacity.
                m_capacity = new_capacity;
            } else if (new_capacity > m_capacity) {
                // Copy everything to a new location and free the old pointer.
                pointer tmp = libk_extern::dynamic_allocate(sizeof(T)*new_capacity);
                if (tmp == nullptr) {
                    LIBK_ABORT("dynamic_allocate failed", nullptr);
                }
                ::libk::memory::copy(tmp, m_array, sizeof(T)*m_count);
                libk_extern::dynamic_free(m_array);
                m_array = tmp;
                m_capacity = new_capacity;
            }
        }
    private:
        predicate_type m_predicate;
        flags          m_flags;
        pointer        m_array;
        size_type      m_count;
        size_type      m_capacity;

        size_type get_insert_index(const_reference value)
        {
            for (size_type index = 0; index < m_count; ++index) {
                if (m_predicate(value, m_array[index])) {
                    return index;
                }
            }
            return m_count;
        }
    };

    /// Create a sorted array with its elements stored at a predetermined address.
    /// \param address The address to store the sorted array elements.
    /// \param capacity The capacity of the sorted array.
    /// \param predicate The order predicate.
    /// \return The sorted array is returned.
    template <typename T>
    sorted_array<T> place_sorted_array(
        T*                              address,
        size_t                          capacity,
        typename sorted_array<T>::flags          flags,
        typename sorted_array<T>::predicate_type predicate
    )
    {
        return sorted_array<T>(address, capacity, flags, predicate);
    }
}

#endif // ! REDSHIFT_LIBK_SORTED_ARRAY_HPP

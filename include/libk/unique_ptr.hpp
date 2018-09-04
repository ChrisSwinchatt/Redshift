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
#ifndef REDSHIFT_LIBK_UNIQUE_PTR_HPP
#define REDSHIFT_LIBK_UNIQUE_PTR_HPP

#include <libk/types.hpp>

namespace libk {
    /// "Owning" smart pointer.
    /// \tparam T The type of the managed object.
    /// \tparam Deleter The type of the deleting function.
    template <typename T, class Deleter = void(void*)>
    class unique_ptr {
    public:
        using value_type      = T;
        using pointer         = T*;
        using const_pointer   = const pointer;
        using reference       = T&;
        using const_reference = const reference;

        /// Construct an empty unique_ptr.
        constexpr unique_ptr() noexcept
        : m_ptr(nullptr)
        , m_deleter(delete)
        {
            // Do nothing.
        }

        /// Construct an empty unique_ptr.
        constexpr unique_ptr(nullptr_t) noexcept
        : m_ptr(nullptr)
        , m_deleter(delete)
        {
            // Do nothing.
        }

        /// Construct from pointer, using operator delete.
        explicit unique_ptr(pointer ptr)
        : m_ptr(ptr)
        , m_deleter(delete)
        {
            // Do nothing.
        }

        /// Construct from pointer, using user-specified deleter.
        unique_ptr(pointer ptr, Deleter& deleter)
        : m_ptr(ptr)
        , m_deleter(deleter)
        {
            // Do nothing.
        }

        /// Move managed object from existing unique_ptr.
        /// \param other An existing unique_ptr which loses ownership of its managed object.
        unique_ptr(unique_ptr&& other)
        : m_ptr(other.release())
        , m_deleter(other.m_deleter)
        {
            // Do nothing.
        }

        /// Deleted copy constructor. unique_ptr is not copyable.
        unique_ptr(const unique_ptr&) = delete;

        /// Destroy unique_ptr and its managed object.
        ~unique_ptr()
        {
            if (m_ptr != nullptr) {
                m_deleter(m_ptr);
                m_ptr = nullptr;
            }
        }

        /// Swap managed object with another unique_ptr.
        void swap(unique_ptr& other)
        {
            auto tmp_ptr = m_ptr;
            m_ptr = other.m_ptr;
            other.m_ptr = tmp_ptr;
        }

        /// Reset managed object to nullptr.
        void reset()
        {
            m_ptr = nullptr;
        }

        /// Reset managed object to nullptr and return previously-managed object.
        pointer release()
        {
            auto tmp_ptr = m_ptr;
            m_ptr = nullptr;
            return tmp_ptr;
        }

        /// Get a pointer to the managed object.
        pointer get()
        {
            return m_ptr;
        }

        /// Get a const pointer to the managed object.
        const_pointer get() const
        {
            return const_cast<const_pointer>(m_ptr);
        }

        /// Get a pointer to the managed object.
        operator pointer()
        {
            return get();
        }

        /// Get a const pointer to the managed object.
        operator const_pointer() const
        {
            return get();
        }

        /// Dereference the managed object.
        value_type& operator*()
        {
            return *m_ptr;
        }

        /// Dereference the managed object and acesss one of its members.
        value_type& operator->()
        {
            return *m_ptr;
        }
    private:
        pointer  m_ptr;
        Deleter& m_deleter;
    };

    /// Make a unique_ptr with template parameter deduction.
    /// \param ptr A pointer to the object to be managed.
    template <typename T>
    unique_ptr<T, void(void*)> make_unique(T* ptr)
    {
        return unique_ptr<T, void(void*)>(ptr);
    }

    /// Make a unique_ptr with template parameter deduction.
    /// \param ptr A pointer to the object to be managed.
    /// \param deleter A function which deletes the pointer.
    template <typename T, class Deleter>
    unique_ptr<T, Deleter> make_unique(T* ptr, Deleter& deleter)
    {
        return unique_ptr<T, Deleter>(ptr, deleter);
    }
}

#endif // ! REDSHIFT_LIBK_UNIQUE_PTR_HPP

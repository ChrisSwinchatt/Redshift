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
#ifndef REDSHIFT_LIBK_OPTIONAL_HPP
#define REDSHIFT_LIBK_OPTIONAL_HPP

#include <libk/abort.hpp>

namespace libk {
    /// Class representing an object which may or may not be present.
    template <typename T>
    struct optional {
        using value_type      = T;
        using reference       = value_type&;
        using const_reference = const reference;

        /// Construct without value.
        optional()
        : m_present(false)
        , m_ref(m_tmp)
        {
            // Do nothing.
        }

        /// Construct with value.
        /// \param value A reference to the value.
        explicit optional(reference value)
        : m_present(true)
        , m_ref(value)
        {
            // Do nothing.
        }

        /// Indicates whether an object is present.
        /// \return True if the object is present, otherwise false.
        bool present() const
        {
            return m_present;
        }

        /// Get a const reference to the object.
        /// \return A const reference to the object.
        /// \warning Abort is called if the object is not present.
        const_reference get() const
        {
            return const_cast<const_reference>(get());
        }

        /// Get a reference to the object.
        /// \return A reference to the object.
        /// \warning Abort is called if the object is not present.
        reference get()
        {
            if (!(m_present)) {
                LIBK_ABORT("can't access value of empty optional<T>", nullptr);
            }
            return m_ref;
        }
    private:
        T         m_tmp;
        reference m_ref;
        bool     m_present;
    };
}

#endif // ! REDSHIFT_LIBK_OPTIONAL_HPP

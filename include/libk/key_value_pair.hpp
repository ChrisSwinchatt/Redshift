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
#ifndef REDSHIFT_LIBK_KEY_VALUE_PAIR_HPP
#define REDSHIFT_LIBK_KEY_VALUE_PAIR_HPP

namespace libk {
    /// Key/value pair.
    template <typename TKey, typename TValue>
    struct key_value_pair {
        using key_type   = T1;
        using value_type = T2;

        /// The key.
        key_type   key;
        /// The value.
        value_type value;

        /// Construct from key and value.
        /// \param key_ The key.
        /// \param value_ The value.
        constexpr key_value_pair(const key_type& key_, const value_type& value_)
        : m_key(key_)
        , m_value(value_)
        {
            // Do nothing.
        }
    };
}

#endif // ! REDSHIFT_LIBK_KEY_VALUE_PAIR_HPP

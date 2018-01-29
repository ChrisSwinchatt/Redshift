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
#ifndef REDSHIFT_LIBK_FORWARD_LIST_HPP
#define REDSHIFT_LIBK_FORWARD_LIST_HPP

#include <libk/memory.hpp>
#include <libk/types.hpp>

namespace libk {
    /// Singly linked list.
    template <typename T>
    class forward_list {
    public:
        using value_type      = T;
        using size_type       = size_t;
        using reference       = value_type&;
        using const_reference = const reference;

        /// Flags.
        enum class flags {
            static_   = 0,      //< Statically allocated.
            dynamic   = 1 << 0, //< Dyamically allocated.
            duplicate = 1 << 1  //< Duplicate (tail) of another list.
        };

        /// Construct empty list.
        forward_list() : m_flags(flags::static_)
        {
            // Do nothing.
        }

        /// Construct empty list with flags.
        /// \param flags The flags.
        explicit forward_list(flags flags_) : m_flags(flags_)
        {
            // Do nothing.
        }

        /// Construct with initial size and data.
        /// \param size Initial number of elements.
        /// \param data Initial value of each element. Value will be copied.
        /// \param flags The flags.
        forward_list(size_type size, const_reference data, flags flags_ = flags::static_) : m_flags(flags_)
        {
            grow(size, data);
        }

        /// Destroy list.
        ~forward_list()
        {
            if (TEST_FLAG(m_flags, flags::dynamic_) && (!(TEST_FLAG(m_flags, flags::duplicate)))) {
                resize(0);
                m_head = nullptr;
                m_last = nullptr;
            }
        }

        /// Increase or decrease the size of the list by adding or removing elements.
        /// \param new_size The size of the list after resizing.
        /// \param data The initial value of any new elements. New elements will be added at the end of the list.
        /// Likewise, old elements will be removed from the end of the list. To remove elements at the beginning of the
        /// list, @see remove.
        /// Ignored if the size is being decreased.
        void resize(size_type new_size, const_reference data = value_type())
        {
            // Remove elements while the list is too big.
            while (m_size > new_size) {
                remove(m_size);
            }
            // Add elements while the list is too small.
            while (m_size < new_size) {
                append(data);
            }
        }

        /// Append an element to the list.
        /// \param data The element to insert.
        void append(const_reference data)
        {
            if (m_head == nullptr) {
                m_head = new_node(data);
                m_last = m_head;
            }
            DEBUG_ASSERT(m_last->next == nullptr);
            m_last->next = new_node(data);
            m_last = m_last->next;
            ++m_size;
        }

        /// Prepend an element to the list.
        /// \param data The element to insert.
        void prepend(const_reference data)
        {
            node* head = new_node();
            head->data = data;
            head->next = m_head;
            m_head = head;
        }

        /// Insert an element at a given index.
        /// \param index The index to insert at. 0 prepends, size() or greater appends.
        /// \param data The element to insert.
        void insert(size_type index, const_reference data)
        {
            node* elem = get_elem_by_index(index);
            node* next = elem->next;
            elem->next = new_node();
            elem = elem->next;
            elem->data = data;
            elem->next = next;
        }

        /// Remove an element from the list and return its value.
        /// \return The element removed from the list.
        reference pop()
        {
            reference ref = m_last->data;
            remove(m_size - 1);
            return ref;
        }

        /// Remove one or more elements at a given index.
        /// \param index
        void remove(size_type index, size_type count = 1)
        {
            if (index + count > m_size) {
                LIBK_ABORT(
                    "can't remove elements past the end of a list (index=%zu, count=%zu, size=%zu)",
                    index,
                    count,
                    m_size
                );
            }
            node* prev;
            node* elem;
            if (index > 0) {
                prev = get_elem_by_index(index - 1);
                elem = prev->next;
            } else {
                prev = get_elem_by_index(index);
                elem = prev;
            }
            DEBUG_ASSERT(prev != nullptr);
            for (; count > 0 && elem != nullptr; --count, --m_size) {
                node* next = elem->next;
                if (TEST_FLAG(m_flags, flags::dynamic)) {
                    libk_extern::dynamic_free(elem);
                    elem = nullptr;
                }
                elem = next;
                prev->next = next;
            }
            DEBUG_ASSERT(count == 0);
        }

        /// Get a const reference to the element at a given index.
        /// \param index The index.
        /// \return A const reference to the element at a given index.
        /// \note abort is called if the index is outside the bounds of the array.
        const_reference operator[](size_type index) const
        {
            return const_cast<const_reference>(operator[](index));
        }

        /// Get a reference to the element at a given index.
        /// \param index The index.
        /// \return A reference to the element at a given index.
        /// \note abort is called if the index is outside the bounds of the array.
        reference operator[](size_type index)
        {
            node* elem = get_elem_by_index(index);
            return elem->data;
        }

        /// Get the number of elements in the list.
        size_type size() const
        {
            return m_size;
        }

        /// Get a const reference to the data contained in the head element.
        /// \return A const reference to the data contained in the head element.
        const_reference head() const
        {
            return m_head->data;
        }

        /// Get a reference to the data contained in the head element.
        /// \return A reference to the data contained in the head element.
        reference head()
        {
            return m_head->data;
        }

        /// Get a list of the tail elements of the list, i.e. everything except the head element.
        /// \return A list of the tail elements of the list.
        const forward_list* tail() const
        {
            forward_list* list = new_list(flags::dynamic | flags::duplicate);
            list->m_head = m_head->next;
            list->m_last = m_last;
            list->m_size = m_size - 1;
            return list;
        }
    private:
        struct node {
            value_type data;
            node*      next;

            node() = default;

            explicit node(const_reference ref) : data(ref) {}
        };

        flags     m_flags;
        node*     m_head;
        node*     m_last;
        size_type m_size;

        node* new_node()
        {
            node* result = nullptr;
            if (TEST_FLAG(m_flags, flags::dynamic)) {
                result = static_cast<node*>(libk_extern::dynamic_allocate(sizeof(*result)));
            } else {
                result = static_cast<node*>(libk_extern::static_allocate(sizeof(*result)));
            }
            DEBUG_ASSERT(result != nullptr);
            memory::zero(result, sizeof(*result));
            result->data = value_type();
            return result;
        }

        forward_list* new_list()
        {
            forward_list* list = nullptr;
            if (TEST_FLAG(m_flags, flags::dynamic)) {
                list = static_cast<forward_list*>(libk_extern::dynamic_allocate(sizeof(*list)));
            } else {
                list = static_cast<forward_list*>(libk_extern::static_allocate(sizeof(*list)));
            }
            DEBUG_ASSERT(list != nullptr);
            memory::zero(list, sizeof(*list));
            return list;
        }

        node* get_elem_by_index(size_type index)
        {
            node* elem = m_head;
            for (size_type i = 0; i < index && elem != nullptr; ++i, elem = elem->next) {
                // Do nothing.
            }
            if (elem == nullptr) {
                LIBK_ABORT("Index out of bounds (index=%zu, size=%zu)", index, m_size);
            }
            return elem;
        }
    };
}

#endif /// ! REDSHIFT_LIBK_FORWARD_LIST_HPP

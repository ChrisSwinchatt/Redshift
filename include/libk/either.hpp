#ifndef REDSHIFT_LIBK_EITHER_HPP
#define REDSHIFT_LIBK_EITHER_HPP

#include <libk/abort.hpp>

namespace libk {
    template <typename TLeft, typename TRight>
    class either
    {
    public:
        using left_type            = TLeft;
        using left_ptr             = TLeft*;
        using const_left_ptr       = const TLeft*;
        using left_reference       = TLeft&;
        using const_left_reference = const TLeft&;

        using right_type = TRight;
        using right_ptr  = TRight*;
        using const_right_ptr       = const TRight*;
        using right_reference       = TRight&;
        using const_right_reference = const TRight&;

        either(const_left_reference left_) : m_left(const_cast<left_ptr>(&left_))
        {
            // Do nothing.
        }

        either(const_right_reference right_) : m_right(const_cast<right_ptr>(&right_))
        {
            // Do nothing.
        }

        const_left_reference left() const
        {
            ensure_left();
            return *m_left;
        }

        left_reference left()
        {
            ensure_left();
            return *m_left;
        }

        const_right_reference right() const
        {
            ensure_right();
            return *m_right;
        }

        right_reference right()
        {
            ensure_right();
            return *m_right;
        }
    private:
        left_ptr  m_left;
        right_ptr m_right;

        void ensure_left()
        {
            if (left == nullptr)
            {
                LIBK_ABORT("Requested left value, but was not present");
            }
        }

        void ensure_right()
        {
            if (right == nullptr)
            {
                LIBK_ABORT("Requested right value, but was not present");
            }
        }
    };
}

#endif // ! REDSHIFT_LIBK_EITHER_HPP
#ifndef REDSHIFT_LIBK_PIPELINE_HPP
#define REDSHIFT_LIBK_PIPELINE_HPP

#include <libk/forward_list.hpp>
#include <libk/macro.hpp>

namespace libk {
    namespace internal {
        template <class TError>
        struct pipeline_success_result : pipeline_result<TError> {
            const_error_ptr error_info()
            {
                return nullptr;
            }

            bool is_success_result()
            {
                return true;
            }
        };

        template <class TError>
        struct pipeline_failure_result : pipeline_result<TError> {
            const_error_ptr error_info() const
            {
                return m_error_info;
            }

            bool is_success_result() const
            {
                return false;
            }
        private:
            const_error_ptr m_error_info;
        };
    }

    template <class TContext, class TError>
    __abstract class pipeline_stage

    template <class TError>
    struct pipeline_result {
        using error_type      = TError;
        using error_ptr       = TError*;
        using const_error_ptr = const TError*;

        virtual const_error_ptr error_info() const = 0;

        virtual bool is_success_result() const = 0;

        bool has_error_info() const
        {
            return !is_success_result();
        }
    private:
        static pipeline_result* success()
        {
            return internal::pipeline_success_result
        }

        static pipeline_result* failure(const_error_ptr error_info)
        {
            return internal::pipeline_failure_result(error_info);
        }

        template <class TContext>
        friend __abstract class pipeline_stage<TContext, TError>;
    };

    template <class TContext, class TError>
    class pipeline;

    template <class TContext, class TError>
    __abstract class pipeline_stage
    {
    public:
        using context_type      = TContext;
        using context_ptr       = TContext*;
        using const_context_ptr = const TContext*;

        using result_type       = pipeline_result<TError>;
        using result_ptr        = pipeline_result<TError>*;
        using const_result_ptr  = const pipeline_result<TError>*;
        using const_error_ptr   = result_type::const_error_ptr;

        using stage_type            = pipeline_stage;
        using stage_ptr             = pipeline_stage*;
        using stage_const_ptr       = pipeline_stage const*;
        using stage_reference       = pipeline_stage&;
        using const_stage_reference = const pipeline_stage&;

        __abstract virtual result_ptr operator()(const_context_ptr context) = 0;
    protected:
        pipeline_stage(stage_const_ptr next) : m_next(next)
        {
            // Do nothing.
        }

        result_ptr next(const_context_ptr context)
        {
            if (m_next != nullptr)
            {
                return m_next->operator()(context);
            }

            return result_type::success();
        }

        result_ptr fail(const_error_ptr error) const
        {
            return result_type::failure(error);
        }
    private:
        stage_const_ptr m_next;

        void set_next(stage_const_ptr next)
        {
            m_next = next;
        }

        friend class pipeline<TContext, TError>;
    };

    template <class TContext, class TError>
    class pipeline {
    public:
        using stage_type        = pipeline_stage<TContext, TError>;
        using stage_ptr         = pipeline_stage<TContext, TError>*;
        using stage_const_ptr   = const pipeline_stage<TContext, TError>*;

        using context_type      = stage_type::context_type;
        using context_ptr       = stage_type::context_ptr;
        using const_context_ptr = stage_type::const_context_ptr;

        using result_type      = pipeline_result<TError>;
        using result_ptr       = pipeline_result<TError>*;
        using const_result_ptr = const pipeline_result<TError>;

        pipeline(stage_const_ptr last) : m_last(last)
        {
            m_stages.append(last);
        }

        void append(stage_const_ptr next)
        {
            auto last = m_stages.last();

            if (last != nullptr)
            {
                last->set_next(next);
            }

            next->set_next(m_last);
        }

        result_ptr operator()(const_context_ptr context)
        {
            auto first = m_stages.head();
            return first(context);
        }
    private:
        stage_const_ptr m_last;
        libk::forward_list<stage_const_ptr> m_stages;
    };
}

#endif // ! REDSHIFT_LIBK_PIPELINE_HPP
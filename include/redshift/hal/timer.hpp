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
#ifndef REDSHIFT_HAL_TIMER_HPP
#define REDSHIFT_HAL_TIMER_HPP

#include <libk/asciz.hpp>
#include <libk/forward_list.hpp>
#include <redshift/kernel.hpp>

namespace redshift { namespace hal {
    /// Timer.
    class timer {
    public:
        /// Timer tick rate (Hz). The maximum is ~1193182 Hz.
        static constexpr unsigned TICK_RATE = 1000000U;

        /// Approximate microseconds elapsed between ticks.
        static constexpr unsigned TICK_USEC = 1000000U/TICK_RATE;

        /// Event callback.
        struct event_callback {
            /// Handle the event.
            /// \return true if the event should continue to be processed, or false if it should be removed from the
            /// queue.
            virtual bool operator()() = 0;
        };

        /// Event.
        struct event {
            /// Event name.
            const char*     name;
            /// Name hash.
            uint32_t        name_hash;
            /// Event period (microseconds).
            uint64_t        period;
            /// Time elapsed since the event was last raised (microseconds).
            uint64_t        elapsed_time;
            /// Callback.
            event_callback& callback;

            /// Construct an event.
            /// \param name A name for the event.
            /// \param period The period of the event -- how many microseconds should elapse between raising the event.
            /// \param callback The function to call when the event is raised. Returns true if the event should stay in
            /// the queue and false if it should be removed.
            event(const char* name_, uint64_t period_, event_callback& callback_);
        };

        /// Initialise the timer.
        static void init();

        /// Add an event to the timer queue.
        /// \param event_ The event to add.
        static void add_event(const event& event_);

        /// Add an event to the timer queue.
        /// \param name A name for the event.
        /// \param period The period of the event -- how many microseconds should elapse between raising the event.
        /// \param callback The function to call when the event is raised.
        static void add_event(const char* name, uint64_t period, event_callback& callback);

        /// Remove an event from the queue.
        void remove_event(const event& event_);

        /// Remove an event from the queue.
        void remove_event(const char* name);
    private:
        static ::libk::forward_list<event> m_queue;

        // Process queue and trigger events.
        static void process_queue(void* ignored);
    };
}}

#endif // ! REDSHIFT_HAL_TIMER_HPP

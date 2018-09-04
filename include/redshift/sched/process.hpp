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
#ifndef REDSHIFT_SCHED_PROCESS_HPP
#define REDSHIFT_SCHED_PROCESS_HPP

#include <redshift/hal/cpu/state.hpp>
#include <redshift/kernel.hpp>
#include <redshift/kernel/interrupt.hpp>
#include <redshift/hal/memory/paging.hpp>

/// Process flags.
typedef enum {
    PROCESS_FLAGS_SUPERVISOR = 0,       /// Process runs in supervisor mode (ring 0).
    PROCESS_FLAGS_USER       = 1 << 0   /// Process runs in user mode (ring 3).
} process_flags_t;

struct process;

typedef enum {
    PROCESS_PRIORITY_MIN  =  0,
    PROCESS_PRIORITY_LOW  =  3,
    PROCESS_PRIORITY_AVG  =  7,
    PROCESS_PRIORITY_HIGH = 11,
    PROCESS_PRIORITY_MAX  = 15
} process_priority_t;

/// Spawns a new process.
/// \param entry_point The entry point of the process.
/// \param page_dir The page directory.
/// \param priority The process priority (0..PROCESS_PRIORITY_MAX).
/// \param stack_addr The address of the *bottom* of the process' stack. If this is zero, a new stack will be created.
/// \param stack_size The size of the process' stack.
/// \return The process ID is returned.
 int process_spawn(
     uintptr_t          entry_point,
     ::redshift::hal::memory_detail::page_directory*    page_dir,
     process_priority_t priority,
     uintptr_t          stack_addr,
     size_t             stack_size,
     process_flags_t    flags
);

/// Switches to the next process.
/// \param regs If not nullptr, points to a hal::cpu::state instance which contains the updated register state of the
/// process which was interrupted.
void __non_reentrant process_switch(void* regs);

/// Get a handle to the currently-executing process.
/// \return A handle to the currently-executing process.
const struct process* __non_reentrant get_current_process();

/// Get the ID of a process.
/// \param process The process.
/// \return The process ID.
int get_process_id(const struct process* process);

/// Get the ID of the currently-executing process. Equivalent to `get_process_id(get_current_process())`;
/// \return The process ID.
int __non_reentrant get_current_process_id();

/// Yield the timeslice of the current process.
static inline void __always_inline __non_reentrant process_yield()
{
    using redshift::hal::cpu;
    cpu::state regs;
    cpu::state::get(&regs);
    process_switch(&regs);
}

#endif // ! REDSHIFT_SCHED_PROCESS_HPP

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
#ifndef REDSHIFT_KERNEL_INTERRUPT_H
#define REDSHIFT_KERNEL_INTERRUPT_H

#include <redshift/hal/cpu.hpp>
#include <redshift/kernel.hpp>

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef enum {
    ISR_TYPE_ABORT,
    ISR_TYPE_FAULT,
    ISR_TYPE_INTERRUPT,
    ISR_TYPE_RESERVED,
    ISR_TYPE_TRAP
} isr_type_t;

typedef enum {
    ISR_DIVIDE_BY_ZERO              = 0x00,
    ISR_DEBUG                       = 0x01,
    ISR_BREAKPOINT                  = 0x03,
    ISR_OVERFLOW                    = 0x04,
    ISR_BOUNDCHECK                  = 0x05,
    ISR_INVALID_OPCODE              = 0x06,
    ISR_DEVICE_NOT_AVAILABLE        = 0x07,
    ISR_DOUBLE_FAULT                = 0x08,
    ISR_COPROCESSOR_SEGMENT_OVERRUN = 0x09,
    ISR_INVALID_TSS                 = 0x0A,
    ISR_SEGMENT_NOT_PRESENT         = 0x0B,
    ISR_STACK_SEGMENT_FAULT         = 0x0C,
    ISR_GENERAL_PROTECTION_FAULT    = 0x0D,
    ISR_PAGE_FAULT                  = 0x0E,
    ISR_FLOATING_POINT              = 0x10,
    ISR_ALIGNMENT_CHECK             = 0x11,
    ISR_MACHINE_CHECK               = 0x12,
    ISR_SIMD_EXCEPTION              = 0x13,
    ISR_VIRTUALIZATION_EXCEPTION    = 0x14,
    ISR_SECURITY_EXCEPTION          = 0x1E,
    ISR_FPU_ERROR                   = 0x21
} isr_t;

struct isr_info {
    isr_type_t  type;
    bool        has_error_code;
    const char* message;
};

extern const struct isr_info isr_info[];

/// ISR handler function
typedef void(* isr_handler_t)(const struct cpu_state*);

/// Create an ISR handler
/// \param n ISR number
/// \param fp Function pointer
void set_interrupt_handler(uint8_t n, isr_handler_t fp);

namespace redshift { namespace kernel {
    /// Interrupt state.
    enum class interrupt_state : int {
        disabled = 0,
        enabled  = 1
    };

    /// Interrupt state guard. Sets the interrupt state and resets it when it goes out of scope.
    class interrupt_state_guard {
    public:
        /// Set an interrupt state.
        explicit interrupt_state_guard(interrupt_state state)
        : m_prev_state(static_cast<interrupt_state>(get_interrupt_state()))
        {
            set_interrupt_state(static_cast<int>(state));
        }

        /// Reset the previous interrupt state.
        ~set_interrupt_state()
        {
            set_interrupt_state(static_cast<int>(m_prev_state));
        }
    private:
        interrupt_state m_prev_state;
    };
}}

#endif // ! REDSHIFT_KERNEL_INTERRUPT_H

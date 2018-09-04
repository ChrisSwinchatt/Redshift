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
#include <libk/asciz.hpp>
#include <libk/memory.hpp>
#include <redshift/hal/cpu/tss.hpp>
#include <redshift/kernel.hpp>

extern "C" void loadtss();

namespace redshift { namespace hal { namespace cpu_detail {
    tss& tss::init()
    {
        libk::memory::fill8(&m_instance, 0, sizeof(m_instance));
        m_instance.esp0 = (uint32_t)__stack_top__;
        m_instance.ss0  = 0x10;
        m_instance.iobt = sizeof(m_instance);
        m_instance.load();
        return m_instance;
    }

    tss& tss::instance()
    {
        return m_instance;
    }

    void tss::load()
    {
        interrupt_state_guard guard(interrupt_state::disable);
        loadtss();
    }

    uintptr_t tss::base()
    {
        return static_cast<uintptr_t>(&m_instance);
    }

    size_t tss::size()
    {
        return sizeof(m_instance);
    }
}}}

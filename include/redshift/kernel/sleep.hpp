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
#ifndef REDSHIFT_KERNEL_SLEEP_HPP
#define REDSHIFT_KERNEL_SLEEP_HPP

#include <redshift/kernel.hpp>

/// Block the calling thread until at least 'sec' seconds have passed.
/// \param sec Number of seconds to sleep for.
void sleep(uint32_t sec);

/// Block the calling thread until at least 'msec' milliseconds have passed.
/// \param msec Number of milliseconds to sleep for.
void msleep(uint64_t msec);

/// Block the calling thread until at least 'usec' microseconds have passed.
/// \param usec The number of microseconds to sleep for.
void usleep(uint64_t usec);

#endif // ! REDSHIFT_KERNEL_SLEEP_HPP

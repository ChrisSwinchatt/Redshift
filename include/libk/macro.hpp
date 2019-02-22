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
#ifndef REDSHIFT_LIBK_MACRO_HPP
#define REDSHIFT_LIBK_MACRO_HPP

/// Stringify helper.
#define __STRINGIFY_HELPER(X)       #X

/// Stringify something.
#define STRINGIFY(X)                __STRINGIFY_HELPER(X)

/// Current line number as a string.
#define __LINE_NO__                 STRINGIFY(__LINE__)

/// Current file and line number as a colon-delimited string.
#define __FILE_LINE__               __FILE__ ":" __LINE_NO__

/// Scoped enum helper. Used when "enum class" causes problems e.g. using as flags.
/// Enum values are accessed by NAME::value, but the type is called NAME_t.
/// \example
///     SCOPED_ENUM(foo, fee = 1, fie, foe, fum);
///     foo_t foo_flags = foo::fee | foo::fum;
/// Using C++11 scoped enums yields:
///     enum class foo { fee = 1, fie, foe, fum };
///     foo foo_flags = foo(int(foo::fee) | int(foo::fum));
#define SCOPED_ENUM(NAME, ...)  \
    struct NAME {               \
        enum t {                \
            __VA_ARGS__         \
        };                      \
    };                          \
    using NAME##_t = NAME::t

/// Return the biggest value out of A and B.
#define MAX(A, B)                   ((A) > (B) ? (A) : (B))

/// Return the smallest value out of A and B.
#define MIN(A, B)                   ((A) < (B) ? (A) : (B))

/// Suppress unused parameter/variable warning.
#define UNUSED(X)                   ((void)(X))

/// Explicitly do nothing.
#define DO_NOTHING                  ;

/// Hint to the compiler that X will usually evaluate true.
#define likely(X)                   __builtin_expect((X), 1)

/// Hint to the compiler that X will usually evaluate false.
#define unlikely(X)                 __builtin_expect((X), 0)

/// Mark a structure as having packed storage.
#ifndef __packed
# define __packed                   __attribute__((packed))
#endif

/// Mark a function as using printf-like formatting.
#ifndef __printf
# define __printf(FMT, ARGS)        __attribute__((format(printf, FMT, ARGS)))
#endif

/// Mark a function as "always inline".
#ifndef __always_inline
# define __always_inline            __attribute__((always_inline))
#endif

/// Mark a function as "no inline".
#ifndef __noinline
# define __noinline                 __attribute__((noinline))
#endif

/// Mark a function as "no return".
#ifndef __noreturn
# define __noreturn                 __attribute__((noreturn))
#endif

/// Mark a function as non-reentrant.
#define __non_reentrant

/// Mark a class as abstract.
#define __abstract

/// Mark a function as a constructor. Constructors are called in sequence during boot, before the scheduler is started.
/// \param PRIORITY Controls the order in which constructors are called (smallest first).
#define __init(PRIORITY)            __attribute__((constructor(PRIORITY)))

/// Mark a function as a destructor. Destructor are called in sequence during shutdown.
/// \param PRIORITY Controls the order in which destructors are called (largest first).
#define __fini(PRIORITY)            __attribute__((destructor(PRIORITY)))

/// Fall through (e.g. in case labels).
#define FALL_THROUGH                __attribute__((fallthrough))

#endif // ! REDSHIFT_LIBK_MACRO_HPP

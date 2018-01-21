#ifndef REDSHIFT_TESTS_UNIT_TEST_H
#define REDSHIFT_TESTS_UNIT_TEST_H

#include <stdio.h>
#include <stdlib.h>

#define BEGIN_TEST(NAME)                    \
    static void test_##NAME(void)           \
    {                                       \
        const char* __test_name__ = #NAME;  \
        int         __test_want_fail__ = 0;

#define END_TEST \
        PASS();  \
    }


#define __DO_PASS()                                     \
    do {                                                \
        fprintf(stderr, " * PASS: %s\n", __test_name__);\
        ++__tests_passed__;                             \
        ++__tests_executed__;                           \
        return;                                         \
    } while (0)

#define __DO_FAIL(FMT, ...)                                                     \
    do {                                                                        \
        fprintf(stderr, " * FAIL: %s " FMT "\n", __test_name__, __VA_ARGS__);   \
        ++__tests_failed__;                                                     \
        ++__tests_executed__;                                                   \
        return;                                                                 \
    } while (0)

#define PASS()                                          \
    do {                                                \
        if (__test_want_fail__) {                       \
            __DO_FAIL("test supposed to fail", NULL);   \
        } else {                                        \
            __DO_PASS();                                \
        }                                               \
    } while (0)

#define FAIL(FMT, ...)                      \
    do {                                    \
        if (!(__test_want_fail__)) {        \
            __DO_FAIL(FMT, __VA_ARGS__);    \
        }                                   \
    } while (0)

#define WANT_PASS               \
    do {                        \
        __test_want_fail__ = 0; \
    } while (0)

#define WANT_FAIL               \
    do {                        \
        __test_want_fail__ = 1; \
    } while (0)

#define ASSERT(X)                                       \
    do {                                                \
        if (!(X)) {                                     \
            FAIL(": assert failed: \"" #X "\"", NULL);  \
        }                                               \
    } while (0)

#define ASSERT_EQUAL_INT(A, B)                              \
    do {                                                    \
        if (A != B) {                                       \
            FAIL(": not equal: expected %d, got %d", A, B); \
        }                                                   \
    } while (0)

#define ASSERT_EQUAL_LONG(A, B)                                 \
    do {                                                        \
        if (A != B) {                                           \
            FAIL(": not equal: expected %ld, got %ld", A, B);   \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_LLONG(A, B)                                \
    do {                                                        \
        if (A != B) {                                           \
            FAIL(": not equal: expected %Ld, got %Ld", A, B);   \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_UINT(A, B)                             \
    do {                                                    \
        if (A != B) {                                       \
            FAIL(": not equal: expected %u, got %u", A, B); \
        }                                                   \
    } while (0)

#define ASSERT_EQUAL_ULONG(A, B)                                \
    do {                                                        \
        if (A != B) {                                           \
            FAIL(": not equal: expected %lu, got %lu", A, B);   \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_ULLONG(A, B)                               \
    do {                                                        \
        if (A != B) {                                           \
            FAIL(": not equal: expected %Lu, got %Lu", A, B);   \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_PTR(A, B)                                  \
    do {                                                        \
        if (strcmp(A, B) != 0) {                                \
            FAIL(": not equal: expected 0x%p, got 0x%p", A, B)  \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_STR(A, B)                              \
    do {                                                    \
        if (strcmp(A, B) != 0) {                            \
            FAIL(": not equal: expected %s, got %s", A, B)  \
        }                                                   \
    } while (0)

#define SETUP(F)                \
    do {                        \
        void(* fn)(void) = F;   \
        if (fn != NULL) {       \
            fn();               \
        }                       \
    } while (0)

#define CLEANUP(F)                                  \
    do {                                            \
        void(* fn)(void) = F;                       \
        if (fn != NULL) {                           \
            fn();                                   \
        }                                           \
        fprintf(                                    \
            stderr,                                 \
            "%d passed and %d failed of %d total",  \
            __tests_passed__,                       \
            __tests_failed__,                       \
            __tests_executed__                      \
        );                                          \
        return __tests_failed__;                    \
    } while (0)

#define RUN_TEST(NAME) test_##NAME()

typedef void(* __test_setup_fn_t__)(void);
typedef void(* __test_cleanup_fn_t__)(void);

static int __tests_passed__   = 0;
static int __tests_failed__   = 0;
static int __tests_executed__ = 0;

int printk(const char* fmt, ...)
{
    fprintf(stderr, "[test output] ");
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}

void panic(const char* fmt, ...)
{
    fprintf(stderr, "[test output] Kernel panic - ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void* static_alloc(size_t size)
{
    return malloc(size);
}

void* kmalloc(size_t size)
{
    return malloc(size);
}

void kfree(void* ptr)
{
    return free(ptr);
}

#endif /* ! REDSHIFT_TESTS_UNIT_TEST_H */

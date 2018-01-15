#ifndef REDSHIFT_TESTS_UNIT_TEST_H
#define REDSHIFT_TESTS_UNIT_TEST_H

#include <stdio.h>

#define DEFINE_SETUP(FUNC)      \
    static void FUNC(void);     \
    __test_setup_fn__ = FUNC;   \
    static void FUNC(void)

#define END_SETUP }

#define DEFINE_CLEANUP(FUNC)    \
    static void FUNC(void);     \
    __test_cleanup_fn__ = FUNC; \
    static void FUNC(void)


#define PASS()                                      \
    do {                                            \
        fprintf(stderr, " * %s: PASS", __name__);   \
        ++__tests_passed__;                         \
        ++__tests_executed__;                       \
        return;                                     \
    } while (0)

#define FAIL(FMT, ...)                                              \
    do {                                                            \
        fprintf(stderr, " * %s: FAIL " FMT, __name__, __VA_ARGS__); \
        ++__tests_failed__;                                         \
        ++__tests_executed__;                                       \
        return;                                                     \
    } while (0)

#define DEFINE_TEST(NAME)               \
    static void test_##NAME(void)       \
    {                                   \
        const char* __name__ = #NAME;


#define END_TEST \
        PASS();  \
    }

#define ASSERT(X)
    do {                                                  \
        if (!(X)) {                                       \
            FAIL(": assert failed: \"" #X "\"", NULL);    \
        }                                                 \
    } while (0)

#define ASSERT_EQUAL_INT32(A, B)
    do {                                                    \
        if (A != B) {                                       \
            FAIL(": not equal: expected %d, got %d", A, B); \
        }                                                   \
    } while (0)

#define ASSERT_EQUAL_INT64(A, B)
    do {                                                        \
        if (A != B) {                                           \
            FAIL(": not equal: expected %Ld, got %Ld", A, B);   \
        }                                                       \
    } while (0)

#define ASSERT_EQUAL_UINT32(A, B)
    do {                                                    \
        if (A != B) {                                       \
            FAIL(": not equal: expected %u, got %u", A, B); \
        }                                                   \
    } while (0)

#define ASSERT_EQUAL_UINT64(A, B)
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

typedef void(* __test_setup_fn_t__)(void);
typedef void(* __test_cleanup_fn_t__)(void);

static int                   __tests_passed__    = 0;
static int                   __tests_failed__    = 0;
static int                   __tests_defined__   = 0;
static int                   __tests_executed__  = 0;
static __test_setup_fn_t__   __test_setup_fn__   = NULL;
static __test_cleanup_fn_t__ __test_cleanup_fn__ = NULL;

#define SETUP()                     \
    do {                            \
        if (__test_setup_fn__) {    \
            __test_setup_fn__();    \
        }                           \
    } while (0)

#define CLEANUP()                                       \
    do {                                                \
        if (__test_cleanup_fn__) {                      \
            __test_cleanup_fn__();                      \
        }                                               \
        fprintf(                                        \
            stderr,                                     \
            "TESTS:\tDEFINED\tEXEC'D\tPASSED\tFAILED\n" \
            "\t%d\t%d\t%d\t%d\n"                        \
        );                                              \
        return __tests_failed__;                        \
    } while (0)

#define RUN_TEST(NAME) test##NAME()

void printk(const char* fmt, ...)
{
    fprintf(stderr, "[test output] ");
    {
        va_list ap;
        va_start(fmt, ap);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
}

void panic(const char* fmt, ...)
{
    fprintf(stderr, "[test output] Kernel panic - ");
    {
        va_list ap;
        va_start(fmt, ap);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    FAIL("", NULL);
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

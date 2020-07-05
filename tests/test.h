#ifndef REDSHIFT_TESTS_UNIT_TEST_H
#define REDSHIFT_TESTS_UNIT_TEST_H

#include <stdio.h>
#include <stdlib.h>

#define BEGIN_TEST(NAME)                    \
    static void test_##NAME(void)           \
    {                                       \
        __tests_executed__++;               \
        const char* __test_name__ = #NAME;  \
        int         __test_want_fail__ = 0;

#define END_TEST \
        PASS();  \
    }


#define __DO_PASS()                                     \
    do {                                                \
        fprintf(stderr, " * PASS: %s\n", __test_name__);\
        __tests_passed__++;                             \
        return;                                         \
    } while (0)

#define __DO_FAIL(FMT, ...)                                                     \
    do {                                                                        \
        fprintf(stderr, " * FAIL: %s " FMT "\n", __test_name__, __VA_ARGS__);   \
        __tests_failed__++;                                                     \
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

#define SETUP(F)                   \
    do {                           \
        __test_setup_fn_t__ fn = F;\
        if (fn != NULL) {          \
            fn();                  \
        }                          \
    } while (0)

#define CLEANUP(F)                   \
    do {                             \
        __test_cleanup_fn_t__ fn = F;\
        if (fn != NULL) {            \
            fn();                    \
        }                            \
    } while (0)

#define RUN_TEST(NAME) test_##NAME()

typedef void(* __test_setup_fn_t__)(void);
typedef void(* __test_cleanup_fn_t__)(void);
typedef void(* __test_case_fn_t__)(void);

#define __TEST_LIST_ITEM_0(...)
#define __TEST_LIST_ITEM_1(FN)       test_##FN
#define __TEST_LIST_ITEM_2(FN, ...)  test_##FN, __TEST_LIST_ITEM_1(__VA_ARGS__)
#define __TEST_LIST_ITEM_3(FN, ...)  test_##FN, __TEST_LIST_ITEM_2(__VA_ARGS__)
#define __TEST_LIST_ITEM_4(FN, ...)  test_##FN, __TEST_LIST_ITEM_3(__VA_ARGS__)
#define __TEST_LIST_ITEM_5(FN, ...)  test_##FN, __TEST_LIST_ITEM_4(__VA_ARGS__)
#define __TEST_LIST_ITEM_6(FN, ...)  test_##FN, __TEST_LIST_ITEM_5(__VA_ARGS__)
#define __TEST_LIST_ITEM_7(FN, ...)  test_##FN, __TEST_LIST_ITEM_6(__VA_ARGS__)
#define __TEST_LIST_ITEM_8(FN, ...)  test_##FN, __TEST_LIST_ITEM_7(__VA_ARGS__)
#define __TEST_LIST_ITEM_9(FN, ...)  test_##FN, __TEST_LIST_ITEM_8(__VA_ARGS__)
#define __TEST_LIST_ITEM_10(FN, ...) test_##FN, __TEST_LIST_ITEM_9(__VA_ARGS__)
#define __TEST_LIST_ITEM_11(FN, ...) test_##FN, __TEST_LIST_ITEM_10(__VA_ARGS__)
#define __TEST_LIST_ITEM_12(FN, ...) test_##FN, __TEST_LIST_ITEM_11(__VA_ARGS__)
#define __TEST_LIST_ITEM_13(FN, ...) test_##FN, __TEST_LIST_ITEM_12(__VA_ARGS__)
#define __TEST_LIST_ITEM_14(FN, ...) test_##FN, __TEST_LIST_ITEM_13(__VA_ARGS__)
#define __TEST_LIST_ITEM_15(FN, ...) test_##FN, __TEST_LIST_ITEM_14(__VA_ARGS__)
#define __TEST_LIST_ITEM_16(FN, ...) test_##FN, __TEST_LIST_ITEM_15(__VA_ARGS__)
#define __TEST_LIST_ITEM_17(FN, ...) test_##FN, __TEST_LIST_ITEM_16(__VA_ARGS__)
#define __TEST_LIST_ITEM_18(FN, ...) test_##FN, __TEST_LIST_ITEM_17(__VA_ARGS__)
#define __TEST_LIST_ITEM_19(FN, ...) test_##FN, __TEST_LIST_ITEM_18(__VA_ARGS__)

#define __GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N

#define __DO_TEST_LIST(...)             \
    __GET_NTH_ARG(                      \
        "ignore",                       \
        __VA_ARGS__,                    \
        __TEST_LIST_ITEM_19,            \
        __TEST_LIST_ITEM_18,            \
        __TEST_LIST_ITEM_17,            \
        __TEST_LIST_ITEM_16,            \
        __TEST_LIST_ITEM_15,            \
        __TEST_LIST_ITEM_14,            \
        __TEST_LIST_ITEM_13,            \
        __TEST_LIST_ITEM_12,            \
        __TEST_LIST_ITEM_11,            \
        __TEST_LIST_ITEM_10,            \
        __TEST_LIST_ITEM_9,             \
        __TEST_LIST_ITEM_8,             \
        __TEST_LIST_ITEM_7,             \
        __TEST_LIST_ITEM_6,             \
        __TEST_LIST_ITEM_5,             \
        __TEST_LIST_ITEM_4,             \
        __TEST_LIST_ITEM_3,             \
        __TEST_LIST_ITEM_2,             \
        __TEST_LIST_ITEM_1,             \
        __TEST_LIST_ITEM_0)(__VA_ARGS__)

#define TEST_LIST(...)                                                       \
    void test_main(void)                                                     \
    {                                                                        \
        __test_case_fn_t__ test_cases[] = {                                  \
            __DO_TEST_LIST(__VA_ARGS__)                                      \
        };                                                                   \
        for (size_t i = 0; i < sizeof(test_cases)/sizeof(*test_cases); i++) {\
            test_cases[i]();                                                 \
        }                                                                    \
    }

extern int __tests_passed__;
extern int __tests_failed__;
extern int __tests_executed__;

#endif /* ! REDSHIFT_TESTS_UNIT_TEST_H */

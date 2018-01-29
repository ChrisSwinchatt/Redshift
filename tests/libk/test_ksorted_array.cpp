#include <stdarg.h>
#include <stdio.h>

#include <redshift/util/kksorted_array.h>

#include "test.h"

static struct kksorted_array* array_asc;
static struct kksorted_array* array_desc;
static const size_t CAPACITY = 100;
static uint32_t value1 = 777;
static uint32_t value2 = 888;
static uint32_t value3 = 999;

static void cleanup(void)
{
    kksorted_array_delete(array_asc);
    kksorted_array_delete(array_desc);
}

BEGIN_TEST(kksorted_array_create)
    array_asc = kksorted_array_create(CAPACITY, false, uint32_t_ascending_order_predicate);
    ASSERT(array_asc != NULL);
    array_desc = kksorted_array_create(CAPACITY, false, uint32_t_descending_order_predicate);
    ASSERT(array_desc != NULL);
END_TEST

BEGIN_TEST(kksorted_array_count)
    const size_t count_asc  = kksorted_array_count(array_asc);
    const size_t count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(0UL, count_asc);
    ASSERT_EQUAL_ULONG(0UL, count_desc);
END_TEST

BEGIN_TEST(kksorted_array_add)
    kksorted_array_add(array_asc,  &value1);
    kksorted_array_add(array_desc, &value1);
    size_t count_asc  = kksorted_array_count(array_asc);
    size_t count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(1UL, count_asc);
    ASSERT_EQUAL_ULONG(1UL, count_desc);
    kksorted_array_add(array_asc,  &value2);
    kksorted_array_add(array_desc, &value2);
    count_asc  = kksorted_array_count(array_asc);
    count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(2UL, count_asc);
    ASSERT_EQUAL_ULONG(2UL, count_desc);
    kksorted_array_add(array_asc,  &value3);
    kksorted_array_add(array_desc, &value3);
    count_asc  = kksorted_array_count(array_asc);
    count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(3UL, count_asc);
    ASSERT_EQUAL_ULONG(3UL, count_desc);
END_TEST

BEGIN_TEST(kksorted_array_get)
    uint32_t asc1  = *(uint32_t*)kksorted_array_get(array_asc,  0);
    uint32_t asc2  = *(uint32_t*)kksorted_array_get(array_asc,  1);
    uint32_t asc3  = *(uint32_t*)kksorted_array_get(array_asc,  2);
    uint32_t desc1 = *(uint32_t*)kksorted_array_get(array_desc, 0);
    uint32_t desc2 = *(uint32_t*)kksorted_array_get(array_desc, 1);
    uint32_t desc3 = *(uint32_t*)kksorted_array_get(array_desc, 2);
    ASSERT_EQUAL_UINT(value1, asc1);
    ASSERT_EQUAL_UINT(value2, asc2);
    ASSERT_EQUAL_UINT(value3, asc3);
    ASSERT_EQUAL_UINT(value3, desc1);
    ASSERT_EQUAL_UINT(value2, desc2);
    ASSERT_EQUAL_UINT(value1, desc3);
END_TEST

BEGIN_TEST(kksorted_array_remove)
    kksorted_array_remove(array_asc,  0);
    kksorted_array_remove(array_desc, 0);
    size_t count_asc  = kksorted_array_count(array_asc);
    size_t count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(2UL, count_asc);
    ASSERT_EQUAL_ULONG(2UL, count_desc);
    uint32_t asc1  = *(uint32_t*)kksorted_array_get(array_asc,  0);
    uint32_t desc1 = *(uint32_t*)kksorted_array_get(array_desc, 0);
    ASSERT_EQUAL_UINT(value2, asc1);
    ASSERT_EQUAL_UINT(value2, desc1);
    kksorted_array_remove(array_asc,  0);
    kksorted_array_remove(array_desc, 0);
    count_asc  = kksorted_array_count(array_asc);
    count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(1UL, count_asc);
    ASSERT_EQUAL_ULONG(1UL, count_desc);
    kksorted_array_remove(array_asc,  0);
    kksorted_array_remove(array_desc, 0);
    count_asc  = kksorted_array_count(array_asc);
    count_desc = kksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(0UL, count_asc);
    ASSERT_EQUAL_ULONG(0UL, count_desc);
END_TEST

#define TEST_LIST(F)        \
    F(kksorted_array_create); \
    F(kksorted_array_count);  \
    F(kksorted_array_add);    \
    F(kksorted_array_get);    \
    F(kksorted_array_remove);

int main(void)
{
    SETUP(NULL);
    TEST_LIST(RUN_TEST)
    CLEANUP(cleanup);
}

#undef TEST_LIST

#include <stdarg.h>
#include <stdio.h>

#include <libk/ksorted_array.h>

#include "../test.h"

static const size_t CAPACITY = 100;
static uint32_t value1 = 777;
static uint32_t value2 = 888;
static uint32_t value3 = 999;

BEGIN_TEST(ksorted_array_create)
    static struct ksorted_array* array_asc;
    static struct ksorted_array* array_desc;
    array_asc = ksorted_array_create(CAPACITY, KSORTED_ARRAY_DYNAMIC, uint32_t_ascending_order_predicate);
    ASSERT(array_asc != NULL);
    array_desc = ksorted_array_create(CAPACITY, KSORTED_ARRAY_DYNAMIC, uint32_t_descending_order_predicate);
    ASSERT(array_desc != NULL);
    ksorted_array_delete(array_asc);
    ksorted_array_delete(array_desc);
END_TEST

BEGIN_TEST(ksorted_array_count)
    static struct ksorted_array* array_asc;
    static struct ksorted_array* array_desc;
    const size_t count_asc  = ksorted_array_count(array_asc);
    const size_t count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(0UL, count_asc);
    ASSERT_EQUAL_ULONG(0UL, count_desc);
    ksorted_array_delete(array_asc);
    ksorted_array_delete(array_desc);
END_TEST

BEGIN_TEST(ksorted_array_add)
    static struct ksorted_array* array_asc;
    static struct ksorted_array* array_desc;
    ksorted_array_add(array_asc,  &value1);
    ksorted_array_add(array_desc, &value1);
    size_t count_asc  = ksorted_array_count(array_asc);
    size_t count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(1UL, count_asc);
    ASSERT_EQUAL_ULONG(1UL, count_desc);
    ksorted_array_add(array_asc,  &value2);
    ksorted_array_add(array_desc, &value2);
    count_asc  = ksorted_array_count(array_asc);
    count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(2UL, count_asc);
    ASSERT_EQUAL_ULONG(2UL, count_desc);
    ksorted_array_add(array_asc,  &value3);
    ksorted_array_add(array_desc, &value3);
    count_asc  = ksorted_array_count(array_asc);
    count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(3UL, count_asc);
    ASSERT_EQUAL_ULONG(3UL, count_desc);
    ksorted_array_delete(array_asc);
    ksorted_array_delete(array_desc);
END_TEST

BEGIN_TEST(ksorted_array_get)
    static struct ksorted_array* array_asc;
    static struct ksorted_array* array_desc;
    uint32_t asc1  = *(uint32_t*)ksorted_array_get(array_asc,  0);
    uint32_t asc2  = *(uint32_t*)ksorted_array_get(array_asc,  1);
    uint32_t asc3  = *(uint32_t*)ksorted_array_get(array_asc,  2);
    uint32_t desc1 = *(uint32_t*)ksorted_array_get(array_desc, 0);
    uint32_t desc2 = *(uint32_t*)ksorted_array_get(array_desc, 1);
    uint32_t desc3 = *(uint32_t*)ksorted_array_get(array_desc, 2);
    ASSERT_EQUAL_UINT(value1, asc1);
    ASSERT_EQUAL_UINT(value2, asc2);
    ASSERT_EQUAL_UINT(value3, asc3);
    ASSERT_EQUAL_UINT(value3, desc1);
    ASSERT_EQUAL_UINT(value2, desc2);
    ASSERT_EQUAL_UINT(value1, desc3);
    ksorted_array_delete(array_asc);
    ksorted_array_delete(array_desc);
END_TEST

BEGIN_TEST(ksorted_array_remove)
    static struct ksorted_array* array_asc;
    static struct ksorted_array* array_desc;
    ksorted_array_remove(array_asc,  0);
    ksorted_array_remove(array_desc, 0);
    size_t count_asc  = ksorted_array_count(array_asc);
    size_t count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(2UL, count_asc);
    ASSERT_EQUAL_ULONG(2UL, count_desc);
    uint32_t asc1  = *(uint32_t*)ksorted_array_get(array_asc,  0);
    uint32_t desc1 = *(uint32_t*)ksorted_array_get(array_desc, 0);
    ASSERT_EQUAL_UINT(value2, asc1);
    ASSERT_EQUAL_UINT(value2, desc1);
    ksorted_array_remove(array_asc,  0);
    ksorted_array_remove(array_desc, 0);
    count_asc  = ksorted_array_count(array_asc);
    count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(1UL, count_asc);
    ASSERT_EQUAL_ULONG(1UL, count_desc);
    ksorted_array_remove(array_asc,  0);
    ksorted_array_remove(array_desc, 0);
    count_asc  = ksorted_array_count(array_asc);
    count_desc = ksorted_array_count(array_desc);
    ASSERT_EQUAL_ULONG(0UL, count_asc);
    ASSERT_EQUAL_ULONG(0UL, count_desc);
    ksorted_array_delete(array_asc);
    ksorted_array_delete(array_desc);
END_TEST

TEST_LIST(
    ksorted_array_create,
    ksorted_array_count,
    ksorted_array_add,
    ksorted_array_get,
    ksorted_array_remove
)

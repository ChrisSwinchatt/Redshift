#include <stdarg.h>
#include <stdio.h>

#include <libk/klist.h>

#include "../test.h"

static uint32_t value1 = 666;
static uint32_t value2 = 777;
static uint32_t value3 = 888;
static uint32_t value4 = 999;

BEGIN_TEST(klist_create)
    struct klist* list = klist_create(KLIST_FLAGS_DYNAMIC, 1, &value2);
    ASSERT_EQUAL_ULONG(1UL, klist_size(list));
    ASSERT_EQUAL_ULONG(value2, *(uint32_t*)klist_index(list, 0));
    ASSERT(list != NULL);
END_TEST

BEGIN_TEST(klist_insert_and_index)
    struct klist* list = klist_create(KLIST_FLAGS_DYNAMIC, 0, NULL);

    klist_insert(list, &value4, 0);
    klist_insert(list, &value3, 0);
    klist_insert(list, &value1, 0);
    klist_insert(list, &value2, 1);
    const uint32_t list1 = *(uint32_t*)klist_index(list, 0);
    const uint32_t list2 = *(uint32_t*)klist_index(list, 1);
    const uint32_t list3 = *(uint32_t*)klist_index(list, 2);
    const uint32_t list4 = *(uint32_t*)klist_index(list, 3);

    ASSERT_EQUAL_ULONG(4UL, klist_size(list));
    ASSERT_EQUAL_UINT(list1, value1);
    ASSERT_EQUAL_UINT(list2, value2);
    ASSERT_EQUAL_UINT(list3, value3);
    ASSERT_EQUAL_UINT(list4, value4);

    klist_shallow_free(list);
END_TEST

BEGIN_TEST(klist_prepend_and_index)
    struct klist* list = klist_create(KLIST_FLAGS_DYNAMIC, 0, NULL);

    klist_prepend(list, &value1);
    klist_prepend(list, &value2);
    klist_prepend(list, &value3);
    const uint32_t list3 = *(uint32_t*)klist_index(list, 0);
    const uint32_t list2 = *(uint32_t*)klist_index(list, 1);
    const uint32_t list1 = *(uint32_t*)klist_index(list, 2);
    const uint32_t list4 = *(uint32_t*)klist_index(list, 3);

    ASSERT_EQUAL_ULONG(4UL, klist_size(list));
    ASSERT_EQUAL_UINT(list1, value1);
    ASSERT_EQUAL_UINT(list2, value2);
    ASSERT_EQUAL_UINT(list3, value3);
    ASSERT_EQUAL_UINT(list4, value4);

    klist_shallow_free(list);
END_TEST

BEGIN_TEST(klist_append_and_index)
    struct klist* list = klist_create(KLIST_FLAGS_DYNAMIC, 0, NULL);

    klist_append(list, &value1);
    klist_append(list, &value2);
    klist_append(list, &value3);
    klist_append(list, &value4);
    const uint32_t list1 = *(uint32_t*)klist_index(list, 0);
    const uint32_t list2 = *(uint32_t*)klist_index(list, 1);
    const uint32_t list3 = *(uint32_t*)klist_index(list, 2);
    const uint32_t list4 = *(uint32_t*)klist_index(list, 3);
    
    ASSERT_EQUAL_ULONG(4UL, klist_size(list));
    ASSERT_EQUAL_UINT(list1, value1);
    ASSERT_EQUAL_UINT(list2, value2);
    ASSERT_EQUAL_UINT(list3, value3);
    ASSERT_EQUAL_UINT(list4, value4);

    klist_shallow_free(list);
END_TEST

BEGIN_TEST(klist_shallow_remove)
    struct klist* list = klist_create(KLIST_FLAGS_DYNAMIC, 0, NULL);
    klist_append(list, &value1);
    klist_append(list, &value2);
    klist_append(list, &value3);
    klist_append(list, &value4);

    klist_shallow_remove(list, 3);
    klist_shallow_remove(list, 2);
    klist_shallow_remove(list, 1);
    klist_shallow_remove(list, 0);

    ASSERT_EQUAL_ULONG(0UL, klist_size(list));
    klist_shallow_free(list);
END_TEST

TEST_LIST(
    klist_create,
    klist_insert_and_index,
    klist_prepend_and_index,
    klist_append_and_index,
    klist_shallow_remove
)

#undef TEST_LIST

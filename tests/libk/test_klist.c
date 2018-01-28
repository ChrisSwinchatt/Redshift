#include <stdarg.h>
#include <stdio.h>

#include <libk/klist.h>

#include "test.h"

static struct klist* list;
static uint32_t value1 = 666;
static uint32_t value2 = 777;
static uint32_t value3 = 888;
static uint32_t value4 = 999;

static void cleanup(void)
{
    klist_shallow_free(list);
}

BEGIN_TEST(klist_create)
    list = klist_create(malloc, free, 1, &value2);
    ASSERT_EQUAL_ULONG(1UL, klist_size(list));
    ASSERT(list != NULL);
END_TEST

BEGIN_TEST(klist_insert_prepend_append)
    klist_append(list, &value4);
    ASSERT_EQUAL_ULONG(2UL, klist_size(list));
    klist_prepend(list, &value1);
    ASSERT_EQUAL_ULONG(3UL, klist_size(list));
    klist_insert(list, &value3, 2);
    ASSERT_EQUAL_ULONG(4UL, klist_size(list));
END_TEST

BEGIN_TEST(klist_index)
    const uint32_t list1 = *(uint32_t*)klist_index(list, 0);
    const uint32_t list2 = *(uint32_t*)klist_index(list, 1);
    const uint32_t list3 = *(uint32_t*)klist_index(list, 2);
    const uint32_t list4 = *(uint32_t*)klist_index(list, 3);
    ASSERT_EQUAL_UINT(list1, value1);
    ASSERT_EQUAL_UINT(list2, value2);
    ASSERT_EQUAL_UINT(list3, value3);
    ASSERT_EQUAL_UINT(list4, value4);
END_TEST

BEGIN_TEST(klist_shallow_remove)
    klist_shallow_remove(list, 3);
    ASSERT_EQUAL_ULONG(3UL, klist_size(list));
    klist_shallow_remove(list, 2);
    ASSERT_EQUAL_ULONG(2UL, klist_size(list));
    klist_shallow_remove(list, 1);
    ASSERT_EQUAL_ULONG(1UL, klist_size(list));
    klist_shallow_remove(list, 0);
    ASSERT_EQUAL_ULONG(0UL, klist_size(list));
END_TEST

#define TEST_LIST(F)                        \
    F(klist_create);                 \
    F(klist_insert_prepend_append);  \
    F(klist_index);                  \
    F(klist_shallow_remove);

int main(void)
{
    SETUP(NULL);
    TEST_LIST(RUN_TEST)
    CLEANUP(cleanup);
}

#undef TEST_LIST

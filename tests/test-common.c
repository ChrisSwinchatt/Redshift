#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int __tests_passed__   = 0;
int __tests_failed__   = 0;
int __tests_executed__ = 0;

extern void test_main(void);

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

void* kextern_static_allocate(size_t size)
{
    return malloc(size);
}

void* kmalloc(size_t size)
{
    return malloc(size);
}

void kfree(void* ptr)
{
    free(ptr);
}

void* kextern_dynamic_allocate(size_t size)
{
    return malloc(size);
}

void kextern_dynamic_free(void* ptr)
{
    free(ptr);
}

void kextern_abort(const char* fmt, ...)
{
    fprintf(stderr, "[test output] Abort - ");
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

int main(void)
{
    test_main();
    fprintf(
        stderr,
        "%d passed and %d failed (out of %d)\n",
        __tests_passed__,
        __tests_failed__,
        __tests_executed__
    );
    return __tests_failed__;
}

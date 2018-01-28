/* Copyright (c) 2012-2018 Chris Swinchatt.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <redshift/hal/cpu.h>
#include <redshift/hal/cpu/cpuid.h>
#include <redshift/hal/cpu/features.h>
#include <redshift/hal/cpu/vendor.h>
#include <redshift/kernel/asm.h>
#include <redshift/kernel/sleep.h>
#include <libk/kstring.h>

enum cpuid_requests {
    CPUID_VENDOR_STRING,
    CPUID_INFO_FEATURES,
    CPUID_CACHE_TLB,
    CPUID_EXTENDED_MAX_FUNCTION = 0x80000000,
    CPUID_EXTENDED_INFO_FEATURES,
    CPUID_EXTENDED_BRANDSTRING_1,
    CPUID_EXTENDED_BRANDSTRING_2,
    CPUID_EXTENDED_BRANDSTRING_3,
    CPUID_EXTENDED_CACHEL1_TLB,
    CPUID_EXTENDED_CACHEL2,
    CPUID_EXTENDED_POWER_MANAGEMENT,
    CPUID_EXTENDED_ADDRESS_SIZES,
};

enum cpuid_regs {
    EAX = 0,
    EBX,
    ECX,
    EDX
};

extern int cpuid_supported(void); /* cpuid_supported.asm */

static void cpuid(uint32_t(* regs)[4])
{
    __asm__ __volatile__(
        "cpuid"
        :"=a"((*regs)[EAX]), "=b"((*regs)[EBX]),
         "=c"((*regs)[ECX]), "=d"((*regs)[EDX])
        : "a"((*regs)[EAX]),  "b"((*regs)[EBX]),
          "c"((*regs)[ECX]),  "d"((*regs)[EDX])
    );
}

static int cpuid_extended_supported(void)
{
    uint32_t regs[4] = { CPUID_EXTENDED_MAX_FUNCTION, 0, 0, 0};
    cpuid(&regs);
    return (regs[EAX] >= CPUID_EXTENDED_MAX_FUNCTION);
}

static void cpuid_get_vendor_string(struct cpuid* info)
{
    uint32_t regs[4] = {0};
    cpuid(&regs);
    kmemory_copy(info->vendor_string + 0, &regs[EBX], 4);
    kmemory_copy(info->vendor_string + 4, &regs[EDX], 4);
    kmemory_copy(info->vendor_string + 8, &regs[ECX], 4);
    info->vendor_string[CPUID_VENDOR_STRING_MAX - 1] = 0;
}

static void cpuid_get_vendor(struct cpuid* info)
{
    if ((kstring_compare(info->vendor_string, VENDOR_AMD_NEW, CPUID_VENDOR_STRING_MAX) == 0) ||
            kstring_compare(info->vendor_string, VENDOR_AMD_OLD, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "AMD", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_CENTAUR, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "Centaur", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_CYRIX, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "Cyrix", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_INTEL, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "Intel", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_TRANSMETA1, CPUID_VENDOR_STRING_MAX) == 0 ||
               kstring_compare(info->vendor_string, VENDOR_TRANSMETA2, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "Transmeta", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_NSC, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "NSC", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_NEXGEN, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "NexGen", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_RISE, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "Rise", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_SIS, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "SiS", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_UMC, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "UMC", CPUID_VENDOR_MAX);
    } else if (kstring_compare(info->vendor_string, VENDOR_VIA, CPUID_VENDOR_STRING_MAX) == 0) {
        kstring_copy(info->vendor, "VIA", CPUID_VENDOR_MAX);
    } else {
        kstring_copy(info->vendor, "Unknown", CPUID_VENDOR_MAX);
    }
}


static void cpuid_get_features(struct cpuid* info)
{
    uint32_t regs[4] = {CPUID_INFO_FEATURES, 0, 0, 0};
    cpuid(&regs);
    info->stepping     = (regs[EAX] & 0x0f);
    info->model        = (regs[EAX] >> 4) + (regs[EAX] >> 16);
    info->family       = (regs[EAX] >> 8) + ((regs[EAX] >> 20) << 4);
    info->type         = (regs[EAX] >> 12);
    info->features     = regs[ECX];
    info->features_ext = regs[EDX];
}

static void cpuid_get_brand_string(struct cpuid* info)
{
    uint32_t regs[4] = {CPUID_EXTENDED_BRANDSTRING_1, 0, 0, 0};
    cpuid(&regs);
    kstring_copy(info->brand_string +  0, (const char*)&regs[EAX], 4);
    kstring_copy(info->brand_string +  3, (const char*)&regs[EBX], 4);
    kstring_copy(info->brand_string +  7, (const char*)&regs[ECX], 4);
    kstring_copy(info->brand_string + 11, (const char*)&regs[EDX], 4);
    regs[EAX] = CPUID_EXTENDED_BRANDSTRING_2;
    cpuid(&regs);
    kstring_copy(info->brand_string + 15, (const char*)&regs[EAX], 4);
    kstring_copy(info->brand_string + 19, (const char*)&regs[EBX], 4);
    kstring_copy(info->brand_string + 23, (const char*)&regs[ECX], 4);
    kstring_copy(info->brand_string + 27, (const char*)&regs[EDX], 4);
    regs[EAX] = CPUID_EXTENDED_BRANDSTRING_3;
    cpuid(&regs);
    kstring_copy(info->brand_string + 31, (const char*)&regs[EAX], 4);
    kstring_copy(info->brand_string + 35, (const char*)&regs[EBX], 4);
    kstring_copy(info->brand_string + 39, (const char*)&regs[ECX], 4);
    kstring_copy(info->brand_string + 43, (const char*)&regs[EDX], 4);
    info->brand_string[47] = 0;
}

static void cpuid_get_cache(struct cpuid* info)
{
    /* TODO.
     */
    UNUSED(info);
}

static void cpuid_get_frequency(struct cpuid* info)
{
    static const uint64_t period = 10ULL; /* ms */
    uint64_t ticks1 = read_ticks();
    msleep(period);
    disable_interrupts(); /* msleep enables interrupts, but we don't want them any more during the boot process. */
    uint64_t ticks2 = read_ticks();
    info->frequency = (uint64_t)((ticks2 - ticks1)*1000ULL/period);
    if (info->frequency == 0) {
        info->frequency = 1;
    }
}

static void cpuid_get_cores(struct cpuid* info)
{
    /* TODO.
     */
    UNUSED(info);
}

int cpuid_init(struct cpuid* info)
{
    if (!(cpuid_supported()) || !(cpuid_extended_supported())) {
        panic("necessary CPUID functions not supported by CPU");
    }
    cpuid_get_vendor_string(info);
    cpuid_get_vendor(info);
    cpuid_get_features(info);
    cpuid_get_brand_string(info);
    cpuid_get_cache(info);
    cpuid_get_frequency(info);
    cpuid_get_cores(info);
    printk(PRINTK_DEBUG "CPU info: %s %s (%s)\n", info->brand_string, info->vendor, info->vendor_string);
    printk(PRINTK_DEBUG "CPU info: <type=%d,family=%d,model=%d,stepping=%d>\n",
           info->type, info->family, info->model, info->stepping);
    return 0;
}

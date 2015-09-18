/* Copyright (c) 2012 Chris Swinchatt.
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
#include <boot/cpuinfo.h>
#include <kernel/asm.h>
#include <kernel/sleep.h>
#include <string.h>
    
/* Known vendor strings
 */ 
#define VENDOR_AMD_OLD    "AMDisbetter!"
#define VENDOR_AMD_NEW    "AuthenticAMD"
#define VENDOR_CENTAUR    "CentaurHauls"
#define VENDOR_CYRIX      "CyrixInstead"
#define VENDOR_INTEL      "GenuineIntel"
#define VENDOR_TRANSMETA1 "TransmetaCPU"
#define VENDOR_TRANSMETA2 "GenuineTMx86"
#define VENDOR_NSC        "Geode by NSC"
#define VENDOR_NEXGEN     "NexGenDriven"
#define VENDOR_RISE       "RiseRiseRise"
#define VENDOR_SIS        "SiS SiS SiS "
#define VENDOR_UMC        "UMC UMC UMC "
#define VENDOR_VIA        "VIA VIA VIA "

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

static void cpuid(uint32_t(*regs)[4]) 
{
    __asm__ __volatile__(
        "cpuid" 
        :"=a"((*regs)[EAX]), "=b"((*regs)[EBX]),
         "=c"((*regs)[ECX]), "=d"((*regs)[EDX]) 
        : "a"((*regs)[EAX]),  "b"((*regs)[EBX]),
          "c"((*regs)[ECX]),  "d"((*regs)[EDX])
    );
} 
 
static int cpuid_supported() 
{
    return 1;
}
 
static int cpuid_extended_supported() 
{
    uint32_t regs[4] = { CPUID_EXTENDED_MAX_FUNCTION, 0, 0, 0};
    cpuid(&regs);
    return (regs[EAX] >= CPUID_EXTENDED_MAX_FUNCTION);
}
 
static void cpuid_get_vendor_string(struct cpuinfo* info) 
{
    uint32_t regs[4] = {0};
    cpuid(&regs);
    memcpy(info->vendor_string + 0, &regs[EBX], 4);
    memcpy(info->vendor_string + 4, &regs[EDX], 4);
    memcpy(info->vendor_string + 8, &regs[ECX], 4);
    info->vendor_string[12] = 0;
    printk(PRINTK_DEBUG " * Vendor ID: %s ", info->vendor_string);
} 
 
static void cpuid_get_vendor(struct cpuinfo* info) 
{
    if ((strcmp(info->vendor_string, VENDOR_AMD_NEW) == 0) ||
            strcmp(info->vendor_string, VENDOR_AMD_OLD) == 0)
        strncpy(info->vendor, "AMD", 23);
    else if (strcmp(info->vendor_string, VENDOR_CENTAUR) == 0)
        strncpy(info->vendor, "Centaur", 23);
    else if (strcmp(info->vendor_string, VENDOR_CYRIX) == 0)
        strncpy(info->vendor, "Cyrix", 23);
    else if (strcmp(info->vendor_string, VENDOR_INTEL) == 0)
        strncpy(info->vendor, "Intel", 23);
    else if (strcmp(info->vendor_string, VENDOR_TRANSMETA1) == 0 ||
               strcmp(info->vendor_string, VENDOR_TRANSMETA2) == 0)
        strncpy(info->vendor, "Transmeta", 23);
    else if (strcmp(info->vendor_string, VENDOR_NSC) == 0)
        strncpy(info->vendor, "National SemiConductor", 23);
    else if (strcmp(info->vendor_string, VENDOR_NEXGEN) == 0)
        strncpy(info->vendor, "NexGen", 23);
    else if (strcmp(info->vendor_string, VENDOR_RISE) == 0)
        strncpy(info->vendor, "Rise", 23);
    else if (strcmp(info->vendor_string, VENDOR_SIS) == 0)
        strncpy(info->vendor, "SiS", 23);
    else if (strcmp(info->vendor_string, VENDOR_UMC) == 0)
        strncpy(info->vendor, "UMC", 23);
    else if (strcmp(info->vendor_string, VENDOR_VIA) == 0)
        strncpy(info->vendor, "VIA", 23);
    else
        strncpy(info->vendor, "Unknown", 23);
    printk("(%s)\n", info->vendor);
}

 
static void cpuid_get_features(struct cpuinfo* info) 
{
    uint32_t regs[4] = {CPUID_INFO_FEATURES, 0, 0, 0};
    cpuid(&regs);
    info->stepping = (regs[EAX] & 0x0f);
    info->model = (regs[EAX] >> 4) + (regs[EAX] >> 16);
    info->family = (regs[EAX] >> 8) + ((regs[EAX] >> 20) << 4);
    info->type = (regs[EAX] >> 12);
    info->features_1 = regs[ECX];
    info->features_2 = regs[EDX];
    printk(PRINTK_DEBUG " * Type: %d\n * Family: %d\n * Model: %d\n * Stepping: %d\n",
           info->type, info->family, info->model, info->stepping);
} 
 
static void cpuid_get_brand_string(struct cpuinfo* info) 
{
    uint32_t regs[4] = {CPUID_EXTENDED_BRANDSTRING_1, 0, 0, 0};
    cpuid(&regs);
    strncpy(info->brand_string +  0, (const char*)&regs[EAX], 4);
    strncpy(info->brand_string +  3, (const char*)&regs[EBX], 4);
    strncpy(info->brand_string +  7, (const char*)&regs[ECX], 4);
    strncpy(info->brand_string + 11, (const char*)&regs[EDX], 4);
    regs[EAX] = CPUID_EXTENDED_BRANDSTRING_2;
    cpuid(&regs);
    strncpy(info->brand_string + 15, (const char*)&regs[EAX], 4);
    strncpy(info->brand_string + 19, (const char*)&regs[EBX], 4);
    strncpy(info->brand_string + 23, (const char*)&regs[ECX], 4);
    strncpy(info->brand_string + 27, (const char*)&regs[EDX], 4);
    regs[EAX] = CPUID_EXTENDED_BRANDSTRING_3;
    cpuid(&regs);
    strncpy(info->brand_string + 31, (const char*)&regs[EAX], 4);
    strncpy(info->brand_string + 35, (const char*)&regs[EBX], 4);
    strncpy(info->brand_string + 39, (const char*)&regs[ECX], 4);
    strncpy(info->brand_string + 43, (const char*)&regs[EDX], 4);
    info->brand_string[47] = 0;
    printk(PRINTK_DEBUG " * Brand string: %s\n", info->brand_string);
} 
 
static void cpuid_get_model(struct cpuinfo* info) 
{
    UNUSED(info);
} 
 
static void cpuid_get_cache(struct cpuinfo* info) 
{
    UNUSED(info);
} 

static void cpuid_get_frequency(struct cpuinfo* info) 
{
    /* FIXME on Qemu this generates wildly fluctuating values. Not sure whether this is Qemu or my code. Haven't tested
     * on real hardware.
     */
#if 0
    static const uint64_t period = 10; /* ms */
    uint64_t ticks1, ticks2;
    ticks1 = read_ticks();
    msleep(period);
    int_disable(); /* msleep enables interrupts, but we don't want them any more during the boot process. */
    ticks2 = read_ticks();
    info->frequency = (uint32_t)((ticks2 - ticks1) * 1000 / period);
    if (info->frequency == 0) {
        info->frequency = 1;
    }
    {
        uint32_t freq = info->frequency;
        const char* unit = "Hz";
        if (info->frequency >= 1000000000UL) {
            freq /= 1000000000UL;
            unit = "GHz";
        } else if (info->frequency >= 1000000UL) {
            freq /= 1000UL;
            unit = "MHz";
        } else if (info->frequency >= 1000UL) {
            freq /= 1000UL;
            unit = "kHz";
        }
        printk(PRINTK_DEBUG " * Clock rate: %lu %s\n", freq, unit);
    }
#endif
    UNUSED(info);
} 
 
static void cpuid_get_cores(struct cpuinfo* info) 
{
    UNUSED(info);
} 
 
int cpuinfo_init(struct cpuinfo* info) 
{
    if (!(cpuid_supported()) || !(cpuid_extended_supported()))
        panic("necessary CPUID functions not supported by CPU");
    cpuid_get_vendor_string(info);
    cpuid_get_vendor(info);
    cpuid_get_features(info);
    cpuid_get_brand_string(info);
    cpuid_get_model(info);
    cpuid_get_cache(info);
    cpuid_get_frequency(info);
    cpuid_get_cores(info);
    return 0;
}

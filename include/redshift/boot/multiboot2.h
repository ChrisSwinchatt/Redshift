/**
 * \file boot/multiboot2.h
 * Multiboot 2 specification header.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef REDSHIFT_BOOT_MULTIBOOT2_H
#define REDSHIFT_BOOT_MULTIBOOT2_H

#define MULTIBOOT2_HEADER_MAGIC     0xE85250D6
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36D76289
#define MULTIBOOT2_MOD_ALIGN        0x00001000
#define MULTIBOOT2_INFO_ALIGN       8
#define MULTIBOOT2_TAG_ALIGN        8

#ifndef __ASM_SOURCE__
# include <redshift/kernel.h>

typedef enum {
    MULTIBOOT2_TAG_TYPE_END = 0,
    MULTIBOOT2_TAG_TYPE_CMDLINE,
    MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME,
    MULTIBOOT2_TAG_TYPE_MODULE,
    MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO,
    MULTIBOOT2_TAG_TYPE_BOOTDEV,
    MULTIBOOT2_TAG_TYPE_MMAP,
    MULTIBOOT2_TAG_TYPE_VBE,
    MULTIBOOT2_TAG_TYPE_FRAMEBUFFER,
    MULTIBOOT2_TAG_TYPE_ELF_SECTIONS,
    MULTIBOOT2_TAG_TYPE_APM
} multiboot2_tag_type_t;

struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_string {
    struct multiboot2_tag tag;
    char                 string[];
};

struct multiboot2_tag_module {
    struct multiboot2_tag tag;
    uint32_t              start;
    uint32_t              end;
    char                  cmdline[];
};

struct multiboot2_tag_basic_meminfo {
    struct multiboot2_tag tag;
    uint32_t              mem_lower;
    uint32_t              mem_upper;
};

struct multiboot2_tag_bootdev {
    struct multiboot2_tag tag;
    uint32_t              biosdev;
    uint32_t              slice;
    uint32_t              part;
};

enum {
    MULTIBOOT2_MEMORY_INVALID,
    MULTIBOOT2_MEMORY_AVAILABLE,
    MULTIBOOT2_MEMORY_RESERVED,
    MULTIBOOT2_MEMORY_RECLAIMABLE,
    MULTIBOOT2_MEMORY_NVS
};

struct multiboot2_mmap_entry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;
    uint32_t reserved;
} __packed;

struct multiboot2_tag_mmap {
    struct multiboot2_tag        tag;
    uint32_t                     entry_size;
    uint32_t                     entry_version;
    struct multiboot2_mmap_entry entries[];
};

struct multiboot2_color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

enum {
    MULTIBOOT2_VBE_INFO_BLOCK_SIZE      = 512,
    MULTIBOOT2_VBE_MODE_INFO_BLOCK_SIZE = 256
};

struct multiboot2_vbe_info_block {
    uint8_t external_specification[MULTIBOOT2_VBE_INFO_BLOCK_SIZE];
};

struct multiboot2_vbe_mode_info_block {
    uint8_t external_specification[MULTIBOOT2_VBE_MODE_INFO_BLOCK_SIZE];
};

struct multiboot2_tag_vbe {
    struct multiboot2_tag                 tag;
    uint16_t                              vbe_mode;
    uint16_t                              vbe_interface_segment;
    uint16_t                              vbe_interface_offset;
    uint16_t                              vbe_interface_length;
    struct multiboot2_vbe_info_block      vbe_control_info;
    struct multiboot2_vbe_mode_info_block vbe_mode_info;
};

enum {
    MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED,
    MULTIBOOT2_FRAMEBUFFER_TYPE_RGB,
    MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT
};

struct multiboot2_tag_framebuffer {
    struct {
        struct multiboot2_tag tag;
        uint64_t              addr;
        uint32_t              pitch;
        uint32_t              width;
        uint32_t              height;
        uint8_t               bpp;
        uint8_t               type;
        uint16_t              reserved;
    } common;
    union {
        struct {
            uint16_t                nb_colors;
            struct multiboot2_color pallete[];
        };
        struct {
            uint8_t                red_field_position;
            uint8_t                red_mask_size;
            uint8_t                green_field_position;
            uint8_t                green_mask_size;
            uint8_t                blue_field_position;
            uint8_t                blue_mask_size;
        };
    };
};

struct multiboot2_tag_elf_sections {
    struct multiboot2_tag tag;
    uint32_t             count;
    uint32_t             entsize;
    uint32_t             shndex;
    char                 sections[];
};

struct multiboot2_tag_apm {
    struct multiboot2_tag tag;
    uint16_t              version;
    uint16_t              cseg;
    uint32_t              offset;
    uint16_t              cseg_16;
    uint16_t              dseg;
    uint16_t              flags;
    uint16_t              cseg_len;
    uint16_t              cseg_16_len;
    uint16_t              dseg_len;
};
#endif /* ! __ASM_SOURCE__ */

#endif /* ! REDSHIFT_BOOT_MULTIBOOT2_H */

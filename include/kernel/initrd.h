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
#ifndef REDSHIFT_KERNEL_INITRD_H
#define REDSHIFT_KERNEL_INITRD_H

#include <kernel/redshift.h>

/**  Path separator. */
#define INITRD_PATH_SEP "/"
/** Path to kernel map in initrd. */
#define INITRD_MAP_PATH INITRD_PATH_SEP "boot" INITRD_PATH_SEP KERNEL ".map"

enum {
    /**
     * The magic number. The numbers are the hexadecimal representation of the ASCII characters "rifs" (Redshift
     * Initial File System).
     */
    INITRD_MAGIC = 0x72696673
};

typedef uint32_t initrd_magic_t;    /**< Magic number type.                     */
typedef uint32_t initrd_major_t;    /**< Major version type.                    */
typedef uint32_t initrd_index_t;    /**< Index type.                            */
typedef uint64_t initrd_offset_t;   /**< Offset type.                           */
typedef uint64_t initrd_size_t;     /**< Size type.                             */

/**
 * Initial ramdisk header.
 */
struct initrd_header {
    initrd_magic_t  magic;          /**< Magic number.                          */
    initrd_major_t  major;          /**< Major version of the creating program. */
    initrd_offset_t off_nodetab;    /**< Offset of the node table.              */
    initrd_offset_t off_strtab;     /**< Offset of the string table.            */
};

/**
 * Initial ramdisk node.
 */
struct initrd_node {
    initrd_index_t  parent;         /**< Parent node index. 0 = root.           */
    initrd_offset_t offset;         /**< Offset of the node's data.             */
    initrd_size_t   size;           /**< Size of the node's data.               */
    initrd_offset_t  name;          /**< Offset of the node's name.             */
    initrd_size_t   name_len;       /**< Length of the node's name.             */
};

#endif /* ! REDSHIFT_KERNEL_INITRD_H */

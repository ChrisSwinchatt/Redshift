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
.intel_syntax noprefix

#define __ASM_SOURCE__
#include <redshift/boot/multiboot2.h>

.set MAGIC,     MULTIBOOT2_HEADER_MAGIC
.set ARCH,      0
.set LENGTH,    end - multiboot2
.set CHECKSUM,  -(MAGIC + ARCH + LENGTH)

.section .multiboot2
.align   8
multiboot2:
    .long MAGIC                     /* Magic number.                      */
    .long ARCH                      /* Architecture.                      */
    .long LENGTH                    /* Header size.                       */
    .long CHECKSUM                  /* Header checksum.                   */
/* Request multiboot2 information.
 */
info:
    .short 1, 0
    .long  modalign - info
    .long  1, 2, 3, 5, 6/*, 7, 8 FIXME */
    .long  0                        /* Pad to 8 bytes. */
/* Request module alignment.
 */
modalign:
    .short 6, 0
    .long  end_tag - modalign
/* End of tags.
 */
end_tag:
    .short 0, 0
    .long  end - end_tag
/* End of header.
 */
end:

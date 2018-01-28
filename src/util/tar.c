/**
 * \file util/tar.c
 * TAR archive reader.
 * \author Chris Swinchatt <c.swinchatt@sussex.ac.uk>
 * \copyright Copyright (c) 2012-2018 Chris Swinchatt.
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
#include <redshift/kernel.h>
#include <libk/kstring.h>
#include <redshift/util/tar.h>
#include <libk/kstring.h>

#define TAR_HEADER_SIZE 512UL /* TAR headers are padded to 512 bytes. */

struct tar_header {
    char filename[TAR_FILENAME_MAX];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
};

static uint32_t tar_oct2dec(const char* s, size_t n)
{
    uint32_t dec   = 0;
    size_t   j     = 0;
    size_t   count = 1;
    for (j = n; j > 0; --j, count *= 8) {
        dec += ((s[j - 1] - '0')*count);
    }
    return dec;
}

static uint32_t tar_get_mode(struct tar_header* header)
{
    return tar_oct2dec(header->mode, ARRAY_SIZE(header->mode));
}

static uint32_t tar_get_size(struct tar_header* header)
{
    return tar_oct2dec(header->size, ARRAY_SIZE(header->size));
}

static uint32_t tar_get_checksum(struct tar_header* header)
{
    return tar_oct2dec(header->checksum, ARRAY_SIZE(header->checksum));
}

size_t tar_extract(struct tar_file* files, size_t max_files, const char* archive, size_t size)
{
    /* Discover files. Stop if we reach the limit of the files array, the archive buffer, or we find
     * a header filled with zeros.
     */
    size_t index  = 0;
    size_t offset = 0;
    printk(PRINTK_DEBUG "Extracting archive: <address=0x%p,size=%lu>\n", archive, size);
    for (; index < max_files && offset < size; ++index) {
        struct tar_header* header = (struct tar_header*)(archive + offset);
        /* Break on NUL header.
         */
        if (header->filename[0] == 0) {
            break;
        }
        /* Copy fields.
         */
        kstring_copy(files[index].filename, header->filename, TAR_FILENAME_MAX);
        files[index].hash     = kstring_hash32(files[index].filename, TAR_FILENAME_MAX);
        files[index].start    = (uintptr_t)header + TAR_HEADER_SIZE;
        files[index].mode     = tar_get_mode(header);
        files[index].size     = tar_get_size(header);
        files[index].checksum = tar_get_checksum(header);
        /* Advance offset past the header, padding and file.
         */
        offset += (files[index].size/TAR_HEADER_SIZE + 1)*TAR_HEADER_SIZE;
        if ((files[index].size % TAR_HEADER_SIZE) != 0) {
            offset += TAR_HEADER_SIZE;
        }
    }
    return index;
}

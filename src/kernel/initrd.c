/**
 * \file kernel/initrd.c
 * Initial ramdisk.
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
#include <redshift/kernel/initrd.h>
#include <redshift/util/tar.h>
#include <libk/kstring.h>

static struct initrd_file initrd_files[INITRD_MAX_FILES];

void initrd_init(const char* initrd, size_t size)
{
    SAVE_INTERRUPT_STATE;
    tar_extract(initrd_files, INITRD_MAX_FILES, initrd, size);
    RESTORE_INTERRUPT_STATE;
}

const struct initrd_file* initrd_get_file_by_name(const char* path)
{
    SAVE_INTERRUPT_STATE;
    uint32_t hash = kstring_hash32(path, INITRD_FILENAME_MAX);
    for (size_t i = 0; i < INITRD_MAX_FILES; ++i) {
        if (initrd_files[i].hash == hash) {
            return initrd_files + i;
        }
    }
    RESTORE_INTERRUPT_STATE;
    return NULL;
}

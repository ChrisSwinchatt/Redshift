/**
 * \file util/tar.h
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
#ifndef REDSHIFT_UTIL_TAR_H
#define REDSHIFT_UTIL_TAR_H

#include <redshift/kernel.h>

#define TAR_FILENAME_MAX 100UL

/**
 * A file in a TAR archive.
 */
struct tar_file {
    char      filename[TAR_FILENAME_MAX];
    uintptr_t start;
    uint32_t  hash;
    uint32_t  mode;
    uint32_t  size;
    uint32_t  checksum;
};

/**
 * Extract files from TAR archive.
 * \param files A pre-allocated array of files.
 * \param max_files The maximum number of files to extract (the size of the files array).
 * \param archive A pointer to the archive in memory.
 * \param size The size of the archive in memory.
 * \return The number of files extracted is returned.
 */
size_t tar_extract(struct tar_file* files, size_t max_files, const char* archive, size_t size);

#endif /* ! REDSHIFT_UTIL_TAR_H */

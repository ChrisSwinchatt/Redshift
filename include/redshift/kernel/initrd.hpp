/// Copyright (c) 2012-2018 Chris Swinchatt.
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#ifndef REDSHIFT_KERNEL_INITRD_HPP
#define REDSHIFT_KERNEL_INITRD_HPP

#include <redshift/util/tar.hpp>

/// Maximum number of files in initrd.
#define INITRD_MAX_FILES    127

/// Maximum length of initrd filename.
#define INITRD_FILENAME_MAX TAR_FILENAME_MAX

/// Initrd file structure (`struct initrd_file`)
#define initrd_file tar_file // util/tar.h

/// Initialise initial ramdisk.
/// \param initrd A pointer to the initial ramdisk in memory.
/// \param size The size of the initial ramdisk.
void initrd_init(const char* initrd, size_t size);

/// Get a read-only pointer to an initial ramdisk file.
/// \param path The path to the file to find.
/// \return A read-only pointer to the file if it exists, otherwise nullptr.
const struct initrd_file* initrd_get_file_by_name(const char* path);

#endif // ! REDSHIFT_KERNEL_INITRD_HPP

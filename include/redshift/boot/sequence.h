/* Copyright (c) 2012-2018 Chris Swinchatt.
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
#ifndef REDSHIFT_BOOT_SEQUENCE_H
#define REDSHIFT_BOOT_SEQUENCE_H

#define BOOT_SEQUENCE_STACK_GUARD_SETUP     1000
#define BOOT_SEQUNECE_INIT_CONSOLE          1001
#define BOOT_SEQUENCE_SPLASH                1002
#define BOOT_SEQUENCE_CHECK_BOOT_ENV        1003
#define BOOT_SEQUENCE_INIT_INTERRUPT_SYSTEM 1004
#define BOOT_SEQUENCE_INIT_BOOT_MODULES_1   1005
#define BOOT_SEQUENCE_INIT_HAL              1006
#define BOOT_SEQUENCE_INIT_MEMORY           1007
#define BOOT_SEQUENCE_INIT_BOOT_MODULES_2   1008
#define BOOT_SEQUENCE_LOAD_INITRD           1009
#define BOOT_SEQUENCE_LOAD_SYMBOL_TABLE     1010
#define BOOT_SEQUENCE_INIT_DEVICES          1011
#define BOOT_SEQUENCE_START_SCHEDULER       1012

#endif /* ! REDSHIFT_BOOT_SEQUENCE_H */

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

/** Stack size. */
.set STACK_SIZE, 0x8000 /* 32 kiB */

.section .text
.global _start
.type   _start, @function
_start:
        /* Set up the stack.
         */
        cli
        mov     esp,                              offset __stack_top__
        xor     ebp,                              ebp
        /* Save bootloader magic number and tags pointer.
         */
        mov     [__multiboot2_bootloader_magic__], eax
        mov     [__multiboot2_bootloader_tags__],  ebx
        /* Start boot.
         */
        call    boot
        jmp     wait_for_interrupt

.global _exit
.type   _exit, @function
_exit:
        /* Call global destructors.
         */
        call    _fini
        jmp     hang

/* Save multiboot2 magic number and tags.
 */
.section .data
.global __multiboot2_bootloader_magic__
__multiboot2_bootloader_magic__: .long 0

.global __multiboot2_bootloader_tags__
__multiboot2_bootloader_tags__:  .long 0

/* 32 kiB stack aligned to 16-bytes per ABI.
 */
.section .bss
.align 16

.global __stack_bottom__
__stack_bottom__: .skip STACK_SIZE

.global __stack_top__
__stack_top__:

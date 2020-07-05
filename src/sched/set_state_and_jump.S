/* Copyright (c) 2012-2018, 2020 Chris Swinchatt <chris@swinchatt.dev>
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

.section .text

.set IF_MASK, 1 << 9

.global set_state_and_jump
.type   set_state_and_jump, @function
set_state_and_jump:
    /* Restore state of interrupted program, then jump to it. We store EFLAGS, CS:EIP and SS:ESP in variables to be
     * restored right before resuming the program with IRET as these registers either can't be changed directly (EIP),
     * trigger a GPF if we change them (CS, SS) or would be modified by other instructions before the jump (EFLAGS).
     * Since we're not returning control, we don't set up a stack frame.
     * NB: The offsets here have to match struct cpu_state (hal/cpu/state.h)
     */
    add   esp,      4               /* Skip return address.              */
    mov   ebx,      [esp]           /* Pointer to register state in EBX. */
    mov   eax,      [ebx + 60]
    mov   [_flags], eax
    mov   eax,      [ebx + 56]
    mov   [_eip],   eax
    mov   eax,      [ebx + 52]      /* SS */
    mov   [_ss],    eax
#if 0
    mov   eax,      [ebx - 48]      /* FS */
    mov   gs,       ax
    mov   eax,      [ebx - 44]      /* FS */
    mov   fs,       ax
    mov   eax,      [ebx - 40]      /* ES */
    mov   es,       ax
    mov   eax,      [ebx - 36]      /* DS */
    mov   ds,       ax
#endif
    mov   eax,      [ebx + 32]      /* CS */
    mov   [_cs],    eax
    mov   edi,      [ebx + 28]
    mov   esi,      [ebx + 24]
    mov   ebp,      [ebx + 20]
    mov   eax,      [ebx + 16]
    mov   [_esp],   eax
    mov   edx,      [ebx + 12]
    mov   ecx,      [ebx +  8]
    mov   eax,      [ebx +  0]
    mov   ebx,      [ebx +  4]  /* Restore EBX last for obvious reasons. */
    push [_ss]
    push [_esp]
    push [_flags]
    push [_cs]
    push [_eip]
    iret

.section .data
_cs:    .long 0
_eip:   .long 0
_flags: .long 0
_ss:    .long 0
_esp:   .long 0

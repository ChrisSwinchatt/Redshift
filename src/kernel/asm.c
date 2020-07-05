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
#include <redshift/kernel.h>

uint64_t read_cpu_ticks(void)
{
    PUSH_INTERRUPT_STATE(0);
    uint32_t hi, lo;
    asm("xorl %%eax, %%eax\ncpuid":::"%eax", "%ebx", "%ecx", "%edx");
    asm("rdtsc":"=a"(lo), "=d"(hi));
    POP_INTERRUPT_STATE();
    return ((uint64_t)(((uint64_t)hi << 32) | (lo & 0x0f)));
}

void io_outb(uint16_t port, uint8_t value)
{
    asm("outb %1, %0"::"dN"(port), "a"(value));
}

void io_outw(uint16_t port, uint16_t value)
{
    asm("outw %1, %0"::"dN"(port), "a"(value));
}

uint8_t io_inb(uint16_t port)
{
    uint8_t ret;
    asm("inb %1, %0":"=a"(ret):"dN"(port));
    return ret;
}

uint16_t io_inw(uint16_t port)
{
    uint16_t ret;
    asm("inw %1, %0":"=a"(ret):"dN"(port));
    return ret;
}

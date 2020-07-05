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
#include <redshift/boot/pic.h>

int pic_init(void)
{
    PUSH_INTERRUPT_STATE(0);
    uint8_t mask_master = io_inb(PIC_MASTER_DATA);
    uint8_t mask_slave  = io_inb(PIC_SLAVE_DATA);
    io_outb(PIC_MASTER_CMND, 0x11);
    io_outb(PIC_SLAVE_CMND,  0x11);
    io_outb(PIC_MASTER_DATA, 0x20);
    io_outb(PIC_SLAVE_DATA,  0x28);
    io_outb(PIC_MASTER_DATA, 0x04);
    io_outb(PIC_SLAVE_DATA,  0x02);
    io_outb(PIC_MASTER_DATA, 0x01);
    io_outb(PIC_SLAVE_DATA,  0x01);
    io_outb(PIC_MASTER_DATA, mask_master);
    io_outb(PIC_SLAVE_DATA,  mask_slave);
    POP_INTERRUPT_STATE();
    return 0;
}

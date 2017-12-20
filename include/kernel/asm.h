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
#ifndef REDSHIFT_KERNEL_ASM_H
#define REDSHIFT_KERNEL_ASM_H

#include <stdint.h>

/** I/O ports. */
enum io_ports {
    PIC_MASTER_CMND = 0x20,  /**< Master PIC command port. */
    PIC_MASTER_DATA = 0x21,  /**< Master PIC data port.    */
    PIT_DATA        = 0x40,  /**< PIT data port.           */
    PIT_CMND        = 0x43,  /**< PIT command port.        */
    KEYBOARD_CMND   = 0x64,  /**< Keyboard command port.   */
    KEYBOARD_DATA   = 0x60,  /**< Keyboard data port.      */
    PIC_SLAVE_CMND  = 0xA0,  /**< Slave PIC command port.  */
    PIC_SLAVE_DATA  = 0xA1,  /**< Slave PIC data port.     */
    VGA_CMND        = 0x3D4, /**< VGA command port.        */
    VGA_DATA        = 0x3D5  /**< VGA data port.           */
};

/** I/O port commands. */
enum io_commands {
    PIC_RESET      = 0x20, /**< Reset PIC */
    KEYBOARD_RESET = 0xFE  /**< Reset keyboard */
};

/**
 * \brief Disables interrupts.
 */
void int_disable();

/**
 * \brief Enables interrupts.
 */
void int_enable();

/**
 * \brief Waits until the next interrupt (be careful with int_disable()!).
 */
void int_wait();

/**
 * \brief Reads the timestamp counter.
 * \return The number of ticks since the last CPU reset.
 */
uint64_t read_ticks();

/**
 * \brief Writes a byte value to a port.
 * \param port The port to write to.
 * \param value The value to write.
 */
void io_outb(uint16_t port, uint8_t value);

/**
 * \brief Writes a word (double byte) value to a port.
 * \param port The port to write to.
 * \param value The value to write.
 */
void io_outw(uint16_t port, uint16_t value);

/**
 * \brief Writes a long (double word) value to a port.
 * \param port The port to write to.
 * \param value The value to write.
 */
void io_outl(uint16_t port, uint32_t value);

/**
 * \brief Reads a byte value from a port.
 * \param port The port to read from.
 * \return The value read is returned.
 */
uint8_t io_inb(uint16_t port);

/**
 * \brief Reads a word (double byte) value from a port.
 * \param port The port to read from.
 * \return The value read is returned.
 */
uint16_t io_inw(uint16_t port);

/**
 * \brief Reads a long (double word) value from a port.
 * \param port The port to read from.
 * \return The value read is returned.
 */
uint32_t io_inl(uint16_t port);

/**
 * \brief 64-bit unsigned division algorithm.
 * \param n The numerator.
 * \param d The denominator.
 * \return The result of division is returned.
 */
extern uint64_t udiv64(uint64_t n, uint64_t d);

/**
 * \brief 64-bit signed division algorithm.
 * \param n The numerator.
 * \param d The denominator.
 * \return The result of division is returned.
 */
extern int64_t idiv64(int64_t n, int64_t d);

#endif /* ! REDSHIFT_KERNEL_ASM_H */

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
#ifndef REDSHIFT_HAL_IO_HPP
#define REDSHIFT_HAL_IO_HPP

namespace redshift { namespace hal { namespace io {
    /// I/O ports.
    enum class port : uint16_t {
        PIC_MASTER_CMND = 0x20,  ///< Master PIC command port.
        PIC_MASTER_DATA = 0x21,  ///< Master PIC data port.
        PIT_DATA        = 0x40,  ///< PIT data port.
        PIT_CMND        = 0x43,  ///< PIT command port.
        KEYBOARD_CMND   = 0x64,  ///< Keyboard command port.
        KEYBOARD_DATA   = 0x60,  ///< Keyboard data port.
        PIC_SLAVE_CMND  = 0xA0,  ///< Slave PIC command port.
        PIC_SLAVE_DATA  = 0xA1,  ///< Slave PIC data port.
        VGA_CMND        = 0x3D4, ///< VGA command port.
        VGA_DATA        = 0x3D5  ///< VGA data port.
    };

    /// I/O port commands.
    enum class command : uint8_t {
        PIC_RESET      = 0x20, ///< Reset PIC
        KEYBOARD_RESET = 0xFE  ///< Reset keyboard
    };

    /// Writes a byte value to a port.
    /// \param port The port to write to.
    /// \param value The value to write.
    void outb(uint16_t port, uint8_t value);

    /// Writes a word (double byte) value to a port.
    /// \param port The port to write to.
    /// \param value The value to write.
    void outw(uint16_t port, uint16_t value);

    /// Writes a long (double word) value to a port.
    /// \param port The port to write to.
    /// \param value The value to write.
    void outl(uint16_t port, uint32_t value);

    /// Reads a byte value from a port.
    /// \param port The port to read from.
    /// \return The value read is returned.
    uint8_t inb(uint16_t port);

    /// Reads a word (double byte) value from a port.
    /// \param port The port to read from.
    /// \return The value read is returned.
    uint16_t inw(uint16_t port);

    /// Reads a long (double word) value from a port.
    /// \param port The port to read from.
    /// \return The value read is returned.
    uint32_t inl(uint16_t port);
}}} // redshift::hal::io

#endif // ! REDSHIFT_HAL_IO_HPP

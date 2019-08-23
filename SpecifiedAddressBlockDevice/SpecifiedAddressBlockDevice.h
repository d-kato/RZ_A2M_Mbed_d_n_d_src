/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
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
#ifndef SPECIFIED_ADDRESS_BLOCK_DEVICE_H
#define SPECIFIED_ADDRESS_BLOCK_DEVICE_H

#include "FlashIAP.h"
#include "BlockDevice.h"
#include "platform/mbed_toolchain.h"


/** Lazily allocated heap-backed block device
 *
 * When writing data of ROM address, heap memory is not used
 * Useful for simulating a block device and tests
 *
 * @code
 * #include "mbed.h"
 * #include "SpecifiedAddressBlockDevice.h"
 *
 * SpecifiedAddressBlockDevice bd(2048, 512); // 2048 bytes with a block size of 512 bytes
 * uint8_t block[512] = "Hello World!\n";
 *
 * int main() {
 *     bd.SetRomAddr(0x18000000, 0x1FFFFFFF); // ROM Address 0x18000000 - 0x1FFFFFFF
 *     bd.init();
 *     bd.program(block, 0);
 *     bd.read(block, 0);
 *     printf("%s", block);
 *     bd.deinit();
 * }
 */
class SpecifiedAddressBlockDevice : public BlockDevice
{
public:

    /** Lifetime of the memory block device
     */
    SpecifiedAddressBlockDevice(uint32_t address = MBED_CONF_FLASHIAP_BLOCK_DEVICE_BASE_ADDRESS,
                        uint32_t size = MBED_CONF_FLASHIAP_BLOCK_DEVICE_SIZE);
    virtual ~SpecifiedAddressBlockDevice();

    /** Initialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int deinit();

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to read blocks into
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int read(void *buffer, mbed::bd_addr_t addr, mbed::bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int program(const void *buffer, mbed::bd_addr_t addr, mbed::bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int erase(mbed::bd_addr_t addr, mbed::bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a programable block in bytes
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a eraseable block in bytes
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;

    virtual int get_erase_value() const;

    /** Get the BlockDevice class type.
     *
     *  @return         A string represent the BlockDevice class type.
     */
    virtual const char *get_type() const;

    bool is_flash_program();

private:
    mbed::FlashIAP _flash;
    mbed::bd_addr_t _base;
    mbed::bd_size_t _size;
    bool _is_initialized;
    uint32_t _init_ref_count;
    uint8_t **_blocks;
    bool _is_flash_program;
};


#endif

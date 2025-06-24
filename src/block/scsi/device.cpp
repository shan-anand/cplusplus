/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.cpp
@brief SCSI device interface. Implements non-virtual functions.
===============================================================================
MIT License

Copyright (c) 2017 Shanmuga (Anand) Gunasekaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===============================================================================
LICENSE: END
*/

/**
 * @file  device.cpp
 * @brief SCSI Device interface that defines all SCSI calls.
 */

#include <block/scsi.hpp>

using namespace sid;
using namespace sid::block::scsi;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//

device::device()
{
  //m_verbose = Verbose::None;
}

device::~device()
{
}

//! Create new SCSI device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  device_ptr dev = dynamic_cast<device*>(_deviceInfo.create().ptr());
  if ( ! dev ) throw sid::exception("Cannot convert to scsi::device_ptr");
  return dev;
}

device_ptr device::to_scsi_device_ptr() const
{
  device_ptr dev = dynamic_cast<device*>(const_cast<device*>(this));
  if ( ! dev ) throw sid::exception("Cannot convert to scsi::device_ptr");
  return dev;
}

bool device::ready()
{
  scsi::sense sense;
  return this->test_unit_ready(sense);
}

block::capacity device::capacity(bool _force/* = false*/)
{
  block::capacity cap;
  {
    scsi::capacity16 cap16;
    if ( ! this->read_capacity(cap16) )
      throw this->exception();
    // Set the return value
    cap.blocks = cap16.num_blocks;
    cap.block_size = cap16.block_size;
  }
  return cap;
}

std::string device::wwn(bool _force/* = false*/)
{
  std::string wwnStr;
  {
    scsi::inquiry::unit_serial_number usn;
    if ( ! this->inquiry(&usn) )
      throw this->exception();
    // Set the return value
    wwnStr = usn.serial_number;
  }
  return wwnStr;
}

bool device::read(io_byte_units& _io_byte_units) // override
{
  bool isSuccess = false;

  try
  {
    const uint32_t blockSize = this->capacity().block_size;

    // Validate all the entries
    for ( const io_byte_unit& io_byte_unit : _io_byte_units )
      io_byte_unit.validate(blockSize);

    scsi::read16_vec read16_vec;
    for ( const io_byte_unit& io_byte_unit : _io_byte_units )
    {
      uint32_t lastSize = 0;
      scsi::read16 read16;
      read16.lba = io_byte_unit.offset / blockSize;
      read16.data = io_byte_unit.data;
      read16.transfer_length = 0;
      for ( uint32_t bytesLeft = io_byte_unit.length; bytesLeft != 0;
            bytesLeft -= lastSize,
              read16.lba += read16.transfer_length,
              read16.data += lastSize)
      {
        lastSize = ( bytesLeft > SCSI_DEFAULT_IO_BYTE_SIZE )?
          SCSI_DEFAULT_IO_BYTE_SIZE : bytesLeft;
        read16.transfer_length = lastSize / blockSize;
        read16_vec.push_back(read16);
      }
    }

    // Call the actual override function that does the IO
    isSuccess = this->read(read16_vec);

    // Fill the output correctly
    size_t i = 0;
    uchar8_t* pData = _io_byte_units[i].data;
    for ( const scsi::read16& read16 : read16_vec )
    {
      // If the current object's data buffer and pData are different
      //   then we need to move to the next byte unit
      if ( read16.data != pData )
        pData = _io_byte_units[++i].data;

      // Update the data processed
      _io_byte_units[i].data_processed += read16.data_size_read;

      // Move the pData pointer to the end of the buffer
      pData += (read16.transfer_length * blockSize);
    }
  }
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    // unhandled exception
    this->exception(-1, std::string(__func__)
                                       + "(" + this->id() + "): Unknown exception");
  }
  return isSuccess;
}

bool device::write(io_byte_units& _io_byte_units) // override
{
  this->exception("block::write() not implemented");
  return false;
}

bool device::read(read16& _read16)
{
  scsi::read16_vec read16_vec;
  read16_vec.push_back(_read16);

  // Call the actual override function that does the IO
  bool isSuccess = this->read(read16_vec);

  // Set the output
  _read16 = read16_vec[0];

  return isSuccess;
}

bool device::write(write16& _write16)
{
  scsi::write16_vec write16_vec;
  write16_vec.push_back(_write16);

  // Call the actual override function that does the IO
  bool isSuccess = this->write(write16_vec);

  // Set the output
  _write16 = write16_vec[0];

  return isSuccess;  
}

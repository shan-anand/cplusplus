/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
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

//! Create new SCSI device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  return dynamic_cast<device*>(_deviceInfo.create().ptr());
}

device::device()
{
  //m_verbose = Verbose::None;
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

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.cpp
@brief block device implementation
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
 * @brief Block device generic implementation.
 */

#include <block/device.hpp>

using namespace sid;
using namespace sid::block;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//

//! Create new block device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  return dynamic_cast<device*>(_deviceInfo.create().ptr());
}

device::device() : smart_ref()
{
  //m_verbose = Verbose::None;
}

device::~device()
{
}

bool device::read(io_read& _io_read)
{
  io_reads io_reads;
  io_reads.push_back(_io_read);
  return this->read(io_reads);
}

bool device::write(io_write& _io_write)
{
  io_writes io_writes;
  io_writes.push_back(_io_write);
  return this->write(io_writes);
}

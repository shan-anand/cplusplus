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
 * @file  Device.cpp
 * @brief SCSI Device interface that defines all SCSI calls.
 */

#include <scsi/constants.hpp>
#include <scsi/datatypes.hpp>
#include <scsi/device.hpp>

using namespace sid;
using namespace sid::scsi;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//
device::device()
{
  //m_verbose = Verbose::None;
}

scsi::capacity device::capacity(bool _force/*= false*/)
{
  // If _force flag is set, clear the current capacity object
  if ( _force )
    m_capacity.clear();

  // If the capacity object is not set, get it afresh
  if ( m_capacity.empty() )
  {
    scsi::capacity16 capacity16;
    if ( ! this->read_capacity(capacity16) )
      throw this->error();

    m_capacity.num_blocks = capacity16.num_blocks;
    m_capacity.block_size = capacity16.block_size;
  }
  return m_capacity;
}


std::string device::wwn(bool _force/*= false*/)
{
  // If _force flag is set, clear the current wwn object
  if ( _force )
    m_wwn.clear();

  // If the wwn object is not set, get it afresh
  if ( m_wwn.empty() )
  {
    scsi::inquiry::unit_serial_number usn;
    if ( ! this->inquiry(&usn) )
      throw this->error();
    m_wwn = usn.serial_number;
  }
  return m_wwn;
}

bool device::read_capacity(/*out*/ scsi::capacity16& _capacity16)
{
  m_capacity.clear();
  if ( this->v_read_capacity(_capacity16) )
    throw this->error();

  m_capacity.num_blocks = _capacity16.num_blocks;
  m_capacity.block_size = _capacity16.block_size;
  return true;
}


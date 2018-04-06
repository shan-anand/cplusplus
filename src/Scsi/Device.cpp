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
 * @file  Device.cpp
 * @brief SCSI Device interface that defines all SCSI calls.
 */

#include <Scsi/Constants.h>
#include <Scsi/DataTypes.h>
#include <Scsi/Device.h>

using namespace Gratis::Scsi;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Device_t
//
Device_t::Device_t()
{
}

const Device_t::Info_t& Device_t::p_get_info()
{
  if ( m_info.num_blocks == 0 || m_info.block_size == 0 )
  {
    Scsi::Capacity16_t capacity16;
    if ( !this->read_capacity(capacity16) )
      throw this->error();
    m_info.num_blocks = capacity16.num_blocks;
    m_info.block_size = capacity16.block_size;
  }
  return m_info;
}

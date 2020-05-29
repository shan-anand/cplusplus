/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.cpp
@brief SCSI disk device implementation.
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
 * @brief SCSI disk device implentation of SCSI calls.
 */

#include <block/scsi/scsi_disk/datatypes.hpp>
#include <block/scsi/scsi_disk/device.hpp>

using namespace sid;
using namespace sid::block::scsi_disk;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_info
//
block::device_ptr device_info::create() const
{
  return device::create(*this)->to_block_device_ptr();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//

//! Create new SCSI-disk device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  device_ptr dev;

  try
  {
    dev = new device();
  }
  catch (const sid::exception&) { throw; }
  catch (const device* p)
  {
    if ( p ) delete p;
    throw sid::exception("Failed to create smart pointer object for scsi_disk device");
  }
  catch (...)
  {
    throw sid::exception("An unknown exeception while creating scsi_disk device");
  }

  if ( !dev )
    throw sid::exception("Failed to create scsi_disk device");

  return dev;
}

device::device()
{
  //m_verbose = Verbose::None;
}

bool device::test_unit_ready(scsi::sense& _sense)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::read_capacity(scsi::capacity16& _capacity)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::read(scsi::read16& _read16)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::write(scsi::write16& _write16)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::inquiry(scsi::inquiry::basic* _inquiry)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
  }
  catch (...)
  {
    this->exception() = sid::exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

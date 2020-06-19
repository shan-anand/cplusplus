/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.cpp
@brief NVME device implementation.
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
 * @brief NVME device implentation of NVME calls.
 */

#include <block/nvme/datatypes.hpp>
#include <block/nvme/device.hpp>
#include <common/regex.hpp>
#include <common/convert.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace sid;
using namespace sid::block::nvme;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_info
//
device_info::device_info()
{
}

std::string device_info::id() const
{
  return path;
}

void device_info::clear()
{
  path.clear();
}

bool device_info::empty() const
{
  return path.empty();
}

void device_info::set(const std::string& _infoStr)
{
  try
  {
    clear();

    sid::regex regEx("^(nvme:/)?(/.+)$");
    sid::regex::result out;
    if ( !regEx.exec(_infoStr.c_str(), /*out*/ out) )
      throw sid::exception(std::string("Invalid device info [") + _infoStr + "]: " + regEx.error());

    this->path = out[2];
  }
  catch (const std::exception&) { throw;  }
  catch (...) { throw sid::exception(std::string("An unhandled exception occurred in device_info::") + __func__); }
}

bool device_info::set(const std::string& _infoStr, std::string& csError) noexcept
{
  bool isSuccess = false;

  try
  {
    this->set(_infoStr);

    // Set successful status
    isSuccess  = true;
  }
  catch (const std::exception& e) { csError = e.what(); }
  catch (...) { csError = std::string("An unhandled exception occurred in device_info::") + __func__; }

  return isSuccess;
}

// Get the device info as a string
std::string device_info::to_str() noexcept
{
  return std::string("nvme:/") + this->path;
}

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
    if ( !dev )
      throw sid::exception("Failed to create nvme device");

    // Set the device info object before
    dev->m_info = _deviceInfo;

    //if ( !dev->p_open() )
    //  throw dev->exception();
  }
  catch (const sid::exception&) { throw; }
  catch (const device* p)
  {
    if ( p ) delete p;
    throw sid::exception("Failed to create smart pointer object for nvme device");
  }
  catch (...)
  {
    throw sid::exception("An unknown exeception while creating nvme device");
  }

  if ( !dev )
    throw sid::exception("Failed to create nvme device");

  return dev;
}

device::device()
{
  //m_verbose = Verbose::None;
}

device::~device()
{
}

bool device::ready()
{
  this->exception() = sid::exception(std::string(__func__) +
                                     "(" + this->id()+ "): Not implemented");

  return false;
}

block::capacity device::capacity(bool _force/* = false*/)
{
  block::capacity cap;
  {
    // Set the return value
    cap.blocks = 0;
    cap.block_size = 0;
  }
  return cap;
}

std::string device::wwn(bool _force/* = false*/)
{
  std::string wwnStr;
  {
    // Set the return value
    wwnStr.clear();
  }
  return wwnStr;
}

bool device::read(io_byte_units& _io_byte_units)
{
  return false;
}

bool device::write(io_byte_units& _io_byte_units)
{
  return false;
}

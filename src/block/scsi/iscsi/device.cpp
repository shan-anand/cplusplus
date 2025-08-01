/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.cpp
@brief ISCSI device implementation.
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
 * @brief ISCSI device implentation of SCSI calls.
 */

#include <block/scsi/iscsi/datatypes.hpp>
#include <block/scsi/iscsi/device.hpp>
#include <common/regex.hpp>

using namespace sid;
using namespace sid::block::iscsi;


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_info
//
device_info::device_info() :
  lun(-1)
{
}

std::string device_info::id() const
{
  return sid::to_str(lun);
}

void device_info::clear()
{
  portal.clear();
  chap.clear();
  mchap.clear();
  lun = -1;
  targets.clear();
}

bool device_info::empty() const
{
  return portal.empty();
}

void device_info::set(const std::string& _infoStr)
{
  try
  {
    clear();

    auto get_cred = [&](const std::string& key, const std::string& in)->basic_cred
      {
        basic_cred cred;
        std::string inEx = in;
        if ( in.empty() )
          ; // No chap credentials are given. It's ok. Don't throw an exception
        else if ( in[0] != '#' )
        {
          // <USERNAME>:<PASSWORD>
          if ( ! cred.set(in, ':') )
            throw sid::exception(key + ": Invalid syntax");
        }
        else if ( in.length() > 1 && in[1] != '#' )
        {
          // #BASE64(<USERNAME>:<PASSWORD>)
          inEx = in.substr(1);
          inEx = sid::base64::decode(inEx);
          if ( ! cred.set(inEx, ':') )
            throw sid::exception(key + ": Invalid syntax");
        }
        else
        {
          // ##BASE64(<USERNAME>):BASE64(<PASSWORD>)
          inEx = in.substr(2);
          if ( ! cred.set(in, ':') )
            throw sid::exception(key + ": Invalid syntax");
          cred.userName = sid::base64::decode(cred.userName);
          cred.password = sid::base64::decode(cred.password);
        }
        return cred;
      };

    auto get_lun = [&](const std::string& key, const std::string& in)->int
      {
        int lun = 0;
        if ( !sid::to_num(in, lun) )
          throw sid::exception(key + ": Invalid value");
        if ( lun < 0 )
          throw sid::exception(key + " cannot be negative");
        return lun;
      };

    auto get_iqn = [&](const std::string& key, const std::string& in)->std::string
      {
        std::string out = sid::trim(in);
        if ( out.empty() )
          throw sid::exception(key + " cannot be empty");
        return out;
      };

    /*
      Syntax: iscsi://<PORTAL>/@KEY=<VALUE>

         1) Portal
         ------- Optional parameters -------
         Syntax: @key=value
         -----------------------------------
         2) @iqn   -- Target IQN. Multiple values are allowed.
         3) @lun   -- non-negative LUN id.
         4) @chap  -- Chap credentials.
         5) @mchap -- Mutual chap credentials.
     */
    if ( ::strncmp(_infoStr.c_str(), "iscsi://", 8) != 0 )
      throw sid::exception("Invalid iSCSI URL syntax");

    std::vector<std::string> outVec;
    sid::split(outVec, _infoStr.substr(8), '/', SPLIT_TRIM);
    // 1st (required) parameter is the portal
    this->portal = outVec[0];

    // Iterate through the optional parameters
    std::string key, value;
    std::set<std::string> keys;
    for ( size_t i = 1; i < outVec.size(); i++ )
    {
      // Each parameter must be of the form @key=value
      const std::string& s = outVec[i];
      // All optional parameters must start with an @ character
      if ( s.empty() || s[0] != '@' )
        throw sid::exception("Invalid parameter at position " + sid::to_str(i));
      size_t pos = s.find('=');
      if ( pos == std::string::npos )
        throw sid::exception("Invalid syntax for parameter-" + s);
      key = s.substr(0, pos);
      if ( key != "@iqn" && keys.find(key) != keys.end() )
        throw sid::exception(key + " cannot be repeated");
      keys.insert(key);
      value = s.substr(pos+1);
      if ( key == "@iqn" )
        this->targets.push_back(get_iqn(key, value));
      else if ( key == "@lun" )
        this->lun = get_lun(key, value);
      else if ( key == "@chap" )
        this->chap = get_cred(key, value);
      else if ( key == "@mchap" )
        this->mchap = get_cred(key, value);
      else
        throw sid::exception("Invalid key " + key);
    }
  }
  catch (const std::exception&)
  {
    this->clear();
    throw;
  }
  catch (...)
  {
    this->clear();
    throw sid::exception(std::string("An unhandled exception occurred in device_info::") + __func__);
  }
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
  auto to_cred = [&](const basic_cred& cred)->std::string
    {
      return std::string("#") + sid::base64::encode(cred.userName + ":" + cred.password);
    };

  std::ostringstream out;

  out << "iscsi://" << this->portal;
  for ( const device_info::target& target : this->targets )
    out << "/@iqn=" << target.iqn;
  if ( this->lun >= 0 )
    out << "/@lun=" << this->lun;
  if ( !this->chap.empty() )
    out << "/@chap=" << to_cred(this->chap);
  if ( !this->mchap.empty() )
    out << "/@mchap=" << to_cred(this->mchap);

  return out.str();
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
  }
  catch (const sid::exception&) { throw; }
  catch (const device* p)
  {
    if ( p ) delete p;
    throw sid::exception("Failed to create smart pointer object for iscsi device");
  }
  catch (...)
  {
    throw sid::exception("An unknown exeception while creating iscsi device");
  }

  if ( !dev )
    throw sid::exception("Failed to create iscsi device");

  return dev;
}

device::device()
{
  //m_verbose = Verbose::None;
}

device::~device()
{
}

bool device::test_unit_ready(scsi::sense& _sense)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    this->exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
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
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    this->exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::read(scsi::read16_vec& _read16_vec)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    this->exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

bool device::write(scsi::write16_vec& _write16_vec)
{
  bool isSuccess = false;

  try
  {
    isSuccess = true;
  }
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    this->exception(-1, std::string(__func__) + "(" + this->id() + "): Unknown exception");
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
  catch (const sid::exception& _e)
  {
    this->exception(_e);
  }
  catch (...)
  {
    this->exception(-1, std::string(__func__) + "(" + this->id()+ "): Unknown exception");
  }

  return isSuccess;
}

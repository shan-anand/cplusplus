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
#include <common/util.hpp>
#include <common/json.hpp>

#include <sstream>

using namespace sid;
using namespace sid::block;

static bool s_enum_block_devices(const std::string& _path, FNDeviceDetailCallback& _fnDeviceDetailCallback);

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

bool device::read(io_byte_unit& _io_byte_unit)
{
  io_byte_units io_byte_units;
  io_byte_units.push_back(_io_byte_unit);
  bool isSuccess = this->read(io_byte_units);
  _io_byte_unit = io_byte_units[0];
  return isSuccess;
}

bool device::write(io_byte_unit& _io_byte_unit)
{
  io_byte_units io_byte_units;
  io_byte_units.push_back(_io_byte_unit);
  bool isSuccess = this->write(io_byte_units);
  _io_byte_unit = io_byte_units[0];
  return isSuccess;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_detail
//
device_detail::device_detail()
{
  clear();
}

void device_detail::clear()
{
  name.clear();
  path.clear();
  size = 0;
  blockSize = 0;
  isReadOnly = false;
  uuid.clear();
  model.clear();
  serial.clear();
  wwn.clear();
  label.clear();
  mountPoint.clear();
  fs.clear();
  part.clear();
  children.clear();
}

void device_detail::filesystem::clear()
{
  type.clear();
  size = available = used = 0;
}

void device_detail::partition::clear()
{
  type.clear();
  uuid.clear();
  label.clear();
}

/*static*/
device_detail device_detail::get(const std::string& _path)
{
  if ( _path.empty() )
    throw sid::exception("Path cannot be empty");

  device_detail deviceDetail;
  FNDeviceDetailCallback fn_callback = [&](const device_detail& _deviceDetail)
    {
      deviceDetail = _deviceDetail;
      return false;
    };
  s_enum_block_devices(_path, fn_callback);
  //s_enum_block_devices(_path, [&](const device_detail& _deviceDetail) {deviceDetail = _deviceDetail; return false;};);
  return deviceDetail;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device_details
//

/*static*/
device_details device_details::get()
{
  device_details deviceDetails;
  FNDeviceDetailCallback fn_callback = [&](const device_detail& _deviceDetail)
    {
      deviceDetails.push_back(_deviceDetail);
      return true;
    };
  device_details::enumerate(fn_callback);
  //device_details::enumerate([&](const device_detail& _deviceDetail) {deviceDetails.push_back(_deviceDetail); return true;});
  return deviceDetails;
}

/*static*/
device_details device_details::get(const std::string& _path)
{
  device_details deviceDetails;
  deviceDetails.push_back(device_detail::get(_path));
  return deviceDetails;
}

/*static*/
device_details device_details::get(const std::vector<std::string>& _paths)
{
  device_details deviceDetails;
  for ( const std::string& path : _paths )
    deviceDetails.push_back(device_detail::get(path));
  return deviceDetails;
}

/*static*/
bool device_details::enumerate(FNDeviceDetailCallback& _fnDeviceDetailCallback)
{
  return s_enum_block_devices(std::string(), _fnDeviceDetailCallback);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// local functions
//
void s_fill_missing_details(device_detail& deviceDetail)
{
  // Check for any missing entries
  union
  {
    struct
    {
      uint8_t serial : 1;
      uint8_t wwn : 1;
    };
    uint8_t set;
  } missing = {0};
  missing.serial = deviceDetail.serial.empty()? 1 : 0;
  missing.wwn = deviceDetail.wwn.empty()? 1 : 0;
  if ( !missing.set )
    return;

  const std::string script = "/usr/bin/udevadm info --query=property " + deviceDetail.path;
  util::command cmdOut = util::command::execute(script);
  if ( cmdOut.retVal )
    throw sid::exception(-1, cmdOut.error);

  // Parse each line of output
  std::istringstream in(cmdOut.response);
  std::string line;

  std::map<std::string, std::string> props;
  while ( std::getline(in, line) )
  {
    size_t pos = line.find('=');
    if ( pos != std::string::npos )
      props[line.substr(0, pos)] = line.substr(pos+1);
  }

  std::map<std::string, std::string>::const_iterator it;
  if ( missing.serial )
  {
    if ( (it = props.find("ID_SERIAL_SHORT")) != props.end() )
      deviceDetail.serial = it->second;
    if ( (it = props.find("SCSI_IDENT_SERIAL")) != props.end() )
      deviceDetail.serial = it->second;
    if ( (it = props.find("ID_SCSI_SERIAL")) != props.end() )
      deviceDetail.serial = it->second;
  }
  if ( missing.wwn )
  {
    if ( (it = props.find("ID_WWN")) != props.end() )
      deviceDetail.wwn = it->second;
  }
}

bool s_enum_block_devices(const std::string& _path, FNDeviceDetailCallback& _fnDeviceDetailCallback)
{
  const std::string script = "/usr/bin/lsblk --bytes --output-all --json " + _path;
  util::command cmdOut = util::command::execute(script);
  if ( cmdOut.retVal )
    throw sid::exception(-1, cmdOut.error);

  json::value jroot;
  json::value::parse(jroot, cmdOut.response);
  const json::value& jdevices = jroot["blockdevices"];
  for ( size_t i = 0; i < jdevices.size(); i++ )
  {
    const json::value& jdevice = jdevices[i];
    device_detail deviceDetail;
    deviceDetail.name = jdevice["name"].get_str();
    deviceDetail.path = jdevice["path"].get_str();
    deviceDetail.serial = jdevice["serial"].get_str();
    deviceDetail.wwn = jdevice["wwn"].get_str();

    // Fill any missing details using another command
    s_fill_missing_details(deviceDetail);

    if ( !_fnDeviceDetailCallback(deviceDetail) )
      break;
  }

  return true;
}

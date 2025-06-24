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
#include <common/convert.hpp>

#include <sstream>

using namespace sid;
using namespace sid::block;

namespace local
{
  bool enum_block_devices(
    const std::string&      _path,
    FNDeviceDetailCallback& _fnDeviceDetailCallback
  );

  void fill_missing_details(device_detail& deviceDetail);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// device
//

device::device() : smart_ref()
{
  //m_verbose = Verbose::None;
}

device::~device()
{
}

//! Create new block device object
/*static*/
device_ptr device::create(const device_info& _deviceInfo)
{
  return _deviceInfo.create()->to_block_device_ptr();
}

device_ptr device::to_block_device_ptr() const
{
  device_ptr dev = dynamic_cast<device*>(const_cast<device*>(this));
  if ( ! dev ) throw sid::exception("Cannot convert to block::device_ptr");
  return dev;
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
  isFloppy = false;
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
  local::enum_block_devices(_path, fn_callback);
  //local::enum_block_devices(_path, [&](const device_detail& _deviceDetail) {deviceDetail = _deviceDetail; return false;};);
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
  return local::enum_block_devices(std::string(), _fnDeviceDetailCallback);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// local functions
//
bool local::enum_block_devices(
  const std::string&      _path,
  FNDeviceDetailCallback& _fnDeviceDetailCallback
  )
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
    jdevice.get_value("name", deviceDetail.name);
    jdevice.get_value("path", deviceDetail.path);
    jdevice.get_value("type", deviceDetail.type);
    jdevice.get_value("size", deviceDetail.size);
    jdevice.get_value("phy-sec", deviceDetail.blockSize);
    jdevice.get_value("ro", deviceDetail.isReadOnly);
    jdevice.get_value("model", deviceDetail.model);
    jdevice.get_value("serial", deviceDetail.serial);
    jdevice.get_value("wwn", deviceDetail.wwn);
    jdevice.get_value("label", deviceDetail.label);
    jdevice.get_value("mountpoint", deviceDetail.mountPoint);

    // Fill any missing details using another command
    local::fill_missing_details(deviceDetail);

    if ( !_fnDeviceDetailCallback(deviceDetail) )
      break;
  }

  return true;
}

void local::fill_missing_details(device_detail& deviceDetail)
{
  // Run the udevadm command and fill the missing entries
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
  {
    if ( deviceDetail.serial.empty() )
    {
      static const char* s_serialPropNames[] = {
	"ID_SERIAL", "ID_SERIAL_SHORT", "SCSI_IDENT_SERIAL", "ID_SCSI_SERIAL"
      };
      for ( auto name : s_serialPropNames )
	if ( (it = props.find(name)) != props.end() )
	  deviceDetail.serial = it->second;
    }
    // Fix the Serial number
    if ( sid::to_lower(deviceDetail.serial.substr(0, 2)) == "0x" )
      deviceDetail.serial = deviceDetail.serial.substr(2);
  }
  {
    if ( deviceDetail.wwn.empty() )
    {
      if ( (it = props.find("ID_WWN")) != props.end() )
        deviceDetail.wwn = it->second;
    }
    // Fix the WWN
    if ( sid::to_lower(deviceDetail.wwn.substr(0, 2)) == "0x" )
      deviceDetail.wwn = deviceDetail.wwn.substr(2);
  }
  if ( (it = props.find("ID_DRIVE_FLOPPY")) != props.end()
       && ( it->second == "1" || sid::to_lower(it->second) == "true" )
       )
    deviceDetail.isReadOnly = true;
}

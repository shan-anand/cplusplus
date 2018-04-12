/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.h
@brief SCSI device interface. Defines virtual functions.
       It will be used as the base class to implement SCSI operations,
       Example: IScsi and Scsi generic device
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
 * @file  Device.h
 * @brief SCSI Device interface that defines all SCSI calls.
 */

#ifndef _GRATIS_SCSI_DEVICE_H_
#define _GRATIS_SCSI_DEVICE_H_

#include "Constants.h"
#include "DataTypes.h"
#include <Util/SmartPtr.h>

#define SCSI_DEFAULT_SINGLE_IO_SIZE        (128*1024)

namespace Gratis {
namespace Scsi {

class Device_t;
using Device_s = SmartPtr<Device_t*>;

enum class DeviceType { Invalid = -1, Generic, IScsi };

/**
 * @class Device_t
 * @brief Send SCSI commands
 */
class Device_t : public SmartRef
{
public:
  virtual ~Device_t();

  //! get/set verbose level
  const Verbose& verbose() const { return m_verbose; }
  void verbose(const Verbose& v) { m_verbose = v; }

  //! get/set error messages
  const std::string& error() const { return m_error; }
  std::string& error() { return m_error; }
  std::string& error(const std::string& str) { m_error = str; return m_error; }

  //! Get the capacity of the device
  Scsi::Capacity_t capacity(bool force = false) throw (std::string);

public:
  //! Clone a new device object
  virtual Device_s clone() throw (std::string) = 0;
  //! Check whether the device is empty
  virtual bool empty() const noexcept = 0;

  virtual bool test_unit_ready() noexcept = 0;
  virtual bool read_capacity(Scsi::Capacity10_t& capacity10) noexcept = 0;
  virtual bool read_capacity(Scsi::Capacity16_t& capacity16) noexcept = 0;

  virtual bool inquiry(Scsi::Inquiry::Standard_t& inquiry) noexcept = 0;
  virtual bool inquiry(Scsi::Inquiry::UnitSerialNumber_t& usn) noexcept = 0;
  virtual bool inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq) noexcept = 0;

  virtual bool read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr) noexcept = 0;
  virtual bool write(Scsi::Write16_t& write16, Scsi::Sense_t* sense = nullptr) noexcept = 0;

  // Implement Scsi::DataTypeEx functions
  bool read(Scsi::Read16Ex_t& read16x) noexcept { return read(read16x, &read16x.sense); }
  bool write(Scsi::Write16Ex_t& write16x) noexcept { return write(write16x, &write16x.sense); }
protected:
  //! Default constructor
  Device_t();
  //! Disable copy constructor
  Device_t(const Device_t&) = delete;
  //! Disable copy operator
  Device_t& operator=(const Device_t&) = delete;

private:
  //! Member variables
  Verbose          m_verbose;
  std::string      m_error;
  Scsi::Capacity_t m_capacity;
};

} // namespace Scsi
} // namespace Gratis

#endif // _GRATIS_SCSI_DEVICE_H_


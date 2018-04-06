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

#ifndef _SCSI_DEVICES_H_
#define _SCSI_DEVICES_H_

#include "Constants.h"
#include "DataTypes.h"
#include <Util/SmartPtr.h>

#define SCSI_DEFAULT_SINGLE_IO_SIZE        (128*1024)

namespace Gratis {
namespace Scsi {

class Device_t;
using Device_s = SmartPtr<Device_t*>;

/**
 * @class Device_t
 * @brief Send SCSI commands
 */
class Device_t : public SmartRef
{
public:
  //! Error string, getter and setters
  const std::string& error() const { return m_error; }
  std::string& error() { return m_error; }
  std::string& error(const std::string& s) { m_error = s; return m_error; }

public:
  //! Clone a new device object
  virtual Device_s clone() = 0;
  //! Check whether the device is empty
  virtual bool empty() const = 0;

  virtual bool test_unit_ready() = 0;
  virtual bool read_capacity(Scsi::Capacity10_t& capacity10) = 0;
  virtual bool read_capacity(Scsi::Capacity16_t& capacity16) = 0;

  virtual bool inquiry(Scsi::Inquiry::Standard_t& inquiry) = 0;
  virtual bool inquiry(Scsi::Inquiry::UnitSerialNumber_t& usn) = 0;
  virtual bool inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq) = 0;

  virtual bool read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr) = 0;
  virtual bool write(Scsi::Write16_t& write16, Scsi::Sense_t* sense = nullptr) = 0;

  // Implement Scsi::DataTypeEx functions
  bool read(Scsi::Read16Ex_t& read16x) { return read(read16x, &read16x.sense); }
  bool write(Scsi::Write16Ex_t& write16x) { return write(write16x, &write16x.sense); }

  //! Get /the total number of blocks for this device (Comes from Scsi::Capacity16_t)
  uint64_t num_blocks() { return p_get_info().num_blocks; }
  //! Get the number of bytes per block for this device (Comes from Scsi::Capacity16_t)
  uint32_t block_size() { return p_get_info().block_size; }

protected:
  //! Default constructor
  Device_t();
  //! Disable copy constructor 
  Device_t(const Device_t&) = delete;
  //! Disable copy operator
  Device_t& operator=(const Device_t&) = delete;

private:
  struct Info_t
  {
    uint64_t num_blocks;
    uint32_t block_size;
    //! Constructor
    Info_t() { clear(); }
    void clear() { num_blocks = 0; block_size = 0; }
  };
  const Info_t& p_get_info();

  //! Member variables
  std::string m_error;
  Info_t      m_info;
};

} // namespace Scsi
} // namespace Gratis

#endif // _SCSI_DEVICES_H_


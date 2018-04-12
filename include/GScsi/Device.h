/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file Device.h
@brief Generic SCSI device object.
       Implements all the virtual functions in SCSI::Device
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
 * @file Device.h
 * @brief Definintion of Device API. C++ wrapper for libgsutil2.
 *        Send SCSI commands to sg device.
 */

#ifndef _GRATIS_GSCSI_DEVICE_H_
#define _GRATIS_GSCSI_DEVICE_H_

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "Scsi/DataTypes.h"
#include "Scsi/Device.h"
#include <Common/SmartPtr.h>

namespace Gratis {
namespace GScsi {

class Device_t;
using Device_s = SmartPtr<Device_t>;

/**
 * @class Device_t
 * @brief Send SCSI commands from an sg device.
 */
class Device_t : public Scsi::Device_t
{
public:
  virtual ~Device_t();

  static Device_s create(const std::string& devicePath, const Verbose& verbose = Verbose::None) throw (std::string);

  bool empty() const { return ( m_fd == -1 ); }
  const std::string& path() const { return m_path; }

  int result() const { return m_res; }
  std::string resultStr() const;

  bool set_warnings_strm(const std::string& filePath);
  bool set_warnings_strm(int fd);
  bool clear_warnings_strm();

  // Virtual functions
  DeviceType type() const noexcept override { return Scsi::Generic; }
  Scsi::Device_s base() noexcept override { return dynamic_cast<Scsi::Device_t*>(this); }

  bool test_unit_ready() noexcept override;
  bool read_capacity(Scsi::Capacity10_t& capacity10) noexcept override;
  bool read_capacity(Scsi::Capacity16_t& capacity16) noexcept override;
  bool inquiry(Scsi::Inquiry::Standard_t& inquiry) noexcept override;
  bool inquiry(Scsi::Inquiry::UnitSerialNumber_t& usn) noexcept override;
  bool inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq) noexcept override;
  bool read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr) noexcept override;
  bool write(Scsi::Write16_t& write16, Scsi::Sense_t* sense = nullptr) noexcept override;

  int sg_reserved_size();
  int sg_set_reserved_size(int rs);

private:
  Device_t();
  Device_t(const Device_t&) = delete;
  Device_t& operator=(const Device_t&) = delete;

  bool p_open(const std::string& devicePath);
  void p_close();
  bool p_inquiry(Scsi::Inquiry::BasicVPD* vpd);

  void p_read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr) throw (std::string);
  void p_write(Scsi::Write16_t& write16, Scsi::Sense_t* sense = nullptr) throw (std::string);

private:
  struct Info
  {
    int      sg_reserved_size;

    Info() { clear(); }
    void clear();
  };

  std::string m_path;
  int         m_fd;
  FILE*       m_fpstream;
  int         m_res;
  bool        m_noisy;
  Info        m_info;
};

} // namespace GScsi
} // namespace Gratis

#endif // _GRATIS_GSCSI_DEVICE_H_

/**
 * @file device.hpp
 * @brief Definintion of Generic Scsi Device API. C++ wrapper for libgsutil2.
 *        Send SCSI commands to sg device.
 */

#ifndef _SID_GSCSI_DEVICE_HPP_
#define _SID_GSCSI_DEVICE_HPP_

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "scsi/datatypes.hpp"
#include "scsi/device.hpp"
#include <common/smart_ptr.h>

namespace sid {
namespace GScsi {

class Device_t;
using Device_s = smart_ptr<Device_t>;

struct DeviceInfo_t;
using DeviceInfo_s = smart_ptr<DeviceInfo_t>;

/**
 * @class DeviceInfo_t
 * @brief Generic SCSI Device information object
 */
struct DeviceInfo_t : public Scsi::DeviceInfo_t
{
  std::string path; //! Device path

  DeviceInfo_t();
  virtual ~DeviceInfo_t();

  //! Virtual functions override
  Scsi::DeviceType type() const override { return Scsi::DeviceType::Generic; }
  bool set(const std::string& infoStr, std::string* pcsError = nullptr) throw (std::string) override;
  void clear() override;
  bool empty() const override;
  std::string id() const override;
  std::string toString() const override;
  Scsi::Device_s create() const throw (std::string) override;
};

/**
 * @class Device
 * @brief Send SCSI commands from an sg device.
 */
class Device_t : public Scsi::Device_t
{
public:
  //! Get the device type
  Scsi::DeviceType type() const override { return Scsi::DeviceType::Generic; }

public:
  virtual ~Device_t();

  static Device_s create(const DeviceInfo_t& info, int verboseLevel = 0) throw (std::string);

  // Virtual functions override
  bool empty() const override { return ( m_fd == -1 ); }
  Scsi::Device_s clone() throw (std::string) override;
  bool test_unit_ready() override;
  bool read_capacity(Scsi::Capacity10_t& capacity10) override;
  bool read_capacity(Scsi::Capacity16_t& capacity16) override;

  bool inquiry(Scsi::Inquiry::Standard_t& inquiry) override;
  bool inquiry(Scsi::Inquiry::UnitSerialNumber_t& usn) override;
  bool inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq) override;

  bool read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr) override;
  bool write(Scsi::Write16& write16, Scsi::Sense_t* sense = nullptr) override;

  // Other public functions
  void setVerboseLevel(int level);

  int result() const { return m_res; }
  std::string resultStr() const;
  const std::string error() const { return m_error; }

  bool set_warnings_strm(const std::string& filePath);
  bool set_warnings_strm(int fd);
  bool clear_warnings_strm();

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
  int         m_verboseLevel;
  Info        m_info;
};

} // namespace GScsi
} // namespace sid

#endif // _SID_GSCSI_DEVICE_HPP_

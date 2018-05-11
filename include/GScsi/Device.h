/**
 * @file Device.h
 * @brief Definintion of Device API. C++ wrapper for libgsutil2.
 *        Send SCSI commands to sg device.
 */

#ifndef _SG_DEVICE_INITIATOR_H_
#define _SG_DEVICE_INITIATOR_H_

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

struct DeviceInfo_t;
using DeviceInfo_s = SmartPtr<DeviceInfo_t>;

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
  virtual ~Device_t();

  static Device_s create(const DeviceInfo_t& info, int verboseLevel = 0) throw (std::string);

  bool empty() const { return ( m_fd == -1 ); }
  const std::string& path() const { return m_path; }

  void setVerboseLevel(int level);

  int result() const { return m_res; }
  std::string resultStr() const;
  const std::string error() const { return m_error; }

  bool set_warnings_strm(const std::string& filePath);
  bool set_warnings_strm(int fd);
  bool clear_warnings_strm();

  bool test_unit_ready();
  bool read_capacity(Scsi::Capacity10_t& capacity10);
  bool read_capacity(Scsi::Capacity16_t& capacity16);

  bool inquiry(Scsi::Inquiry::Standard_t& inquiry);
  bool inquiry(Scsi::Inquiry::UnitSerialNumber_t& usn);
  bool inquiry(Scsi::Inquiry::SupportedVPDPages_t& inq);

  bool read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr);
  bool write(Scsi::Write16& write16, Scsi::Sense_t* sense = nullptr);

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
} // namespace Gratis

#endif // _SG_DEVICE_INITIATOR_H_

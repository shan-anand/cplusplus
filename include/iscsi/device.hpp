/**
 * @file device.hpp
 * @brief Definintion of IScsi Device API.
 *        Send SCSI commands to IScsi device.
 */

#pragma once

#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "scsi/datatypes.hpp"
#include "scsi/device.hpp"
#include <common/smart_ptr.hpp>

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
  struct Target
  {
    std::string portal, iqn;
    Target() {}
    Target(const std::string& _portal, const std::string& _iqn) : portal(_portal), iqn(_iqn) {}
    void clear() { portal.clear(); iqn.clear(); }
    bool empty() const { return portal.empty() || iqn.empty(); }
  };
  // target["portal"] = "Target-IQN"
  struct Targets : public std::vector<Target>
  {
    bool set(const Target& target);
    bool set(const std::string& portal, const std::string& iqn);
  };
  Targets             targets;
  //iscsi_session_type  type;
  //iscsi_header_digest digest;
  BasicCredential     chap;
  BasicCredential     mutualChap;
  LUN                 lun; //! Applicable only for normal login

  std::string portal() const { return targets.empty()? std::string() : targets[0].portal; }
  std::string iqn() const { return targets.empty()? std::string() : targets[0].iqn; }

  DeviceInfo_t();
  virtual ~DeviceInfo_t();

  //! Virtual functions override
  Scsi::DeviceType type() const override { return Scsi::DeviceType::IScsi; }
  bool set(const std::string& infoStr, std::string* pcsError = nullptr) override;
  void clear() override;
  bool empty() const override;
  std::string id() const override;
  std::string toString() const override;
  Scsi::Device_s create() const override;
};

/**
 * @class Device
 * @brief Send SCSI commands from an sg device.
 */
class Device_t : public Scsi::Device_t
{
public:
  //! Get the device type
  Scsi::DeviceType type() const override { return Scsi::DeviceType::IScsi; }

public:
  virtual ~Device_t();

  static Device_s create(const DeviceInfo_t& info, int verboseLevel = 0);

  // Virtual functions override
  bool empty() const override { return ( m_fd == -1 ); }
  Scsi::Device_s clone() override;
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

private:
  Device_t();
  Device_t(const Device_t&) = delete;
  Device_t& operator=(const Device_t&) = delete;

  bool p_open(const std::string& devicePath);
  void p_close();
  bool p_inquiry(Scsi::Inquiry::BasicVPD* vpd);

  void p_read(Scsi::Read16_t& read16, Scsi::Sense_t* sense = nullptr);
  void p_write(Scsi::Write16_t& write16, Scsi::Sense_t* sense = nullptr);

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

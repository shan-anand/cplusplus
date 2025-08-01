/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.hpp
@brief ISCSI device definition
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
 * @file  device.hpp
 * @brief Device definition for ISCSI
 */

#pragma once

#include <string>
#include <common/smart_ptr.hpp>
#include "datatypes.hpp"
#include "../device.hpp"
#include <common/simple_types.hpp>
#include <common/convert.hpp>

namespace sid::block::iscsi {

class device;
using device_ptr = smart_ptr<device>;

/**
 * @struct device_info
 * @brief Connection interface for ISCSI device
 */
struct device_info : public scsi::device_info
{
  using super = scsi::device_info;

  struct target
  {
    std::string iqn;
    target() {}
    target(const std::string& _iqn) : iqn(_iqn) {}
    void clear() { iqn.clear(); }
  };
  using dev_targets = std::vector<target>;

  //! Member variables
  std::string portal;   //! ISCSI device portal
  basic_cred  chap;     //! Chap credentials (if required)
  basic_cred  mchap;    //! Mutual chap credentials (if required)
  int         lun;      //! Current lun id used (used only for normal login if provided)
  dev_targets targets;  //! Target IQNs

  device_info();

  //=============================================================================
  //! Override functions from block::device_info
  block::device_type type() const override final { return block::device_type::iscsi; }
  std::string id() const override final;
  void clear() override final;
  bool empty() const override final;
  // Set the device info object
  void set(const std::string& _infoStr) override final;
  bool set(const std::string& _infoStr, std::string& csError) noexcept override final;
  // Get the device info as a string
  std::string to_str() noexcept override final;
  //! Create a new IScsi device object
  block::device_ptr create() const override final;
  //=============================================================================
};

/**
 * @struct device
 * @brief ISCSI device interface definition
 */
class device : public scsi::device
{
public:
  using super = scsi::device;

  // Virtual destructor
  virtual ~device();

  // Do not allow copy operation
  device(const device&) = delete;
  device& operator=(const device&) = delete;

  //! Create new IScsi device object
  static device_ptr create(const device_info& _deviceInfo);

  //=============================================================================
  //! Override functions from block::device
  block::device_type type() const override final { return m_info.type(); }
  std::string id() const override final { return m_info.id(); }
  //=============================================================================

  //=============================================================================
  //! Override functions from scsi::device
  bool test_unit_ready(scsi::sense& _sense) override;
  bool read_capacity(scsi::capacity16& _capacity) override;
  bool read(scsi::read16_vec& _read16_vec) override;
  bool write(scsi::write16_vec& _write16_vec) override;
  bool inquiry(scsi::inquiry::basic* _inquiry) override;
  //=============================================================================

  using super::read;
  using super::write;

  const device_info& info() const { return m_info; }
  void set_lun(int lun) { m_info.lun = lun; }

  //! TODO: Perform IScsi discovery
  void discovery();
  //! TODO: Get all the available luns
  std::vector<int> luns();

private:
  device_info m_info;

private:
  device();
};

  //  if ( scsi_device->type() == block::device_type::iscsi )
  //dynamic_cast<iscsi::device*>(scsi_device.ptr())->set_lun(lun_id);

} // namespace sid::block::iscsi

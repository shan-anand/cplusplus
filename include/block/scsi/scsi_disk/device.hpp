/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.hpp
@brief scsi disk device definition
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
 * @brief Device definition for scsi disk
 */

#pragma once

#include <string>
#include <fcntl.h>
#include <common/smart_ptr.hpp>
#include "datatypes.hpp"
#include "../device.hpp"

namespace sid::block::scsi_disk {

class device;
using device_ptr = smart_ptr<device>;

struct access_mode
{
  enum ID {
    read_only  = O_RDONLY,
    write_only = O_WRONLY,
    read_write = O_RDWR
  };

  //! Constructors
  access_mode(const ID& _id = access_mode::read_write) : m_id(_id) {}
  access_mode(const access_mode&) = default;

  //! Overloaded assignment operators
  access_mode& operator=(const access_mode&) = default;
  access_mode& operator=(const ID& _id) { m_id = _id; return *this; }

  //! Overloaded comparison operators
  bool operator==(const ID& _id) const { return m_id == _id; }

  bool has_read() const {
    return m_id == access_mode::read_only || m_id == access_mode::read_write;
  }
  bool has_write() const {
    return m_id == access_mode::write_only || m_id == access_mode::read_write;
  }

  const ID& operator()() const { return m_id; }
  int flag() const { return static_cast<int>(m_id); }

private:
  ID m_id;
};

/**
 * @struct device_info
 * @brief Connection interface for SCSI disk device
 */
struct device_info : public scsi::device_info
{
  using super = scsi::device_info;

  //! Member variables
  std::string path; //! Device path
  access_mode mode; //! Access mode for opening the device

  device_info();

  //=============================================================================
  //! Override functions from block::device_info
  block::device_type type() const final { return block::device_type::scsi_disk; }
  std::string id() const final;
  void clear() final;
  bool empty() const final;
  // Set the device info object
  void set(const std::string& _infoStr) final;
  bool set(const std::string& _infoStr, std::string& csError) noexcept final;
  // Get the device info as a string
  std::string to_str() noexcept final;
  //! Create a new block device object
  block::device_ptr create() const final;
  //=============================================================================
};

/**
 * @struct device
 * @brief SCSI-disk device interface definition
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

  //! Create new SCSI-disk device object
  static device_ptr create(const device_info& _deviceInfo);

  //=============================================================================
  //! Override functions from block::device
  block::device_type type() const final { return m_info.type(); }
  std::string id() const final { return m_info.id(); }
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

  bool is_char_device() const;
  bool is_block_device() const;

private:
  device_info m_info;    //! Device information associated with the current device connection
  int         m_fd;      //! File descriptor of the device
  uint32_t    m_devMode; //! read/write mode associated with the device

private:
  //! Private constructor
  device();
  //! Open the device
  bool p_open();
  //! Close the device
  void p_close();
  //! Set non-blocking mode
  bool p_set_non_blocking();

  //! Block read using SCSI READ16
  void p_read(scsi::read16& _read16);
  //! Block write using SCSI WRITE16
  void p_write(scsi::write16& _write16);
};

} // namespace sid::block::scsi_disk

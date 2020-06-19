/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.hpp
@brief NVME device definition
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
 * @brief Device definition for NVME
 */

#ifndef _SID_NVME_DEVICE_H_
#define _SID_NVME_DEVICE_H_

#include <string>
#include <common/smart_ptr.hpp>
#include "datatypes.hpp"
#include "../device.hpp"

namespace sid {
namespace block {
namespace nvme {

class device;
using device_ptr = smart_ptr<device>;

/**
 * @struct device_info
 * @brief Connection interface for NVME device
 */
struct device_info : public block::device_info
{
  //! Member variables
  std::string path; //! Device path

  device_info();

  //=============================================================================
  //! Override functions from block::device_info
  block::device_type type() const final { return block::device_type::nvme; }
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
 * @brief NVMe device interface definition
 */
class device : public block::device
{
public:
  using super = block::device;

  // Virtual destructor
  virtual ~device();

  // Do not allow copy operation
  device(const device&) = delete;
  device& operator=(const device&) = delete;

  //! Create new NVMe-disk device object
  static device_ptr create(const device_info& _deviceInfo);

  //=============================================================================
  //! Override functions from block::device
  block::device_type type() const final { return m_info.type(); }
  std::string id() const final { return m_info.id(); }
  bool ready() override;
  block::capacity capacity(bool _force = false) override;
  std::string wwn(bool _force = false) override;
  bool read(io_byte_units& _io_byte_units) override;
  bool write(io_byte_units& _io_byte_units) override;
  //=============================================================================

  using super::read;
  using super::write;

private:
  device_info m_info;

private:
  device();
};

} // namespace nvme
} // namespace block
} // namespace sid

#endif // _SID_NVME_DEVICE_H_

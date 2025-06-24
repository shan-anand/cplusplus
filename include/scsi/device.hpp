/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.hpp
@brief SCSI device interface
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
 * @brief Constant values in SCSI
 */

#pragma once

#include <string>
#include <cstdint>
#include <common/exception.hpp>
#include <common/opt.hpp>
#include <common/smart_ptr.hpp>

namespace sid {
namespace scsi {

class device;
using device_ptr = smart_ptr<device>;

//! SCSI device types
enum class device_type : uint8_t { invalid, gscsi, iscsi };

/**
 * @struct device_info
 * @brief Connection interface for SCSI device
 */
struct device_info
{
  virtual device_type type() const = 0;
  virtual std::string id() const = 0;
};

/**
 * @struct device
 * @brief SCSI device interface definition
 */
class device : public smart_ref
{
public:
  device();
  static device_ptr create(device_info* _deviceInfo);

  virtual device_type type() const = 0;

  virtual std::string id() const = 0;
  bool read_capacity(/*out*/ scsi::capacity16& _capacity16);
  virtual bool read(/*in/out*/ scsi::read16& _read16) = 0;
  virtual bool write(/*in*/ const scsi::write16& _write16) = 0;
  virtual bool test_unit_ready(/*out*/ scsi::sense& _sense) = 0;

  virtual bool inquiry(scsi::inquiry::basic* _inquiry) = 0;

public:
  scsi::capacity capacity(bool _force = false);
  std::string wwn(bool _force = false);
  const sid::exception& error() const { return m_error; }

protected:
  sid::exception m_error;

  virtual bool v_read_capacity(/*out*/ scsi::capacity16& _capacity16) = 0;
  virtual bool v_read(/*in/out*/ scsi::read16& _read16) = 0;
  virtual bool v_write(/*in*/ const scsi::write16& _write16) = 0;

private:
  scsi::capacity m_capacity;
  std::string    m_wwn;
};

} // namespace scsi
} // namespace sid

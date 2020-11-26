/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file device.hpp
@brief block device interface
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
 * @brief Constant values in block
 */

#ifndef _SID_BLOCK_DEVICE_H_
#define _SID_BLOCK_DEVICE_H_

#include <string>
#include <common/smart_ptr.hpp>
#include "datatypes.hpp"

namespace sid {
namespace block {

class device;
using device_ptr = smart_ptr<device>;

/**
 * @struct device_info
 * @brief Connection interface for any block device
 */
struct device_info
{
  virtual device_type type() const = 0;
  virtual std::string id() const = 0;
  virtual void clear() = 0;
  virtual bool empty() const = 0;
  // Set the device info object
  virtual void set(const std::string& _infoStr) = 0;
  virtual bool set(const std::string& _infoStr, std::string& csError) noexcept = 0;
  // Get the device info as a string
  virtual std::string to_str() noexcept = 0;
  //! Create a new block device object
  virtual device_ptr create() const = 0;
};
  
/**
 * @struct device
 * @brief block device interface definition
 */
class device : public smart_ref
{
public:
  // Virtual destructor
  virtual ~device();

  // Do not allow copy operation
  device(const device&) = delete;
  device& operator=(const device&) = delete;

  //! Create new block device object
  static device_ptr create(const device_info& _deviceInfo);

  device_ptr to_block_device_ptr() const { return dynamic_cast<device*>(const_cast<device*>(this)); }

  //! Virtual functions to be overwritten in the derived classes
  virtual device_type type() const = 0;
  virtual std::string id() const = 0;
  virtual bool ready() = 0;
  virtual block::capacity capacity(bool _force = false) = 0;
  virtual std::string wwn(bool _force = false) = 0;
  virtual bool read(io_byte_units& _io_byte_units) = 0;
  virtual bool write(io_byte_units& _io_byte_units) = 0;

  bool read(io_byte_unit& _io_byte_unit);
  bool write(io_byte_unit& _io_byte_unit);

  /*
  struct file
  {
    int fd;
    int mode;

    file() : fd(-1), mode(0) {}
    bool is_open() const { return (fd >= 0); }
    void set(int _fd, int _mode) { fd = _fd; mode = _mode; }
    void close();

    bool is_char_mode() const;
    bool is_block_mode() const;
    bool is_socket_mode() const;

    bool set_blocking(bool _enable);
    bool is_blocking() const;
    bool is_non_blocking() const;
  };
  */
  //! Get file descriptor
  const int& fd() const { return m_fd1; }
  //! Get device mode
  const int& mode() const { return m_mode; }
  //! Get exception
  const sid::exception& exception() const { return m_ex; }

  bool is_char_device() const;
  bool is_block_device() const;
  bool is_socket_device() const;

protected:
  //! Set file descriptor
  int fd(int _fd) { int old = m_fd1; m_fd1 = _fd; return old; }
  //! Set device mode
  int mode(int _mode) { int old = m_mode; m_mode = _mode; return old; }
  //! Set exception
  const sid::exception& exception(const sid::exception& _ex) { m_ex = _ex; return m_ex; }
  const sid::exception& exception(const std::string& _msg) { m_ex = sid::exception(_msg); return m_ex; }
  const sid::exception& exception(int _code, const std::string& _msg) { m_ex = sid::exception(_code, _msg); return m_ex; }

private:
  int            m_fd1;
  int            m_mode;
  sid::exception m_ex;

protected:
  device();
};

} // namespace block
} // namespace sid

#endif // _SID_BLOCK_DEVICE_H_

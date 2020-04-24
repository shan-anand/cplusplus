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
  virtual device_ptr create() const = 0;
};
  
/**
 * @struct device
 * @brief block device interface definition
 */
class device : public smart_ref
{
public:
  device();
  static device_ptr create(device_info* _deviceInfo);

  device_ptr to_block_device_ptr() const { return dynamic_cast<device*>(const_cast<device*>(this)); }

  //! Virtual functions to be overwritten in the derived classes
  virtual device_type type() const = 0;
  virtual std::string id() const = 0;
  virtual block::capacity capacity(bool _force = false) = 0;
  virtual std::string wwn(bool _force = false) = 0;
};

} // namespace block
} // namespace sid

#endif // _SID_BLOCK_DEVICE_H_

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief HTTP library implementation in C++
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
 * @file version.hpp
 * @brief Defines data type for HTTP version
 */
#ifndef _SID_HTTP_VERSION_H_
#define _SID_HTTP_VERSION_H_

#include <string>
#include "common/exception.hpp"

namespace sid {
namespace http {

//! HTTP version ID
enum class version_id : uint8_t { v10, v11 };

/**
 * @class version
 * @brief Defines HTTP version.
 */
class version
{
public:
  //! Get the version ID
  const version_id& id() const { return m_id; }

  //! Get/Set the version ID
  version_id& id() { return m_id; }

  //! Get the string name of the version
  std::string to_str() const;

  //! Get the version object using the given name.
  static version get(const std::string& name);

public:
  //! Default constructor
  version();

  //! Copy constructor
  version(const version&) = default;

  //! Constructor using version ID
  version(const version_id& _id);

  //! Clear the object so that it can be reused again
  void clear();

  //! Copy operators
  version& operator=(const version&) = default;
  version& operator=(const version_id& _id);

  //! Equality operators
  bool operator==(const version& _obj) const { return (m_id == _obj.m_id); }
  bool operator==(const version_id& _id) const { return (m_id == _id); }
  bool operator!=(const version& _obj) const { return (m_id != _obj.m_id); }
  bool operator!=(const version_id& _id) const { return (m_id != _id); }

private:
  version_id m_id; //! HTTP version ID
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_VERSION_H_

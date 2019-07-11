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
 * @file method.hpp
 * @brief Defines all input HTTP methods.
 *
 * A class that defines all the input HTTP methods like Get, Post, Put etc.
 */
#ifndef _SID_HTTP_METHOD_H_
#define _SID_HTTP_METHOD_H_

#include <string>
#include "common/convert.hpp"

namespace sid {
namespace http {

//! Types of HTTP methods
enum class method_type : uint8_t { options, get, head, post, put, delete_, patch, trace, connect, custom };

/**
 * @class method
 * @brief Definition for all input HTTP methods.
 */
class method
{
public:
  //! Get the method method_type
  const method_type& type() const { return m_type; }

  //! Get or set method method_type
  method_type& type() { return m_type; }

  //! Get the string name of the method
  std::string to_str() const;

  //! Get the method object using the given method name.
  static method get(const std::string& _name, const sid::match_case& _matchCase = sid::match_case::exact);

public:
  //! Default constructor
  method();

  //! Copy constructor
  method(const method&) = default;

  //! Constructor using method type
  method(const method_type& _type);

  //! Clear the object so that it can be reused again
  void clear();

  //! Copy operators
  method& operator=(const method&) = default;
  method& operator=(const method_type& _type);

  //! Equality operators
  bool operator==(const method& _obj) const;
  bool operator==(const method_type& _type) const;
  bool operator!=(const method& _obj) const;
  bool operator!=(const method_type& _type) const;

private: 
  method_type m_type;        //! method Type (@see method_type)
  std::string m_customName;  //! Custom name of the method if it is method_type::custom
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_METHOD_H_

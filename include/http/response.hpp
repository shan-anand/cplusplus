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
 * @file response.hpp
 * @brief Defines the HTTP response object.
 */

#pragma once

#include "version.hpp"
#include "status.hpp"
#include "method.hpp"
#include "headers.hpp"
#include "content.hpp"
#include "connection.hpp"
#include <string>

namespace sid::http {

/**
 * @class response
 * @brief Definition of HTTP response object.
 */
class response
{
public:
  //! Default constructor
  response();

  /**
   * @fn void clear();
   * @brief Clear the object so that it can be reused again
   */
  void clear();

  /**
   * @fn void set(const std::string& _input);
   * @brief Set the contents of the object using the complete HTTP response string.
   *        If there is an error a sid::exception is thrown.
   */
  void set(const std::string& _input);

  /**
   * @fn std::string to_str(bool _showContent = true) const;
   * @param _showContent [IN] Indicates whether to show the content or not. If comitted it defaults to true.
   * @brief Return the complete HTTP response as a string.
   */
  std::string to_str(bool _showContent = true) const;

  bool send(connection_ptr _conn);
  bool recv(connection_ptr _conn, const method& _requestMethod);

public:
  http::version version;    //! HTTP version in Line-1 of response
  http::status  status;     //! Status code and message in Line-1 of response
  http::headers headers;    //! List of response headers
  http::content content;    //! HTTP response payload
  std::string   error;
};

} // namespace sid::http

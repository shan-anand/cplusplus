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
 * @file request.hpp
 * @brief Defines the HTTP request object.
 */
#ifndef _SID_HTTP_REQUEST_H_
#define _SID_HTTP_REQUEST_H_

#include "method.hpp"
#include "version.hpp"
#include "headers.hpp"
#include "content.hpp"
#include "connection.hpp"
#include <string>

namespace sid {
namespace http {

/**
 * @class request
 * @brief Definition of HTTP request object.
 */
class request
{
public:
  //! Default constructor
  request();

  //! Copy constructor
  request(const request&) = default;

  //! Virtual destructor
  virtual ~request();

  //! Copy operator
  request& operator=(const request&) = default;

  /**
   * @fn void clear();
   * @brief Clear the object so that it can be reused again
   */
  void clear();

  /**
   * @fn void set(const std::string& _input);
   * @brief Set the contents of the object using the complete HTTP request string.
   *        If there is an error a sid::exception is thrown.
   */
  void set(const std::string& _input);

  /**
   * @fn std::string to_str() const;
   * @brief Return the complete HTTP request as a string.
   */
  std::string to_str() const;
  std::string to_str(bool _withContent) const;

  /**
   * @fn void set_content(const std::string& data, size_t len);
   * @brief Sets the payload of the request
   *
   * @param data Payload
   * @param len  if std::string::npos it takes the whole string, otherwise it restricts the length.
   *
   * @note This also sets the "Content-Length" field in the headers.
   */
  void set_content(const std::string& _data, size_t _len = std::string::npos);

  /**
   * @fn const http::content& content() const;
   * @brief Gets the payload of the request.
   */
  const http::content& content() const { return m_content; }
  http::content& content() { return m_content; }

  bool send(connection_ptr _conn);
  bool send(connection_ptr _conn, const std::string& _data);
  bool send(connection_ptr _conn, const void* _buffer, size_t _count);
  bool recv(connection_ptr _conn);

private:
  http::content m_content;   //! HTTP request payload

public:
  http::method  method;      //! HTTP method in Line-1 of request
  std::string   uri;         //! resource identifier in Line-1 of request
  http::version version;     //! HTTP version
  http::headers headers;     //! List of request headers
  std::string   userName;    //! Username for challenge authentication (used in www_authenticate)
  std::string   password;    //! Password for challenge authentication
  bool          content_is_file_path;
  std::string   error;
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_REQUEST_H_

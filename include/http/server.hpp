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
 * @file server.hpp
 * @brief Defines the HTTP Server command object.
 */
#ifndef _SID_HTTP_SERVER_H_
#define _SID_HTTP_SERVER_H_

#include "connection.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>
#include <functional>

namespace sid {
namespace http {

//! Forward declaration of server class
class server;

// A lambda function to process every client accepted
typedef std::function<void(connection_ptr _conn)> FNProcessCallback;

// A lambda function for redirect callback
typedef std::function<bool()> FNExitCallback;

//! A smart pointer to the server object.
using server_ptr = sid::smart_ptr<server>;

struct server_info
{
  connection_type         type;
  connection_family       family;
  ssl::client_certificate sslClientCert;  //! Used only for https connection type
  uint16_t                port;
};
  
/**
 * @class server
 * @brief Definition of HTTP server object.
 */
class server : public sid::smart_ref
{
public:
  /**
   * @fn server_ptr create();
   * @brief Creates an empty server object. In case of error it throws a sid::exception.
   *
   * @return Smart pointer to the server object. It is guaranteed not to return a null pointer.
   */
  static server_ptr create(const connection_type& _type, const connection_family& _family = connection_family::none);
  static server_ptr create(const ssl::client_certificate& _sslClientCert, const connection_family& _family = connection_family::none);

  //! Returns the last exception object
  const sid::exception& exception() const { return m_exception; }
  //! Used for setting the last exception
  sid::exception& exception() { return m_exception; }

  /**
   * @fn bool run(uint16_t _port, FNProcessCallback& _fnProcessCallback, FNExitCallback& _fnExitCallback)
   * @brief Used for sending a request and receiving a response from the server.
   *
   * @param _port [in] Port on which to run the server. If it is zero the default values are 80 for http and 443 for https
   * @param _fnExitCallback [in] Callback function called to determine whether to exit the server loop or not
   *
   * @return true if exchange was successful, false otherwise.
   *         exception() will contain the last exception object in case of failure.
   */
  bool run(uint16_t _port, FNProcessCallback& _fnProcessCallback, FNExitCallback& _fnExitCallback);

  void stop();

  bool is_running() const { return m_isRunning; }
  uint32_t port() const { return m_port; }

private:
  //! Default constructor
  server();
  server(const server&) = delete;
  server& operator=(const server&) = delete;

  static server_ptr p_create(const connection_type& _type, const ssl::client_certificate& _sslClientCert, const connection_family& _family);

public:
  connection_type         m_type;
  connection_family       m_family;
  ssl::client_certificate m_sslClientCert;  //! Used only for https connection type
  uint16_t                m_port;
  int                     m_socket;
  bool                    m_isRunning;
  bool                    m_exitLoop;
  sid::exception          m_exception; //! Last exception
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_SERVER_H_

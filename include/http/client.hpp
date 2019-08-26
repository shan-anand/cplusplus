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
 * @file client.hpp
 * @brief Defines the HTTP Client command object.
 */
#ifndef _SID_HTTP_CLIENT_H_
#define _SID_HTTP_CLIENT_H_

#include "connection.hpp"
#include "request.hpp"
#include "response.hpp"
#include <string>
#include <functional>

namespace sid {
namespace http {

// A lambda function for redirect callback
typedef std::function<void(request&)> FNRedirectCallback;

/**
 * @class client
 * @brief Definition of HTTP client object.
 */
class client
{
public:
  http::connection_ptr conn;      //! HTTP connection pointer
  http::request        request;   //! HTTP request object
  http::response       response;  //! HTTP response object

private:
  sid::exception m_exception; //! Last exception

public:
  //! Default constructor
  client();

  /**
   * @fn void clear();
   * @brief Clear the object so that it can be reused again
   */
  void clear();


  //! Returns the last exception object
  const sid::exception& exception() const { return m_exception; }
  //! Used for setting the last exception
  sid::exception& exception() { return m_exception; }

  /**
   * @fn bool run(bool followRedirects)
   * @brief Used for sending a request and receiving a response from the server.
   *        The conn and request objects must be set. On receiving a response,
   *        the response object is populated.
   *
   * @param _followRedirects [in] Handle 301 and 302 redirect messages by resetting request object and re-sending the message.
   *
   * @return true if exchange was successful, false otherwise.
   *         exception() will contain the last exception object in case of failure.
   */
  bool run(bool _followRedirects = false);

  /**
   * @fn bool run(FNRedirectCallback& _fnRedirectCallbackbool _followRedirects)
   * @brief Used for sending a request and receiving a response from the server.
   *        The conn and request objects must be set. On receiving a response,
   *        the response object is populated.
   *
   * @param _fnRedirectCallback [in] Callback function called just before doing a redirect
   * @param _followRedirects [in] Handle 301 and 302 redirect messages by resetting request object and re-sending the message.
   *
   * @return true if exchange was successful, false otherwise.
   *         exception() will contain the last exception object in case of failure.
   */
  bool run(FNRedirectCallback& _fnRedirectCallback, bool _followRedirects = false);
};

} // namespace http
} // namespace sid

#endif // _SID_HTTP_CLIENT_H_

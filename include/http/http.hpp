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
 * @file http.hpp
 * @brief Master include file for applications to include.
 *
 * It is enough to include just this file for applications to use.
 */
#ifndef _SID_HTTP_H_
#define _SID_HTTP_H_

#include "method.hpp"
#include "version.hpp"
#include "headers.hpp"
#include "request.hpp"
#include "connection.hpp"
#include "response.hpp"
#include "cookies.hpp"
#include "status.hpp"
#include "url.hpp"
#include "www_authenticate.hpp"
#include "client.hpp"
#include "common.hpp"

namespace sid {
namespace http {
  bool library_init();
}}

/*
  Usage: HTTP connection
  =======================

  http::client cmd;
  std::string content = "From CTA Client";

  // [1] create HTTP connection object
  cmd.conn = http::connection::create(http::connection::http); // Use http::connection::https to create a HTTPS connection object

  // [2] open a connection to the server at default port 80.
  if ( ! cmd.conn->open("www.myserver.com") )
    throw cmd.conn->error();

  // [3] set the request object
  cmd.request.method = http::method::Post;
  cmd.request.uri = "/servers";
  cmd.request.version = http::version_id::v11;
  cmd.request.headers("Content-Type", "text/plain");
  cmd.request.headers("User-Agent", "HTTP Library");
  cmd.request.headers("Host", cmd.conn->server());
  cmd.request.headers("Content-Length", sid::to_str(content.length()));
  cmd.request.content = content;

  // [4] send the request and receive the response
  if ( ! cmd.run() )
    throw cmd.error();

  // [5] process response object that contains the response from the server
  // response will be in cmd.response
*/

#endif // _SID_HTTP_H_

//////////////////////////////////////////////////////
//
// client.cpp
//
//////////////////////////////////////////////////////

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

#include "http/http.hpp"
#include "common/convert.hpp"
#include <strings.h>

using namespace std;
using namespace sid;
using namespace sid::http;

//! Default constructor
client::client()
{
  clear();
}

void client::clear()
{
}

bool client::run(bool _followRedirects)
{
  // A lambda function for redirect callback
  http::FNRedirectCallback redirect_callback = [&](http::request& request)
    {
      // Nothing to do here
    };

  return this->run(redirect_callback, _followRedirects);
}

bool client::run(FNRedirectCallback& _redirect_callback, bool _followRedirects)
{
  bool isSuccess = false;
  bool loop = _followRedirects;
  http::connection_ptr currentConn;  //! HTTP connection pointer used for request/response

  try
  {
    // The first connection object is the same as the current connection object.
    // It will change if there is a redirect
    currentConn = this->conn;

    do
    {
      bool expecting100Continue = false;
      isSuccess = false;
      this->response.clear();

      if ( this->request.method == method_type::post || this->request.method == method_type::put )
      {
        bool isFound = false;
        std::string hval = this->request.headers.get("Expect", &isFound);
        expecting100Continue = ( isFound && ::strcasecmp(hval.c_str(), "100-continue") == 0 );
      }
      std::string data = this->request.to_str(!expecting100Continue);

      if ( http::is_verbose() )
      {
        cerr << "=================================" << endl;
        cerr << data << endl;
      }

      if ( ! this->request.send(currentConn, data) )
        throw sid::exception(this->request.error);

      if ( ! this->response.recv(currentConn, this->request.method) )
        throw sid::exception(this->response.error);

      if ( http::is_verbose() )
      {
        cerr << "=================================" << endl;
        http::content_encoding encoding = this->response.headers.content_encoding();
        cerr << this->response.to_str( (encoding == http::content_encoding::identity) ) << endl;
      }

      if ( this->response.status.code() == http::status_code::Unauthorized )
      {
        std::string wwwAuth;
        if ( this->response.headers.exists("WWW-Authenticate", &wwwAuth)
             && !this->request.headers.exists("Authorization") )
        {
          www_authenticate_list authList;
          authList.set(wwwAuth);
          if ( authList.empty() )
            throw sid::exception("Failed to perform authentication");

          std::string authStr = authList[0].get_auth_string(this->request);
          this->request.headers("Authorization", authStr);
          continue;
        }
      }
      else if ( this->response.status.code() == http::status_code::Continue )
      {
        this->response.clear();

        data = this->request.content().to_str();
        cerr << "=================================" << endl;
        cerr << "Sending actual data of size " << data.length() << endl;
        /*
        size_t totalLen = data.length();
        const char* buffer = data.data();
        const size_t BUF_SIZE = 4095;
        do
        {
          size_t bufferLen = (totalLen <= BUF_SIZE)? totalLen : BUF_SIZE;
          cerr << "     Sending chunk of size " << bufferLen << endl;
          if ( ! this->request.send(currentConn, buffer, bufferLen) )
            throw sid::exception(this->request.error);
          totalLen -= bufferLen;
          buffer += bufferLen;
        } while ( totalLen != 0 );
        */

        if ( ! this->request.send(currentConn, data) )
          throw sid::exception(this->request.error);

        if ( ! this->response.recv(currentConn, this->request.method) )
          throw sid::exception(this->response.error);

        if ( http::is_verbose() )
        {
          cerr << "=================================" << endl;
          cerr << this->response.to_str() << endl;
        }
      }

      isSuccess = (static_cast<int>(this->response.status.code()) >= 200 && static_cast<int>(this->response.status.code()) < 300 );

      if ( _followRedirects )
      {
        http::status::redirect_info redirectInfo;
        if ( this->response.status.is_redirect(&redirectInfo) )
        {
          std::string location = this->response.headers.get("Location", nullptr);
          if ( location.empty() )
            throw sid::exception("Site moved permanently, but redirect location not specified");

          http::url url;
          if ( ! url.set(location) )
            throw sid::exception(url.error);

          // create new connection pointer object
          currentConn = http::connection::create(url.type);
          if ( !currentConn->open(url.server, url.port) )
            throw sid::exception(currentConn->error());

          this->request.headers.remove_all("Cookie");
          this->request.headers.remove_all("Host");
          http::cookies s_cookies = http::cookies::get_session_cookies(url.server);

          this->request.uri = url.resource;
          this->request.headers("Host", url.server);
          s_cookies.add(this->request, currentConn);
          // The callback function will update any headers from the list
          // For example authentication signature recalulation for various cloud APIs
          _redirect_callback(this->request);

          // If the object was permanently moved, then we need update the original connection,
          // otherwise leave the original connection as it is
          if ( redirectInfo.isPermanent )
            this->conn = currentConn;
        }
        else
          loop = false;
      }
    }
    while ( loop );

    // If the status failed, set the status message
    if ( !isSuccess )
      throw sid::exception((int)this->response.status.code(), "[" + sid::to_str((int)this->response.status.code()) + "] " + this->response.status.message());
  }
  catch (const sid::exception& e)
  {
    this->exception() = e;
    isSuccess = false;
  }
  catch (...)
  {
    this->exception() = sid::exception("An unhandled exception occurred while send and receiving data");
    isSuccess = false;
  }

  return isSuccess;
}

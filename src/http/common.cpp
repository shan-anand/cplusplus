//////////////////////////////////////////////////////
//
// common.cpp
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

#include "http/common.hpp"
#include "common/convert.hpp"
#include <string.h>
#include <sstream>
#include <iomanip>

//OpenSSL includes
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

using namespace sid;

#define HTTP_DATE_FORMAT "%a, %d %b %y %T %Z"
//#define HTTP_DATE_FORMAT "%a, %d %b %y %T %z"
static const std::string urlReservedChars = " !\'();:@&+$,?%#[]/\"";

static bool gbVerbose = false;
static bool g_bInitSuccessful = false;
const SSL_METHOD* g_clientMethod; 
const SSL_METHOD* g_serverMethod; 

bool http::library_init()
{
  static bool firstTime = true;
  if ( !firstTime ) return g_bInitSuccessful;

  ::SSL_library_init();

  ::SSL_load_error_strings();
  //::ERR_load_BIO_strings();
  ::SSLeay_add_ssl_algorithms();
    
  // Set up OpenSSL to enable all algorithms, ciphers and digests
  ::OpenSSL_add_all_algorithms();
  ::OpenSSL_add_all_ciphers();
  ::OpenSSL_add_all_digests();

  g_serverMethod = ::SSLv23_server_method();
  if ( ! g_serverMethod ) 
  {
    cerr << "No serverMethod" << endl; // XXX
    return g_bInitSuccessful;
  }

  g_clientMethod = ::SSLv23_client_method();
  if ( ! g_clientMethod  ) 
  {
    cerr << "No clientMethod" << endl; // XXX
    return g_bInitSuccessful;
  }

  //SetupSSLCallbacks(); // for multithreading 

  g_bInitSuccessful = true;
  return g_bInitSuccessful;
}

void http::library_cleanup()
{
  if ( !g_bInitSuccessful ) return;

#ifdef RSA_SSL_C
  ::SSL_library_cleanup();
#endif
}

void http::set_verbose(bool _turnOn) { gbVerbose = _turnOn; }

bool http::is_verbose() { return gbVerbose; }

bool http::get_line(const std::string& _input, size_t& _pos1, std::string& _output)
{
  size_t eol = _input.find(CRLF, _pos1);
  if ( eol == std::string::npos )
    return false;
  _output = _input.substr(_pos1, eol - _pos1);
  _pos1 += (eol - _pos1) + 2;
  return true;
}

std::string http::date_to_str(const struct tm& _tm)
{
  char szDate[256] = {0};
  size_t len = ::strftime(szDate, sizeof(szDate)-1, HTTP_DATE_FORMAT, &_tm);
  szDate[len] = '\0';
  return std::string(szDate);
}

std::string http::date_to_str(const time_t& _tt)
{
  struct tm tm = {0};
  ::gmtime_r(&_tt, /*out*/ &tm);
  return date_to_str(tm);
}

bool http::date_from_str(const std::string& _input, struct tm& _tm)
{
  return ( ! ::strptime(_input.c_str(), HTTP_DATE_FORMAT, &_tm) );
}

bool http::date_from_str(const std::string& _input, time_t& _tt)
{
  struct tm tm = {0};
  if ( ! date_from_str(_input, /*out*/ tm) )
    return false;
  _tt = ::mktime(&tm);
  return true;
}

std::string http::url_encode(const std::string& _input)
{
  // define a string stream for the output
  std::ostringstream out;

  for ( size_t i = 0; i < _input.length(); i++ )
  {
    char ch = _input[i];
    // check whether the character is one of the reserved characters
    if ( urlReservedChars.find(ch) != std::string::npos )
    {
      // if it is a reserverd character, write "%<hex>"
      // where <hex> is the hexadecimal value of the character
      out << '%' << std::setbase(16) << std::setw(2) << std::setfill('0') << ((int) ch);
    }
    else
    {
      // if it is not a reserved character, just write it to the stream
      out << ch;
    }
  }
  // return the encoded string
  return out.str();
}

std::string http::url_decode(const std::string& _input)
{
  // define a string stream for the output
  std::ostringstream out;
  char ia[4] = {0};

  for ( size_t i=0; i < _input.length(); i++ )
  {
    char ch = _input[i];
    // if the character is a %, then we've encountered an encoded value
    if ( ch == '%' ) // the next 2 characters are hexadecimal
    {
      // make sure there are atleast 2 characters after %
      //   otherwise throw an exception
      if ( i+2 > _input.length() )
        throw sid::exception("Invalid input to urlDecode: " + _input);
      // get the next 2 characters (after %)
      ia[0] = _input[++i];
      ia[1] = _input[++i];
      // make sure the 2 characters are hexdecimal characters,
      //   otherwise throw an exception
      if ( !::isxdigit(ia[0]) || !::isxdigit(ia[1]) )
        throw sid::exception("Invalid hex input to urlDecode: " + _input);
      // convert the hexadecimal string to long and
      //   write it to the buffer as a character
      out << (char) ::strtol(ia, NULL, 16);
    }
    else
    {
      // write any non-% character directly to the output stream
      out << ch;
    }
  }
  // return the decoded string
  return out.str();
}

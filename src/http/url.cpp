//////////////////////////////////////////////////////
//
// url.cpp
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

using namespace std;
using namespace sid;
using namespace sid::http;

//! Default constructor
url::url()
{
  clear();
}

void url::clear()
{
  type = connection_type::http;
  server.clear();
  port = 0;
  resource.clear();
  error.clear();
}

bool url::set(const std::string& _csUrl)
{
  bool bStatus = false;

  try
  {
    this->clear();

    size_t p1, p2;

    p1 = _csUrl.find("://");
    if ( p1 == std::string::npos )
      throw std::string("Invalid URL format");
    std::string value = _csUrl.substr(0, p1);
    if ( value == "http" )
      this->type = connection_type::http;
    else if ( value == "https" )
      this->type = connection_type::https;
    else if ( value.empty() )
      throw std::string("Protocol cannot be empty");
    else
      throw std::string("Invalid protocol: ") + value;


    p1+=3; // For "://"
    p2 = _csUrl.find_first_of(":/?", p1);
    if ( p2 == std::string::npos )
    {
      this->server = _csUrl.substr(p1);
      if ( this->server.empty() )
        throw std::string("Invalid URL format: No server name");
    }
    else
    {
      this->server = _csUrl.substr(p1, p2-p1);
      if ( this->server.empty() )
        throw std::string("Invalid URL format: No server name");
      if ( _csUrl[p2] == '/' || _csUrl[p2] == '?' )
      {
        if ( _csUrl[p2] == '?' ) this->resource = "/";
        this->resource += _csUrl.substr(p2);
      }
      else // if :
      {
        p1 = p2+1;
        p2 = _csUrl.find_first_of("/?", p1);
        std::string csError;
        if ( p2 == std::string::npos )
          value = _csUrl.substr(p1);
        else
        {
          value = _csUrl.substr(p1, p2-p1);
          if ( _csUrl[p2] == '?' ) this->resource = "/";
          this->resource += _csUrl.substr(p2);
        }
        if ( value.empty() )
          throw std::string("Invalid URL format: Port number is empty");
        if ( ! sid::to_num(value, /*out*/ this->port, &csError) )
          throw std::string("Invalid URL format: Port number - ") + csError;
      }
    }
    if ( this->resource.empty() )
      this->resource = "/";

    bStatus = true;
  }
  catch (const std::string& csErr)
  {
    this->error = csErr;
  }
  catch (...)
  {
    this->error = "Unable to parse URL due to unhandled exception";
  }
  return bStatus;
}

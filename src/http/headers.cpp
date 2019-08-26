//////////////////////////////////////////////////////
//
// headers.cpp
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

#include "http/headers.hpp"
#include "http/common.hpp"
#include "common/convert.hpp"
#include <sstream>
#include <strings.h>

using namespace std;
using namespace sid;
using namespace sid::http;

///////////////////////////////////////////////////////////////////////////
//
// header class implementation
//
///////////////////////////////////////////////////////////////////////////
header::header()
{
}

header::header(const std::string& _key, const std::string& _value)
{
  this->key = _key;
  this->value = _value;
}

std::string header::to_str() const
{
  return this->key + ": " + this->value;
}

/*static*/
http::header header::get(const std::string& _input)
{
  // data should not include CRLF
  http::header header;
  size_t pos = _input.find(':');

  if ( pos == std::string::npos )
    throw sid::exception("Invalid header format");

  header.key = _input.substr(0, pos);
  pos++;

  while ( _input[pos] == ' ' ) pos++;
  header.value = _input.substr(pos);

  return header;
}

///////////////////////////////////////////////////////////////////////////
//
// headers class implementation
//
///////////////////////////////////////////////////////////////////////////
headers::headers()
{
}

http::header& headers::operator()(const std::string& _key, const std::string& _value, const header_action& _action)
{
  http::header header(_key, _value);
  return (*this)(header, _action);
}

http::header& headers::operator()(const header& _header, const header_action& _action)
{
  headers::iterator it = find(_header.key);
  if ( it != this->end() )
  {
    if ( _action == header_action::replace )
    {
      it->key = _header.key;
      it->value = _header.value;
    }
    return *it;
  }
  else
  {
    this->push_back(_header);
    return this->back();
  }
}

http::header& headers::add(const std::string& _data)
{
  this->push_back(header::get(_data));
  return this->back();
}

http::header& headers::add(const std::string& _key, const std::string& _value)
{
  http::header header(_key, _value);
  this->push_back(header);
  return this->back();
}

void headers::add(const http::headers& _headers, const header_action& _action)
{
  for ( const http::header& header : _headers )
  {
    headers::iterator it = find(header.key);
    if ( it == this->end() )
      this->push_back(header);
    else if ( _action == header_action::replace )
    {
      it->key = header.key;
      it->value = header.value;
    }
  }
}

bool headers::exists(const std::string& _key, std::string* _pValue) const
{
  bool isFound = false;

  headers::const_iterator it = this->find(_key);
  if ( it != this->end() )
  {
    isFound = true;
    if ( _pValue ) *_pValue = it->value;
  }
  return isFound;
}

std::string headers::get(const std::string& _key, bool* _pisFound) const
{
  std::string value;
  bool isFound = exists(_key, &value);
  if ( _pisFound ) *_pisFound = isFound;
  return value;
}

std::vector<std::string> headers::get_all(const std::string& _key) const
{
  std::vector<std::string> values;

  for ( const http::header& header : *this )
  {
    if ( strcasecmp(header.key.c_str(), _key.c_str()) == 0 )
      values.push_back(header.value);
  }

  return values;
}

size_t headers::remove_all(const std::string& _key)
{
  size_t count = 0;
  headers::iterator it;
  while ( (it = this->find(_key)) != this->end() )
  {
    this->erase(it);
    ++count;
  }
  return count;
}

std::string headers::to_str() const
{
  std::ostringstream out;
  for ( size_t i = 0; i < this->size(); i++ )
  {
    const http::header& header = (*this)[i];
    out << header.to_str() << CRLF;
  }
  return out.str();
}

headers::iterator headers::find(const std::string& _key)
{
  for ( headers::iterator it = this->begin(); it != this->end(); it++ )
  {
    if ( strcasecmp(it->key.c_str(), _key.c_str()) == 0 )
      return it;
  }
  return this->end();
}

headers::const_iterator headers::find(const std::string& _key) const
{
  for ( headers::const_iterator it = this->begin(); it != this->end(); it++ )
  {
    if ( strcasecmp(it->key.c_str(), _key.c_str()) == 0 )
      return it;
  }
  return this->end();
}

uint64_t headers::content_length(bool* _pisFound/* = nullptr*/) const
{
  uint64_t contentLength = 0;
  std::string value;
  bool isFound = this->exists("Content-Length", &value);
  if ( _pisFound ) *_pisFound = isFound;
  if ( isFound )
    sid::to_num(value, /*out*/ contentLength);
  return contentLength;
}

http::content_encoding headers::content_encoding(bool* _pisFound/* = nullptr*/) const
{
  http::content_encoding encoding = http::content_encoding::identity;
  std::string value;
  bool isFound = this->exists("Content-Encoding", &value);
  if ( _pisFound ) *_pisFound = isFound;
  if ( isFound )
  {
    std::vector<std::string> encodingVec;
    if ( 0 < sid::split(/*out*/ encodingVec, value, ',', SPLIT_TRIM_SKIP_EMPTY) )
    {
      std::string enc = encodingVec[0];
      if ( enc == "gzip" || enc == "x-gzip" ) // x-gzip as per http/1.1 recommendation
        encoding = http::content_encoding::gzip;
      else if ( enc == "compress" )
        encoding = http::content_encoding::compress;
      else if ( enc == "deflate" )
        encoding = http::content_encoding::deflate;
      else if ( enc == "identity" )
        encoding = http::content_encoding::identity;
      else if ( enc == "br" )
        encoding = http::content_encoding::br;
    }
  }
  return encoding;
}

http::transfer_encoding headers::transfer_encoding(bool* _pisFound/* = nullptr*/) const
{
  http::transfer_encoding encoding = http::transfer_encoding::none;
  std::string value;
  bool isFound = this->exists("Transfer-Encoding", &value);
  if ( _pisFound ) *_pisFound = isFound;
  if ( isFound )
  {
    size_t tpos = value.find(':');
    if ( tpos != std::string::npos )
      value = sid::trim(value.substr(0, tpos));

    if ( strcasecmp(value.c_str(), "chunked") == 0 )
      encoding = http::transfer_encoding::chunked;
    else if ( strcasecmp(value.c_str(), "compress") == 0 )
      encoding = http::transfer_encoding::compress;
    else if ( strcasecmp(value.c_str(), "deflate") == 0 )
      encoding = http::transfer_encoding::deflate;
    else if ( strcasecmp(value.c_str(), "gzip") == 0 )
      encoding = http::transfer_encoding::gzip;
    else if ( strcasecmp(value.c_str(), "identity") == 0 )
      encoding = http::transfer_encoding::identity;
    else
      throw std::string("Invalid Transfer-Encoding enountered: ") + value;
  }
  return encoding;
}

//! Get "Connection" header
http::header_connection headers::connection(bool* _pisFound/* = nullptr*/) const
{
  http::header_connection res = http::header_connection::close;
  std::string value;
  bool isFound = this->exists("Connection", &value);
  if ( _pisFound ) *_pisFound = isFound;
  if ( isFound )
  {
    if ( strcasecmp(value.c_str(), "Close") == 0 )
      res = http::header_connection::close;
    else if ( strcasecmp(value.c_str(), "Keep-Alive") == 0 )
      res = http::header_connection::keep_alive;
  }
  return res;
}

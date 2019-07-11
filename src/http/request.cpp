//////////////////////////////////////////////////////
//
// request.cpp
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
#include <sstream>

using namespace sid;
using namespace sid::http;

//! Default constructor
request::request()
{
  clear();
}

//! Virtual destructor
request::~request()
{
}

//! Clear the object so that it can be reused again
void request::clear()
{
  // clear all the request objects
  this->method.clear();
  this->uri = "*";
  this->version.clear();
  this->headers.clear();
  this->userName.clear();
  this->password.clear();
  this->content_is_file_path = false;
  this->error.clear();
  this->m_content.clear();
}

/**
 * @fn void set_content(const std::string& _data, size_t _len);
 * @brief Sets the payload of the request
 *
 * @param _data Payload
 * @param _len  if std::string::npos it takes the whole string, otherwise it restricts the length.
 *
 * @note This also sets the "Content-Length" field in the headers.
 */
void request::set_content(const std::string& _data, size_t _len)
{
  if ( _len == std::string::npos )
    this->m_content = _data;
  else
    this->m_content = _data.substr(0, _len);

  _len = this->m_content.size();
  this->headers("Content-Length", sid::to_str(_len));
}

/**
 * @fn std::string to_str() const;
 * @brief Return the complete HTTP request as a string.
 */
std::string request::to_str() const { return to_str(true); }

std::string request::to_str(bool _withContent) const
{
  std::ostringstream out;

  out << this->method.to_str() << " "
      << this->uri << " "
      << this->version.to_str()
      << CRLF;

  out << this->headers.to_str();
  out << CRLF; // Extra CRLF to mark the start of data

  if ( _withContent )
  {
    if ( ! this->content_is_file_path )
      out << this->m_content;
    else
      out << "File: " << this->m_content;
  }

  return out.str();
}

bool request::send(connection_ptr _conn)
{
  bool bStatus = false;

  try
  {
    this->error.clear();

    if ( _conn.empty() || ! _conn->is_open() )
      throw std::string("Connection is not established");

    std::string csRequest = this->to_str();

    // set the return status to true
    bStatus = send(_conn, csRequest);
  }
  catch ( const std::string& csErr )
  {
    this->error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    this->error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

bool request::send(connection_ptr _conn, const std::string& _data)
{
  return send(_conn, _data.data(), _data.length());
}

bool request::send(connection_ptr _conn, const void* _buffer, size_t _count)
{
  bool bStatus = false;

  try
  {
    this->error.clear();

    if ( _conn.empty() || ! _conn->is_open() )
      throw sid::exception("Connection is not established");

    ssize_t written = _conn->write(_buffer, _count);
    if ( written < 0 || _count != (size_t) written)
    {
      cerr << _count << ": " << written << endl;
      throw sid::exception("Failed to write data");
    }

    // set the return status to true
    bStatus = true;
  }
  catch ( const std::string& csErr )
  {
    this->error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    this->error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

bool request::recv(connection_ptr _conn)
{
  bool bStatus = false;
  char buffer[32*1024] = {0};
  int nread = 0;

  try
  {
    this->error.clear();

    if ( _conn.empty() || ! _conn->is_open() )
      throw sid::exception("Connection is not established");

    ////////////////////////////////////////////////////
    // NOTE: BASIC IMPLEMENTATION. TO BE CHANGED LATER.
    ////////////////////////////////////////////////////
    std::string csRequest;
    while ( (nread = _conn->read(buffer, sizeof(buffer)-1)) > 0 )
      csRequest.append(buffer, nread);

    this->set(csRequest);
    ////////////////////////////////////////////////////

    // set the return status to true
    bStatus = true;
  }
  catch ( const std::string& csErr )
  {
    this->error = __func__ + std::string(": ") + csErr;
  }
  catch (...)
  {
    this->error = __func__ + std::string(": Unhandled exception occurred");
  }

  return bStatus;
}

/**
 * @fn void set(const std::string& _input);
 * @brief Set the contents of the object using the complete HTTP request string.
 *        If there is an error an exception of std::string() is thrown.
 */
void request::set(const std::string& _input)
{
  size_t pos1, pos2;
  size_t eol;
  std::string headerStr;

  try
  {
    pos1 = 0;
    eol = _input.find(CRLF, pos1);

    pos2 = _input.find(' ', pos1);
    if ( pos2 == std::string::npos || pos2 > eol )
      throw sid::exception("Invalid request from client");
    this->method = method::get(_input.substr(pos1, pos2-pos1));
    pos1 = pos2+1;

    pos2 = _input.find(' ', pos1);
    if ( pos2 == std::string::npos || pos2 > eol )
      throw sid::exception("Invalid request from client");
    this->uri = _input.substr(pos1, pos2-pos1);
    pos1 = pos2+1;

    this->version = version::get(_input.substr(pos1, eol-pos1));

    pos1 = eol + 2;
    do
    {
      pos2 = _input.find(CRLF, pos1);
      if ( pos2 == std::string::npos ) throw sid::exception("Invalid request from client");
      headerStr = _input.substr(pos1, (pos2-pos1));
      pos1 = pos2+2;
      if ( headerStr.empty() ) break;
      this->headers.add(headerStr);
    }
    while (true);
    this->m_content = _input.substr(pos1);
  }
  catch (const std::string& csErr)
  {
    throw sid::exception(csErr);
  }
  catch (...)
  {
    throw sid::exception("Unhandled exception in request::set");
  }
}

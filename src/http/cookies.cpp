//////////////////////////////////////////////////////
//
// cookies.cpp
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
#include "http/cookies.hpp"
#include <sstream>
#include <cstring>

using namespace sid;
using namespace sid::http;

static cookies_map g_cookies_map;

///////////////////////////////////////////////////////////////////////////
//
// cookie class implementation
//
///////////////////////////////////////////////////////////////////////////
void cookie::clear()
{
  entry.clear();
  is_secure = false;
  is_http_only = false;
  domain.clear();
  path.clear();
  expiration.type = cookie_expiration::none;
  time_received = 0;
}

bool cookie::equals(const std::string& _name) const
{
  return ( ::strcasecmp(_name.c_str(), this->entry.name.c_str()) == 0 );
}

bool cookie::set(const std::string& _value)
{
  clear();

  std::string temp;
  bool isEntryExtracted = false;
  for ( size_t p2 = 0, p1 = 0; p2 != std::string::npos; p1 = p2+1 )
  {
    p2 = _value.find(';', p1);
    if ( p2 == std::string::npos )
      temp = sid::trim(_value.substr(p1));
    else
      temp = sid::trim(_value.substr(p1, p2-p1));

    if ( temp.empty() ) continue;

    std::string key, val;
    size_t pos = temp.find('=');
    if ( pos == std::string::npos )
      key = temp;
    else
    {
      key = temp.substr(0, pos);
      val = temp.substr(pos+1);
    }

    if ( ! isEntryExtracted )
    {
      this->entry.name = key;
      this->entry.value = val;
      isEntryExtracted = true;
    }
    else if ( ::strcasecmp(key.c_str(), "secure") == 0 )
      this->is_secure = true;
    else if ( ::strcasecmp(key.c_str(), "httponly") == 0 )
      this->is_http_only = true;
    else if ( ::strcasecmp(key.c_str(), "domain") == 0 )
      this->domain = val;
    else if ( ::strcasecmp(key.c_str(), "path") == 0 )
      this->path = val;
    else if ( ::strcasecmp(key.c_str(), "expires") == 0 )
    {
      this->expiration.type = cookie_expiration::expire;
      if ( ! http::date_from_str(val, expiration.time) )
        this->expiration.time = 0;
    }
  }

  // set the current time
  this->time_received = ::time(nullptr);
  return true;
}

std::string cookie::to_str(bool _forRequest) const
{
  std::ostringstream out;

  out << this->entry.to_str();

  if ( ! _forRequest )
  {
    switch ( expiration.type )
    {
    case cookie_expiration::expire:
      out << "; Expires=" << date_to_str(this->expiration.time);
      break;
    case cookie_expiration::max_age:
      out << "; Max-Age=" << this->expiration.max_age;
      break;
    case cookie_expiration::none:
      break;
    }
    if ( ! this->domain.empty() )
      out << "; Domain=" << this->domain;
    if ( ! this->path.empty() )
      out << "; Path=" << this->path;
    if ( this->is_secure )
      out << "; Secure";
    if ( this->is_http_only )
      out << "; HttpOnly";
  }

  return out.str();
}

bool cookie::is_expired() const
{
  bool isExpired = false;

  switch ( this->expiration.type )
  {
  case cookie_expiration::expire:
    isExpired = ( ::difftime(this->expiration.time, this->time_received) <= 0 );
    break;
  case cookie_expiration::max_age:
    {
      time_t currentTime = ::time(nullptr);
      time_t expirationTime = this->time_received + this->expiration.max_age;
      isExpired = ( ::difftime(currentTime, expirationTime) <= 0 );
    }
    break;
  case cookie_expiration::none:
    break;
  }

  return isExpired;
}

///////////////////////////////////////////////////////////////////////////
//
// cookies class implementation
//
///////////////////////////////////////////////////////////////////////////
size_t cookies::add(http::response& _response) const
{
  std::string key = "Set-cookie";
  std::string value;
  size_t count = 0;

  for ( const http::cookie& cookie: *this )
  {
    value = cookie.to_str(false);
    _response.headers(key, value);
    ++count;
  }

  return count;
}

size_t cookies::add(http::request& _request, const http::connection_ptr _conn) const
{
  if ( _conn.empty() )
    throw sid::exception("Expecting the connection object to be set while generating cookie headers");

  std::string key = "cookie";
  std::string value, values;
  size_t count = 0;

  for ( const http::cookie& cookie: *this )
  {
    if ( cookie.is_secure && _conn->type() != connection_type::https )
      continue;

    if ( cookie.is_expired() )
      continue;

    if ( ! cookie.domain.empty() )
    {
      if ( ::strcasecmp(cookie.domain.c_str(), _conn->server().c_str()) != 0 )
        continue;
    }
    if ( ! cookie.path.empty() )
    {
      if ( _request.uri.length() >= cookie.path.length() &&
           ::strncasecmp(cookie.path.c_str(), _request.uri.c_str(), cookie.path.length()) != 0 )
        continue;
    }
    value = cookie.to_str(true);
    if ( ! values.empty() ) values += "; ";
    values += value;
    ++count;
  }

  if ( ! values.empty() )
    _request.headers(key, values);

  return count;
}

//! Get all the cookies from the response headers
/*static*/
http::cookies cookies::get_response_cookies(const http::headers& _headers, bool _forceGetAll)
{
  http::cookies cookies;

  for ( const http::header& header : _headers )
  {
    if ( ::strcasecmp(header.key.c_str(), "Set-cookie") == 0 )
    {
      http::cookie cookie;
      if ( cookie.set(header.value) && (_forceGetAll || !cookie.is_expired() ) )
        cookies.push_back(cookie);
    }
  }

  return cookies;
}

cookies::const_iterator cookies::find(const std::string& _name) const
{
  cookies::const_iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    if ( it->equals(_name) )
      break;
  }
  return it;
}

cookies::iterator cookies::find(const std::string& _name)
{
  cookies::iterator it;
  for ( it = this->begin(); it != this->end(); it++ )
  {
    if ( it->equals(_name) )
      break;
  }
  return it;
}

bool cookies::remove(const std::string& _name, http::cookie* _pcookie)
{
  cookies::iterator it = this->find(_name);
  if ( it == this->end() )
    return false;

  if ( _pcookie ) *_pcookie = *it;
  this->erase(it);
  return true;
}

/*static*/
cookies cookies::get_session_cookies(const std::string& _host)
{
  return g_cookies_map.get(_host);
}

/*static*/
void cookies::set_session_cookie(const std::string& _host, const http::cookie& _cookie)
{
  g_cookies_map.set(_host, _cookie);
}

/*static*/
bool cookies::remove_session_cookie(const std::string& _host, const std::string& _name, http::cookie* _pcookie)
{
  return g_cookies_map.remove(_host, _name, _pcookie);
}

/*static*/
void cookies::clear_session_cookies(const std::string& _host)
{
  g_cookies_map.clear(_host);
}


///////////////////////////////////////////////////////////////////////////
//
// cookies_map class implementation
//
///////////////////////////////////////////////////////////////////////////
cookies cookies_map::get(const std::string& _host) const
{
  http::cookies cookies;

  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  cookies_map::const_iterator it = this->find(_host);
  if ( it != this->end() )
    cookies = it->second;

  return cookies;
}

void cookies_map::set(const std::string& _host, const http::cookie& _cookie)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  http::cookies& cookies = (*this)[_host];
  cookies::iterator it = cookies.find(_cookie.entry.name);
  if ( it != cookies.end() )
    *it = _cookie;
  else
    cookies.push_back(_cookie);
}

bool cookies_map::remove(const std::string& _host, const std::string& _name, http::cookie* _pcookie)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  http::cookies& cookies = (*this)[_host];
  return cookies.remove(_name, _pcookie);
}

void cookies_map::clear(const std::string& _host)
{
  std::lock_guard<std::recursive_mutex> lock(m_mutex);
  cookies_map::const_iterator it = this->find(_host);
  if ( it != this->end() )
    this->erase(it);
}

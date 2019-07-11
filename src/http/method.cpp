//////////////////////////////////////////////////////
//
// method.cpp
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

#include "http/method.hpp"
#include <strings.h>

using namespace sid;
using namespace sid::http;

struct MapMethod { method_type type; std::string name; };

//! HTTP methods map
static MapMethod gMapMethod[] =
  {
    { method_type::options, "OPTIONS" },
    { method_type::get,     "GET" },
    { method_type::post,    "POST" },
    { method_type::put,     "PUT" },
    { method_type::head,    "HEAD" },
    { method_type::delete_, "DELETE" },
    { method_type::patch,   "PATCH" },
    { method_type::trace,   "TRACE" },
    { method_type::connect, "CONNECT" },
    { method_type::custom,  "" }
  };


//! Default constructor
method::method()
{
  clear();
}

//! Constructor using method type
method::method(const method_type& _type)
{
  *this = _type;
}

void method::clear()
{
  m_type = method_type::custom; // Default it to custom method
}

//! Copy operator using method type
method& method::operator=(const method_type& _type)
{
  m_type = _type;
  return *this;
}

////////////////////////////////////////////////
//! Equality operators
bool method::operator==(const method& _obj) const
{
  return ( m_type == _obj.m_type && (m_type != method_type::custom || (m_type == method_type::custom && m_customName == _obj.m_customName)) );
}

bool method::operator!=(const method_type& _type) const
{
  return m_type != _type;
}

bool method::operator!=(const method& _obj) const
{
  return (!( m_type == _obj.m_type && (m_type != method_type::custom || (m_type == method_type::custom && m_customName == _obj.m_customName)) ));
}

bool method::operator==(const method_type& _type) const
{
  return m_type == _type;
}
////////////////////////////////////////////////

//! Get the string name of the method
std::string method::to_str() const
{
  if ( m_type == method_type::custom ) return m_customName;

  size_t cnt = sizeof(gMapMethod)/sizeof(gMapMethod[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( m_type == gMapMethod[i].type ) return gMapMethod[i].name;
  throw std::string("Invalid Method Type");
}

//! Get the method object using the given method name.
http::method method::get(const std::string& _name, const sid::match_case& _matchCase /* = sid::match_case::exact */)
{
  http::method method(method_type::custom);

  size_t cnt = sizeof(gMapMethod)/sizeof(gMapMethod[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( sid::equals(gMapMethod[i].name, _name, _matchCase) ) { method = gMapMethod[i].type; break; }
  if ( method == method_type::custom )
    method.m_customName = _name;
  return method;
}


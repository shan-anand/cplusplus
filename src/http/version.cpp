//////////////////////////////////////////////////////
//
// version.cpp
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

#include "http/version.hpp"

using namespace sid;
using namespace sid::http;

struct MapVersion { version_id id; std::string name, sname; };

static MapVersion gMapVersion[] =
  {
    { version_id::v10, "HTTP/1.0", "1.0" },
    { version_id::v11, "HTTP/1.1", "1.1" }
  };


version::version()
{
  clear();
}

version::version(const version_id& _id)
{
  *this = _id;
}

void version::clear()
{
  m_id = version_id::v11; // Default HTTP version is set to 1.1
}

version& version::operator=(const version_id& _id)
{
  m_id = _id;
  return *this;
}

std::string version::to_str() const
{
  size_t cnt = sizeof(gMapVersion)/sizeof(gMapVersion[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( m_id == gMapVersion[i].id ) return gMapVersion[i].name;
  throw sid::exception("Invalid Version ID");
}

/*static*/
http::version version::get(const std::string& _name)
{
  size_t cnt = sizeof(gMapVersion)/sizeof(gMapVersion[0]);
  for ( size_t i = 0; i < cnt; i++ )
    if ( _name == gMapVersion[i].name || _name == gMapVersion[i].sname ) return gMapVersion[i].id;
  throw sid::exception("Invalid Version: " + _name);
}

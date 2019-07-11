/**
 * @file uuid.cpp
 * @brief uuid class implementation
 *        Handling Univerally Unique Identifier
 */

/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief List of common functions in C++
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

#include "common/uuid.hpp"

using namespace sid;

//! Default constructor
uuid::uuid()
{
  clear();
}

//! Copy constructor
uuid::uuid(const uuid& _obj)
{
  *this = _obj;
}

//! Virtual destructor
uuid::~uuid()
{
}

//! Clears the object and makes it empty
void uuid::clear()
{
  uuid_clear(m_data);
}

//! Checks whether the object is empty or not
bool uuid::empty() const
{
  return uuid_is_null(m_data);
}

//! Checks for equality of 2 objects
bool uuid::operator ==(const uuid& _obj) const
{
  return (uuid_compare(m_data, _obj.m_data) == 0);
}

//! Checks for inequality of 2 objects
bool uuid::operator !=(const uuid& _obj) const
{
  return (uuid_compare(m_data, _obj.m_data) != 0);
}

//! Copy operator. Copies obj to the current object
uuid& uuid::operator =(const uuid& _obj)
{
  uuid_copy(m_data, _obj.m_data);
  return *this;
}

//! Generates a new UUID
bool uuid::generate()
{
  return generate(uuid::type::def);
}

//! Generates a new UUID according to the specified type
bool uuid::generate(const uuid::type& _eType)
{
  // clear the current object before we generate a new UUID
  clear();

  // Generate a new UUID according to the specified type
  switch ( _eType )
  {
  case uuid::type::def:
    uuid_generate(m_data);
    break;
  case uuid::type::time:
    uuid_generate_time(m_data);
    break;
  case uuid::type::random:
    uuid_generate_random(m_data);
    break;
  }

  // If the generated UUID is empty or if there is an error, return false
  return !empty();
}

//! Parses the given string and sets the UUID
bool uuid::parse(const std::string& csuuid)
{
  clear();
  return uuid_parse(csuuid.c_str(), m_data) == 0;
}

//! Converts the object to the string according to the case specified in the current locale
std::string uuid::to_str() const
{
  return to_str(uuid::case_type::def);
}

//! Converts the object to the string according to the case specified as parameter
std::string uuid::to_str(const uuid::case_type& _eCaseType) const
{
  char szuuid[64] = {0};

  switch ( _eCaseType )
  {
  case uuid::case_type::def:
    uuid_unparse(m_data, szuuid);
    break;
  case uuid::case_type::lower:
    uuid_unparse_lower(m_data, szuuid);
    break;
  case uuid::case_type::upper:
    uuid_unparse_upper(m_data, szuuid);
    break;
  }

  return std::string(szuuid);
}


/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@file json.cpp
@brief Json handling using c++
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
 * @file  json.cpp
 * @brief Implementation of json parser and handler
 */
#include <common/json.hpp>
#include <common/convert.hpp>
#include <stack>

using namespace sid;

#define REMOVE_LEADING_SPACES(value, i)  for (; isspace(value[i]) && i < value.length(); i++ );

namespace sid {
namespace json {

/**
 * @struct parser
 * @brief Local json parser
 */
struct parser
{
  std::string m_value;  //! Json string value
  size_t      m_i;      //! Current location of the parser
  json::value m_jroot;  //! json::value object
  std::stack<sid::json::element> m_stack;
  //! parse object
  json::value parse(const std::string& _value);

  void parse_object();
  void parse_array();
  void parse_string();
  void parse_number();
  void parse_value();
};

} // namespace json
} // namespace sid

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::value
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/*static*/
json::value json::value::get(const std::string& _value)
{
  json::parser jparser;
  return jparser.parse(_value);
}

json::value::value(const json::element _type)
{
  m_type = m_data.init(_type);
}

json::value::value(const value& _obj)
{
  m_type = m_data.init(_obj.m_data, _obj.m_type);
}

json::value::value(const int64_t _val)
{
  m_type = m_data.init(_val);
}

json::value::value(const uint64_t _val)
{
  m_type = m_data.init(_val);
}

json::value::value(const bool _val)
{
  m_type = m_data.init(_val);
}

json::value::value(const std::string& _val)
{
  m_type = m_data.init(_val);
}

json::value::value(const char* _val)
{
  m_type = m_data.init(std::string(_val));
}

json::value::value(const int _val)
{
  m_type = m_data.init(static_cast<int64_t>(_val));
}

json::value::~value()
{
  clear();
}

void json::value::clear()
{
  m_type = m_data.clear(m_type);
}

json::value& json::value::operator=(const value& _obj)
{
  this->clear();
  m_type = m_data.init(_obj.m_data, _obj.m_type);
  return *this;
}

json::value& json::value::operator=(const int64_t _val)
{
  this->clear();
  m_type = m_data.init(_val);
  return *this;
}

json::value& json::value::operator=(const uint64_t _val)
{
  this->clear();
  m_type = m_data.init(_val);
  return *this;
}

json::value& json::value::operator=(const bool _val)
{
  this->clear();
  m_type = m_data.init(_val);
  return *this;
}

json::value& json::value::operator=(const std::string& _val)
{
  this->clear();
  m_type = m_data.init(_val);
  return *this;
}

json::value& json::value::operator=(const char* _val)
{
  this->clear();
  m_type = m_data.init(std::string(_val));
  return *this;
}

json::value& json::value::operator=(const int _val)
{
  return operator=(static_cast<int64_t>(_val));
}

bool json::value::has_key(const std::string& _key) const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  for ( const auto& entry : m_data._map )
    if ( entry.first == _key ) return true;
  return false;
}

std::vector<std::string> json::value::get_keys() const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  std::vector<std::string> keys;
  for ( const auto& entry : m_data._map )
    keys.push_back(entry.first);
  return keys;
}

size_t json::value::size() const
{
  if ( is_array() )
    return m_data._arr.size();
  else if ( is_object() )
    return m_data._map.size();
  throw sid::exception(__func__ + std::string("() can be used only for array and object types"));
}

int64_t json::value::get_int64() const
{
  if ( is_number() )
    return m_data._i64;
  throw sid::exception(__func__ + std::string("() can be used only for number type"));
}

uint64_t json::value::get_uint64() const
{
  if ( is_number() )
    return m_data._u64;
  throw sid::exception(__func__ + std::string("() can be used only for number type"));
}

std::string json::value::get_str() const
{
  if ( is_string() )
    return m_data._str;
  throw sid::exception(__func__ + std::string("() can be used only for string type"));
}

bool json::value::get_bool() const
{
  if ( is_bool() )
    return m_data._bval;
  throw sid::exception(__func__ + std::string("() can be used only for boolean type"));
}

std::string json::value::as_str() const
{
  if ( is_string() )
    return m_data._str;
  else if ( is_bool() )
    return sid::to_str(m_data._bval);
  else if ( is_signed() )
    return sid::to_str(m_data._i64);
  else if ( is_unsigned() )
    return sid::to_str(m_data._u64);
  throw sid::exception(__func__ + std::string("() can be used only for string, number or boolean types"));
}

const json::value& json::value::operator[](size_t _index) const
{
  if ( ! is_array() )
    throw sid::exception(__func__ + std::string("() can be used only for array type"));
  if ( _index >= m_data._arr.size() )
    throw sid::exception(__func__ + std::string("() index out of range"));
  return m_data._arr[_index];
}

const json::value& json::value::operator[](const std::string& _key) const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  for ( const auto& entry : m_data._map )
    if ( entry.first == _key ) return entry.second;
  throw sid::exception(__func__ + std::string("() key not found"));
}

json::value& json::value::operator[](const std::string& _key)
{
  if ( ! is_object() )
  {
    this->clear();
    m_type = m_data.init(json::element::object);
  }
  return m_data._map[_key];
}

void json::value::append(const value& _obj)
{
  if ( ! is_array() )
  {
    this->clear();
    m_type = m_data.init(json::element::array);
  }
  m_data._arr.push_back(_obj);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::value::union_data
//
///////////////////////////////////////////////////////////////////////////////////////////////////
json::value::union_data::union_data(const json::element _type)
{
  init(_type);
}

json::value::union_data::union_data(const union_data& _obj, const json::element _type)
{
  init(_obj, _type);
}

json::element json::value::union_data::clear(const json::element _type)
{
  switch ( _type )
  {
  case json::element::string: _str.~string(); break;
  case json::element::array:  _arr.~array(); break;
  case json::element::object: _map.~object(); break;
  default: break;
  }
  return json::element::null;
}

json::element json::value::union_data::init(const json::element _type/* = json::element::null*/)
{
  switch ( _type )
  {
  case json::element::null:            break;
  case json::element::string:          new (&_str) string; break;
  case json::element::_signed:   _i64 = 0; break;
  case json::element::_unsigned: _u64 = 0; break;
  case json::element::boolean:         _bval = false; break;
  case json::element::array:           new (&_arr) array; break;
  case json::element::object:          new (&_map) object; break;
  }
  return _type;
}

json::element json::value::union_data::init(const union_data& _obj, const json::element _type/* = json::element::null*/)
{
  switch ( _type )
  {
  case json::element::null:            break;
  case json::element::string:          init(_obj._str); break;
  case json::element::_signed:   init(_obj._i64);  break;
  case json::element::_unsigned: init(_obj._u64);  break;
  case json::element::boolean:         init(_obj._bval); break;
  case json::element::array:           init(_obj._arr); break;
  case json::element::object:          init(_obj._map); break;
  }
  return _type;
}

json::element json::value::union_data::init(const int64_t _val)
{
  _i64 = _val;
  return json::element::_signed;
}

json::element json::value::union_data::init(const uint64_t _val)
{
  _u64 = _val;
  return json::element::_unsigned;
}

json::element json::value::union_data::init(const bool _val)
{
  _bval = _val;
  return json::element::boolean;
}

json::element json::value::union_data::init(const std::string& _val)
{
  new (&_str) string(_val);
  return json::element::string;
}

json::element json::value::union_data::init(const array& _val)
{
  new (&_arr) array(_val);
  return json::element::array;
}

json::element json::value::union_data::init(const object& _val)
{
  new (&_map) object(_val);
  return json::element::object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::parser
//
///////////////////////////////////////////////////////////////////////////////////////////////////
json::value json::parser::parse(const std::string& _value)
{
  m_value = _value;
  m_i = 0;

  REMOVE_LEADING_SPACES(m_value, m_i);
  char ch = m_value[m_i];
  if ( ch == '{' )
    parse_object();
  else if ( ch == '[' )
    parse_array();
  else
    throw std::string("Invalid character [") + ch + "] at position. Expecting { or [";
  return m_jroot;
}

void json::parser::parse_object()
{
  char ch = 0;
  while ( true )
  {
    m_i++;
    // "string" : value
    REMOVE_LEADING_SPACES(m_value, m_i);
    parse_string();
    REMOVE_LEADING_SPACES(m_value, m_i);
    if ( m_value[m_i] != ':' )
      throw sid::exception("Expected : at position i");
    REMOVE_LEADING_SPACES(m_value, m_i);
    parse_value();
    ch = m_value[m_i];
    // Can have a ,
    // Must end with }
    if ( ch == '}' ) break;
    if ( ch != ',' )
      throw sid::exception("Expected , or } at position i");
  }
}

void json::parser::parse_array()
{
  char ch = 0;
  while ( true )
  {
    m_i++;
    // value
    REMOVE_LEADING_SPACES(m_value, m_i);
    parse_value();
    ch = m_value[m_i];
    // Can have a ,
    // Must end with ]
    if ( ch == ']' ) break;
    if ( ch != ',' )
      throw sid::exception("Expected , or ] at position i");
  }
}

void json::parser::parse_string()
{
  if ( m_value[m_i] != '\"' )
    throw sid::exception("Expected \" at position i");

  char ch = 0;
  size_t i = m_i;
  while ( (ch = m_value[++m_i]) != '\"' )
  {
    if ( ch == '\0' ) throw sid::exception("Missing \" for string starting at i");
  }
  std::string s = m_value.substr(m_i, i-m_i-1);
  cout << s << endl;
}

void json::parser::parse_value()
{
  char ch = m_value[m_i];
  if ( ch == '{' )
    parse_object();
  else if ( ch == '[' )
    parse_array();
  else if ( ch == '\"' )
    parse_string();
  else if ( ch == '-' || ::isdigit(ch) )
    parse_number();
  else if ( ch == '\0' )
    throw sid::exception("Unexpected end of data while expecting a value");
  else
  {
    size_t pos = m_value.find_first_of(",}] \t", m_i);
    if ( pos == std::string::npos )
      m_i = m_value.length();
    else if ( m_i == pos )
      throw std::string("Expected value not found at position i");
    std::string val = m_value.substr(m_i, pos-m_i);
    if ( val == "null" )
      ;
    else if ( val == "true" || val == "false" )
      ;
    else
      throw std::string("Invalid value type. Did you miss enclosing in \"\"");
  }
  REMOVE_LEADING_SPACES(m_value, m_i);
}

void json::parser::parse_number()
{
}

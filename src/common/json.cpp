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
#include <common/optional.hpp>
#include <stack>

using namespace sid;

#define REMOVE_LEADING_SPACES(value, i)  for (; ::isspace(value[i]) && i < value.length(); i++ );

namespace sid {
namespace json {

/**
 * @struct parser
 * @brief Local json parser
 */
struct parser
{
  bool        m_strict; //! Use strict or flexible parsing
  std::string m_value;  //! Json string value
  size_t      m_i;      //! Current location of the parser
  json::value m_jroot;  //! json::value object
  std::stack<sid::json::element> m_containerStack;
  //! parse object
  json::value parse(const std::string& _value);

  json::value parse_object();
  json::value parse_array();
  json::value parse_string();
  json::value parse_number();
  json::value parse_value();
};

} // namespace json
} // namespace sid

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::value
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn json::value get(const std::string& _value, bool _strict = true);
 * @brief Convert the given json string to json object
 *
 * @param _value [in] Input json string
 * @param _strict [in] Use strict parsing (default)
 *                     If set to false, it relaxes the parsing logic for boolean and null types by accepting
 *                     True, TRUE, False, FALSE, Null, NULL (in addition to true, false, null)
 */
/*static*/
json::value json::value::get(const std::string& _value, bool _strict/* = true*/)
{
  json::parser jparser;
  jparser.m_strict = _strict;
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

json::value::value(const double _val)
{
  m_type = m_data.init(static_cast<long double>(_val));
}

json::value::value(const long double _val)
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
  if ( _val != nullptr )
    m_type = m_data.init(std::string(_val));
  else
    m_type = m_data.init(json::element::null);
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

json::value& json::value::operator=(const double _val)
{
  return operator=(static_cast<long double>(_val));
}

json::value& json::value::operator=(const long double _val)
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
  if ( _val != nullptr )
    m_type = m_data.init(std::string(_val));
  else
    m_type = m_data.init(json::element::null);
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
  for ( const auto& entry : *m_data._map )
    if ( entry.first == _key ) return true;
  return false;
}

std::vector<std::string> json::value::get_keys() const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  std::vector<std::string> keys;
  for ( const auto& entry : *m_data._map )
    keys.push_back(entry.first);
  return keys;
}

size_t json::value::size() const
{
  if ( is_array() )
    return m_data._arr.size();
  else if ( is_object() )
    return m_data._map->size();
  throw sid::exception(__func__ + std::string("() can be used only for array and object types"));
}

int64_t json::value::get_int64() const
{
  if ( is_num() )
    return m_data._i64;
  throw sid::exception(__func__ + std::string("() can be used only for number type"));
}

uint64_t json::value::get_uint64() const
{
  if ( is_num() )
    return m_data._u64;
  throw sid::exception(__func__ + std::string("() can be used only for number type"));
}

long double json::value::get_double() const
{
  if ( is_num() )
    return m_data._dbl;
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
  else if ( is_double() )
    return sid::to_str(m_data._dbl);
  throw sid::exception(__func__ + std::string("() can be used only for string, number or boolean types"));
}

std::string json::value::to_str(json::format _format/* = json::format::compact*/) const
{
  if ( is_object() || is_array() )
    return private_to_str(_format, pretty_formatter(), 0);
  throw sid::exception("to_str() can be applied only on a object or array");
}

std::string json::value::to_str(const pretty_formatter& _formatter) const
{
  if ( is_object() || is_array() )
  {
    if ( ! ::isspace(_formatter.sep_char) && _formatter.sep_char != '\0' )
      throw sid::exception("to_str() formatter must be a valid space character. It cannot be \"" + std::string(1, _formatter.sep_char) + "\"");
    return private_to_str(json::format::pretty, _formatter, 0);
  }
  throw sid::exception("to_str() can be applied only on a object or array");
}

std::string json::value::private_to_str(json::format _format, const pretty_formatter& _formatter, uint32_t _level) const
{
  auto escape_string =[&](const std::string& _input)->std::string
    {
      std::string _output;
      for ( size_t i = 0; i < _input.length(); i++ )
      {
	char ch = _input[i];
	switch ( ch )
	{
	  //case '/':
	  //_output += '\\';
	  //_output += ch;
	  //break;
	case '\b':
	  _output += "\\b";
	  break;
	case '\f':
	  _output += "\\f";
	  break;
	case '\n':
	  _output += "\\n";
	  break;
	case '\r':
	  _output += "\\r";
	  break;
	case '\t':
	  _output += "\\t";
	  break;
	case '\\':
	case '\"': // Quotation mark
	  _output += '\\';
	  _output += + ch;
	  break;
	  /*
	case '\u':
	  _output += "\\u";
	  break;
	  */
	default:
	  _output += ch;
	  break;
	}
      }
      return _output;
    };

  std::string padding, final_padding;
  if ( _format == json::format::pretty && _formatter.sep_char != '\0' )
  {
    padding = std::string((_level+1) * _formatter.sep_count, _formatter.sep_char);
    final_padding = std::string(_level * _formatter.sep_count, _formatter.sep_char);
  }
  std::ostringstream out;
  if ( is_object() )
  {
    out << "{";
    bool isFirst = true;
    for ( const auto& entry : *m_data._map )
    {
      if ( ! isFirst )
      {
	out << ",";
	if ( _format == json::format::pretty )
	  out << endl;
      }
      else if ( _format == json::format::pretty )
	out << endl;
      isFirst = false;
      if ( _format == json::format::compact )
	out << "\"" << entry.first << "\":" << entry.second.private_to_str(_format, _formatter, _level+1);
      else if ( _format == json::format::pretty )
	out << padding << "\"" << entry.first << "\" : " << entry.second.private_to_str(_format, _formatter, _level+1);
    }
    if ( !isFirst && _format == json::format::pretty )
      out << endl << final_padding;
    out << "}";
  }
  else if ( is_array() )
  {
    out << "[";
    bool isFirst = true;
    for ( size_t i = 0; i < this->size(); i++ )
    {
      if ( ! isFirst )
      {
	out << ",";
	if ( _format == json::format::pretty )
	  out << endl;
      }
      else if ( _format == json::format::pretty )
	out << endl;
      isFirst = false;
      if ( _format == json::format::pretty )
	out << padding;
      out << (*this)[i].private_to_str(_format, _formatter, _level+1);
    }
    if ( !isFirst && _format == json::format::pretty )
      out << endl << final_padding;
    out << "]";
  }
  else if ( is_string() )
    out << "\"" << escape_string(m_data._str) << "\"";
  else if ( is_null() )
    out << "null";
  else
    out << this->as_str();

  return out.str();
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
  for ( const auto& entry : *m_data._map )
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
  return (*m_data._map)[_key];
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

json::value::union_data::~union_data()
{
}

json::element json::value::union_data::clear(const json::element _type)
{
  switch ( _type )
  {
  case json::element::string: _str.~string(); break;
  case json::element::array:  _arr.~array(); break;
  case json::element::object: delete _map; _map = nullptr; break;
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
  case json::element::_double:   _dbl = 0; break;
  case json::element::boolean:         _bval = false; break;
  case json::element::array:           new (&_arr) array; break;
  case json::element::object:          _map = new object; break;
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
  case json::element::_double:   init(_obj._dbl);  break;
  case json::element::boolean:         init(_obj._bval); break;
  case json::element::array:           init(_obj._arr); break;
  case json::element::object:          init(*_obj._map); break;
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

json::element json::value::union_data::init(const long double _val)
{
  _dbl = _val;
  return json::element::_double;
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
  _map = new object(_val);
  return json::element::object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::parser
//
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
From http://www.json.org/
___________________________________________________________________________________________________
|                              |                                   |                              |
|  object                      |  string                           |  number                      |
|      {}                      |      ""                           |      int                     |
|      { members }             |      "chars"                      |      int frac                |
|  members                     |  chars                            |      int exp                 |
|      pair                    |      char                         |      int frac exp            |
|      pair , members          |      char chars                   |  int                         |
|  pair                        |  char                             |      digit                   |
|      string : value          |      any-Unicode-character-       |      digit1-9 digits         |
|  array                       |          except-"-or-\-or-        |      - digit                 |
|      []                      |          control-character        |      - digit1-9 digits       |
|      [ elements ]            |      \"                           |  frac                        |
|  elements                    |      \\                           |      . digits                |
|      value                   |      \/                           |  exp                         |
|      value , elements        |      \b                           |      e digits                |
|  value                       |      \f                           |  digits                      |
|      string                  |      \n                           |      digit                   |
|      number                  |      \r                           |      digit digits            |
|      object                  |      \t                           |  e                           |
|      array                   |      \u four-hex-digits           |      e                       |
|      true                    |                                   |      e+                      |
|      false                   |                                   |      e-                      |
|      null                    |                                   |      E                       |
|                              |                                   |      E+                      |
|                              |                                   |      E-                      |
|                              |                                   |                              |
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

json::value json::parser::parse(const std::string& _value)
{
  m_value = _value;
  m_i = 0;

  REMOVE_LEADING_SPACES(m_value, m_i);
  char ch = m_value[m_i];
  if ( ch == '{' )
    m_jroot = parse_object();
  else if ( ch == '[' )
    m_jroot = parse_array();
  else if ( ch != '\0' )
    throw sid::exception(std::string("Invalid character [") + ch + "] at position " + sid::to_str(m_i) + ". Expecting { or [");
  else
    throw sid::exception(std::string("End of data reached at position ") + sid::to_str(m_i) + ". Expecting { or [");
  return m_jroot;
}

json::value json::parser::parse_object()
{
  char ch = 0;
  json::value jobj = json::value(json::element::object);

  m_containerStack.push(json::element::object);
  while ( true )
  {
    ++m_i;
    // "string" : value
    REMOVE_LEADING_SPACES(m_value, m_i);
    // This is the case where there are no elements in the object (An empty object)
    if ( m_value[m_i] == '}' ) { ++m_i; break; }
    json::value jkey = parse_string();
    REMOVE_LEADING_SPACES(m_value, m_i);
    if ( m_value[m_i] != ':' )
      throw sid::exception("Expected : at position " + sid::to_str(m_i));
    m_i++;
    REMOVE_LEADING_SPACES(m_value, m_i);
    jobj[jkey.as_str()] = parse_value();
    ch = m_value[m_i];
    // Can have a ,
    // Must end with }
    if ( ch == '}' ) { ++m_i; break; }
    if ( ch != ',' )
      throw sid::exception("Encountered " + std::string(1, ch) + ". Expected , or } at position " + sid::to_str(m_i));
  }
  m_containerStack.pop();
  return jobj;
}

json::value json::parser::parse_array()
{
  char ch = 0;
  json::value jarr = json::value(json::element::array);

  m_containerStack.push(json::element::array);
  while ( true )
  {
    ++m_i;
    // value
    REMOVE_LEADING_SPACES(m_value, m_i);
    // This is the case where there are no elements in the array (An empty array)
    if ( m_value[m_i] == ']' ) { ++m_i; break; }
    jarr.append(parse_value());
    ch = m_value[m_i];
    // Can have a ,
    // Must end with ]
    if ( ch == ']' ) { ++m_i; break; }
    if ( ch != ',' )
      throw sid::exception("Expected , or ] at position " + sid::to_str(m_i));
  }
  m_containerStack.pop();
  return jarr;
}

json::value json::parser::parse_string()
{
  if ( m_value[m_i] != '\"' )
    throw sid::exception("Expected \" at position " + sid::to_str(m_i) + ", found \"" + std::string(1, m_value[m_i]) + "\"");

  auto check_hex = [&](char ch)->char
    {
      if ( ch == '\0' )
	throw sid::exception("Missing hexadecimal sequence characters at the end position " + sid::to_str(m_i));
      if ( ! ::isxdigit(ch) )
	throw sid::exception("Missing hexadecimal character at " + sid::to_str(m_i));
      return ch;
    };

  char ch = 0;
  //  size_t i = m_i+1;
  std::string out;
  while ( (ch = m_value[++m_i]) != '\"' )
  {
    if ( ch == '\0' ) throw sid::exception("Missing \" for string starting at " + sid::to_str(m_i));
    if ( ch != '\\' ) { out += ch; continue; }
    // We're encountered an escape character. Process it
    {
      ch = m_value[++m_i];
      switch ( ch )
      {
      case '/':  out += ch;   break;
      case 'b':  out += '\b'; break;
      case 'f':  out += '\f'; break;
      case 'n':  out += '\n'; break;
      case 'r':  out += '\r'; break;
      case 't':  out += '\t'; break;
      case '\\': out += ch;   break;
      case '\"': out += ch;   break;
      case 'u':
	{
	  size_t i = m_i;
	  // Must be followed by 4 hex digits
	  for ( int i = 0; i < 4; i++ )
	    check_hex(m_value[++m_i]);
	  out += m_value.substr(i, m_i-i);
	}
	break;
      case '\0':
	throw sid::exception("Missing escape sequence characters at the end position " + sid::to_str(m_i));
      default:
	throw sid::exception("Invalid escape sequence (" + std::string(1, ch) + ") for string at " + sid::to_str(m_i));
      }
    }
  }
  ++m_i;
  return out;//m_value.substr(i, m_i-i-1);
}

json::value json::parser::parse_value()
{
  json::value jval;
  char ch = m_value[m_i];
  if ( ch == '{' )
    jval = parse_object();
  else if ( ch == '[' )
    jval = parse_array();
  else if ( ch == '\"' )
    jval = parse_string();
  else if ( ch == '-' || ::isdigit(ch) )
    jval = parse_number();
  else if ( ch == '\0' )
    throw sid::exception("Unexpected end of data while expecting a value");
  else
  {
    bool is_valid = true;
    size_t pos = m_value.find_first_of(",}] \t\r\n", m_i);
    if ( pos == std::string::npos )
      m_i = m_value.length();
    else if ( m_i == pos )
      throw sid::exception("Expected value not found at position " + sid::to_str(m_i));
    std::string val = m_value.substr(m_i, pos-m_i);
    if ( val == "null" )
      ;
    else if ( val == "true" )
      jval = true;
    else if ( val == "false" )
      jval = false;
    else if ( ! m_strict )
    {
      if ( val == "Null" || val == "NULL" )
	;
      else if ( val == "True" || val == "TRUE" )
	jval = true;
      else if ( val == "False" || val == "FALSE" )
	jval = false;
      else
	is_valid = false;
    }
    else
      is_valid = false;

    if ( ! is_valid )
      throw sid::exception("Invalid value [" + val + "] at position " + sid::to_str(m_i) + ". Did you miss enclosing in \"\"?");
    m_i = pos;
  }
  REMOVE_LEADING_SPACES(m_value, m_i);
  return jval;
}

json::value json::parser::parse_number()
{
  struct number
  {
    struct s_integer
    {
      bool     negative = false;
      uint64_t digits = 0;
    };
    s_integer integer;
    bool hasFraction = false;
    bool hasExponent = false;
  };
  number num;

  size_t pos_start = m_i;
  REMOVE_LEADING_SPACES(m_value, m_i);
  if ( (num.integer.negative = (m_value[m_i] == '-')) )
    ++m_i;
  size_t pos_int = m_i;
  char ch = m_value[m_i];
  if ( ch < '0' || ch > '9' )
    throw sid::exception("Missing integer digit at position" + sid::to_str(m_i));
  if ( ch == '0' )
  {
    ch = m_value[++m_i];
    if ( ch >= '0' && ch <= '9' )
      throw sid::exception("Invalid digit (" + std::string(1, ch) + ") after first 0 at position " + sid::to_str(m_i));
    num.integer.digits = 0;
  }
  else
  {
    while ( (ch = m_value[++m_i]) >= '0' && ch <= '9'  )
      ;
    std::string errStr, intStr = m_value.substr(pos_int, m_i-pos_int);
    if ( ! sid::to_num(intStr, /*out*/ num.integer.digits, &errStr) )
      throw sid::exception("Unable to convert (" + intStr + ") to nummeric at position " + sid::to_str(pos_int) + ": " + errStr);
  }
  // Check whether it has fraction and populate accordingly
  if ( ch == '.' )
  {
    bool hasDigits = false;
    while ( (ch = m_value[++m_i]) >= '0' && ch <= '9'  )
      hasDigits = true;
    if ( !hasDigits )
      throw sid::exception("Invalid digit (" + std::string(1, ch) + ") Expected a digit for fraction at position " + sid::to_str(m_i));
    num.hasFraction = true;
  }
  // Check whether it has an exponent and populate accordingly
  if ( ch == 'e' || ch == 'E' )
  {
    ch = m_value[++m_i];
    if ( ch != '-' && ch != '+' )
      --m_i;
    bool hasDigits = false;
    while ( (ch = m_value[++m_i]) >= '0' && ch <= '9'  )
      hasDigits = true;
    if ( !hasDigits )
      throw sid::exception("Invalid digit (" + std::string(1, ch) + ") Expected a digit for exponent at position " + sid::to_str(m_i));
    num.hasExponent = true;
  }
  //size_t pos_end = m_i;
  REMOVE_LEADING_SPACES(m_value, m_i);
  ch = m_value[m_i];
  const char chContainer = (m_containerStack.top() == json::element::object)? '}' : ']' ;
  if ( ch != ',' && ch != '\0' && ch != chContainer )
    throw sid::exception("Invalid character " + std::string(1, ch) + " Expected , or " + std::string(1, chContainer) + " at position " + sid::to_str(m_i));

  if ( num.hasFraction || num.hasExponent )
  {
    std::string numStr = m_value.substr(pos_start, m_i-pos_start);
    long double dbl = sid::to_num<long double>(numStr);
    return json::value(dbl);
  }
  else if ( num.integer.negative )
  {
    int64_t i_num = static_cast<int64_t>(num.integer.digits);
    if ( i_num < 0 )
      throw sid::exception("Too big a negative number");
    i_num = -i_num;
    return json::value(i_num);
  }
  return json::value(num.integer.digits);
}

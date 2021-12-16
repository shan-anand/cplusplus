/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
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
#include <fstream>
#include <stack>
#include <iomanip>
#include <ctime>
#include <unistd.h>

using namespace sid;

//#define REMOVE_LEADING_SPACES(p)  for (; ::isspace(*p) && *p != '\0'; p++ );

namespace sid {
namespace json {

/**
 * @struct parser
 * @brief Internal json parser
 */
struct parser
{
  json::value&         m_jroot;  //! json::value output object
  json::parser_stats&  m_stats;  //! statistics object
  json::parser_control m_ctrl;   //! Parser control flags
  std::string          m_input;  //! The Json string to be parsed
  std::stack<sid::json::element> m_containerStack; //! Container stack

  // constructor
  parser(json::value& _jout, json::parser_stats& _stats) : m_jroot(_jout), m_stats(_stats) {}

  //! parse the string and convert it to json object
  bool parse(const std::string& _value);

private:
  struct line_info
  {
    const char* begin; //! Beginning of word position
    uint64_t    count; //! Current line number
  };
  //! Key value for object. It is reused in recursion.
  std::string m_key;
  const char* m_p;
  line_info   m_line;

  //! get the current location of paring in the string
  std::string loc_str(const line_info& line, const char* p) const {
    return std::string("@line:") + sid::to_str(line.count) + ", @pos:" + sid::to_str(p-line.begin+1);
  }
  std::string loc_str(const line_info& line) const { return loc_str(line, m_p); }
  std::string loc_str(const char* p) const { return loc_str(m_line, p); }
  std::string loc_str() const { return loc_str(m_line, m_p); }

  //! parse object
  void parse_object(json::value& _jobj);
  //! parse array
  void parse_array(json::value& _jarr);
  //! parse key
  void parse_key(std::string& _str);
  //! parse string
  void parse_string(std::string& _str, bool _isKey);
  void parse_string(json::value& _jstr, bool _isKey);
  //! parser number
  void parse_number(json::value& _jnum, bool bFullCheck);
  //! parse json value
  void parse_value(json::value& _jval);

  //! check for space character
  bool is_space(const char* _p);
  //! remove leading spaces and comments
  void REMOVE_LEADING_SPACES(const char*& _p);
};

} // namespace json
} // namespace sid

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::parser_stats
//
///////////////////////////////////////////////////////////////////////////////////////////////////
json::parser_stats::parser_stats()
{
  clear();
}

uint64_t gobjects_alloc = 0;

void json::parser_stats::clear()
{
  objects = 0;
  arrays = 0;
  strings = 0;
  numbers = 0;
  booleans = 0;
  nulls = 0;
  keys = 0;
  time_ms = 0;
}

std::string json::parser_stats::to_str() const
{
  std::ostringstream out;
  out << "objects.......: " << sid::get_sep(objects)
      << " (" << sid::get_sep(gobjects_alloc) << ")" << endl
      << "arrays........: " << sid::get_sep(arrays) << endl
      << "strings.......: " << sid::get_sep(strings) << endl
      << "numbers.......: " << sid::get_sep(numbers) << endl
      << "booleans......: " << sid::get_sep(booleans) << endl
      << "nulls.........: " << sid::get_sep(nulls) << endl
      << "(keys)........: " << sid::get_sep(keys) << endl
      << "(time taken)..: " << sid::get_sep(time_ms/1000)
      << "." << std::setfill('0') << std::setw(3) << (time_ms % 1000) << " seconds" << endl
    ;
  return out.str();
}

std::string sid::to_str(const json::element& _type)
{
  std::string out;
  switch ( _type )
  {
  case json::element::null:      out = "null";     break;
  case json::element::string:    out = "string";   break;
  case json::element::_signed:   out = "signed";   break;
  case json::element::_unsigned: out = "unsigned"; break;
  case json::element::_double:   out = "double";   break;
  case json::element::boolean:   out = "boolean";  break;
  case json::element::array:     out = "array";    break;
  case json::element::object:    out = "object";   break;
  }
  return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::value
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @fn bool parse(json::value&          _jout,
 *                parser_stats&         _stats,
 *                const std::string&    _value,
 *                const parser_control& _ctrl = parser_control()
 *               );
 * @brief Convert the given json string to json object
 *
 * @param _jout [out] json output
 * @param _value [in] Input json string
 * @param _stats [out] Parser statistics
 * @param _ctrl [in] Parser control flags
 */
/*static*/
bool json::value::parse(
  json::value&          _jout,
  const std::string&    _value,
  const parser_control& _ctrl /*= parser_control()*/
  )
{
  json::parser_stats stats;
  return json::value::parse(_jout, stats, _value, _ctrl);
}

bool json::value::parse(
  json::value&          _jout,
  parser_stats&         _stats,
  const std::string&    _value,
  const parser_control& _ctrl /*= parser_control()*/
  )
{
  json::parser jparser(_jout, _stats);
  jparser.m_ctrl = _ctrl;
  return jparser.parse(_value);
}

void json::value::p_set(const json::element _type/* = json::element::null*/)
{
  m_type = m_data.init(_type);
}

json::value::value(const json::element _type/* = json::element::null*/)
{
  m_type = m_data.init(_type);
}

json::value::value(const value& _obj)
{
  m_type = m_data.init(_obj.m_data, true, _obj.m_type);
}

// Move constructor
json::value::value(value&& _obj) noexcept
{
  m_type = m_data.init(std::move(_obj.m_data), _obj.m_type);
  _obj.m_type = json::element::null;
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
  m_type = m_data.init(_val);
}

json::value::value(const int _val)
{
  m_type = m_data.init(_val);
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
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  const bool is_new = ( m_type != _obj.m_type || m_type != json::element::object );
#else
  const bool is_new = true;
#endif
  if ( is_new )
    this->clear();
  m_type = m_data.init(_obj.m_data, is_new, _obj.m_type);
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

bool json::value::has_index(const size_t _index) const
{
  if ( ! is_array() )
    throw sid::exception(__func__ + std::string("() can be used only for array type"));
  return ( _index < m_data._arr.size() );
}

bool json::value::has_key(const std::string& _key) const
{
  json::value jval;
  return has_key(_key, jval);
}

bool json::value::has_key(const std::string& _key, value& _obj) const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  for ( const auto& entry : m_data.map() )
  {
    if ( entry.first == _key )
    {
      _obj = entry.second;
      return true;
    }
  }
  return false;
}

std::vector<std::string> json::value::get_keys() const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string("() can be used only for object type"));
  std::vector<std::string> keys;
  for ( const auto& entry : m_data.map() )
    keys.push_back(entry.first);
  return keys;
}

size_t json::value::size() const
{
  if ( is_array() )
    return m_data._arr.size();
  else if ( is_object() )
    return m_data.map().size();
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

bool json::value::get_bool() const
{
  if ( is_bool() )
    return m_data._bval;
  throw sid::exception(__func__ + std::string("() can be used only for boolean type"));
}

std::string json::value::get_str() const
{
  if ( is_string() )
    return m_data._str;
  throw sid::exception(__func__ + std::string("() can be used only for string type"));
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
  throw sid::exception(
    __func__ + std::string("() can be used only for string, number or boolean types"));
}

int json::value::get_value(bool& _val) const
{
  if ( is_null() )
    return -1;
  _val = get_bool();
  return 1;
}

int json::value::get_value(std::string& _val) const
{
  if ( is_null() )
    return -1;
  _val = as_str();
  return 1;
}

//! Convert json to string format
std::string json::value::to_str(json::format _format/* = json::format::compact*/) const
{
  std::ostringstream out;
  this->write(out, _format);
  return out.str();
}

//! Convert json to string format using pretty formatter
std::string json::value::to_str(const pretty_formatter& _formatter) const
{
  std::ostringstream out;
  this->write(out, _formatter);
  return out.str();
}

//! Write json to the given output stream
void json::value::write(std::ostream& _out, json::format _format/* = json::format::compact*/) const
{
  if ( ! is_object() && ! is_array() )
    throw sid::exception("Can be applied only on a object or array");

  this->p_write(_out, pretty_formatter(_format), 0);
}

//! Write json to the given output stream using pretty format
void json::value::write(std::ostream& _out, const pretty_formatter& _formatter) const
{
  if ( ! is_object() && ! is_array() )
    throw sid::exception("Can be applied only on a object or array");

  if ( ! ::isspace(_formatter.sep_char) && _formatter.sep_char != '\0' )
    throw sid::exception("Formatter must be a valid space character. It cannot be \""
                         + std::string(1, _formatter.sep_char) + "\"");

  this->p_write(_out, _formatter, 0);
}

void json::value::p_write(std::ostream& _out, const pretty_formatter& _formatter, uint32_t _level) const
{
  auto escape_string =[&](const std::string& _input)->std::string
    {
      std::string output;
      for ( size_t i = 0; i < _input.length(); i++ )
      {
        char ch = _input[i];
        switch ( ch )
        {
          //case '/':
          //output += '\\';
          //output += ch;
          //break;
        case '\b':
          output += "\\b";
          break;
        case '\f':
          output += "\\f";
          break;
        case '\n':
          output += "\\n";
          break;
        case '\r':
          output += "\\r";
          break;
        case '\t':
          output += "\\t";
          break;
        case '\\':
        case '\"': // Quotation mark
          output += '\\';
          output += + ch;
          break;
          /*
            case '\u':
            output += "\\u";
            break;
          */
        default:
          output += ch;
          break;
        }
      }
      return output;
    };

  std::string padding;
  const char* final_padding = padding.c_str();;
  if ( _formatter.type == json::format::pretty && _formatter.sep_char != '\0' )
  {
    padding = std::string((_level+1) * _formatter.sep_count, _formatter.sep_char);
    final_padding = (padding.c_str() + _formatter.sep_count);
  }

  if ( is_object() )
  {
    _out << "{";
    bool isFirst = true;
    for ( const auto& entry : m_data.map() )
    {
      if ( ! isFirst )
      {
        _out << ",";
        if ( _formatter.type == json::format::pretty )
          _out << endl;
      }
      else if ( _formatter.type == json::format::pretty )
        _out << endl;
      isFirst = false;
      if ( _formatter.type == json::format::compact )
      {
        if ( ! _formatter.key_no_quotes )
          _out << "\"" << entry.first << "\":";
        else
          _out << entry.first << ":";
        entry.second.p_write(_out, _formatter, _level+1);
      }
      else if ( _formatter.type == json::format::pretty )
      {
        if ( ! _formatter.key_no_quotes )
          _out << padding << "\"" << entry.first << "\" : ";
        else
          _out << padding << entry.first << " : ";
        entry.second.p_write(_out, _formatter, _level+1);
      }
    }
    if ( !isFirst && _formatter.type == json::format::pretty )
      _out << endl << final_padding;
    _out << "}";
  }
  else if ( is_array() )
  {
    _out << "[";
    bool isFirst = true;
    for ( size_t i = 0; i < this->size(); i++ )
    {
      if ( ! isFirst )
      {
        _out << ",";
        if ( _formatter.type == json::format::pretty )
          _out << endl;
      }
      else if ( _formatter.type == json::format::pretty )
        _out << endl;
      isFirst = false;
      if ( _formatter.type == json::format::pretty )
        _out << padding;
      (*this)[i].p_write(_out, _formatter, _level+1);
    }
    if ( !isFirst && _formatter.type == json::format::pretty )
      _out << endl << final_padding;
    _out << "]";
  }
  else if ( is_string() )
  {
    if ( ! _formatter.string_no_quotes
         || ( m_data._str.length() == 4 && (m_data._str == "true" || m_data._str == "null") )
         || ( m_data._str.length() == 5 && m_data._str == "false") )
     _out << "\"" << escape_string(m_data._str) << "\"";
    else
     _out << escape_string(m_data._str);
  }
  else if ( is_null() )
    _out << "null";
  else
    _out << this->as_str();
}

const json::value& json::value::operator[](const size_t _index) const
{
  if ( ! is_array() )
    throw sid::exception(__func__ + std::string(": can be used only for array type"));
  if ( _index >= m_data._arr.size() )
    throw sid::exception(__func__ + std::string(": index(") + sid::to_str(_index)
                         + ") out of range(" + sid::to_str(m_data._arr.size()) + ")");
  return m_data._arr[_index];
}

json::value& json::value::operator[](const size_t _index)
{
  if ( ! is_array() )
    throw sid::exception(__func__ + std::string(": can be used only for array type"));
  if ( _index >= m_data._arr.size() )
    throw sid::exception(__func__ + std::string(": index(") + sid::to_str(_index)
                         + ") out of range(" + sid::to_str(m_data._arr.size()) + ")");
  return m_data._arr[_index];
}

const json::value& json::value::operator[](const std::string& _key) const
{
  if ( ! is_object() )
    throw sid::exception(__func__ + std::string(": can be used only for object type"));
  for ( const auto& entry : m_data.map() )
    if ( entry.first == _key ) return entry.second;
  throw sid::exception(__func__ + std::string(": key(") + _key + ") not found");
}

json::value& json::value::operator[](const std::string& _key)
{
  if ( ! is_object() )
  {
    this->clear();
    m_type = m_data.init(json::element::object);
  }
  return (m_data.map())[_key];
}

json::value& json::value::append(const value& _obj)
{
  if ( ! is_array() )
  {
    this->clear();
    m_type = m_data.init(json::element::array);
  }
  m_data._arr.push_back(_obj);
  return m_data._arr[m_data._arr.size()-1];
}

json::value& json::value::append()
{
  if ( ! is_array() )
  {
    this->clear();
    m_type = m_data.init(json::element::array);
  }
  json::value jval;
  m_data._arr.push_back(std::move(jval));
  return m_data._arr[m_data._arr.size()-1];
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
  init(_obj, true, _type);
}

json::value::union_data::union_data(union_data&& _obj, const json::element _type) noexcept
{
  init(std::move(_obj), _type);
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
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  case json::element::object: delete _map; _map = nullptr; break;
#else
  case json::element::object: _map.~object(); break;    
#endif
  default: break;
  }
  return json::element::null;
}

json::element json::value::union_data::init(const json::element _type/* = json::element::null*/)
{
  switch ( _type )
  {
  case json::element::null:      break;
  case json::element::string:    new (&_str) string; break;
  case json::element::_signed:   _i64 = 0; break;
  case json::element::_unsigned: _u64 = 0; break;
  case json::element::_double:   _dbl = 0; break;
  case json::element::boolean:   _bval = false; break;
  case json::element::array:     new (&_arr) array; break;
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  case json::element::object:    _map = new object; ++gobjects_alloc; break;
#else
  case json::element::object:    new (&_map) object; break;
#endif
  }
  return _type;
}

json::element json::value::union_data::init(
  const union_data&   _obj,
  const bool          _new/* = true*/,
  const json::element _type/* = json::element::null*/
  )
{
  switch ( _type )
  {
  case json::element::null:      break;
  case json::element::string:    init(_obj._str);  break;
  case json::element::_signed:   init(_obj._i64);  break;
  case json::element::_unsigned: init(_obj._u64);  break;
  case json::element::_double:   init(_obj._dbl);  break;
  case json::element::boolean:   init(_obj._bval); break;
  case json::element::array:     init(_obj._arr);  break;
  case json::element::object:    init(_obj.map(), _new); break;
  }
  return _type;
}

json::element json::value::union_data::init(const int _val)
{
  return this->init(static_cast<int64_t>(_val));
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

json::element json::value::union_data::init(const char* _val)
{
  if ( _val != nullptr )
    return init(std::string(_val));
  return init(json::element::null);
}

json::element json::value::union_data::init(const array& _val)
{
  new (&_arr) array(_val);
  return json::element::array;
}

json::element json::value::union_data::init(const object& _val, const bool _new/* = true*/)
{
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  if ( _new )
  {
    _map = new object(_val);
    ++gobjects_alloc;
  }
  else
    *_map = _val;
#else
  new (&_map) object(_val);
#endif
  return json::element::object;
}

//! Move initializer routine
json::element json::value::union_data::init(union_data&& _obj, json::element _type) noexcept
{
  switch ( _type )
  {
  case json::element::null:            break;
  case json::element::string:          new (&_str) string(_obj._str); break;
  case json::element::_signed:         _i64  = _obj._i64;  break;
  case json::element::_unsigned:       _u64  = _obj._u64;  break;
  case json::element::_double:         _dbl  = _obj._dbl;  break;
  case json::element::boolean:         _bval = _obj._bval; break;
  case json::element::array:           new (&_arr) array(_obj._arr); break;
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  case json::element::object:          _map  = _obj._map;  break;
#else
  case json::element::object:          new (&_map) object(_obj._map); break;
#endif
  }
  // We've made a copy of the string and array objects.
  // So, we need to clear them from _obj
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
  // object uses a pointer. We don't want to clear the object type alone
  if ( _type != json::element::object )
    _obj.clear(_type);
#else
    _obj.clear(_type);
#endif
  return _type;
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

struct time_calc
{
  struct timespec t_start, t_end;

  time_calc() { clear(); }
  void clear()
  {
    ::memset(&t_start, 0, sizeof(t_start));
    ::memset(&t_end, 0, sizeof(t_end));
  }
  void start() { t_start = p_capture(); }
  void stop() { t_end = p_capture(); }

  uint64_t diff_secs() const { return diff_millisecs() / 1000; }
  uint64_t diff_millisecs() const { return diff_microsecs() / 1000; }
  uint64_t diff_microsecs() const
  {
    if ( t_end.tv_sec > t_start.tv_sec ||
         (t_end.tv_sec == t_start.tv_sec && t_end.tv_nsec >= t_start.tv_nsec) )
    {
      uint64_t s = (t_end.tv_sec - t_start.tv_sec) * 1000000;
      uint64_t x = t_end.tv_nsec / 1000;
      uint64_t y = t_start.tv_nsec / 1000;
      if ( x < y )
      {
        --s;
        x += 1000000;
      }
      s += x;
      return s;
    }
    return 0;
  }
  
private:
  struct timespec p_capture() const
  {
    struct timespec ts = {0};
    clock_gettime(CLOCK_REALTIME, &ts); // CLOCK_PROCESS_CPUTIME_ID
    return ts;
  }
};

//! check for space character
bool json::parser::is_space(const char* _p)
{
  if ( *_p == '\n' )
  {
    ++m_line.count;
    m_line.begin = _p + 1;
    return true;
  }
  return ::isspace(*_p);
}

void json::parser::REMOVE_LEADING_SPACES(const char*& _p)
{
  do
  {
    for (; is_space(_p) && *_p != '\0'; _p++ );
    if ( *_p != '/' )
      return;

    if ( *(_p+1) == '/' )
    {
      // C++ style comment encountered. Parse until end of line
      for ( _p++; *_p != '\n' && *_p != '\0'; _p++ );
    }
    else if ( *(_p+1) == '*' )
    {
      const line_info old_line = m_line;
      const char* old_p = _p;
      // C style comment encountered. Parse until */
      do
      {
        for ( _p++; *_p != '*' && *_p != '\0'; _p++ )
        {
          if ( *_p == '\n' )
          {
            ++m_line.count;
            m_line.begin = _p + 1;
          }
        }
        if ( *_p != '*' )
          throw sid::exception(std::string("Comments starting " + loc_str(old_line, old_p))
                               + " is not closed");
      }
      while ( *(_p+1) != '/' );
      _p += 2;
    }
  }
  while ( true );
}

bool json::parser::parse(const std::string& _value)
{
  time_calc tc;

  try
  {
    tc.start();

    m_jroot.clear();
    m_stats.clear();

    m_input = _value;
    m_p = m_line.begin = m_input.c_str();
    m_line.count = 1;
    REMOVE_LEADING_SPACES(m_p);
    char ch = *m_p;
    if ( ch == '{' )
      parse_object(m_jroot);
    else if ( ch == '[' )
      parse_array(m_jroot);
    else if ( ch != '\0' )
      throw sid::exception(std::string("Invalid character [") + ch + "] " + loc_str()
                           + ". Expecting { or [");
    else
      throw sid::exception(std::string("End of data reached ") + loc_str() + ". Expecting { or [");

    tc.stop();
    m_stats.time_ms = tc.diff_millisecs();
  }
  catch (...)
  {
    tc.stop();
    m_stats.time_ms = tc.diff_millisecs();
    throw;
  }

  //cout << "Object allocations: " << sid::get_sep(gobjects_alloc) << endl;
  // return the top-level json
  return true;
}

void json::parser::parse_object(json::value& _jobj)
{
  char ch = 0;
  if ( ! _jobj.is_object() )
    _jobj.p_set(json::element::object);

  m_containerStack.push(json::element::object);
  m_stats.objects++;
  while ( true )
  {
    ++m_p;
    // "string" : value
    REMOVE_LEADING_SPACES(m_p);
    // This is the case where there are no elements in the object (An empty object)
    if ( *m_p == '}' ) { ++m_p; break; }

    parse_key(m_key);
    // Check whether this key already exists in the object map
    bool isDuplicateKey = _jobj.has_key(m_key);
    if ( isDuplicateKey )
    {
      // Handle duplicate key scenario
      if ( m_ctrl.dupKey == parser_control::dup_key::reject )
        throw sid::exception("Duplicate key \"" + m_key + "\" encountered");
    }

    m_stats.keys++;
    REMOVE_LEADING_SPACES(m_p);
    if ( *m_p != ':' )
      throw sid::exception("Expected : " + loc_str());
    m_p++;
    REMOVE_LEADING_SPACES(m_p);
    if ( ! isDuplicateKey )
    {
      parse_value(_jobj[m_key]);
    }
    // Handle duplicate key based on the input mode
    else if ( m_ctrl.dupKey == parser_control::dup_key::accept )
    {
      // Accept the value and overwrite it
      parse_value(_jobj[m_key]);
    }
    else if ( m_ctrl.dupKey == parser_control::dup_key::ignore )
    {
      // Parse the value, but ignore it
      json::value jignore;
      parse_value(jignore);
    }
    else if ( m_ctrl.dupKey == parser_control::dup_key::append )
    {
      // make it as an array and append the duplicate keys
      if ( ! _jobj[m_key].is_array() )
      {
        // make a copy of the existing key's value
        json::value jcopy = _jobj[m_key];
        // make they key as an array
        _jobj[m_key].clear();
        _jobj[m_key].p_set(json::element::array);
        // append the copied value to the array
        _jobj[m_key].append(jcopy);
      }
      // Append the new value to the array
      json::value& jval = _jobj[m_key].append();
      parse_value(jval);
    }
    ch = *m_p;
    // Can have a ,
    // Must end with }
    if ( ch == '}' ) { ++m_p; break; }
    if ( ch != ',' )
      throw sid::exception("Encountered " + std::string(m_p, 1) + ". Expected , or } " + loc_str());
  }
  m_containerStack.pop();
}

void json::parser::parse_array(json::value& _jarr)
{
  char ch = 0;
  if ( ! _jarr.is_array() )
    _jarr.p_set(json::element::array);

  m_containerStack.push(json::element::array);
  m_stats.arrays++;
  while ( true )
  {
    ++m_p;
    // value
    REMOVE_LEADING_SPACES(m_p);
    // This is the case where there are no elements in the array (An empty array)
    if ( *m_p == ']' ) { ++m_p; break; }

    json::value& jval = _jarr.append();
    parse_value(jval);
    ch = *m_p;
    // Can have a ,
    // Must end with ]
    if ( ch == ']' ) { ++m_p; break; }
    if ( ch != ',' )
      throw sid::exception("Expected , or ] " + loc_str());
  }
  m_containerStack.pop();
}

void json::parser::parse_key(std::string& _str)
{
  parse_string(_str, true);
}

void json::parser::parse_string(std::string& _str, bool _isKey)
{
  _str.clear();
  const char chContainer = (m_containerStack.top() == json::element::object)? '}' : ']' ;
  bool hasQuotes = true;
  if ( ( _isKey && m_ctrl.mode.allowFlexibleKeys ) || ( ! _isKey && m_ctrl.mode.allowFlexibleStrings ) )
    hasQuotes = (*m_p == '\"');
  else if ( *m_p != '\"' )
    throw sid::exception("Expected \" " + loc_str() + ", found \"" + std::string(1, *m_p) + "\"");

  const line_info old_line = m_line;
  const char* old_p = m_p;

  auto check_hex = [&](char ch)->char
    {
      if ( ch == '\0' )
        throw sid::exception("Missing hexadecimal sequence characters at the end position "
                             + loc_str());
      if ( ! ::isxdigit(ch) )
        throw sid::exception("Missing hexadecimal character at " + loc_str());
      return ch;
    };

  char ch = 0;
  if ( !hasQuotes )
    --m_p;

  while ( true )
  {
    ch = *(++m_p);
    if ( hasQuotes )
    {
      if ( ch == '\"' ) break;
      if ( ch == '\n' )
      {
        ++m_line.count;
        m_line.begin = m_p + 1;
      }
      else if ( ch == '\0' )
        throw sid::exception("Missing \" for string starting " + loc_str(old_line, old_p));
    }
    else
    {
      // Cannot have double-quotes, it must be escaped
      if ( ch == '\"' ) throw sid::exception("Character \" must be escaped " + loc_str());
      // A space character denotes end of the string
      if ( is_space(m_p) )
        break;
      // For a key : denotes end of the key
      // For a value , and the end of container key denotes end of the value
      if ( ( _isKey && ch == ':' ) || ( ! _isKey && (ch == ',' || ch == chContainer) ) )
        { --m_p; break; }
      if ( ch == '\0' )
        throw sid::exception("End of string character not found for string starting " + loc_str());
    }
    if ( ch != '\\' ) { _str += ch; continue; }
    // We're encountered an escape character. Process it
    {
      ch = *(++m_p);
      switch ( ch )
      {
      case '/':  _str += ch;   break;
      case 'b':  _str += '\b'; break;
      case 'f':  _str += '\f'; break;
      case 'n':  _str += '\n'; break;
      case 'r':  _str += '\r'; break;
      case 't':  _str += '\t'; break;
      case '\\': _str += ch;   break;
      case '\"': _str += ch;   break;
      case 'u':
      {
        const char* p = m_p;
        // Must be followed by 4 hex digits
        for ( int i = 0; i < 4; i++ )
          check_hex(*(++m_p));
        _str.append(p, (p-m_p));
      }
      break;
      case '\0':
        throw sid::exception("Missing escape sequence characters at the end position " + loc_str());
      default:
        throw sid::exception("Invalid escape sequence (" + std::string(1, ch) +
                             ") for string at " + loc_str());
      }
    }
  }
  ++m_p;
}

void json::parser::parse_string(json::value& _jstr, bool _isKey)
{
  std::string str;
  parse_string(str, _isKey);
  _jstr = str;
}

void json::parser::parse_value(json::value& _jval)
{
  char ch = *m_p;
  if ( ch == '{' )
    parse_object(_jval);
  else if ( ch == '[' )
    parse_array(_jval);
  else if ( ch == '\"' )
    parse_string(_jval, false);
  else if ( ch == '-' || ::isdigit(ch) )
    parse_number(_jval, true);
  else if ( ch == '\0' )
    throw sid::exception("Unexpected end of data while expecting a value");
  else
  {
    const char chContainer = (m_containerStack.top() == json::element::object)? '}' : ']' ;
    const char* p_start = m_p;
    const line_info old_line = m_line;
    for ( char ch; (ch = *m_p) != '\0'; ++m_p )
    {
      if ( ch == ',' || is_space(m_p) || ch == chContainer )
        break;
    }
    if ( m_p == p_start )
      throw sid::exception("Expected value not found " + loc_str());

    bool found = true;
    size_t len = m_p - p_start;
    if ( len == 4 )
    {
      if ( ::strncmp(p_start, "null", len) == 0 )
        ;
      else if ( ::strncmp(p_start, "true", len) == 0 )
        _jval = true;
      else if ( ! m_ctrl.mode.allowNocaseValues )
        found = false;
      else
      {
        if ( ::strncmp(p_start, "Null", len) == 0 || ::strncmp(p_start, "NULL", len) == 0 )
          ;
        else if ( ::strncmp(p_start, "True", len) == 0 || ::strncmp(p_start, "TRUE", len) == 0 )
          _jval = true;
        else
          found = false;
      }
    }
    else if ( len == 5 )
    {
      if ( ::strncmp(p_start, "false", len) == 0 )
        _jval = false;
      else if ( ! m_ctrl.mode.allowNocaseValues )
        found = false;
      else
      {
        if ( ::strncmp(p_start, "False", len) == 0 || ::strncmp(p_start, "FALSE", len) == 0 )
          _jval = false;
        else
          found = false;
      }
    }
    else
      found = false;
    if ( ! found )
    {
      if ( m_ctrl.mode.allowFlexibleStrings )
      {
        m_p = p_start;
        m_line = old_line;
        parse_string(_jval, false);
      }
      else
        throw sid::exception("Invalid value [" + std::string(p_start, len) + "] " + loc_str()
                             + ". Did you miss enclosing in \"\"?");
    }
  }
  // Set the statistics of non-container objects here
  if ( _jval.is_string() )
      m_stats.strings++;
  else if ( _jval.is_num() )
      m_stats.numbers++;
  else if ( _jval.is_bool() )
      m_stats.booleans++;
  else if ( _jval.is_null() )
      m_stats.nulls++;

  REMOVE_LEADING_SPACES(m_p);
}

void json::parser::parse_number(json::value& _jnum, bool bFullCheck)
{
  REMOVE_LEADING_SPACES(m_p);
  const char* p_start = m_p;
  bool isDouble = false;
  bool isNegative = false;
  const char chContainer = (m_containerStack.top() == json::element::object)? '}' : ']' ;

  if ( ! bFullCheck )
  {
    isNegative = (*m_p == '-');
    for ( ; *m_p != '\0'; m_p++ )
    {
      if ( *m_p == ',' || is_space(m_p) ||  *m_p == chContainer )
        break;
      if ( !isDouble && (*m_p == '.' || *m_p == 'e' || *m_p == 'E') )
        isDouble = true;
    }
  }
  else
  {
    // Perform full check and get the number
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

    if ( (num.integer.negative = (*m_p == '-')) )
      ++m_p;
    char ch = *m_p;
    if ( ch < '0' || ch > '9' )
      throw sid::exception("Missing integer digit" + loc_str());

    if ( ch == '0' )
    {
      ch = *(++m_p);
      if ( ch >= '0' && ch <= '9' )
        throw sid::exception("Invalid digit (" + std::string(1, ch) + ") after first 0 " + loc_str());
      num.integer.digits = 0;
    }
    else
    {
      while ( (ch = *(++m_p)) >= '0' && ch <= '9'  )
        ;
    }
    // Check whether it has fraction and populate accordingly
    if ( ch == '.' )
    {
      bool hasDigits = false;
      while ( (ch = *(++m_p)) >= '0' && ch <= '9'  )
        hasDigits = true;
      if ( !hasDigits )
        throw sid::exception("Invalid digit (" + std::string(1, ch)
                             + ") Expected a digit for fraction " + loc_str());
      num.hasFraction = true;
    }
    // Check whether it has an exponent and populate accordingly
    if ( ch == 'e' || ch == 'E' )
    {
      ch = *(++m_p);
      if ( ch != '-' && ch != '+' )
        --m_p;
      bool hasDigits = false;
      while ( (ch = *(++m_p)) >= '0' && ch <= '9'  )
        hasDigits = true;
      if ( !hasDigits )
        throw sid::exception("Invalid digit (" + std::string(1, ch)
                             + ") Expected a digit for exponent " + loc_str());
      num.hasExponent = true;
    }

    isNegative = num.integer.negative;
    isDouble = ( num.hasFraction || num.hasExponent );
  }

  const char* p_end = m_p;
  REMOVE_LEADING_SPACES(m_p);
  char ch = *m_p;
  if ( ch != ',' && ch != '\0' && ch != chContainer )
    throw sid::exception("Invalid character " + std::string(1, ch) + " Expected , or "
                         + std::string(1, chContainer) + " " + loc_str());

  std::string numStr(p_start, p_end-p_start);
  if ( isDouble )
  {
    long double dbl = sid::to_num<long double>(numStr);
    _jnum = dbl;
  }
  else
  {
    std::string errStr;
    if ( isNegative )
    {
      int64_t v = 0;
      if ( ! sid::to_num(numStr, /*out*/ v, &errStr) )
        throw sid::exception("Unable to convert (" + numStr + ") to numeric " + loc_str()
                             + ": " + errStr);
      _jnum = v;
    }
    else
    {
      uint64_t v = 0;
      if ( ! sid::to_num(numStr, /*out*/ v, &errStr) )
        throw sid::exception("Unable to convert (" + numStr + ") to numeric " + loc_str()
                             + ": " + errStr);
      _jnum = v;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::schema
//
///////////////////////////////////////////////////////////////////////////////////////////////////
void json::schema::types::add(const value& _value)
{
  if ( _value.is_string() )
    this->add(type::null);
  else if ( _value.is_array() )
  {
    // Each array element must be a string
    this->add(type::null);
  }
  else
    throw sid::exception("type parameter must be string or an array of unique string");
}

json::schema::schema()
{
}

void json::schema::clear()
{
  _schema.clear();
  _id.clear();
  title.clear();
  description.clear();
  type.clear();
  properties.clear();
  required.clear();
}

/*static*/
json::schema json::schema::parse_file(const std::string& schemaFile)
{
  std::ifstream in;
  in.open(schemaFile);
  if ( ! in.is_open() )
    throw sid::exception("Failed to open schema file: " + schemaFile);
  char buf[8096] = {0};
  std::string jsonStr;
  while ( ! in.eof() )
  {
    ::memset(buf, 0, sizeof(buf));
    in.read(buf, sizeof(buf)-1);
    if ( in.bad() )
      throw sid::exception(sid::to_errno_str("Failed to read schema file"));
    jsonStr += buf;
  }
  return parse(jsonStr);
}

/*static*/
json::schema json::schema::parse(const std::string& schemaData)
{
  json::value jroot;
  json::value::parse(jroot, schemaData);
  return parse(jroot);
}

/*static*/
json::schema json::schema::parse(const value& jroot)
{
  schema schema;
  json::value jval;
  if ( jroot.has_key("$schema", jval) && !jval.is_null() )
    schema._schema = jval.get_str();
  if ( jroot.has_key("$id", jval) && !jval.is_null() )
    schema._id = jval.get_str();
  if ( jroot.has_key("title", jval) && !jval.is_null() )
    schema.title = jval.get_str();
  if ( jroot.has_key("description", jval) && !jval.is_null() )
    schema.description = jval.get_str();

  if ( !jroot.has_key("type", jval) )
    throw sid::exception("type missing in schema");
  else
    schema.type.add(jval);

  return schema;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Implementation of json::schema::property
//
///////////////////////////////////////////////////////////////////////////////////////////////////
json::schema::property::property()
{
}

void json::schema::property::clear()
{
  key.clear();
  description.clear();
  type.clear();
  // For numbers
  minimum.clear(0);
  exclusiveMinimum.clear(0);
  maximum.clear(0);
  exclusiveMaximum.clear(0);
  multipleOf.clear(0);
  // For strings
  minLength.clear(0);
  maxLength.clear(0);
  pattern.clear();
  // For arrays
  minItems.clear(0);
  maxItems.clear(0);
  uniqueItems.clear(false);
  minContains.clear(0);
  maxContains.clear(0);
  // For objects
  minProperties.clear(0);
  maxProperties.clear(0);
  required.clear();
  properties.clear();
}

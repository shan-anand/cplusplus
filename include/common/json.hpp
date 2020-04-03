/*
LICENSE: BEGIN
===============================================================================
@author Shanmuga (Anand) Gunasekaran
@email anand.gs@gmail.com
@source https://github.com/shan-anand
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

#pragma once

#include <string>
#include <vector>
#include <map>
#include "exception.hpp"
#include "smart_ptr.hpp"

namespace sid {
namespace json {

//! json element type
enum class element : uint8_t { null, object, array, string, boolean, _signed, _unsigned, _double };

//! json formatter
enum class format : uint8_t { compact, pretty };

//! json formatter for pretty formatting
struct pretty_formatter
{
  char     sep_char;
  uint32_t sep_count;
  pretty_formatter(char _sep_char = ' ', uint32_t _sep_count = 2) : sep_char(_sep_char), sep_count(_sep_count) {}
};

//! Forward declaration of parser (not exposed)
struct parser;

#pragma pack(push)
#pragma pack(1)

/**
 * @class value
 * @bried json value class
 */
class value
{
  friend class parser;
public:
  /**
   * @fn json::value get(const std::string& _value, bool _strict = true);
   * @brief Convert the given json string to json object
   *
   * @param _value [in] Input json string
   * @param _strict [in] Use strict parsing (default)
   *                     If set to false, it relaxes the parsing logic for boolean and null types by accepting
   *                     True, TRUE, False, FALSE, Null, NULL (in addition to true, false, null)
   */
  static value get(const std::string& _value, bool _strict = true);

  // Constructors
  value(const json::element _type = json::element::null);
  value(const int64_t _val);
  value(const uint64_t _val);
  value(const double _val);
  value(const long double _val);
  value(const bool _val);
  value(const std::string& _val);
  value(const char* _val);
  value(const int _val);
  // Copy constructor
  value(const value& _obj);
  // Virtual destructor
  virtual ~value();

  bool empty() const { return m_type == json::element::null; }
  void clear();

  //! get the element type
  json::element type() const { return m_type; }
  //! element type check as functions
  bool is_null() const { return m_type == json::element::null; }
  bool is_string() const { return m_type == json::element::string; }
  bool is_signed() const { return m_type == json::element::_signed; }
  bool is_unsigned() const { return m_type == json::element::_unsigned; }
  bool is_double() const { return m_type == json::element::_double; }
  bool is_num() const { return is_signed() || is_unsigned() || is_double(); }
  bool is_bool() const { return m_type == json::element::boolean; }
  bool is_array() const { return m_type == json::element::array; }
  bool is_object() const { return m_type == json::element::object; }

  // operator= overloads
  value& operator=(const value& _obj);
  value& operator=(const int64_t _val);
  value& operator=(const uint64_t _val);
  value& operator=(const double _val);
  value& operator=(const long double _val);
  value& operator=(const bool _val);
  value& operator=(const std::string& _val);
  value& operator=(const char* _val);
  value& operator=(const int _val);

  bool has_key(const std::string& _key) const;
  std::vector<std::string> get_keys() const;
  size_t size() const; // For array and object type

  int64_t get_int64() const;
  uint64_t get_uint64() const;
  long double get_double() const;
  std::string get_str() const;
  std::string as_str() const;
  bool get_bool() const;
  const value& operator[](size_t _index) const;
  const value& operator[](const std::string& _key) const;
  value& operator[](const std::string& _key);
  //! Append value to the array
  value& append();
  value& append(const value& _obj);
  template <typename T> value& append(const T& _val)
  {
    if ( ! is_array() )
      {
	clear();
	m_type = m_data.init(json::element::array);
      }
    value& jval = append();
    jval.m_data.init(_val);
    return jval;
  }

  //! Convert json to string format
  std::string to_str(json::format _format = json::format::compact) const;
  //! Convert json to string format using pretty formatter
  std::string to_str(const pretty_formatter& _formatter) const;

  //! Write json to the given output stream
  void write(std::ostream& _out, json::format _format = json::format::compact) const;
  //! Write json to the given output stream using pretty format
  void write(std::ostream& _out, const pretty_formatter& _formatter) const;

private:
  void p_write(std::ostream& _out, json::format _format, const pretty_formatter& _formatter, uint32_t _level) const;

private:
  using array = std::vector<value>;
  using object = std::map<std::string, value>;

  union union_data
  {
    int64_t     _i64;
    uint64_t    _u64;
    long double _dbl;
    bool        _bval;
    std::string _str;
    array       _arr;
    object*     _map;

    union_data(const json::element _type = json::element::null);
    union_data(const union_data& _obj, const json::element _type = json::element::null);
    ~union_data();

    json::element clear(const json::element _type);

    json::element init(const json::element _type = json::element::null);
    json::element init(const union_data& _obj, const json::element _type = json::element::null);
    json::element init(const int _val);
    json::element init(const int64_t _val);
    json::element init(const uint64_t _val);
    json::element init(const long double _val);
    json::element init(const bool _val);
    json::element init(const std::string& _val);
    json::element init(const char* _val);
    json::element init(const array& _val);
    json::element init(const object& _val);
  };

  json::element m_type;
  union_data    m_data;
};

#pragma pack(pop)

} // namespace json
} // namespace sid

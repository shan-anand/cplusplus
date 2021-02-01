/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
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

// ======================
// Compiler optimization for the size of the structure
#define SID_JSON_MAP_OPTIMIZE_FOR_SIZE

#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
#pragma message "Compiler flag set to optimize json for size"
#endif
// ======================

namespace sid {
namespace json {

//! json element type
enum class element : uint8_t { null, object, array, string, boolean, _signed, _unsigned, _double };
} // namespace json

// to_str in the sid namespace
std::string to_str(const json::element& _type);

namespace json {

//! json formatter
enum class format : uint8_t { compact, pretty };

//! json formatter for pretty formatting
struct pretty_formatter
{
  format   type;
  char     sep_char;
  uint32_t sep_count;
  bool     key_no_quotes;
  bool     string_no_quotes;

  pretty_formatter()
    : type(format::compact), sep_char(' '), sep_count(2), key_no_quotes(false), string_no_quotes(false)  {}
  pretty_formatter(const format& _type, bool _key_no_quotes = false, bool _string_no_quotes = false)
    : pretty_formatter() { type =_type; key_no_quotes = _key_no_quotes; string_no_quotes = _string_no_quotes; }
  pretty_formatter(bool _key_no_quotes, bool _string_no_quotes)
    : pretty_formatter() { key_no_quotes = _key_no_quotes; string_no_quotes = _string_no_quotes; }
};

//! Forward declaration of parser (not exposed)
struct parser;

#ifdef SID_JSON_MAP_OPTIMIZE_FOR_SIZE
#pragma pack(push)
#pragma pack(1)
#endif

//! Parser statistics object
struct parser_stats
{
  uint64_t objects;
  uint64_t arrays;
  uint64_t strings;
  uint64_t numbers;
  uint64_t booleans;
  uint64_t nulls;
  uint64_t keys;
  uint64_t time_ms;

  parser_stats();
  void clear();
  std::string to_str() const;
};

#define SID_JSON_PARSE_MODE_ALLOW_FLEXIBLE_KEYS    1
#define SID_JSON_PARSE_MODE_ALLOW_FLEXIBLE_STRINGS 2
#define SID_JSON_PARSE_MODE_ALLOW_NOCASE_VALUES    4

//! Parser control parameters
struct parser_control
{
  enum class dup_key : uint8_t { accept = 0, ignore, append, reject };
  union parse_mode
  {
    struct
    {
      uint8_t allowFlexibleKeys    : 1; //! If set to 1, accept key names not enclosed within double-quotes
                                        //!   Must encode characters with unicode character (\u xxxx)
                                        //!     " -> \u
                                        //!     : -> \u
      uint8_t allowFlexibleStrings : 1; //! If set to 1, accept string values not enclosed within double-quotes
                                        //!     " -> \u
                                        //!     , -> \u
                                        //!     ] -> \u
                                        //!     } -> \u
      uint8_t allowNocaseValues    : 1; //! If set to 1, it relaxes the parsing logic for boolean and null types by accepting
                                        //!    True, TRUE, False, FALSE, Null, NULL (in addition to true, false, null)
    };
    uint8_t flags;
    parse_mode() : flags(0) {}
    parse_mode(uint8_t _flags) : flags(_flags) {}
  };

  //! Members
  parse_mode mode;    //! Parser control modes
  dup_key    dupKey;  //! Duplicate key handling

  //! Default constructor
  parser_control(const parse_mode& _mode = parse_mode(), const dup_key& _dupKey = dup_key::accept)
    : mode(_mode), dupKey(_dupKey) {}
  //! One argment constructor
  parser_control(const dup_key& _dupKey, const parse_mode& _mode = parse_mode())
    : mode(_mode), dupKey(_dupKey) {}
};

/**
 * @class value
 * @brief json value class
 */
class value
{
  friend class parser;
public:
  /**
   * @fn bool parse(json::value& _jout, parser_stats& _stats, const std::string& _value, const parser_control& _ctrl = parser_control());
   * @brief Convert the given json string to json object
   *
   * @param _jout [out] json output
   * @param _stats [out] Parser statistics
   * @param _value [in] Input json string
   * @param _ctrl [in] Parser control flags
   */
  static bool parse(json::value& _jout, const std::string& _value, const parser_control& _ctrl = parser_control());
  static bool parse(json::value& _jout, parser_stats& _stats, const std::string& _value, const parser_control& _ctrl = parser_control());

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
  // Move constructor
  value(value&& _obj) noexcept;

  // Destructor
  ~value();

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
  bool is_basic_type() const { return ! ( is_array() || is_object() ); }

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

  bool has_index(const size_t _index) const;
  bool has_key(const std::string& _key) const;
  bool has_key(const std::string& _key, value& _obj) const;
  std::vector<std::string> get_keys() const;
  size_t size() const; // For array and object type

  //! get functions
  int64_t get_int64() const;
  uint64_t get_uint64() const;
  long double get_double() const;
  bool get_bool() const;
  std::string get_str() const;
  std::string as_str() const;

  //! get functions with arguments
  //    0 : doesn't exist
  //    1 : Exists with non-null value
  //   -1 : Exists but it has null value
  template <typename T> int get_value(T& _val) const
  {
    if ( is_null() )
      return -1;
    if ( ! is_num() )
      throw sid::exception(__func__ + std::string("() can be used only for number type"));

    if ( std::is_floating_point<T>::value )
      _val = static_cast<T>(get_double());
    else if ( std::is_signed<T>::value )
      _val = static_cast<T>(get_int64());
    else
      _val = static_cast<T>(get_uint64());
    return 1;
  }
  int get_value(bool& _val) const;
  int get_value(std::string& _val) const;

  int get_value(const std::string& _key, value& _obj) const
  {
    return has_key(_key, _obj)? ( ! _obj.is_null()? 1 : -1 ) : 0;
  }

  template <typename T> int get_value(const std::string& _key, T& _val) const
  {
    value jval;
    return has_key(_key, jval)? jval.get_value(_val) : 0;
  }

  const value& operator[](const size_t _index) const;
  value& operator[](const size_t _index);
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
    jval.m_type = jval.m_data.init(_val);
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
  void p_write(std::ostream& _out, const pretty_formatter& _formatter, uint32_t _level) const;
  void p_set(const json::element _type = json::element::null);

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
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
    object*     _map;
#else
    object      _map;
#endif
    //! Default constructor
    union_data(const json::element _type = json::element::null);
    //! Copy constructor
    union_data(const union_data& _obj, const json::element _type = json::element::null);
    //! Move constructor
    union_data(union_data&& _obj, const json::element _type = json::element::null) noexcept;
    //! Destructor
    ~union_data();

    //! Clear the memory used by the object
    json::element clear(const json::element _type);

    //! Copy initializer routines
    json::element init(const json::element _type = json::element::null);
    json::element init(const union_data& _obj, const bool _new = true, const json::element _type = json::element::null);
    json::element init(const int _val);
    json::element init(const int64_t _val);
    json::element init(const uint64_t _val);
    json::element init(const long double _val);
    json::element init(const bool _val);
    json::element init(const std::string& _val);
    json::element init(const char* _val);
    json::element init(const array& _val);
    json::element init(const object& _val, const bool _new = true);
    //! Move initializer routine
    json::element init(union_data&& _obj, json::element _type) noexcept;

    union_data& operator=(const union_data& _obj) { *this = std::move(_obj); return *this; }
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
    const object& map() const { return (*_map); }
    object& map() { return (*_map); }
#else
    const object& map() const { return _map; }
    object& map() { return _map; }
#endif
  };

  json::element m_type; //! Type of the object
  union_data    m_data; //! The object
};

#ifdef SID_JSON_MAP_OPTIMIZE_FOR_SIZE
#pragma pack(pop)
#endif

/*
namespace std {
using strings = std::vector<std::string>;
}
using opt_size_t = sid::optional<size_t>;
using opt_int = sid::optional<int>;
using opt_bool = sid::optional<bool>;
using opt_string = sid::optional<std::string>;
 
enum class element : uint8_t { null, object, array, string, boolean, _signed, _unsigned, _double };

enum class schema_type : uint8_t { null_, boolean, object, array, number, integer, string };

struct property
{
  std::string           key;
  std::string           description;
  schema_type           type;
  // For numbers
  sid::optional<int>    minimum;
  sid::optional<bool>   exclusiveMinimum;
  sid::optional<int>    maximum;
  sid::optional<bool>   exclusiveMaximum;
  sid::optional<int>    multipleOf;
  // For strings
  sid::optional<size_t> maxLength;
  sid::optional<size_t> minLength;
  std::string           pattern;
  // For arrays
  sid::optional<size_t> maxItems;
  sid::optional<size_t> minItems;
  sid::optional<bool>   uniqueItems;
  sid::optional<size_t> maxContains;
  sid::optional<size_t> minContains;
  // For objects
  sid::optional<size_t> maxProperties;
  sid::optional<size_t> minProperties;
  std::strings          required;
};

struct schema
{
  std::string  schema;
  std::string  title;
  std::string  description;
  schema_type  type;
  properties   properties;
  std::strings required;
};

json::schema schema;

json::parse(jsonFile, schemaFile);
json::parse(jsonFile, schema);

json::schema schema = json::schema::parse(schemaFile);


*/

} // namespace json
} // namespace sid

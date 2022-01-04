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
#include <set>
#include "exception.hpp"

#include "optional.hpp"
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

//! json value type
enum value_type : uint8_t {
  null, object, array, string, boolean, _signed, _unsigned, _double
};

//! json format type
enum class format_type : uint8_t {
  compact, pretty
};

//! json format stucture
struct format
{
  format_type type;
  char        separator;
  uint32_t    indent;
  bool        key_no_quotes;
  bool        string_no_quotes;

  format() :
    type(format_type::compact), separator(' '),
    indent(2),
    key_no_quotes(false),
    string_no_quotes(false)
    {}
  format(
    const format_type& _type,
    bool               _key_no_quotes = false,
    bool               _string_no_quotes = false
    ) :
    format() {
      type =_type; key_no_quotes = _key_no_quotes; string_no_quotes = _string_no_quotes;
  }
  format(
    bool _key_no_quotes,
    bool _string_no_quotes
    ) :
    format() {
    key_no_quotes = _key_no_quotes; string_no_quotes = _string_no_quotes;
  }

  std::string to_str() const;
  static format get(const std::string& _value);
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
  enum class dup_key : uint8_t {
    accept = 0, ignore, append, reject
  };

  union parse_mode
  {
    struct
    {
      uint8_t allowFlexibleKeys    : 1; //! If set to 1, accept key names not enclosed
                                        //!   within double-quotes
                                        //!   Must encode characters with unicode character
                                        //    (\u xxxx)
                                        //!     " -> \u
                                        //!     : -> \u
      uint8_t allowFlexibleStrings : 1; //! If set to 1, accept string values not enclosed
                                        //!   within double-quotes
                                        //!     " -> \u
                                        //!     , -> \u
                                        //!     ] -> \u
                                        //!     } -> \u
      uint8_t allowNocaseValues    : 1; //! If set to 1, it relaxes the parsing logic for
                                        //!   boolean and null types by accepting
                                        //!   True, TRUE, False, FALSE, Null, NULL
                                        //!   (in addition to true, false, null)
    };
    uint8_t flags;
    parse_mode() : flags(0) {}
    parse_mode(uint8_t _flags) : flags(_flags) {}
  };

  //! Members
  parse_mode mode;    //! Parser control modes
  dup_key    dupKey;  //! Duplicate key handling

  //! Default constructor
  parser_control(
    const parse_mode& _mode = parse_mode(),
    const dup_key&    _dupKey = dup_key::accept
    ) : mode(_mode), dupKey(_dupKey)
    {}
  //! One argment constructor
  parser_control(
    const dup_key&    _dupKey,
    const parse_mode& _mode = parse_mode()
    ) : mode(_mode), dupKey(_dupKey)
    {}
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
   * @fn bool parse(value&                _jout,
   *                parser_stats&         _stats,
   *                const std::string&    _value,
   *                const parser_control& _ctrl = parser_control()
   *               );
   * @brief Convert the given json string to json object
   *
   * @param _jout [out] json output
   * @param _stats [out] Parser statistics
   * @param _value [in] Input json string
   * @param _ctrl [in] Parser control flags
   */
  static bool parse(
    value&                _jout,
    const std::string&    _value,
    const parser_control& _ctrl = parser_control()
    );
  static bool parse(
    value&                _jout,
    parser_stats&         _stats,
    const std::string&    _value,
    const parser_control& _ctrl = parser_control()
    );

  // Constructors
  value(const value_type _type = value_type::null);
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

  bool empty() const { return m_type == value_type::null; }
  void clear();

  //! get the value_type
  value_type type() const { return m_type; }
  //! value_type check as functions
  bool is_null() const { return m_type == value_type::null; }
  bool is_string() const { return m_type == value_type::string; }
  bool is_signed() const { return m_type == value_type::_signed; }
  bool is_unsigned() const { return m_type == value_type::_unsigned; }
  bool is_decimal() const { return is_signed() || is_unsigned(); }
  bool is_double() const { return m_type == value_type::_double; }
  bool is_num() const { return is_decimal() || is_double(); }
  bool is_bool() const { return m_type == value_type::boolean; }
  bool is_array() const { return m_type == value_type::array; }
  bool is_object() const { return m_type == value_type::object; }
  bool is_basic_type() const { return ! ( is_array() || is_object() ); }
  bool is_complex_type() const { return ( is_array() || is_object() ); }

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
      m_type = m_data.init(value_type::array);
    }
    value& jval = append();
    jval.m_type = jval.m_data.init(_val);
    return jval;
  }

  //! Convert json to string using the given format type
  std::string to_str(const format_type _type = format_type::compact) const;
  //! Convert json to string using the given format
  std::string to_str(const format& _format) const;

  //! Write json to the given output stream
  void write(std::ostream& _out, const format_type _type = format_type::compact) const;
  //! Write json to the given output stream using pretty format
  void write(std::ostream& _out, const format& _format) const;

private:
  void p_write(std::ostream& _out, const format& _format, uint32_t _level) const;
  void p_set(const value_type _type = value_type::null);

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
    union_data(const value_type _type = value_type::null);
    //! Copy constructor
    union_data(const union_data& _obj, const value_type _type = value_type::null);
    //! Move constructor
    union_data(union_data&& _obj, const value_type _type = value_type::null) noexcept;
    //! Destructor
    ~union_data();

    //! Clear the memory used by the object
    value_type clear(const value_type _type);

    //! Copy initializer routines
    value_type init(const value_type _type = value_type::null);
    value_type init(const union_data& _obj, const bool _new = true,
                       const value_type _type = value_type::null);
    value_type init(const int _val);
    value_type init(const int64_t _val);
    value_type init(const uint64_t _val);
    value_type init(const long double _val);
    value_type init(const bool _val);
    value_type init(const std::string& _val);
    value_type init(const char* _val);
    value_type init(const array& _val);
    value_type init(const object& _val, const bool _new = true);
    //! Move initializer routine
    value_type init(union_data&& _obj, value_type _type) noexcept;

    union_data& operator=(const union_data& _obj) { *this = std::move(_obj); return *this; }
#if defined(SID_JSON_MAP_OPTIMIZE_FOR_SIZE)
    const object& map() const { return (*_map); }
    object& map() { return (*_map); }
#else
    const object& map() const { return _map; }
    object& map() { return _map; }
#endif
  };

  value_type m_type; //! Type of the object
  union_data m_data; //! The object
};

/**
 * @class schema_type
 * @brief json schema_type
 */
struct schema_type
{
  enum ID : uint8_t {
    null, object, array, string, boolean, number, integer
  };

public:
  ID id() const { return m_id; }
  std::string name() const;

  static bool get(const std::string& _name, /*out*/ schema_type& _type);
  static schema_type get(const std::string& _name);

public:
  schema_type() : m_id(schema_type::null) {}
  schema_type(const schema_type&) = default;
  schema_type(const schema_type::ID& _id) : m_id(_id) {}

  schema_type& operator=(const schema_type&) = default;
  schema_type& operator=(const schema_type::ID& _id) { m_id = _id; return *this; }
  //bool operator==(const schema_type& _obj) const = { return (m_id == _obj.m_id); }
  bool operator==(const schema_type::ID& _id) const { return (m_id == _id); }
  bool operator<(const schema_type& _obj) const { return (m_id < _obj.m_id); }

  void clear() { m_id = schema_type::null; }
  bool empty() const { return (m_id == schema_type::null); }

private:
  ID m_id;
};

struct schema_types : public std::set<schema_type>
{
  void add(const schema_type& _type) { this->insert(_type); }
  void add(const value& _value);
  bool exists(const schema_type& _type) const { return this->find(_type) != this->end(); }
  void remove(const schema_type& _type) { this->erase(_type); }
  value to_json() const;
};

/**
 * @class schema
 * @brief json schema class
 */
class schema
{
public:
  struct property;

  struct property_vec : public std::vector<property>
  {
    void set(const value& _jproperties);
    std::string to_str() const;
    value to_json() const;
  };

  struct property
  {
    std::string           key;
    std::string           description;
    schema_types          type;
    // For numbers
    sid::optional<int64_t> minimum;
    sid::optional<int64_t> exclusiveMinimum;
    sid::optional<int64_t> maximum;
    sid::optional<int64_t> exclusiveMaximum;
    sid::optional<int64_t> multipleOf;
    // For strings
    sid::optional<size_t> minLength;
    sid::optional<size_t> maxLength;
    std::string           pattern;
    // For arrays
    sid::optional<size_t> minItems;
    sid::optional<size_t> maxItems;
    sid::optional<bool>   uniqueItems;
    sid::optional<size_t> minContains;
    sid::optional<size_t> maxContains;
    // For objects
    sid::optional<size_t> minProperties;
    sid::optional<size_t> maxProperties;
    std::set<std::string> required;
    property_vec          properties;

    property();
    void clear();
    void set(const value& _jproperties, const std::string& _key);
    std::string to_str() const;
    value to_json() const;
  };

  //! Schema members
  std::string           _schema;     //! Json schema URI
  std::string           _id;         //! Identifier
  std::string           title;       //! Schema title
  std::string           description; //! Description of the schema
  schema_types          type;        //! Schema type
  property_vec          properties;  //! Properties associated with the schema
  std::set<std::string> required;    //! Required properties

  schema();
  void clear();
  std::string to_str() const;
  value to_json() const;

  static schema parse_file(const std::string& _schemaFile);
  static schema parse(const std::string& _schemaData);
  static schema parse(const value& _jroot);
};


#ifdef SID_JSON_MAP_OPTIMIZE_FOR_SIZE
#pragma pack(pop)
#endif

} // namespace json

std::string to_str(const json::value_type& _type);

} // namespace sid

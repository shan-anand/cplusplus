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

#ifndef _SID_CONVERT_H_
#define _SID_CONVERT_H_

#include <string>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <set>
#include "exception.hpp"

namespace sid {

//! Base type of a number
enum class num_base { any = 0, binary = 2, decimal = 10, hex = 16, octal = 8 };

//! Match case options
enum class match_case { exact, any, lower, upper, camel };

#define DEFAULT_NUM_BASE num_base::any

int is_binary(int c);
int is_octal(int c);
int is_decimal(int c);
int is_hex(int c);

//////////////////////////////////////////////////////////////////////////////////
//
// Boolean conversion functions
//
/**
 * @brief Function to convert boolean value to string.
 *
 * @param bool [IN] Boolean value to be converted
 *
 * @return string value of the decimal number
 *
 * @see num_base, to_bool()
 */
std::string to_str(const bool& _bVal, const match_case _matchCase = match_case::exact);
bool to_bool(const std::string& _input, const match_case _matchCase = match_case::exact) throw (sid::exception);
bool to_bool(const std::string& _input, const bool& _defValueOnError) noexcept;
bool to_bool(const std::string& _input, const match_case _matchCase, const bool& _defValueOnError) noexcept;
bool to_bool(const std::string& _input, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError = nullptr) noexcept;
bool to_bool(const std::string& _input, const match_case _matchCase, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError = nullptr) noexcept;
////////////////////////////////////////////////////////////////////////

/**
 * @brief Template function to convert any numerical value to string.
 *
 * @param _number: Any decimal value (char, uchar, short, ushort, int, uint, long, ulong, llong, ullong etc)
 * @param _baseType: Base type to convert to (decimal, binary, octal or hexadecimal)
 * @param _bShowBase: Show the base along with the string (0x for hexadecimal)
 *
 * @return string value of the decimal number
 *
 * @see num_base, to_num()
 */
template <typename T>
std::string to_str(const T& _number, const num_base& _baseType = num_base::any, bool _bShowBase = false)
{
  std::string res;
  T baseValue = static_cast<T>(( _baseType != num_base::any )? _baseType : num_base::decimal);
  T copy = _number < 0 ? -_number : _number;

  do
  {
    T val = (copy % baseValue);
    char ch = (char) (((val < 10)? '0':('A'-10)) + val);
    res.insert(0, 1, ch);
  }
  while ( (copy /= baseValue) != 0 );

  if ( _bShowBase )
  {
    switch ( _baseType )
    {
    case num_base::any:
    case num_base::decimal:
      break;
    case num_base::binary:
      res.insert(0, "0b");
      break;
    case num_base::octal:
      res.insert(0, "0");
      break;
    case num_base::hex:
      res.insert(0, "0x");
      break;
    }
  }
  if ( _number < 0 )
    res.insert(0, "-");
  return res;
}

//! Convert double to string
std::string to_str(const double& _number);

/**
 * @brief Template class to convert from string to decimal value. Throws an std::string exception on error.
 *        Valid datatypes can be char, short, int, long, long long and unsigned versions of these.
 *
 * @param _nptr: The character string to be converted to decimal.
 *              Leading and trailing blank spaces will be discarded during conversion.
 *              You can have a + or - as the first character.
 *              Examples: "9458" for Decimal, "0b110001001" for Binary, "0xAF80FF" for hexadecimal, "0755" for Octal.
 *              The prefixes can be in upper case or lowercase "0b" or "0B" for Binary, "0x" or "0X" for hexadecimal.
 * @param _baseType: Base of the number (any, decimal, binary, octal or hexadecimal)
 *                  If the type is num_base::any, then the type is determined by the prefix of the string.
 *                  If the base type is not num_base::any, then the prefix is not required.
 *
 * @return if successful it returns the numeric value of the string. if unsuccessful it throws a string exception.
 *
 * @see num_base, to_str()
 * 
 * @example to_num("+5000", num_base::any) and to_num("+5000", num_base::decimal) are same.
 *          to_num("0b101010", num_base::any), to_num("0b101010", num_base::binary), to_num("101010", num_base::binary) are all same.
 *          to_num("0xFF0E0A", num_base::any), to_num("0xFF0E0A", num_base::hex) to_num("FF0E0A", num_base::Hexadecimal) are all same.
 *          to_num("0755", num_base::any), to_num("0755", num_base::octal) to_num("755", num_base::octal) are all same.
 */
template <typename T>
T to_num(const char* _nptr, const num_base& _baseType) throw (sid::exception)
{
  typedef typename std::conditional<std::is_unsigned<T>::value, unsigned long long int, long long int>::type _num_type;
  _num_type res = 0;
  num_base base = _baseType;
  std::string csVal;

  try
  {
    if ( !_nptr ) throw EINVAL;
    const char* ps = _nptr;
    // discard the leading spaces
    while ( *ps == ' ' ) ps++;
    if ( *ps == '+' || *ps == '-' ) { csVal += *ps; ps++; }
    if ( *ps == '\0' ) throw EINVAL;

    const char* pe = ps+strlen(ps)-1;
    // discard the trailing spaces
    while ( isspace(*pe) ) pe--;

    // if it starts with 0x or 0X then it's hexadecimal
    if ( *ps == '0' && tolower(*(ps+1)) == 'x' )
    {
      base = num_base::hex;
      ps+=2;
    }
    // if it starts with 0b or 0B then it's binary
    else if ( *ps == '0' && tolower(*(ps+1)) == 'b' )
    {
      base = num_base::binary;
      ps+=2;
    }
    // if it starts with 0 and does not follow with 'x' or 'b', then it may be octal
    else if ( *ps == '0' && *(ps+1) != '\0' )
    {
      base = (_baseType == num_base::any)? num_base::octal : _baseType;
      ps++;
    }
    else
      base = (_baseType == num_base::any)? num_base::decimal : _baseType;

    if ( _baseType != num_base::any && _baseType != base )
    {
      base = _baseType;
      throw EINVAL;
    }

    for ( const char* p = ps; p <= pe; p++ )
    {
      switch ( base )
      {
      case num_base::decimal: if ( !sid::is_decimal(*p) ) throw EINVAL;  break;
      case num_base::hex:     if ( !sid::is_hex(*p) ) throw EINVAL; break;
      case num_base::octal:   if ( !sid::is_octal(*p) ) throw EINVAL;  break;
      case num_base::binary:  if ( !sid::is_binary(*p) ) throw EINVAL; break;
      default: break;
      }
      csVal += *p;
    }
    if ( *ps == '\0' || ps > pe) throw EINVAL;
    errno = 0;

    if ( std::is_unsigned<T>::value )
      res = strtoull(csVal.c_str(), nullptr, static_cast<int>(base));
    else
      res = strtoll(csVal.c_str(), nullptr, static_cast<int>(base));

    if ( !errno )
      errno = ( res != ((_num_type) ((T) res)) || (res < 0 && ((T) res) > 0) )? ERANGE:0;

    if ( errno ) throw errno;
  }
  catch (int iError)
  {
    errno = iError;
    char szBuf[256] = {0};
    const char* pbase = ( _baseType == num_base::any)? "":
                        ( base == num_base::decimal )? " in decimal base" :
                        ( base == num_base::hex )? " in hexadecimal base":
                        ( base == num_base::octal )? " in octal base":
                        ( base == num_base::binary )? " in binary base" : "";
    throw sid::exception(iError, std::string(strerror_r(errno, szBuf, sizeof(szBuf)-1)) + pbase + ": " + ((_nptr)? _nptr:"NULL"));
  }
  catch (const std::string& error)
  {
    throw sid::exception(error);
  }
  catch (...)
  {
    const char* pbase = ( _baseType == num_base::any )? "":
                        ( base == num_base::decimal )? " from decimal" :
                        ( base == num_base::hex )? " from hexadecimal":
                        ( base == num_base::octal )? " from octal":
                        ( base == num_base::binary )? " from binary" : "";
    throw sid::exception(std::string("An unhandled exception occurred while converting") + pbase + ": " + ((_nptr)? _nptr:"NULL"));
  }

  return (T) res;
}

template <typename T>
T to_num(const char* _nptr) throw (sid::exception)
{
  return to_num<T>(_nptr, DEFAULT_NUM_BASE);
}

/**
 * @brief Convert from std::string to decimal. Throws an std::string exception on error.
 */
template <typename T>
T to_num(const std::string& _csVal, const num_base& _baseType) throw (sid::exception)
{
  return to_num<T>(_csVal.c_str(), _baseType);
}

template <typename T>
T to_num(const std::string& _csVal) throw (sid::exception)
{
  return to_num<T>(_csVal.c_str(), DEFAULT_NUM_BASE);
}

/**
 * @brief Convert char* to decimal value. If there is an error _pcsError is set and the function returns false.
 */
template <typename T>
bool to_num(const char* _nptr, const num_base& _baseType, /*out*/ T& _outVal, /*out*/ std::string* _pcsError = nullptr)
{
  try { _outVal = to_num<T>(_nptr, _baseType); }
  catch (const sid::exception& e) { if ( _pcsError ) *_pcsError = e.what(); return false; }
  return true;
}

template <typename T>
bool to_num(const char* _nptr, /*out*/ T& _outVal, /*out*/ std::string* _pcsError = nullptr)
{
  return to_num<T>(_nptr, DEFAULT_NUM_BASE, _outVal, _pcsError);
}

/**
 * @brief Convert string to decimal value. If there is an error _pcsError is set and the function returns false.
 */
template <typename T>
bool to_num(const std::string& _csVal, const num_base& _baseType, /*out*/ T& _outVal, /*out*/ std::string* _pcsError = nullptr)
{
  return to_num<T>(_csVal.c_str(), _baseType, _outVal, _pcsError);
}

template <typename T>
bool to_num(const std::string& _csVal, /*out*/ T& _outVal, /*out*/ std::string* _pcsError = nullptr)
{
  return to_num<T>(_csVal.c_str(), DEFAULT_NUM_BASE, _outVal, _pcsError);
}

/**
 * @brief Convert string to decimal value. If there was an error in converting return the default value.
 *        It does not throw any exception.
 */
template <typename T>
T to_num(const char* _nptr, const num_base& _baseType, const T& _defaultValueOnError)
{
  try { return to_num<T>(_nptr, _baseType); }
  catch (const sid::exception& ) {}
  return _defaultValueOnError;
}

/**
 * @brief Convert string to decimal value. If there was an error in converting return the default value.
 *        It does not throw any exception.
 */
template <typename T>
T to_num(const std::string& _csVal, const num_base& _baseType, const T& _defaultValueOnError)
{
  try { return to_num<T>(_csVal.c_str(), _baseType); }
  catch (const sid::exception& ) {}
  return _defaultValueOnError;
}

template <> double to_num<double>(const char* _nptr) throw (sid::exception);
template <> double to_num<double>(const std::string& _csVal) throw (sid::exception);
template <> bool to_num<double>(const char* _nptr, /*out*/ double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/);
template <> bool to_num<double>(const std::string& _csVal, /*out*/ double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/);


bool equals(const std::string& _primary, const std::string& _secondary, const match_case& _matchCase);
bool equals(const std::string& _primary, const char* _secondary, const match_case& _matchCase);
bool equals(const char* _primary, const std::string& _secondary, const match_case& _matchCase);
bool equals(const char* _primary, const char* _secondary, const match_case& _matchCase);

namespace base64
{
  std::string encode(const std::string& _input);
  std::string encode(const char* _input, size_t _inputLen = std::string::npos);
  std::string decode(const std::string& _input) throw (sid::exception);
  std::string decode(const char* _input, size_t _inputLen = std::string::npos) throw (sid::exception);
}

namespace rc4
{
  bool convert(const std::string& _key, const std::string& _input, std::string& _output);
}

#define SPLIT_TRIM       1
#define SPLIT_SKIP_EMPTY 2
#define SPLIT_TRIM_SKIP_EMPTY (SPLIT_TRIM | SPLIT_SKIP_EMPTY)

enum class size_unit { B, KB, MB, GB, TB, PB };

std::string to_time_str(uint64_t _seconds, bool _include);
std::string to_size_str(uint64_t _bytes);
std::string to_speed_str(uint64_t _bytes, double _seconds);

bool to_size(const std::string& _input, uint64_t& _outVal, std::string* _pcsError = nullptr);
bool to_size(const std::string& _input, uint64_t& _outVal, const size_unit _eMaxUnitSize, std::string* _pcsError = nullptr);
bool to_size(const std::string& _input, uint64_t& _outVal, const uint64_t _defaultSize, std::string* _pcsError = nullptr);
bool to_size(const std::string& _input, uint64_t& _outVal, const uint64_t _defaultSize, const size_unit _eMaxUnitSize, std::string* _pcsError = nullptr);

  /**
   * @fn size_t split(T& _out, const std::string& _input, const char _sep, int _splitFlag);
   * @brief Splits the given string into tokens and stores it in one of the following containers
   *        (1) std::vector<std::string>
   *        (2) std::set<std::string>
   *
   * @param [OUT] _out : One of the above containers
   * @param [IN] _input : Input string that needs to be split
   * @param [IN] _sep : Character separator
   * @param [IN] _splitFlag : Split control flags. See SPLIT_xxx
   *
   * @return The number of values added to the container
   *       
   */
  //template <typename T> size_t split(T& out, const std::string& input, const char sep, int splitFlag = 0);
  size_t split(std::vector<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag = 0);
  size_t split(std::set<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag = 0);

  /**
   * @brief split the string to an array of strings using the given separator
   */
  int split(
    std::vector<std::string>& _result,   // the resulting vector array
    const std::string&        _input,    // the input string to be split
    const char&               _sep,      // the character separator
    std::string::size_type    _start = 0 // position in string to start
	);

  // Input functions
  int get_char_no_return(const char* _validChars = nullptr);
  int get_char_no_return_silent(const char* _validChars = nullptr);

  int get_char();
  int get_char(const char* _validChars);
  int get_char(const std::string& message, const char* _validChars);
  int get_char_silent();

  std::string get_string(int _maxLength = -1);
  std::string get_string_silent(int _maxLength = -1);

  // conversion functions
  std::string get_sep(size_t _number);

  // string manipulation functions
  std::string trim(const std::string& _input);
  std::string to_lower(const std::string& _input);
  std::string to_upper(const std::string& _input);

  std::string bytes_to_hex(const std::string& _input) throw (sid::exception);
  bool bytes_to_hex(const std::string& _input, std::string& _output, std::string* _pcsError = nullptr) noexcept;
  std::string hex_to_bytes(const std::string& _input) throw (sid::exception);
  bool hex_to_bytes(const std::string& _input, std::string& _output, std::string* _pcsError = nullptr) noexcept;
} // namespace sid

#endif // _SID_CONVERT_H_

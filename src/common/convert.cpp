/**
 * @file convert.cpp
 * @brief Generic conversion functions defined in convert.hpp
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

#include "common/convert.hpp"
#include <iostream>

// C++ includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>
#include <limits>

// curses and terminal IO includes
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

using namespace std;

#define WHITESPACE " \t\r\n"

template <>
double sid::to_num<double>(const char* _nptr)
{
  double res = 0;
  try
  {
    if ( !_nptr ) throw EINVAL;
    char* endptr = nullptr;
    errno = 0;
    res = strtod(_nptr, &endptr);
    if ( res == 0 )
    {
      if ( errno == ERANGE )
	throw errno;
      if ( endptr == _nptr )
	throw sid::exception("No conversion was performed");
    }
    else if ( res == HUGE_VALF || res == HUGE_VALL )
    {
      if ( errno == ERANGE )
	throw errno;
    }
  }
  catch (int iError)
  {
    errno = iError;
    char szBuf[256] = {0};
    throw sid::exception(iError, std::string(strerror_r(errno, szBuf, sizeof(szBuf)-1))  + " from double: " + ((_nptr)? _nptr:"NULL"));
  }
  catch (const std::string& error)
  {
    throw sid::exception(error);
  }
  catch (...)
  {
    throw sid::exception(std::string("An unhandled exception occurred while converting from double: ") + ((_nptr)? _nptr:"NULL"));
  }
  return res;
}

template <>
double sid::to_num<double>(const std::string& _csVal)
{
  return to_num<double>(_csVal.c_str());
}

template <>
bool sid::to_num<double>(const char* _nptr, /*out*/ double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/)
{
  try { _outVal = to_num<double>(_nptr); return true; }
  catch (const sid::exception& e) { if ( _pcsError ) *_pcsError = e.message(); }
  return false;
}

template <>
bool sid::to_num<double>(const std::string& _csVal, /*out*/ double& _outVal, /*out*/ std::string* _pcsError/* = nullptr*/)
{
  return to_num<double>(_csVal.c_str(), _outVal, _pcsError);
}

std::string sid::to_str(const long double& _number)
{
  std::ostringstream out;
  out << std::setprecision(std::numeric_limits<long double>::digits10) << _number;
  return out.str();
}

int sid::is_binary(int c) { return (c == '0' || c == '1')? 1 : 0; }

int sid::is_octal(int c) { return (c >= '0' && c <= '7')? 1 : 0; }

int sid::is_decimal(int c) { return (c >= '0' && c <= '9')? 1 : 0; }

int sid::is_hex(int c) { return isxdigit(c); }

std::string sid::trim(const std::string& _input)
{
  std::string csOutput;

  std::string::size_type iPos = _input.find_first_not_of(WHITESPACE, 0);

  if ( iPos != std::string::npos )
  {
    int iStartPos = iPos;
    iPos = _input.find_last_not_of(WHITESPACE, string::npos);
    if ( iPos == std::string::npos )
      csOutput = _input.substr(iStartPos, _input.length() - iStartPos);
    else
      csOutput = _input.substr(iStartPos, iPos - iStartPos + 1);
  }

  return csOutput;
}

std::string sid::to_lower(const std::string& _input)
{
  std::string csOutput;

  for ( size_t i = 0; i < _input.size(); i++ )
  {
    char ch = tolower(_input[i]);
    csOutput.append(1, ch);
  }

  return csOutput;

  /*
  std::string csOutput = _input;
  std::transform(_input.begin(), _input.end(),
		 csOutput.begin(),
		 (int(*)(int)) std::tolower);
  */
}

std::string sid::to_upper(const std::string& _input)
{
  std::string csOutput;

  for ( size_t i = 0; i < _input.size(); i++ )
  {
    char ch = tolower(_input[i]);
    csOutput.append(1, ch);
  }

  return csOutput;

  /*
  std::string csOutput = _input;
  std::transform(_input.begin(), _input.end(),
		 csOutput.begin(),
		 (int(*)(int)) std::toupper);
  */
}

//////////////////////////////////////////////////////////////////////////////////
//
// Conversion between Bytes and Hex values
//
std::string sid::bytes_to_hex(const std::string& _input)
{
  std::ostringstream out;
  std::string csHex;

  for ( size_t i = 0; i < _input.length(); i++ )
  {
    unsigned char ch = (unsigned char) _input[i];
    csHex = sid::to_str(ch, num_base::hex);
    if ( csHex.length() == 1 ) csHex = "0" + csHex;
    out << csHex;
  }
  return out.str();
}

bool sid::bytes_to_hex(const std::string& _input, std::string& _output, std::string* _pcsError/* = nullptr*/) noexcept
{
  try
  {
    _output = sid::bytes_to_hex(_input);
  }
  catch (const sid::exception& e)
  {
    if ( _pcsError ) *_pcsError = e.what();
  }
  catch (...)
  {
    if ( _pcsError ) *_pcsError = std::string(__func__) + "An unhandled exeption occurred";
  }
  return true;
}

std::string sid::hex_to_bytes(const std::string& _input)
{
  std::ostringstream out;
  unsigned char ch = 0;
  std::string csError;

  for ( size_t i = 0; i < _input.length(); )
  {
    char szHex[3] = {0};
    szHex[0] = _input[i++]; szHex[1] = _input[i++];
    if ( ! sid::to_num(szHex, num_base::hex, /*out*/ ch, &csError) )
      throw sid::exception(std::string(__func__) + ": " + csError);
    out << ((char) ch);
  }
  return out.str();
}

bool sid::hex_to_bytes(const std::string& _input, std::string& _output, std::string* _pcsError/* = nullptr*/) noexcept
{
  bool bStatus = false;

  try
  {
    _output = sid::hex_to_bytes(_input);
    bStatus = true;
  }
  catch (const sid::exception& e)
  {
    if ( _pcsError ) *_pcsError = e.what();
  }
  catch (...)
  {
    if ( _pcsError ) *_pcsError = std::string(__func__) + "An unhandled exeption occurred";
  }
  return bStatus;
}

//////////////////////////////////////////////////////////////////////////////////
//
// Boolean conversion functions
//
std::string sid::to_str(const bool& _bVal, const match_case _matchCase /*= match_case::exact*/)
{
  std::string val = (_bVal)? "true":"false";
  switch ( _matchCase )
  {
  case match_case::exact:
  case match_case::any:
  case match_case::lower:
    // The default of all the above cases is lower-case
    break;
  case match_case::upper:
    val = sid::to_upper(val);
    break;
  case match_case::camel:
    // Convert the entry to camel case
    val[0] = (char) toupper(val[0]);
    break;
  }
  return val;
}

bool sid::to_bool(const std::string& _input, const match_case _matchCase)
{
  const char* valid_values[5] = {"true", "false", nullptr};
  for ( size_t i = 0; valid_values[i] != nullptr; i++ )
  {
    bool found = true;
    const char* entry = valid_values[i];
    switch ( _matchCase )
    {
    case match_case::exact:
      found = ( _input == entry );
      break;
    case match_case::any:
      found = ( strcasecmp(_input.c_str(), entry) == 0 );
      break;
    case match_case::lower:
      found = ( _input == sid::to_lower(entry) );
      break;
    case match_case::upper:
      found = ( _input == sid::to_upper(entry) );
      break;
    case match_case::camel:
      // Convert the entry to camel case
      found = ( toupper(entry[0]) == _input[0] && strcmp(&entry[1], _input.c_str()+1) == 0 );
      break;
    }
    if ( found ) return (i == 0);
  }
  throw sid::exception(std::string("Invalid value \"") + _input + "\". Must be true or false");
}

bool sid::to_bool(const std::string& _input, const bool& _defValueOnError) noexcept
{
  return sid::to_bool(_input, match_case::exact, _defValueOnError);
}

bool sid::to_bool(const std::string& _input, const match_case _matchCase, const bool& _defValueOnError) noexcept
{
  bool outVal = _defValueOnError;
  sid::to_bool(_input, _matchCase, outVal);
  return outVal;
}

bool sid::to_bool(const std::string& _input, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError /*= nullptr*/) noexcept
{
  return sid::to_bool(_input, match_case::exact, _outVal, _pcsError);
}

bool sid::to_bool(const std::string& _input, const match_case _matchCase, /*out*/ bool& _outVal, /*out*/ std::string* _pcsError /*= nullptr*/) noexcept
{
  try { _outVal = sid::to_bool(_input, _matchCase); }
  catch (const sid::exception& e) { if ( _pcsError ) *_pcsError = e.what(); return false; }
  return true;
}

bool sid::equals(const std::string& _primary, const std::string& _secondary, const match_case& _matchCase)
{
  return sid::equals(_primary.c_str(), _secondary.c_str(), _matchCase);
}

bool sid::equals(const std::string& _primary, const char* _secondary, const match_case& _matchCase)
{
  return sid::equals(_primary.c_str(), _secondary, _matchCase);
}

bool sid::equals(const char* _primary, const std::string& _secondary, const match_case& _matchCase)
{
  return sid::equals(_primary, _secondary.c_str(), _matchCase);
}

bool sid::equals(const char* _primary, const char* _secondary, const match_case& _matchCase)
{
  if ( _primary == _secondary ) return true;
  if ( !_primary || !_secondary ) return false;
  const char* a1 = _primary;
  const char* a2 = _secondary;
  switch ( _matchCase )
  {
  case match_case::exact:
    for ( ; *a1 != '\0' && *a2 != '\0'; ++a1, ++a2)
      if ( *a1 != *a2 ) return false;
    break;
  case match_case::any:
    for ( ; *a1 != '\0' && *a2 != '\0'; ++a1, ++a2)
      if ( toupper(*a1) != toupper(*a2) ) return false;
    break;
  case match_case::upper:
    for ( ; *a1 != '\0' && *a2 != '\0'; ++a1, ++a2)
      if ( *a2 != toupper(*a2) || toupper(*a1) != toupper(*a2)) return false;
    break;
  case match_case::camel:
    if ( *a2 != toupper(*a2) || toupper(*a1) != toupper(*a2)) return false;
    if ( *a1 == '\0' || *a2 == '\0' ) break;
    ++a1; ++a2;
    // rest of the checking is lowercase checking
  case match_case::lower:
    for ( ; *a1 != '\0' && *a2 != '\0'; ++a1, ++a2)
      if ( *a2 != tolower(*a2) || tolower(*a1) != tolower(*a2)) return false;
    break;
  }
  return ( *a1 == '\0' && *a2 == '\0' );
}

//////////////////////////////////////////////////////////////////////////////////
//
// base64 conversion functions
//
const static char encodeLookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static char padCharacter = '=';

std::string sid::base64::encode(const std::string& _input)
{
  std::ostringstream out;

  uint32_t temp;
  size_t i = 0;
  for ( size_t idx = 0; idx < _input.size()/3; idx++ )
  {
    temp  = ( (unsigned char) _input[i++]) << 16; // Convert to big endian
    temp += ( (unsigned char) _input[i++]) << 8;
    temp += ( (unsigned char) _input[i++]);
    out << encodeLookup[(temp & 0x00FC0000) >> 18]
        << encodeLookup[(temp & 0x0003F000) >> 12]
        << encodeLookup[(temp & 0x00000FC0) >> 6 ]
        << encodeLookup[(temp & 0x0000003F)      ];
  }

  switch( _input.size() % 3 )
  {
  case 1:
    temp  = ( (unsigned char) _input[i++]) << 16; // Convert to big endian
    out << encodeLookup[(temp & 0x00FC0000) >> 18]
        << encodeLookup[(temp & 0x0003F000) >> 12]
        << padCharacter
        << padCharacter;
    break;
  case 2:
    temp  = ( (unsigned char) _input[i++]) << 16; // Convert to big endian
    temp += ( (unsigned char) _input[i++]) << 8;
    out << encodeLookup[(temp & 0x00FC0000) >> 18]
        << encodeLookup[(temp & 0x0003F000) >> 12]
        << encodeLookup[(temp & 0x00000FC0) >> 6 ]
        << padCharacter;
    break;
  }

  return out.str();
}

std::string sid::base64::encode(const char* _input, size_t _inputLen)
{
  if ( _input && _inputLen == std::string::npos )
    _inputLen = strlen(_input);
  std::string csInput(_input, _inputLen);
  return encode(csInput);
}

std::string sid::base64::decode(const std::string& _input)
{
  std::ostringstream out;

  if ( _input.length() % 4 != 0 )
    throw sid::exception(EINVAL, "base64::decode: Invalid input");

  // allocate memory on the stack
  char* decoding_table = (char *) alloca(256);
  if ( decoding_table == NULL )
    throw sid::exception(ENOMEM, "base64::decode: Unable to allocate memory");

  for ( size_t x = 0; x < 64; x++ )
    decoding_table[(unsigned char) encodeLookup[x]] = x;

  size_t input_length = _input.length();
  size_t output_length = _input.length() / 4 * 3;
  if ( _input[input_length - 1] == padCharacter ) (output_length)--;
  if ( _input[input_length - 2] == padCharacter ) (output_length)--;

  for ( size_t i = 0, j = 0; i < input_length; )
  {
    uint32_t sextet_a = (_input[i] == padCharacter)? 0 & i++ : decoding_table[(unsigned char) _input[i++]];
    uint32_t sextet_b = (_input[i] == padCharacter)? 0 & i++ : decoding_table[(unsigned char) _input[i++]];
    uint32_t sextet_c = (_input[i] == padCharacter)? 0 & i++ : decoding_table[(unsigned char) _input[i++]];
    uint32_t sextet_d = (_input[i] == padCharacter)? 0 & i++ : decoding_table[(unsigned char) _input[i++]];

    uint32_t triple = (sextet_a << 3 * 6)
      + (sextet_b << 2 * 6)
      + (sextet_c << 1 * 6)
      + (sextet_d << 0 * 6);

    if ( j < output_length ) { ++j; out << (char) ((triple >> 2 * 8) & 0xFF); }
    if ( j < output_length ) { ++j; out << (char) ((triple >> 1 * 8) & 0xFF); }
    if ( j < output_length ) { ++j; out << (char) ((triple >> 0 * 8) & 0xFF); }
  }

  return out.str();
}

std::string sid::base64::decode(const char* _input, size_t _inputLen)
{
  if ( _input && _inputLen == std::string::npos )
    _inputLen = strlen(_input);
  std::string csInput(_input, _inputLen);
  return decode(csInput);
}

#define RC4_BYTES 256

namespace sid { namespace rc4 {
  struct context
  {
    unsigned char S[RC4_BYTES];
    int i, j;

    bool init(const std::string& _key);
    unsigned char get_byte();
  };
}}

bool sid::rc4::convert(const std::string& _key, const std::string& _input, std::string& _output)
{
  sid::rc4::context ctx;

  if ( ! ctx.init(_key) )
    return false;

  _output = _input;
  for ( size_t x = 0; x < _input.length(); x++ )
    _output[x] ^= ctx.get_byte();

  return true;
}

bool sid::rc4::context::init(const std::string& _key)
{
  if ( _key.empty() || _key.length() > RC4_BYTES )
    return false;

  for ( i = 0; i < RC4_BYTES; i++ )
    S[i] = (unsigned char) i;

  for ( i = j = 0; i < RC4_BYTES; i++ )
  {
    j = (j + S[i] + _key[i % _key.length()]) % RC4_BYTES;
    std::swap(S[i], S[j]);
  }

  // Initialize i, j to 0
  i = j = 0;

  return true;
}

unsigned char sid::rc4::context::get_byte()
{
  i = (i + 1) % RC4_BYTES;
  j = (j + S[i]) % RC4_BYTES;
  std::swap(S[i], S[j]);
  return S[(S[i] + S[j]) % RC4_BYTES];
}

//////////////////////////////////////////////////////////////
void _internal_split_add_string(std::vector<std::string>& out, const std::string& val) { out.push_back(val); }
void _internal_split_add_string(std::set<std::string>& out, const std::string& val) { out.insert(val); }

/**
 * @fn size_t split(T& out, const std::string& input, const char sep, int splitFlag);
 * @brief Splits the given string into tokens and stores it in one of the following containers
 *        (1) std::vector<std::string>
 *        (2) std::set<std::string>
 *
 * @param [OUT] out : One of the above containers
 * @param [IN] input : Input string that needs to be split
 * @param [IN] sep : Character separator
 * @param [IN] splitFlag : Split control flags. See SPLIT_xxx
 *
 * @return The number of values added to the container
 *       
 */
template <class T> size_t _internal_split(T& out, const std::string& input, const char sep, int splitFlag)
{
  size_t count = 0;
  if ( !input.empty() )
  {
    std::string temp;
    size_t p2, p1 = 0;
    do
    {
      p2 = input.find(sep, p1);
      if ( p2 == std::string::npos )
        temp = input.substr(p1);
      else
      {
        temp = input.substr(p1, p2-p1);
        p1 = p2+1;
      }
      if ( splitFlag & SPLIT_TRIM ) sid::trim(temp);
      if ( ! ((splitFlag & SPLIT_SKIP_EMPTY) && temp.empty()) )
      {
        _internal_split_add_string(out, temp);
        ++count;
      }
    }
    while ( p2 != std::string::npos );
  }
  return count;
}

std::string sid::to_time_str(uint64_t _seconds, bool _include)
{
  uint64_t uData = static_cast<uint64_t>(_seconds);
  struct { int days, hours, mins, secs; } ts = {0};

#define XTIME_SET(var, f) var = uData % (f); uData /= (f)
  XTIME_SET(ts.secs, 60);
  XTIME_SET(ts.mins, 60);
  XTIME_SET(ts.hours, 24);
  ts.days = static_cast<int>(uData);
#undef XTIME_SET

  std::ostringstream out;
  if ( ts.days > 0 )
    out << ts.days << "-";
  out << std::setfill('0') << std::setw(2) << ts.hours
      << ":" << std::setfill('0') << std::setw(2) << ts.mins
      << ":" << std::setfill('0') << std::setw(2) << ts.secs;
  if ( _include )
    out << " (" << _seconds << ")";

  return out.str();
}

std::string sid::to_size_str(uint64_t _bytes)
{
  double dBytes = (double) _bytes;
  char units[] = {' ', 'K', 'M', 'G', 'T', 'P'};
  int up = 0;
  while ( dBytes >= 1024 )
  {
    up++;
    dBytes /= 1024;
    if ( up == (sizeof(units)/sizeof(units[0])) ) break;
  }
  std::ostringstream out;
  out << std::fixed << std::setprecision(2) << dBytes << " " << units[up] << "B";
  return out.str();
}

std::string sid::to_speed_str(uint64_t _bytes, double _seconds)
{
  double dSpeed = (_seconds <= 0)? 0 : (_bytes / _seconds);
  char units[] = {' ', 'k', 'm', 'g', 't', 'p'};
  int up = 0;
  while ( dSpeed >= 1024 )
  {
    up++;
    dSpeed /= 1024;
    if ( up == (sizeof(units)/sizeof(units[0])) ) break;
  }
  std::ostringstream out;
  out << std::fixed << std::setprecision(2) << dSpeed << " " << units[up] << "bps";
  return out.str();
}

size_t sid::split(std::vector<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag)
{
  return _internal_split(_out, _input, _sep, _splitFlag);
}

size_t sid::split(std::set<std::string>& _out, const std::string& _input, const char _sep, int _splitFlag)
{
  return _internal_split(_out, _input, _sep, _splitFlag);
}

/**
 * @brief split the string to an array of strings using the given separator
 */
int sid::split(std::vector<std::string>& _result, const string& _input, const char& _sep, string::size_type _start)
{
  std::string::size_type pos, newPos;
  for ( pos = _start; (newPos = _input.find(_sep, pos)) != std::string::npos ; pos = newPos+1 )
  {
    _result.push_back(_input.substr(pos, newPos-pos));
  }
  _result.push_back(_input.substr(pos));
  return _result.size();
}

#define CONTROL(x)  ((x) & 0x1F)

int sid::get_char_no_return(const char* _validChars)
{
  int ch = sid::get_char_no_return_silent(_validChars);
  cout << (char) ch;
  return ch;
}

int sid::get_char_no_return_silent(const char* _validChars)
{
  char ch;

  termios ors, nrs;

  // get current settings
  tcgetattr(fileno(stdin), &ors);
  // make a copy of the current settings
  nrs = ors;
  // turn off the echo flag
  nrs.c_lflag = 0;
  nrs.c_cc[VTIME] = 0;
  nrs.c_cc[VMIN] = 1;

  // disable ECHO and do the read
  //tcsetattr(fileno(stdin), TCSAFLUSH, &nrs);
  tcflush(fileno(stdin), TCIFLUSH);
  tcsetattr(fileno(stdin), TCSANOW, &nrs);
  int validCharsLength = ( _validChars != nullptr )? strlen(_validChars):0;
  while ( true )
  {
    ch = getchar();
    if (validCharsLength == 0) break;
    if (strchr(_validChars, ch) != nullptr) break;
  }

  //re-enable echo by applying old settings
  //tcsetattr(fileno(stdin), TCSAFLUSH, &ors);
  tcsetattr(fileno(stdin), TCSANOW, &ors);

  return ch;
}

int sid::get_char()
{
  std::string input = sid::get_string();
  return (input.length() == 1)? input[0]:0;
  /*
  int ch, d = '\0';
  for (ch=d=getchar(); d != '\n' && d != EOF; d=getchar());
  return ch;
  */
}

int sid::get_char(const char* _validChars)
{
  int ch;
  int validCharsLength = ( _validChars != nullptr )? strlen(_validChars):0;
  while ( true )
  {
    ch = get_char();
    if ( validCharsLength == 0 ) break;
    if ( ch != 0 && strchr(_validChars, ch) != nullptr ) break;
  }
  return ch;
}

int sid::get_char(const std::string& _message, const char* _validChars)
{
  int ch;
  int validCharsLength = ( _validChars != nullptr )? strlen(_validChars):0;
  while ( true )
  {
    cout << _message;
    ch = get_char();
    if ( validCharsLength == 0 ) break;
    if ( ch != 0 && strchr(_validChars, ch) != nullptr ) break;
  }
  return ch;
}

int sid::get_char_silent()
{
  char ch;

  termios ors, nrs;

  // get current settings
  tcgetattr(fileno(stdin), &ors);
  // make a copy of the current settings
  nrs = ors;
  // turn off the echo flag
  nrs.c_lflag = ~ECHO;

  // disable ECHO and do the read
  tcsetattr(fileno(stdin), TCSAFLUSH, &nrs);
  ch = get_char();

  //re-enable echo by applying old settings
  tcsetattr(fileno(stdin), TCSAFLUSH, &ors);

  return ch;
}

std::string sid::get_string(int _maxLength)
{
  std::string output;
  int i, ch = '\0';
  // a maxLength of -1 is valid and means any length of input
  if ( _maxLength < -1 )
    return output;
  for ( i = 0; (ch = getchar()) != '\n'; i++ )
  {
    if ( ch == KEY_UP ) { output.clear(); break; }
    if ( ch == EOF || ch == CONTROL('d') || ch == CONTROL('D') )
    {
      output = "exit";
      break;
    }
    // only save the characters up to maxLength
    if ( _maxLength == -1 || i < _maxLength )
        output += ch;
    if ( ch == '\b' && i != 0 ) i--;
  }
  return output;
}

std::string sid::get_string_silent(int _maxLength)
{
  std::string output;

  termios ors, nrs;

  // get current settings
  tcgetattr(fileno(stdin), &ors);
  // make a copy of the current settings
  nrs = ors;
  // turn off the echo flag
  nrs.c_lflag = ~ECHO;

  // disable ECHO and do the read
  tcsetattr(fileno(stdin), TCSAFLUSH, &nrs);
  output = get_string(_maxLength);

  //re-enable echo by applying old settings
  tcsetattr(fileno(stdin), TCSAFLUSH, &ors);

  return output;
}

// Takes a number like 11115 and adds
// commas like 11,115
std::string sid::get_sep(size_t _number)
{
  std::string out = sid::to_str(_number);
  for ( int i = out.length()-3; i > 0; i -= 3 )
    out.insert(i, ",");
  return out;
}

///////////////////////////////
struct size_unit_map
{
  const sid::size_unit eUnit;
  const char*          name;
};

static size_unit_map g_size_unit_map[] =
  {
    {sid::size_unit::B,  "B"},
    {sid::size_unit::KB, "KB"},
    {sid::size_unit::MB, "MB"},
    {sid::size_unit::GB, "GB"},
    {sid::size_unit::TB, "TB"},
    {sid::size_unit::PB, "PB"}
  };

static bool s_to_size(
  bool                 _supportDefault,
  std::string          _input,
  uint64_t&            _outVal,
  const uint64_t       _defaultSize,
  const sid::size_unit _eMaxUnitSize,
  std::string*         _pcsError
  )
{
  bool bStatus = false;

  try
  {
    std::string csVal = sid::trim(_input);
    _outVal = _defaultSize;
    uint64_t factor = 1;
    if ( csVal.empty() ) return _defaultSize;
    if ( csVal == "default" )
    {
      if ( !_supportDefault )
        throw sid::exception(std::string("\"default\" cannot be supported if sid::to_size() is not accompanied by defaultSize value"));
      return _defaultSize;
    }

    char suffix[3] = {0};
    char ch1 = csVal[csVal.length()-1];
    if ( !isdigit(ch1) )
    {
      int i = 0;
      char ch2 = (csVal.length() > 1)? csVal[csVal.length()-2] : ' ';
      if ( !isdigit(ch2) && ch2 != ' ' ) suffix[i++] = ch2;
      suffix[i++] = ch1;
      suffix[i] = '\0';

      factor = 0;
      for ( size_t i = 0; i < sizeof(g_size_unit_map)/sizeof(g_size_unit_map[0]); i++ )
      {
        if ( strcmp(suffix, g_size_unit_map[i].name) == 0 || suffix[0] == g_size_unit_map[i].name[0] )
        {
          factor = static_cast<uint64_t>(pow(1024, i));
          break;
        }
        // Check only till this unit
        if ( _eMaxUnitSize == g_size_unit_map[i].eUnit ) break;
      }

      if ( factor == 0 )
      {
        std::string validUnits;
        for ( size_t i = 0; i < sizeof(g_size_unit_map)/sizeof(g_size_unit_map[0]); i++ )
        {
          if ( !validUnits.empty() ) validUnits += "|";
          validUnits += g_size_unit_map[i].name;
          if ( _eMaxUnitSize == g_size_unit_map[i].eUnit ) break;
        }
        throw sid::exception(std::string("Invalid unit [") + suffix + std::string("]. Must be ") + validUnits);
      }

      size_t suffixLen = strlen(suffix);
      csVal = csVal.substr(0, csVal.length()-suffixLen);
    }

    std::string csError;
    if ( !sid::to_num(csVal, _outVal, &csError) )
      throw sid::exception(csError);
    _outVal *= factor;

    // Set the return value to true
    bStatus = true;
  }
  catch (const sid::exception& e)
  {
    if ( _pcsError ) *_pcsError = e.what();
  }
  catch (...)
  {
    if ( _pcsError ) *_pcsError = std::string("An unhandled exception occured while trying to convert: ") + _input;
  }

  return bStatus;
}

#define DEFAULT_MAX_BYTE_UNIT sid::size_unit::PB

bool sid::to_size(const std::string& _input, uint64_t& _outVal, std::string* _pcsError)
{
  return s_to_size(false, _input, _outVal, 0, DEFAULT_MAX_BYTE_UNIT, _pcsError);
}

bool sid::to_size(const std::string& _input, uint64_t& _outVal, const size_unit _eMaxUnitSize, std::string* _pcsError)
{
  return s_to_size(false, _input, _outVal, 0, _eMaxUnitSize, _pcsError);
}

bool sid::to_size(const std::string& _input, uint64_t& _outVal, const uint64_t _defaultSize, std::string* _pcsError)
{
  return s_to_size(true, _input, _outVal, _defaultSize, DEFAULT_MAX_BYTE_UNIT, _pcsError);
}

bool sid::to_size(const std::string& _input, uint64_t& _outVal, const uint64_t _defaultSize, const size_unit _eMaxUnitSize, std::string* _pcsError)
{
  return s_to_size(true, _input, _outVal, _defaultSize, _eMaxUnitSize, _pcsError);
}

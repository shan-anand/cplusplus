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

#include "common/regex.hpp"
#include <string.h>
#include <alloca.h>
#include <iostream>

using namespace std;
using namespace sid;

bool regex::result::exists(const size_t _key, std::string* pValue/* = nullptr*/) const
{
  result::const_iterator it;
  if ( (it = this->find(_key)) == this->end() )
    return false;
  if ( pValue ) *pValue = it->second;
  return true;
}

/**
 * @brief Regular Expression class constructor. It compiles the regular expression using the
 *        regular expression library function and set the error code and initialization flag.
 *
 * @param _pattern [in] POSIX Regular Expression
 * @param _cflags [in] flags that control the execution. Defaults to REG_EXTENDED.
 *
 * @seealso initialized(), error_code(), error(), Exex()
 */
regex::regex(const char* _pattern, int _cflags)
{
  m_pattern = _pattern;
  ::memset(&m_buffer, 0, sizeof(m_buffer));
  // call the library function to compile the regular expression.
  m_errorCode = ::regcomp(&m_buffer, _pattern, _cflags);
  m_isInitialized = ( m_errorCode == 0 );
}

//! Destructor
regex::~regex()
{
  // call the library function to release the buffer allocated by regcomp()
  ::regfree(&m_buffer);
}

//! Returns the error string associated with the last action on the object.
std::string regex::error() const
{
  return error(m_errorCode);
}

//! Returns the error string associated with the given error number.
std::string regex::error(const int _errorCode) const
{
  std::string csError;
  // call the library function with NULL parameter to get the length of the error string.
  size_t uLen = ::regerror(_errorCode, &m_buffer, NULL, 0);
  if ( uLen > 0 )
  {
    // allocate memory on the stack to get the error string. This memory will be deleted automatically when the function returns.
    char* pszErr = (char*) alloca(uLen+1);
    // call the library function to fill the error string
    uLen = ::regerror(_errorCode, &m_buffer, pszErr, uLen);
    pszErr[uLen] = '\0';
    csError = pszErr;
  }
  return csError;
}

//! Returns the regular expression string
const std::string& regex::pattern() const
{
  return m_pattern;
}

/**
 * @fn exec(const char* _input)
 *
 * @brief Check the given input against the regular expression and return whether it matches or not.
 *
 * @param _input [in] Input string to be checked
 *
 * @returns true if the input matches the regular expression, false otherwise
 *
 * @seealso error_code(), error()
 */
bool regex::exec(const char* _input)
{
  return ( p_exec(_input, nullptr) == 0 );
}

/**
 * @fn exec(const char* _input, result& _result)
 *
 * @brief Check the given input against the regular expression and return whether it matches or not.
 *        It also returns the result in a string vector.
 *
 * @param _input [in] Input string to be checked
 * @param _result [out] String vector that holds the resulting match on return.
 *
 * @returns true if the input matches the regular expression, false otherwise
 *
 * @seealso error_code(), error()
 */
bool regex::exec(const char* _input, result& _result)
{
  return ( p_exec(_input, &_result) == 0 );
}

/**
 * @fn exec(const char* _pattern, const char* _input, std::string* _pcsError)
 *
 * @brief static function used to check whether given input string matches the regular expression or not
 *        without manually creating the regex object. It returns the error code associated with the check
 *        and optionally the error string, if requested.
 *
 * @param _pattern [in] POSIX Regular Expression
 * @param _input [in] Input string to be checked
 * @param _pcsError [out] A pointer to std::string that'll return the error string in case of an error. Can be NULL if error string is not required.
 *
 * @returns 0 on success or an error code returned by regex library function in case of an error.
 */
/*static*/
int regex::exec(const char* _pattern, const char* _input, std::string* _pcsError)
{
  regex regEx(_pattern);
  regEx.exec(_input);
  if ( _pcsError ) *_pcsError = regEx.error();
  return regEx.error_code();
}

/**
 * @fn exec(const char* _pattern, const char* _input, result& _result, std::string* _pcsError)
 *
 * @brief static function used to check whether given input string matches the regular expression or not
 *        without manually creating the regex object and returns the result in a string vector.
 *        It returns the error code associated with the check and optionally the error string, if requested.
 *
 * @param _pattern [in] POSIX Regular Expression
 * @param _input [in] Input string to be checked
 * @param _result [out] String vector that holds the resulting match on return.
 * @param _pcsError [out] A pointer to std::string that'll return the error string in case of an error. Can be NULL if error string is not required.
 *
 * @returns 0 on success or an error code returned by regex library function in case of an error.
 */
/*static*/
int regex::exec(const char* _pattern, const char* _input, result& _result, std::string* _pcsError)
{
  regex regEx(_pattern);
  regEx.p_exec(_input, &_result);
  if ( _pcsError ) *_pcsError = regEx.error();
  return regEx.error_code();
}

/**
 * @fn p_exec(const char* _input, result* _result)
 *
 * @brief Private function that actually checks the given input against the regular expression and return whether it matches or not.
 *        It also returns the result in a string vector, if requested
 *
 * @param _input [in] Input string to be checked
 * @param _result [out] Pointer to string vector that holds the resulting match on return. Can be NULL if result is not required.
 *
 * @returns 0 on success or an error code returned by regex library function in case of an error.
 *
 * @seealso exec(), error_code(), error()
 */
int regex::p_exec(const char* _input, result* _result)
{
  if ( !m_isInitialized ) return m_errorCode;

  // we limit the match enties to 1000 (as defined in REGEX_MATCH_SIZE in regex.hpp)
  regmatch_t match[REGEX_MATCH_SIZE];
  ::memset(match, 0, sizeof(match));

  // call the library function to check the input string against the regular expression
  m_errorCode = ::regexec(&m_buffer, _input, (_result? REGEX_MATCH_SIZE:0), match, 0);
  if ( m_errorCode == 0 && _result )
  {
    // fill the result in the string vector
    _result->clear();
    for ( size_t i = 0; i < REGEX_MATCH_SIZE; i++ )
    {
      regmatch_t& m = match[i];
      if ( m.rm_so == -1 ) continue;
      std::string data(&_input[m.rm_so], m.rm_eo-m.rm_so);
      _result->insert(std::pair<size_t, std::string>(i, data));
    }
  }
  return m_errorCode;
}

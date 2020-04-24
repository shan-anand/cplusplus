/*
LICENSE: BEGIN
===============================================================================
@author Shan Anand
@email anand.gs@gmail.com
@source https://github.com/shan-anand
@brief C++ Wrapper for pcre regular expression
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
#ifndef _SID_REGEX_H_
#define _SID_REGEX_H_

#include <sys/types.h>
#include <regex.h>
#include <map>
#include <string>

#define REGEX_MATCH_SIZE 1000

#define REGEX_DATE_YYYYMMDD "^([0-9]{4})/([0-9]{2})/([0-9]{2})$" // (YYYY)/(MM)/(DD)
#define REGEX_DATE_DDMMYYYY "^([0-9]{2})/([0-9]{2})/([0-9]{4})$" // (DD)/(MM)/(YYYY)
#define REGEX_TIME_HHMMSS   "^([0-9]{2}):([0-9]{2}):([0-9]{2})$" // (HH):(MM):(SS)
#define REGEX_IPADDRESS     "^([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})$" // (aaa).(bbb).(ccc).(ddd)
#define REGEX_FQDN          "^([^\.]+)\.(.+)$" // (host).(domain)
#define REGEX_NETBIOS_NAME  "^([a-zA-z][a-zA-Z0-9\-_]*[a-zA-Z0-9])$" // (netbiosname)
// Offline Path:  "^(nfs|cifs|http)://([^/]+)/(.+)"
// Export:        "^([^:]+):/(.*)"
// Share:         "^\\\\([^\\]+)\\(.+)"
// ShareOrExport: "^\\\\([^\\]+)\\(.+)|^([^:]+):/(.*)" // (cifsServer)(cifsShare)(nfsServer)(nfsExport)
// Volume:        "^(nfs|cifs)://([^/]+)/(.*)"

namespace sid {

/**
 * Regular expression class definition
 */
class regex
{
public:
  struct result : public std::map<size_t, std::string>
  {
    bool exists(const size_t _key, std::string* pValue = nullptr) const;
  };

  /**
   * @brief Regular Expression class constructor. It compiles the regular expression using the
   *        regular expression library function and set the error code and initialization flag.
   *
   * @param _pattern [in] POSIX Regular Expression
   * @param cflags [in] flags that control the execution. Defaults to REG_EXTENDED.
   *
   * @seealso initialized(), error_code(), error(), Exex()
   */
  regex(const char* _pattern, int _cflags = REG_EXTENDED);
  //! Destructor
  virtual ~regex();

  //! Checks the initialization status of the object. Initialization status is set by the constructor.
  bool is_initialized() const { return m_isInitialized; }
  //! Returns the error code associated with the last action on the object.
  int error_code() const { return m_errorCode; }
  //! Returns the error string associated with the last action on the object.
  std::string error() const;
  //! Returns the error string associated with the given error number.
  std::string error(const int _errorCode) const;
  //! Returns the regular expression pattern string
  const std::string& pattern() const;

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
  bool exec(const char* _input);

  /**
   * @fn exec(const char* _input, result& _result)
   *
   * @brief Check the given input against the regular expression and return whether it matches or not.
   *        It also returns the result in a map.
   *
   * @param _input [in] Input string to be checked
   * @param _result [out] Map that holds the resulting match on return.
   *
   * @returns true if the input matches the regular expression, false otherwise
   *
   * @seealso error_code(), error()
   */
  bool exec(const char* _input, result& _result);

  /**
   * @fn exec(const char* _pattern, const char* _input, std::string* _pcsError)
   *
   * @brief static function used to check whether given input string matches the regular expression or not
   *        without manually creating the RegEx object. It returns the error code associated with the check
   *        and optionally the error string, if requested.
   *
   * @param _pattern [in] POSIX Regular Expression
   * @param _input [in] Input string to be checked
   * @param _pcsError [out] A pointer to std::string that'll return the error string in case of an error. Can be NULL if error string is not required.
   *
   * @returns 0 on success or an error code returned by regex library function in case of an error.
   */
  static int exec(const char* _pattern, const char* _input, std::string* _pcsError = nullptr);

  /**
   * @fn exec(const char* _pattern, const char* _input, result& _result, std::string* _pcsError)
   *
   * @brief static function used to check whether given input string matches the regular expression or not
   *        without manually creating the RegEx object and returns the result in a map.
   *        It returns the error code associated with the check and optionally the error string, if requested.
   *
   * @param _pattern [in] POSIX Regular Expression
   * @param _input [in] Input string to be checked
   * @param _result [out] Map that holds the resulting match on return.
   * @param _pcsError [out] A pointer to std::string that'll return the error string in case of an error. Can be NULL if error string is not required.
   *
   * @returns 0 on success or an error code returned by regex library function in case of an error.
   */
  static int exec(const char* _pattern, const char* _input, result& _result, std::string* _pcsError = nullptr);

private:
  /**
   * @fn p_exec(const char* _input, result* _result)
   *
   * @brief Private function that actually checks the given input against the regular expression and return whether it matches or not.
   *        It also returns the result in a map, if requested
   *
   * @param _input [in] Input string to be checked
   * @param _result [out] Pointer to the map that holds the resulting match on return. Can be NULL if result is not required.
   *
   * @returns 0 on success or an error code returned by regex library function in case of an error.
   *
   * @seealso exec(), error_code(), error()
   */
  int p_exec(const char* _input, result* _result = nullptr);

private:
  std::string m_pattern;      //! Regular expression string
  regex_t     m_buffer;       //! Regular expression buffer. Defined in standard library regex.h
  int         m_errorCode;    //! Error code returned by regex library functions.
  bool        m_isInitialized; //! Initialization flag. Set by the constructor
};

} // sid

#endif // _SID_REGEX_H_
